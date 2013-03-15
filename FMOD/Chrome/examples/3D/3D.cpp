/*===============================================================================================
 3d Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to basic 3d positioning
===============================================================================================*/
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>
#include <math.h>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/url_loader.h"
#include "ppapi/cpp/url_request_info.h"
#include "ppapi/cpp/url_response_info.h"
#include "ppapi/cpp/file_ref.h"
#include "ppapi/cpp/file_io.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/audio.h"
#include "ppapi/cpp/audio_config.h"
#include "ppapi/c/ppb.h"

#include "../common/nacl_file.h"

#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmodnacl.h"

#define ERRCHECK(result) if (result != FMOD_OK) { SetStatus("FMOD error! (%d) %s", result, FMOD_ErrorString(result)); assert(0); }

namespace positionalSound
{

static const char* ToggleSound1Message = "toggleSound1";
static const char* ToggleSound2Message = "toggleSound2";
static const char* Play2dSoundMessage = "play2dSound";
static const char* ToggleAutoMoveMessage = "toggleAutoMove";
static const char* MoveForwardMessage = "moveForwards";
static const char* MoveBackwardsMessage = "moveBackwards";

static const int   InterfaceUpdateTime = 50;      // 50ms update for interface
static const float DistanceFactor = 1.0f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

class PositionalSoundInstance : public pp::Instance
{
	public:
	explicit PositionalSoundInstance(PP_Instance instance);
	virtual ~PositionalSoundInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
	
	void SetStatus(const char* format, ...);

	private:	
	FMOD::System    *mSystem;
	FMOD::Channel   *mChannel[3];
	FMOD::Sound     *mSound[3];	
	
	FMOD_VECTOR		mListenerPos;
	FMOD_VECTOR		mLastPos;
	bool			mAutoUpdate;
	float			mTime;
	
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOADING,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

PositionalSoundInstance::PositionalSoundInstance(PP_Instance instance) :
pp::Instance(instance),
mSystem(0),
mAutoUpdate(true),
mTime(0)
{
	for (int i = 0; i < 3; i++)
	{
		mChannel[i] = NULL;
		mSound[i] = NULL;
	}
	mListenerPos.x = mListenerPos.y = mListenerPos.z = 0;
	mLastPos = mListenerPos;
}

bool PositionalSoundInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
{
    FMOD_RESULT      result;
    unsigned int     version;
	
	SetStatus("Starting FMOD");
	
    /*
        Create a System object and initialize.
    */
	//FMOD::Debug_SetLevel(FMOD_DEBUG_ALL);
    result = FMOD::System_Create(&mSystem);
    ERRCHECK(result);

    result = mSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		assert(0);
    }

	/*
		Pass a pointer to the module instance as the platform-specific third argument
	*/
	
	
	FMOD_NACL_EXTRADRIVERDATA extraDriverData;
	memset(&extraDriverData, 0, sizeof(FMOD_NACL_EXTRADRIVERDATA));
	extraDriverData.instance = pp_instance();
		
    result = mSystem->init(32, FMOD_INIT_NORMAL, &extraDriverData);
    ERRCHECK(result);
	
	SetStatus("Downloading sound files");
	mLoadState = LOADSTATE_FETCHING;
	nacl_file::FileManager::Init(this, 20*1024*1024);
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&PositionalSoundInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/drumloop.wav");
	nacl_file::FileManager::Fetch("/media/swish.wav");
	nacl_file::FileManager::Fetch("/media/jaguar.wav");
	
