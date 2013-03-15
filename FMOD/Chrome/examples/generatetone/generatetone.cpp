/*===============================================================================================
 GenerateTone Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how simply play generated tones using FMOD::System::payDSP instead of 
 manually connecting and disconnecting DSP units.
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

#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmodnacl.h"

#define ERRCHECK(result) if (result != FMOD_OK) { SetStatus("FMOD error! (%d) %s", result, FMOD_ErrorString(result)); assert(0); }

namespace generateTone 
{

const char* const PlaySineWaveMsg = "playSineWave";
const char* const PlaySquareWaveMsg = "playSquareWave";
const char* const PlaySawWaveMsg = "playSawWave";
const char* const PlayTriangleWaveMsg = "playTriangleWave";
const char* const PlayWhiteNoiseMsg = "playWhiteNoise";
const char* const StopChannelMsg = "stopChannel";
const char* const VolumeUpMsg = "volumeUp";
const char* const VolumeDownMsg = "volumeDown";
const char* const FrequencyUpMsg = "frequencyUp";
const char* const FrequencyDownMsg = "frequencyDown";
const char* const PanLeftMsg = "panLeft";
const char* const PanRightMsg = "panRight";

class GenerateToneInstance : public pp::Instance
{
	public:
	explicit GenerateToneInstance(PP_Instance instance);
	virtual ~GenerateToneInstance();
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void SetStatus(const char* format, ...);
	void Update();

	private:
	FMOD::System    *mSystem;
	FMOD::Channel   *mChannel;
	FMOD::DSP       *mDsp;
};

GenerateToneInstance::GenerateToneInstance(PP_Instance instance) :
pp::Instance(instance),
mSystem(0),
mChannel(0),
mDsp(0)
{
}

bool GenerateToneInstance::Init(uint32_t argc, const char *argn[], const char * argv[])
{
    FMOD_RESULT      result;
    unsigned int     version;
	
    /*
        Create a System object and initialize.
    */
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
        Create an oscillator DSP units for the tone.
    */
    result = mSystem->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &mDsp);
    ERRCHECK(result);
    result = mDsp->setParameter(FMOD_DSP_OSCILLATOR_RATE, 440.0f);       /* musical note 'A' */
    ERRCHECK(result);
	
	Update();
}

GenerateToneInstance::~GenerateToneInstance()
{
	FMOD_RESULT      result;
	result = mDsp->release();
	ERRCHECK(result);
	result = mSystem->release();
	ERRCHECK(result);
}

void UpdateCallback(void *data, int32_t unused)
{
	((GenerateToneInstance*)data)->Update();
}

void GenerateToneInstance::Update()
{
	mSystem->update();
	
	bool playing;
	mChannel->isPlaying(&playing);
	if (!playing)
	{		
		SetStatus("Ready");
	}

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(10, nexttick, 0);
}

void GenerateToneInstance::SetStatus(const char* format, ...)
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


void GenerateToneInstance::HandleMessage(const pp::Var& var_message) 
{
	if (!var_message.is_string()) 
	{
		return;
	}
	std::string message = var_message.AsString();
  
	if (message == PlaySineWaveMsg) 
	{
		FMOD_RESULT result;
		result = mSystem->playDSP(FMOD_CHANNEL_REUSE, mDsp, true, &mChannel);
		ERRCHECK(result);
		mChannel->setVolume(0.5f);
		result = mDsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 0);
		ERRCHECK(result);
		mChannel->setPaused(false);
		SetStatus("Playing sine wave");
	}
	if (message == PlaySquareWaveMsg) 
	{
		FMOD_RESULT result;
		result = mSystem->playDSP(FMOD_CHANNEL_REUSE, mDsp, true, &mChannel);
		ERRCHECK(result);
		mChannel->setVolume(0.125f);
		result = mDsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 1);
		ERRCHECK(result);
		mChannel->setPaused(false);
		SetStatus("Playing square wave");
	}
	if (message == PlaySawWaveMsg) 
	{
		FMOD_RESULT result;
		result = mSystem->playDSP(FMOD_CHANNEL_REUSE, mDsp, true, &mChannel);
		ERRCHECK(result);
		mChannel->setVolume(0.125f);
		result = mDsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 2);
		ERRCHECK(result);
		mChannel->setPaused(false);
		SetStatus("Playing saw wave");
	}
	if (message == PlayTriangleWaveMsg) 
	{
		FMOD_RESULT result;
		result = mSystem->playDSP(FMOD_CHANNEL_REUSE, mDsp, true, &mChannel);
		ERRCHECK(result);
		mChannel->setVolume(0.5f);
		result = mDsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 4);
		ERRCHECK(result);
		mChannel->setPaused(false);
		SetStatus("Playing triangle wave");
	}
	if (message == PlayWhiteNoiseMsg) 
	{
		FMOD_RESULT result;
		result = mSystem->playDSP(FMOD_CHANNEL_REUSE, mDsp, true, &mChannel);
		ERRCHECK(result);
		mChannel->setVolume(0.25f);
		result = mDsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 5);
		ERRCHECK(result);
		mChannel->setPaused(false);
		SetStatus("Playing white noise");
	}
	if (message == StopChannelMsg) 
	{
		mChannel->stop();
	}
	if (message == VolumeUpMsg) 
	{
		float volume;
		mChannel->getVolume(&volume);
		volume += 0.1f;
		mChannel->setVolume(volume);
	}
	if (message == VolumeDownMsg) 
	{
		float volume;
		mChannel->getVolume(&volume);
		volume -= 0.1f;
		mChannel->setVolume(volume);
	}
	if (message == FrequencyUpMsg) 
	{
		float frequency;
		mChannel->getFrequency(&frequency);
		frequency += 500.0f;
		mChannel->setFrequency(frequency);
	}
	if (message == FrequencyDownMsg) 
	{
		float frequency;
		mChannel->getFrequency(&frequency);
		frequency -= 500.0f;
		mChannel->setFrequency(frequency);
	}
	if (message == PanLeftMsg)
	{
		float pan;
		mChannel->getPan(&pan);
		pan -= 0.1f;
		mChannel->setPan(pan);
	}
	if (message == PanRightMsg)
	{
		float pan;
		mChannel->getPan(&pan);
		pan += 0.1f;
		mChannel->setPan(pan);
	}
	
	{
		float frequency;
		float pan;
		float volume;
		
		mChannel->getVolume(&volume);
		mChannel->getFrequency(&frequency);
		mChannel->getPan(&pan);
		
		char buf[32];
		sprintf(buf, "%.1f:%.1f:%.1f", volume, frequency, pan);
		
		PostMessage(pp::Var(buf));
	}
}

class GenerateToneModule : public pp::Module 
{
	public:
	GenerateToneModule() : pp::Module() {}
	virtual ~GenerateToneModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new GenerateToneInstance(instance);
	}
};

}  // namespace generateTone


namespace pp 
{

Module* CreateModule()
{
	return new generateTone::GenerateToneModule();
}

}  // namespace pp
