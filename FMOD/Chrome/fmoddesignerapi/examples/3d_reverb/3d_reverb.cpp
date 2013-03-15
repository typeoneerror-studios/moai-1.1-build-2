/*==================================================================================================
 3D Reverb Example
 Copyright (c), Firelight Technologies Pty, Ltd 2005-2011.

 Example to demonstrate 3d reverb spheres, global reverb, multiple reverb instances and music reverb
===================================================================================================*/
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

#include "fmod_event.hpp"
#include "fmod_errors.h"
#include "fmodnacl.h"

#include "examples.h"

#define ERRCHECK(result) if ((result) != FMOD_OK) { SetStatus("FMOD error! (%d) %s", result, FMOD_ErrorString(result)); assert(0); }

namespace reverb_3d 
{

static const char* ToggleEventMessage = "toggleEvent";
static const char* ToggleMusicMessage = "toggleMusic";
static const char* ToggleAutoMoveMessage = "toggleAutoMove";
static const char* MoveForwardMessage = "moveForwards";
static const char* MoveBackwardsMessage = "moveBackwards";
static const char* IncReverbMessage = "increaseReverb";
static const char* DecReverbMessage = "decreaseReverb";

static const float DistanceFactor = 1.0f;
static const int UpdateTime = 33;

class Reverb3dInstance : public pp::Instance
{
	public:
	explicit Reverb3dInstance(PP_Instance instance);
	virtual ~Reverb3dInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
		
	void SetStatus(const char* format, ...);

	public:	
	FMOD::EventSystem	*mEventSystem;
	FMOD::Event   		*mEvent;
	
    FMOD::MusicSystem   *mMusicSystem;
    FMOD::MusicPrompt   *mMusicPrompt;
    FMOD::EventReverb   *mEventReverb1;
	FMOD::EventReverb	*mEventReverb2;
	
