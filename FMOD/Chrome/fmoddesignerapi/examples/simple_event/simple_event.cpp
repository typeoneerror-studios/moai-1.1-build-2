/*===============================================================================================
 SimpleEvent Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 Demonstrates basic usage of FMOD's data-driven event library
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

#include "fmod_event.hpp"
#include "fmod_errors.h"
#include "fmodnacl.h"

#define ERRCHECK(result) if (result != FMOD_OK) { SetStatus("FMOD error! (%d) %s", result, FMOD_ErrorString(result)); assert(0); }

namespace simple_event 
{

static const char* PlayEventMessage = "playEvent";

class SimpleEventInstance : public pp::Instance
{
	public:
	explicit SimpleEventInstance(PP_Instance instance);
	virtual ~SimpleEventInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
		
	void SetStatus(const char* format, ...);

	public:	
	FMOD::EventSystem	*mEventSystem;
	FMOD::Event   		*mEvent;
	
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOAD_PROJECT,
		LOADSTATE_LOAD_EVENTS,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

SimpleEventInstance::SimpleEventInstance(PP_Instance instance) :
pp::Instance(instance),
mEventSystem(0),
mEvent(0)
{
}

bool SimpleEventInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
{
    FMOD_RESULT      result;
    unsigned int     version;
	
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
	
    result = mEventSystem->init(64, FMOD_INIT_NORMAL, &extraDriverData, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
	
	result = mEventSystem->setMediaPath("/media/");
	ERRCHECK(result);
	
	mLoadState = LOADSTATE_FETCHING;
	nacl_file::FileManager::Init(this, 20*1024*1024);
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&SimpleEventInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/examples.fev");
	nacl_file::FileManager::Fetch("/media/tutorial_bank.fsb");
	
	Update(0);
}

SimpleEventInstance::~SimpleEventInstance()
{
	FMOD_RESULT result;
	result = mEventSystem->release();
	ERRCHECK(result);
}


void SimpleEventInstance::SetStatus(const char* format, ...)
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


void SimpleEventInstance::FilesLoaded()
{
	mLoadState = LOADSTATE_LOAD_PROJECT;
	SetStatus("Loading project data...");
}

void UpdateCallback(void *data, int32_t unused)
{
	((SimpleEventInstance*)data)->Update(unused);
}

void SimpleEventInstance::Update(int32_t unused)
{
	mEventSystem->update();
	
	FMOD_RESULT	 result;
	
	if (mLoadState == LOADSTATE_FETCHING)
	{
		SetStatus("Downloading sound files...");
	}
	
	if (mLoadState == LOADSTATE_LOAD_PROJECT)
	{
		result = mEventSystem->load("examples.fev", 0, 0);
		ERRCHECK(result);
		
		result = mEventSystem->getEvent("examples/FeatureDemonstration/Basics/SimpleEvent", FMOD_EVENT_NONBLOCKING, &mEvent);
		ERRCHECK(result);

		mLoadState = LOADSTATE_LOAD_EVENTS;
		SetStatus("Loading event data...");		
	}
	
	
	if (mEvent)
	{
		FMOD_EVENT_STATE state;
		result = mEvent->getState(&state);
		ERRCHECK(result);
		
		if (state & FMOD_EVENT_STATE_PLAYING)
		{
			SetStatus("Event is playing");
		}
		else if (!(state & FMOD_EVENT_STATE_LOADING))
		{
			mLoadState = LOADSTATE_DONE;
			SetStatus("Event is ready");
		}
	
	}


	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(33, nexttick, 0);
}

void SimpleEventInstance::HandleMessage(const pp::Var& var_message) 
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
  
	if (message == PlayEventMessage) 
	{	
		if (mEvent)
		{
			FMOD_RESULT result;
			result = mEvent->start();
			ERRCHECK(result);
		}
	}
}

class SimpleEventModule : public pp::Module 
{
	public:
	SimpleEventModule() : pp::Module() {}
	virtual ~SimpleEventModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new SimpleEventInstance(instance);
	}
};

}  // namespace playSound


namespace pp 
{

Module* CreateModule()
{
	return new simple_event::SimpleEventModule();
}

}  // namespace pp