	Update(0);
}

PositionalSoundInstance::~PositionalSoundInstance()
{
	FMOD_RESULT result;
	result = mSystem->release();
	ERRCHECK(result);
}


void PositionalSoundInstance::SetStatus(const char* format, ...)
{
	char status[1024] = "status:";
	
    va_list argptr;
    va_start(argptr, format);
    vsprintf(status + strlen(status), format, argptr);
    va_end(argptr);
	
	printf(status);
	printf("\n");
	
	PostMessage(pp::Var(status));
}

void PositionalSoundInstance::FilesLoaded()
{
	SetStatus("Reading sound files");
	FMOD_RESULT result;
	result = mSystem->createSound("/media/drumloop.wav", FMOD_SOFTWARE | FMOD_3D | FMOD_NONBLOCKING, 0, &mSound[0]);
	ERRCHECK(result);              
	result = mSystem->createSound("/media/jaguar.wav", FMOD_SOFTWARE | FMOD_3D | FMOD_NONBLOCKING, 0, &mSound[1]);
	ERRCHECK(result);              
	result = mSystem->createSound("/media/swish.wav", FMOD_SOFTWARE | FMOD_2D | FMOD_NONBLOCKING, 0, &mSound[2]);
	ERRCHECK(result);              
	mLoadState = LOADSTATE_LOADING;
}

void UpdateCallback(void *data, int32_t unused)
{
	((PositionalSoundInstance*)data)->Update(unused);
}

void PositionalSoundInstance::Update(int32_t unused)
{
	mSystem->update();
	
	FMOD_RESULT	 result;
	
	if (mLoadState == LOADSTATE_LOADING)
	{	
		bool allLoaded = true;
		FMOD_OPENSTATE openState;
		
		for (int i = 0; i < 3; i++)
		{
			mSound[i]->getOpenState(&openState, NULL, NULL, NULL);
			allLoaded &= (openState == FMOD_OPENSTATE_READY);
		}
				
		if (allLoaded)
		{
			SetStatus("Playing sounds");
			mLoadState = LOADSTATE_DONE;
			
			result = mSound[0]->set3DMinMaxDistance(0.5f * DistanceFactor, 5000.0f * DistanceFactor);
			ERRCHECK(result);
			result = mSound[0]->setMode(FMOD_LOOP_NORMAL);
			ERRCHECK(result);			
			
			result = mSound[1]->set3DMinMaxDistance(0.5f * DistanceFactor, 5000.0f * DistanceFactor);
			ERRCHECK(result);
			result = mSound[1]->setMode(FMOD_LOOP_NORMAL);
			ERRCHECK(result);			
			
			
			/*
				Play sounds at certain positions
			*/
			{
				FMOD_VECTOR pos = { -10.0f * DistanceFactor, 0.0f, 0.0f };
				FMOD_VECTOR vel = {  0.0f, 0.0f, 0.0f };

				result = mSystem->playSound(FMOD_CHANNEL_FREE, mSound[0], true, &mChannel[0]);
				ERRCHECK(result);
				result = mChannel[0]->set3DAttributes(&pos, &vel);
				ERRCHECK(result);
				result = mChannel[0]->setPaused(false);
				ERRCHECK(result);
			}
			
			 {
				FMOD_VECTOR pos = { 15.0f * DistanceFactor, 0.0f, 0.0f };
				FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

				result = mSystem->playSound(FMOD_CHANNEL_FREE, mSound[1], true, &mChannel[1]);
				ERRCHECK(result);
				result = mChannel[1]->set3DAttributes(&pos, &vel);
				ERRCHECK(result);
				result = mChannel[1]->setPaused(false);
				ERRCHECK(result);
			}
		}
	}
	
	{
		FMOD_VECTOR forward        = { 0.0f, 0.0f, 1.0f };
		FMOD_VECTOR up             = { 0.0f, 1.0f, 0.0f };
		FMOD_VECTOR vel;

		if (mAutoUpdate)
		{
			mListenerPos.x = (float)sin(mTime * 0.05f) * 33.0f * DistanceFactor; // left right pingpong
		}

		// ********* NOTE ******* READ NEXT COMMENT!!!!!
		// vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
		vel.x = (mListenerPos.x - mLastPos.x) * (1000 / InterfaceUpdateTime);
		vel.y = (mListenerPos.y - mLastPos.y) * (1000 / InterfaceUpdateTime);
		vel.z = (mListenerPos.z - mLastPos.z) * (1000 / InterfaceUpdateTime);

		// store pos for next time
		mLastPos = mListenerPos;

		result = mSystem->set3DListenerAttributes(0, &mListenerPos, &vel, &forward, &up);
		ERRCHECK(result);

		mTime += (30 * (1.0f / (float)InterfaceUpdateTime));    // t is just a time value .. it increments in 30m/s steps in this example

		// print out a small visual display
		{
			char s[80];

			sprintf(s, "|.......................<1>......................<2>....................|");

			s[(int)(mListenerPos.x / DistanceFactor) + 35] = 'L';
			PostMessage(pp::Var(s));
		}
	}
		

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(InterfaceUpdateTime, nexttick, 0);
}

void PositionalSoundInstance::HandleMessage(const pp::Var& var_message) 
{
	if (!var_message.is_string()) 
	{
		return;
	}
	if (mLoadState != LOADSTATE_DONE)
	{
		return;
	}
	std::string message = var_message.AsString();
	
	if (message == ToggleSound1Message)
	{		
		bool paused;
		mChannel[0]->getPaused(&paused);
		mChannel[0]->setPaused(!paused);
	}
	if (message == ToggleSound2Message)
	{		
		bool paused;
		mChannel[1]->getPaused(&paused);
		mChannel[1]->setPaused(!paused);
	}
	if (message == Play2dSoundMessage)
	{		
		FMOD_RESULT result = mSystem->playSound(FMOD_CHANNEL_FREE, mSound[2], false, &mChannel[2]);
		ERRCHECK(result);
	}
	if (message == ToggleAutoMoveMessage)
	{
		mAutoUpdate = !mAutoUpdate;
	}
	if (message == MoveForwardMessage)
	{
		if (!mAutoUpdate)
		{
			mListenerPos.x -= 1.0f * DistanceFactor;
			if (mListenerPos.x < -35 * DistanceFactor)
			{
				mListenerPos.x = -35 * DistanceFactor;
			}
		}
	}
	if (message == MoveBackwardsMessage)
	{
		if (!mAutoUpdate)
		{
			mListenerPos.x += 1.0f * DistanceFactor;
			if (mListenerPos.x > 36 * DistanceFactor)
			{
				mListenerPos.x = 36 * DistanceFactor;
			}
		}
	}
  
}

class PositionalSoundModule : public pp::Module 
{
	public:
	PositionalSoundModule() : pp::Module() {}
	virtual ~PositionalSoundModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new PositionalSoundInstance(instance);
	}
};

}  // namespace positionalAudio


namespace pp 
{

Module* CreateModule()
{
	return new positionalSound::PositionalSoundModule();
}

}  // namespace pp