	FMOD_VECTOR			mListenerPos;
	FMOD_VECTOR			mLastPos;
	bool				mAutoUpdate;
	float				mTime;
	
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOAD_PROJECT,
		LOADSTATE_LOAD_EVENTS,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

Reverb3dInstance::Reverb3dInstance(PP_Instance instance) :
pp::Instance(instance),
mEventSystem(0),
mEvent(0),
mMusicSystem(0),
mMusicPrompt(0),
mEventReverb1(0),
mEventReverb2(0),
mAutoUpdate(true),
mTime(0)
{
	mListenerPos.x = mListenerPos.y = mListenerPos.z = 0;
	mLastPos = mListenerPos;
}

bool Reverb3dInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
{
	FMOD_REVERB_CHANNELPROPERTIES    reverbchannelproperties;    
    FMOD_VECTOR                      reverbposition;
    float                            mindistance, maxdistance;
    FMOD_RESULT      result;
	unsigned int version;
	
	SetStatus("Starting FMOD Event");
	
    /*
        Create a System object and initialize.
    */
	//FMOD::Debug_SetLevel(FMOD_DEBUG_ALL);
    result = FMOD::EventSystem_Create(&mEventSystem);
    ERRCHECK(result);

    result = mEventSystem->getVersion(&version);
    ERRCHECK(result);

	/*
		Pass a pointer to the module instance as the platform-specific third argument
	*/	
	FMOD_NACL_EXTRADRIVERDATA extraDriverData;
	memset(&extraDriverData, 0, sizeof(FMOD_NACL_EXTRADRIVERDATA));
	extraDriverData.instance = pp_instance();
	
    result = mEventSystem->init(64, FMOD_INIT_3D_RIGHTHANDED, &extraDriverData, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
	
	result = mEventSystem->setMediaPath("/media/");
	ERRCHECK(result);
	
	mLoadState = LOADSTATE_FETCHING;
	nacl_file::FileManager::Init(this, 20*1024*1024);
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&Reverb3dInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/examples.fev");
	nacl_file::FileManager::Fetch("/media/tutorial_bank.fsb");
	
	Update(0);
}

Reverb3dInstance::~Reverb3dInstance()
{
	FMOD_RESULT result;
	result = mEventSystem->release();
	ERRCHECK(result);
}


void Reverb3dInstance::SetStatus(const char* format, ...)
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


void Reverb3dInstance::FilesLoaded()
{
	mLoadState = LOADSTATE_LOAD_PROJECT;
	SetStatus("Loading project data...");
}

void UpdateCallback(void *data, int32_t unused)
{
	((Reverb3dInstance*)data)->Update(unused);
}

void Reverb3dInstance::Update(int32_t unused)
{	
	FMOD_RESULT	 result;
	
	if (mLoadState == LOADSTATE_FETCHING)
	{
		SetStatus("Downloading sound files...");
	}
	
	if (mLoadState == LOADSTATE_LOAD_PROJECT)
	{
		result = mEventSystem->load("examples.fev", 0, 0);
		ERRCHECK(result);
		
		result = mEventSystem->getEvent("examples/FeatureDemonstration/Basics/FootStep", FMOD_EVENT_NONBLOCKING, &mEvent);
		ERRCHECK(result);
		
		mLoadState = LOADSTATE_LOAD_EVENTS;
		SetStatus("Loading event data...");		
	}
	
	if (mLoadState == LOADSTATE_LOAD_EVENTS)
	{
		FMOD_EVENT_STATE state;
		result = mEvent->getState(&state);
		ERRCHECK(result);
		
		if (!(state & FMOD_EVENT_STATE_LOADING))
		{
			mLoadState = LOADSTATE_DONE;
			SetStatus("Event is ready");
		
			result = mEventSystem->getMusicSystem(&mMusicSystem);
			ERRCHECK(result);
			
			result = mMusicSystem->prepareCue(MUSICCUE_EXAMPLES_ENTER_MENU__VIBES_, &mMusicPrompt);
			ERRCHECK(result);
			
			result = mEventSystem->createReverb(&mEventReverb1);
			ERRCHECK(result);
			
			result = mEventSystem->createReverb(&mEventReverb2);
			ERRCHECK(result);
			
			FMOD_REVERB_CHANNELPROPERTIES    reverbchannelproperties;    
			FMOD_VECTOR                      reverbposition;
			float                            mindistance, maxdistance;
			
			FMOD_REVERB_PROPERTIES sphere1reverb = FMOD_PRESET_HANGAR;
			reverbposition.x = -10.0;
			reverbposition.y = 0;
			reverbposition.z = 0;
			mindistance = 10.0;
			maxdistance = 20.0;
			ERRCHECK(result = mEventReverb1->setProperties(&sphere1reverb));
			ERRCHECK(result = mEventReverb1->set3DAttributes(&reverbposition, mindistance, maxdistance));
			ERRCHECK(result = mEventReverb1->setActive(true));

			FMOD_REVERB_PROPERTIES sphere2reverb = FMOD_PRESET_SEWERPIPE;
			reverbposition.x = 10.0;
			reverbposition.y = 0;
			reverbposition.z = 0;
			mindistance = 10.0;
			maxdistance = 20.0;
			ERRCHECK(result = mEventReverb2->setProperties(&sphere2reverb));
			ERRCHECK(result = mEventReverb2->set3DAttributes(&reverbposition, mindistance, maxdistance));
			ERRCHECK(result = mEventReverb2->setActive(true));

			/*
				Setup ambient reverb
			*/
			FMOD_REVERB_PROPERTIES ambientreverb = FMOD_PRESET_ROOM;
			ERRCHECK(result = mEventSystem->setReverbAmbientProperties(&ambientreverb));
			
			/*
				Setup global reverb instance #1 for music
			*/
			FMOD_REVERB_PROPERTIES musicreverb = FMOD_PRESET_GENERIC;
			musicreverb.Room = 0;
			musicreverb.Instance = 1;
			ERRCHECK(result = mEventSystem->setReverbProperties(&musicreverb));

			/*
				Turn off global reverb instance #1 for event
			*/
			memset(&reverbchannelproperties, 0, sizeof(FMOD_REVERB_CHANNELPROPERTIES));
			reverbchannelproperties.Flags = FMOD_REVERB_CHANNELFLAGS_INSTANCE1;
			ERRCHECK(result = mEvent->getReverbProperties(&reverbchannelproperties));
			reverbchannelproperties.Room = -10000;                                          // wet level -10000 sets instance #1 off for this event
			ERRCHECK(result = mEvent->setReverbProperties(&reverbchannelproperties));

			/*
				Start music and event
			*/
			ERRCHECK(result = mMusicPrompt->begin());
			ERRCHECK(result = mMusicSystem->setVolume(0.5f));

			FMOD_VECTOR eventposition = {0, 0, 5.0f};
			FMOD_VECTOR eventvelocity = {0, 0, 0};
			ERRCHECK(mEvent->set3DAttributes(&eventposition, &eventvelocity));
			ERRCHECK(mEvent->start());
		}

	}
	
	if (mLoadState == LOADSTATE_DONE)
	{
		FMOD_VECTOR forward        = { 0.0f, 0.0f, 1.0f };
		FMOD_VECTOR up             = { 0.0f, 1.0f, 0.0f };
		FMOD_VECTOR vel;

		if (mAutoUpdate)
		{
			mListenerPos.x = (float)sin(mTime * 0.05f) * 33.0f * DistanceFactor; // left right pingpong
		}

		// vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
		vel.x = (mListenerPos.x - mLastPos.x) * (1000 / (float)UpdateTime);
		vel.y = (mListenerPos.y - mLastPos.y) * (1000 / (float)UpdateTime);
		vel.z = (mListenerPos.z - mLastPos.z) * (1000 / (float)UpdateTime);

		// store pos for next time
		mLastPos = mListenerPos;

		result = mEventSystem->set3DListenerAttributes(0, &mListenerPos, &vel, &forward, &up);
		ERRCHECK(result);

		mTime += (30 * (1.0f / (float)UpdateTime)); 
		
		{
			char s[80];

			sprintf(s, ".....x.........m.........1.........|.........2.........m.........x.......");

			s[(int)(mListenerPos.x / DistanceFactor) + 35] = 'L';
			PostMessage(pp::Var(s));
		}
	}
		
	mEventSystem->update();

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(UpdateTime, nexttick, 0);
}

void Reverb3dInstance::HandleMessage(const pp::Var& var_message) 
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
  
	FMOD_RESULT result;
	
	if (message == ToggleEventMessage) 
	{	
		FMOD_EVENT_STATE state;
		ERRCHECK(result = mEvent->getState(&state));
		if (state & FMOD_EVENT_STATE_PLAYING)
		{
			ERRCHECK(result = mEvent->stop());
		}
		else
		{
			ERRCHECK(result = mEvent->start());
		}
	}
	
	if (message == ToggleMusicMessage)
	{
		bool active;
		ERRCHECK(result = mMusicPrompt->isActive(&active));
		if (active)
		{
			ERRCHECK(result = mMusicPrompt->end());
			ERRCHECK(result = mMusicSystem->reset());
		}
		else
		{
			ERRCHECK(result = mMusicPrompt->begin());
		}
	}
	
	if (message == IncReverbMessage)
	{
		FMOD_REVERB_CHANNELPROPERTIES reverbchannelproperties;
		reverbchannelproperties.Flags = FMOD_REVERB_CHANNELFLAGS_INSTANCE1;
		ERRCHECK(result = mMusicSystem->getReverbProperties(&reverbchannelproperties));

		reverbchannelproperties.Room += 100;
		if (reverbchannelproperties.Room > 0)
		{
			reverbchannelproperties.Room = 0;
		}

		ERRCHECK(result = mMusicSystem->setReverbProperties(&reverbchannelproperties));
	}
	if (message == DecReverbMessage)
	{
		FMOD_REVERB_CHANNELPROPERTIES reverbchannelproperties;
		reverbchannelproperties.Flags = FMOD_REVERB_CHANNELFLAGS_INSTANCE1;
		ERRCHECK(result = mMusicSystem->getReverbProperties(&reverbchannelproperties));

		reverbchannelproperties.Room -= 100;
		if (reverbchannelproperties.Room < -2000)
		{
			reverbchannelproperties.Room = -2000;
		}

		ERRCHECK(result = mMusicSystem->setReverbProperties(&reverbchannelproperties));
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

class Reverb3dModule : public pp::Module 
{
	public:
	Reverb3dModule() : pp::Module() {}
	virtual ~Reverb3dModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new Reverb3dInstance(instance);
	}
};

}  // namespace playSound


namespace pp 
{

Module* CreateModule()
{
	return new reverb_3d::Reverb3dModule();
}

}  // namespace pp
