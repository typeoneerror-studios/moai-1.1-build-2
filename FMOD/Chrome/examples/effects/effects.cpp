/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to apply some of the built in software effects to sounds. 
 This example filters the global mix.  All software sounds played here would be filtered in the 
 same way.
 To filter per channel, and not have other channels affected, simply replace system->addDSP with
 channel->addDSP.
 Note in this example you don't have to add and remove units each time, you could simply add them 
 all at the start then use DSP::setActive to toggle them on and off.
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

namespace effects 
{

static const char* ToggleLowPassMessage = "toggleLowPass";
static const char* ToggleHighPassMessage = "toggleHighPass";
static const char* ToggleEchoMessage = "toggleEcho";
static const char* ToggleFlangeMessage = "toggleFlange";
static const char* ToggleDistortionMessage = "toggleDistortion";
static const char* ToggleChorusMessage = "toggleChorus";
static const char* ToggleParamEqMessage = "toggleParamEq";

class EffectsInstance : public pp::Instance
{
	public:
	explicit EffectsInstance(PP_Instance instance);
	virtual ~EffectsInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
	
	void SetStatus(const char* format, ...);

	private:	
	FMOD::System    *mSystem;
	FMOD::Channel   *mChannel;
	FMOD::Sound     *mSound;	
    FMOD::DSP       *mDspLowpass;
    FMOD::DSP       *mDspHighpass;
    FMOD::DSP       *mDspEcho;
    FMOD::DSP       *mDspFlange;
    FMOD::DSP       *mDspDistortion;
    FMOD::DSP       *mDspChorus;
    FMOD::DSP       *mDspParameq;
	
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOADING,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

EffectsInstance::EffectsInstance(PP_Instance instance) :
pp::Instance(instance),
mSystem(0),
mChannel(0),
mSound(0),
mDspLowpass(0),
mDspHighpass(0),
mDspEcho(0),
mDspFlange(0),
mDspDistortion(0),
mDspChorus(0),
mDspParameq(0)
{
}

bool EffectsInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
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
		Pass a handle to the module instance in the platform-specific third argument
	*/	
	FMOD_NACL_EXTRADRIVERDATA extraDriverData;
	memset(&extraDriverData, 0, sizeof(FMOD_NACL_EXTRADRIVERDATA));
	extraDriverData.instance = pp_instance();
	
    result = mSystem->init(32, FMOD_INIT_NORMAL, &extraDriverData);
    ERRCHECK(result);
	
	/*
		Download all required sounds to the sand-boxed file system
	*/
	SetStatus("Downloading sound files");
	mLoadState = LOADSTATE_FETCHING;
	nacl_file::FileManager::Init(this, 20*1024*1024);
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&EffectsInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/drumloop.wav");
	
	
    /*
        Create some effects to play with.
    */
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &mDspLowpass);
    ERRCHECK(result);
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &mDspHighpass);
    ERRCHECK(result);
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &mDspEcho);
    ERRCHECK(result);
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_FLANGE, &mDspFlange);
    ERRCHECK(result);
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &mDspDistortion);
    ERRCHECK(result);
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_CHORUS, &mDspChorus);
    ERRCHECK(result);
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &mDspParameq);
    ERRCHECK(result);
	
	Update(0);
}

EffectsInstance::~EffectsInstance()
{
	FMOD_RESULT result;
	result = mSystem->release();
	ERRCHECK(result);
}


void EffectsInstance::SetStatus(const char* format, ...)
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

void EffectsInstance::FilesLoaded()
{
	FMOD_RESULT result;
	result = mSystem->createSound("/media/drumloop.wav", FMOD_SOFTWARE | FMOD_NONBLOCKING, 0, &mSound);
	mLoadState = LOADSTATE_LOADING;
	SetStatus("Reading sound files");
}

void UpdateCallback(void *data, int32_t unused)
{
	((EffectsInstance*)data)->Update(unused);
}

void EffectsInstance::Update(int32_t unused)
{
	mSystem->update();
	
	if (mLoadState == LOADSTATE_LOADING)
	{	
		FMOD_OPENSTATE openState;
		
		mSound->getOpenState(&openState, NULL, NULL, NULL);
		
		if (openState == FMOD_OPENSTATE_READY)
		{
			mLoadState = LOADSTATE_DONE;
			SetStatus("Playing sound");
			FMOD_RESULT result;
			result = mSystem->playSound(FMOD_CHANNEL_FREE, mSound, false, &mChannel);
			ERRCHECK(result);
		}
	}

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(33, nexttick, 0);
}

void EffectsInstance::HandleMessage(const pp::Var& var_message) 
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
	
	
	if (message == ToggleLowPassMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspLowpass->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspLowpass->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspLowpass, 0);
			ERRCHECK(result);
		}
	}
	if (message == ToggleHighPassMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspHighpass->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspHighpass->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspHighpass, 0);
			ERRCHECK(result);
		}
	}
	if (message == ToggleEchoMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspEcho->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspEcho->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspEcho, 0);
			ERRCHECK(result);
			result = mDspEcho->setParameter(FMOD_DSP_ECHO_DELAY, 50.0f);
			ERRCHECK(result);
		}
	}
	if (message == ToggleFlangeMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspFlange->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspFlange->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspFlange, 0);
			ERRCHECK(result);
		}
	}
	if (message == ToggleDistortionMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspDistortion->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspDistortion->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspDistortion, 0);
			ERRCHECK(result);
			result = mDspDistortion->setParameter(FMOD_DSP_DISTORTION_LEVEL, 0.8f);
			ERRCHECK(result);
		}
	}
	if (message == ToggleChorusMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspChorus->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspChorus->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspChorus, 0);
			ERRCHECK(result);
		}
	}
	if (message == ToggleParamEqMessage)
	{
		bool active;
		FMOD_RESULT result;

		result = mDspParameq->getActive(&active);
		ERRCHECK(result);

		if (active)
		{
			result = mDspParameq->remove();
			ERRCHECK(result);
		}
		else
		{
			result = mSystem->addDSP(mDspParameq, 0);
			ERRCHECK(result);
			result = mDspParameq->setParameter(FMOD_DSP_PARAMEQ_CENTER, 5000.0f);
			ERRCHECK(result);
			result = mDspParameq->setParameter(FMOD_DSP_PARAMEQ_GAIN, 0.0f);
			ERRCHECK(result);
		}
	}
}

class EffectsModule : public pp::Module 
{
	public:
	EffectsModule() : pp::Module() {}
	virtual ~EffectsModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new EffectsInstance(instance);
	}
};

}  // namespace effects


namespace pp 
{

Module* CreateModule()
{
	return new effects::EffectsModule();
}

}  // namespace pp
