/*===============================================================================================
 Submixing Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to put channels into channel groups, so that you can affect a group
 of channels at a time instead of just one channel at a time.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

#define NUM_SOUNDS 5

FMOD_SYSTEM  *gSystem  = 0;
FMOD_CHANNEL *gChannel[NUM_SOUNDS];
FMOD_SOUND	 *gSound[NUM_SOUNDS];
FMOD_CHANNELGROUP *gGroupA, *gGroupB, *gMasterGroup;
FMOD_DSP          *gDSPHighPass, *gDSPChorus, *gDSPLowpass;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_submixing_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int count = 0;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/drumloop.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &gSound[0]);
    CHECK_RESULT(result);
    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/jaguar.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &gSound[1]);
    CHECK_RESULT(result);
    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/c.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &gSound[2]);
    CHECK_RESULT(result);
    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/d.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &gSound[3]);
    CHECK_RESULT(result);
    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/e.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &gSound[4]);
    CHECK_RESULT(result);

    result = FMOD_System_CreateChannelGroup(gSystem, "Group A", &gGroupA);
    CHECK_RESULT(result);

    result = FMOD_System_CreateChannelGroup(gSystem, "Group B", &gGroupB);
    CHECK_RESULT(result);

    result = FMOD_System_GetMasterChannelGroup(gSystem, &gMasterGroup);
    CHECK_RESULT(result);

    result = FMOD_ChannelGroup_AddGroup(gMasterGroup, gGroupA);
    CHECK_RESULT(result);

    result = FMOD_ChannelGroup_AddGroup(gMasterGroup, gGroupB);
    CHECK_RESULT(result);

    /*
        Start all the sounds!
    */
    for (count = 0; count < 5; count++)
    {
        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[count], 1, &gChannel[count]);
        CHECK_RESULT(result);
        if (count < 2)
        {
            result = FMOD_Channel_SetChannelGroup(gChannel[count], gGroupA);
        }
        else
        {
            result = FMOD_Channel_SetChannelGroup(gChannel[count], gGroupB);
        }
        CHECK_RESULT(result);
        result = FMOD_Channel_SetPaused(gChannel[count], 0);
        CHECK_RESULT(result);
    }

    /*
        Create the DSP effects we want to apply to our submixes.
    */
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_HIGHPASS, &gDSPHighPass);
    CHECK_RESULT(result);

    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_CHORUS, &gDSPChorus);
    CHECK_RESULT(result);

    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_LOWPASS, &gDSPLowpass);
    CHECK_RESULT(result);
    result = FMOD_DSP_SetParameter(gDSPLowpass, FMOD_DSP_LOWPASS_CUTOFF, 500.0f);
    CHECK_RESULT(result);
}

void Java_org_fmod_submixing_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_submixing_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	unsigned int i = 0;

	for (i = 0; i < NUM_SOUNDS; i++)
	{
		result = FMOD_Sound_Release(gSound[i]);
		CHECK_RESULT(result);
	}

    result = FMOD_DSP_Release(gDSPHighPass);
    CHECK_RESULT(result);
    result = FMOD_DSP_Release(gDSPChorus);
    CHECK_RESULT(result);
    result = FMOD_DSP_Release(gDSPLowpass);
    CHECK_RESULT(result);

    result = FMOD_ChannelGroup_Release(gGroupA);
    CHECK_RESULT(result);
    result = FMOD_ChannelGroup_Release(gGroupB);
    CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

jint Java_org_fmod_submixing_Example_cGetChannelsPlaying(JNIEnv *env, jobject thiz)
{
	int channelsplaying = 0;

	FMOD_System_GetChannelsPlaying(gSystem, &channelsplaying);

	return channelsplaying;
}

void Java_org_fmod_submixing_Example_cToggleMuteGroupA(JNIEnv *env, jobject thiz)
{
	int mute = 0;

	FMOD_ChannelGroup_GetMute(gGroupA, &mute);
	FMOD_ChannelGroup_SetMute(gGroupA, !mute);
}

void Java_org_fmod_submixing_Example_cToggleMuteGroupB(JNIEnv *env, jobject thiz)
{
	int mute = 0;

	FMOD_ChannelGroup_GetMute(gGroupB, &mute);
	FMOD_ChannelGroup_SetMute(gGroupB, !mute);
}

void Java_org_fmod_submixing_Example_cToggleHighPassGroupA(JNIEnv *env, jobject thiz)
{
    static int reverb = 1;

    if (reverb)
    {
        FMOD_ChannelGroup_AddDSP(gGroupA, gDSPHighPass, 0);
    }
    else
    {
        FMOD_DSP_Remove(gDSPHighPass);
    }

    reverb = !reverb;
}

void Java_org_fmod_submixing_Example_cToggleChorusGroupB(JNIEnv *env, jobject thiz)
{
    static int flange = 1;

    if (flange)
    {
        FMOD_ChannelGroup_AddDSP(gGroupB, gDSPChorus, 0);
    }
    else
    {
        FMOD_DSP_Remove(gDSPChorus);
    }

    flange = !flange;
}

void Java_org_fmod_submixing_Example_cToggleLowpassMasterGroup(JNIEnv *env, jobject thiz)
{
    static int lowpass = 1;

    if (lowpass)
    {
        FMOD_ChannelGroup_AddDSP(gMasterGroup, gDSPLowpass, 0);
    }
    else
    {
        FMOD_DSP_Remove(gDSPLowpass);
    }

    lowpass = !lowpass;
}
