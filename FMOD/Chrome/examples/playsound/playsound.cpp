/*===============================================================================================
 PlaySound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to simply load and play multiple sounds.  This is about the simplest
 use of FMOD.
 This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
 up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is 
 streamed in realtime as it plays.
===============================================================================================*/
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>

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

namespace playSound 
{

static const char* PlayMonoSoundMessage = "playMonoSound";
static const char* PlayStereoSoundMessage = "playStereoSound";

class PlaySoundInstance : public pp::Instance
{
	public:
	explicit PlaySoundInstance(PP_Instance instance);
	virtual ~PlaySoundInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
	
	void SetStatus(const char* format, ...);

	private:	
	FMOD::System    *mSystem;
	FMOD::Channel   *mChannel;
	FMOD::Sound     *mSoundMono;
	FMOD::Sound     *mSoundStereo;
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOADING,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

PlaySoundInstance::PlaySoundInstance(PP_Instance instance) :
pp::Instance(instance),
mSystem(0),
mChannel(0),
mSoundMono(0),
mSoundStereo(0)
{
}

bool PlaySoundInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
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
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&PlaySoundInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/drumloop.wav");
	nacl_file::FileManager::Fetch("/media/swish.wav");
	
	Update(0);
}

PlaySoundInstance::~PlaySoundInstance()
{
	FMOD_RESULT result;
	result = mSystem->release();
	ERRCHECK(result);
}


void PlaySoundInstance::SetStatus(const char* format, ...)
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

void PlaySoundInstance::FilesLoaded()
{
	SetStatus("Reading sound files");
	FMOD_RESULT result;
	result = mSystem->createSound("/media/drumloop.wav", FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_NONBLOCKING, 0, &mSoundMono);
	ERRCHECK(result);              
	result = mSystem->createSound("/media/swish.wav", FMOD_SOFTWARE | FMOD_NONBLOCKING, 0, &mSoundStereo);
	ERRCHECK(result);
	mLoadState = LOADSTATE_LOADING;
}

void UpdateCallback(void *data, int32_t unused)
{
	((PlaySoundInstance*)data)->Update(unused);
}

void PlaySoundInstance::Update(int32_t unused)
{
	mSystem->update();
	
	FMOD_RESULT	 result;
	unsigned int ms = 0;
	unsigned int lenms = 0;
	bool         playing = 0;
	bool         paused = 0;
	int          channelsplaying = 0;
	
	if (mLoadState == LOADSTATE_LOADING)
	{	
		bool allLoaded = true;
		FMOD_OPENSTATE openState;
		
		mSoundMono->getOpenState(&openState, NULL, NULL, NULL);
		allLoaded &= (openState == FMOD_OPENSTATE_READY);
		
		mSoundStereo->getOpenState(&openState, NULL, NULL, NULL);
		allLoaded &= (openState == FMOD_OPENSTATE_READY);
				
		if (allLoaded)
		{
			SetStatus("Ready to play sounds");
			mLoadState = LOADSTATE_DONE;
		}
	}

	if (mChannel)
	{
		FMOD::Sound *currentsound = 0;

		result = mChannel->isPlaying(&playing);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		result = mChannel->getPaused(&paused);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		result = mChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		mChannel->getCurrentSound(&currentsound);
		if (currentsound)
		{
			result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
			if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
			{
				ERRCHECK(result);
			}
		}
	}

	mSystem->getChannelsPlaying(&channelsplaying);

	char buf[256];
	sprintf(buf, "%d:%d:%s:%d", ms , lenms , paused ? "Paused " : playing ? "Playing" : "Stopped", channelsplaying);
	
	PostMessage(pp::Var(buf));

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(10, nexttick, 0);
}

void PlaySoundInstance::HandleMessage(const pp::Var& var_message) 
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
  
	if (message == PlayMonoSoundMessage) 
	{	
		FMOD_RESULT result;
		result = mSystem->playSound(FMOD_CHANNEL_FREE, mSoundMono, false, &mChannel);
		ERRCHECK(result);
	}  
	if (message == PlayStereoSoundMessage) 
	{	
		FMOD_RESULT result;
		result = mSystem->playSound(FMOD_CHANNEL_FREE, mSoundStereo, false, &mChannel);
		ERRCHECK(result);
	}
}

class PlaySoundModule : public pp::Module 
{
	public:
	PlaySoundModule() : pp::Module() {}
	virtual ~PlaySoundModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new PlaySoundInstance(instance);
	}
};

}  // namespace playSound


namespace pp 
{

Module* CreateModule()
{
	return new playSound::PlaySoundModule();
}

}  // namespace pp
