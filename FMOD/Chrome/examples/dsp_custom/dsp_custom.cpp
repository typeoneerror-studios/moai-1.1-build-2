/*===============================================================================================
 Custom DSP Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to add a user created DSP callback to process audio data.
 A read callback is generated at runtime, and can be added anywhere in the DSP network.

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

static const char* ToggleCustomDspMessage = "toggleCustomDsp";

FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels) 
{ 
    unsigned int count, userdata;
    int count2;
    char name[256]; 
    FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance; 

    /* 
        This redundant call just shows using the instance parameter of FMOD_DSP_STATE and using it to 
        call a DSP information function. 
    */
    thisdsp->getInfo(name, 0, 0, 0, 0);

    thisdsp->getUserData((void **)&userdata);

    /*
        This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0' 
        as the number of channels in FMOD_DSP_DESCRIPTION.  
        Specifying an actual channel count will mean you have to take care of any number of channels coming in,
        but outputting the number of channels specified.  Generally it is best to keep the channel 
        count at 0 for maximum compatibility.
    */
    for (count = 0; count < length; count++) 
    { 
        /*
            Feel free to unroll this.
        */
        for (count2 = 0; count2 < outchannels; count2++)
        {
            /* 
                This DSP filter just halves the volume! 
                Input is modified, and sent to output.
            */
            outbuffer[(count * outchannels) + count2] = inbuffer[(count * inchannels) + count2] * 0.2f;
        }
    } 

    return FMOD_OK; 
} 

class DspCustomInstance : public pp::Instance
{
	public:
	explicit DspCustomInstance(PP_Instance instance);
	virtual ~DspCustomInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
	
	void SetStatus(const char* format, ...);

	private:	
	FMOD::System    *mSystem;
	FMOD::Channel   *mChannel;
	FMOD::Sound     *mSound;
    FMOD::DSP       *mDsp;
	bool			mDspBypass;
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOADING,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

DspCustomInstance::DspCustomInstance(PP_Instance instance) :
pp::Instance(instance),
mSystem(0),
mChannel(0),
mSound(0),
mDsp(0),
mDspBypass(true)
{
}

bool DspCustomInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
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
	
	
    /*
        Create the DSP effects.
    */  
    { 
        FMOD_DSP_DESCRIPTION  dspdesc; 

        memset(&dspdesc, 0, sizeof(FMOD_DSP_DESCRIPTION)); 

        strcpy(dspdesc.name, "My first DSP unit"); 
        dspdesc.channels     = 0;                   // 0 = whatever comes in, else specify. 
        dspdesc.read         = myDSPCallback; 
        dspdesc.userdata     = (void *)0x12345678; 

        result = mSystem->createDSP(&dspdesc, &mDsp); 
        ERRCHECK(result); 
    } 

    /*
        Inactive by default.
    */
    mDsp->setBypass(mDspBypass);

    result = mSystem->addDSP(mDsp, 0); 
	ERRCHECK(result);
	
	SetStatus("Downloading sound files");
	mLoadState = LOADSTATE_FETCHING;
	nacl_file::FileManager::Init(this, 20*1024*1024);
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&DspCustomInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/drumloop.wav");
	
	Update(0);
}

DspCustomInstance::~DspCustomInstance()
{
	FMOD_RESULT result;
	result = mSystem->release();
	ERRCHECK(result);
}


void DspCustomInstance::SetStatus(const char* format, ...)
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

void DspCustomInstance::FilesLoaded()
{
	SetStatus("Reading sound files");
	FMOD_RESULT result;
	result = mSystem->createSound("/media/drumloop.wav", FMOD_SOFTWARE | FMOD_NONBLOCKING, 0, &mSound);
	ERRCHECK(result);              
	mLoadState = LOADSTATE_LOADING;
}

void UpdateCallback(void *data, int32_t unused)
{
	((DspCustomInstance*)data)->Update(unused);
}

void DspCustomInstance::Update(int32_t unused)
{
	mSystem->update();
	
	FMOD_RESULT	 result;
	
	if (mLoadState == LOADSTATE_LOADING)
	{	
		bool allLoaded = true;
		FMOD_OPENSTATE openState;
		
		mSound->getOpenState(&openState, NULL, NULL, NULL);
		
		if (openState == FMOD_OPENSTATE_READY)
		{			
			result = mSystem->playSound(FMOD_CHANNEL_FREE, mSound, false, &mChannel);
			ERRCHECK(result);
			SetStatus("Playing sounds");
			mLoadState = LOADSTATE_DONE;
		}
	}

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(10, nexttick, 0);
}

void DspCustomInstance::HandleMessage(const pp::Var& var_message) 
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
  
	if (message == ToggleCustomDspMessage) 
	{	
		mDspBypass = !mDspBypass;
		mDsp->setBypass(mDspBypass);
	} 
}

class DspCustomModule : public pp::Module 
{
	public:
	DspCustomModule() : pp::Module() {}
	virtual ~DspCustomModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new DspCustomInstance(instance);
	}
};

}  // namespace playSound


namespace pp 
{

Module* CreateModule()
{
	return new playSound::DspCustomModule();
}

}  // namespace pp
