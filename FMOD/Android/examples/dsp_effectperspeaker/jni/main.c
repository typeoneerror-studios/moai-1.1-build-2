/*===============================================================================================
 DSP Effect per speaker Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to manipulate a DSP network and as an example, creates 2 dsp effects,
 and splits a single sound into 2 audio paths, which it then filters seperately.
 To only have each audio path come out of one speaker each, FMOD_DSPConnection_setLevels is used just
 before the 2 branches merge back together again.

 For more speakers:
 1. Use FMOD_System_SetSpeakerMode or FMOD_System_SetOutputFormat.
 2. Create more effects, currently 2 for stereo (reverb and chorus), create one per speaker.
 3. Under the 'Now connect the 2 effects to channeldsp head.' section, connect the extra effects
    by duplicating the code more times.
 4. Filter each effect to each speaker by calling FMOD_DSP_SetInputLevels.  Expand the existing code
    by extending the level arrays from 2 to the number of speakers you require, and change the
    numlevels parameter in FMOD_DSP_SetInputLevels from 2 to the correct number accordingly.

===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

#define NUM_SOUNDS 3

FMOD_SYSTEM        *gSystem  = 0;
FMOD_CHANNEL       *gChannel = 0;
FMOD_SOUND	       *gSound   = 0;
FMOD_DSP           *gDSPLowpass, *gDSPChorus, *gDSPHead, *gDSPChannelmixer;
FMOD_DSPCONNECTION *gDSPLowpassconnection, *gDSPChorusconnection;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_dspeffectperspeaker_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/drumloop.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &gSound);
	CHECK_RESULT(result);

    result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
    CHECK_RESULT(result);

    /*
        Create the DSP effects.
    */
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_LOWPASS, &gDSPLowpass);
    CHECK_RESULT(result);

    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_CHORUS, &gDSPChorus);
    CHECK_RESULT(result);

    /*
        Connect up the DSP network
    */

    /*
        When a sound is played, a subnetwork is set up in the DSP network which looks like this.
        Wavetable is the drumloop sound, and it feeds its data from right to left.

        [DSPHEAD]<------------[DSPCHANNELMIXER]
    */
    result = FMOD_System_GetDSPHead(gSystem, &gDSPHead);
    CHECK_RESULT(result);

    result = FMOD_DSP_GetInput(gDSPHead, 0, &gDSPChannelmixer, 0);
    CHECK_RESULT(result);

    /*
        Now disconnect channeldsp head from wavetable to look like this.

        [DSPHEAD]             [DSPCHANNELMIXER]
    */
    result = FMOD_DSP_DisconnectFrom(gDSPHead, gDSPChannelmixer);
    CHECK_RESULT(result);

    /*
        Now connect the 2 effects to channeldsp head.
		Store the 2 connections this makes so we can set their speakerlevels later.

                  [DSPLOWPASS]
                 /x
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y
                  [DSPCHORUS]
    */
    result = FMOD_DSP_AddInput(gDSPHead, gDSPLowpass, &gDSPLowpassconnection); /* x = dsplowpassconnection */
    CHECK_RESULT(result);
    result = FMOD_DSP_AddInput(gDSPHead, gDSPChorus, &gDSPChorusconnection);   /* y = dspchorusconnection */
    CHECK_RESULT(result);

    /*
        Now connect the wavetable to the 2 effects

                  [DSPLOWPASS]
                 /x          \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y          /
                  [DSPCHORUS]
    */
    result = FMOD_DSP_AddInput(gDSPLowpass, gDSPChannelmixer, NULL);  /* Null for connection - we dont care about it. */
    CHECK_RESULT(result);
    result = FMOD_DSP_AddInput(gDSPChorus, gDSPChannelmixer, NULL);   /* Null for connection - we dont care about it. */
    CHECK_RESULT(result);

    /*
        Now the drumloop will be twice as loud, because it is being split into 2, then recombined at the end.
        What we really want is to only feed the dspchannelmixer->dsplowpass through the left speaker, and
        dspchannelmixer->dspchorus to the right speaker.
        We can do that simply by setting the pan, or speaker levels of the connections.

                  [DSPLOWPASS]
                 /x=1,0      \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y=0,1      /
                  [DSPCHORUS]
    */
    {
        float leftinputon[2]  = { 1.0f, 0.0f };
        float rightinputon[2] = { 0.0f, 1.0f };
        float inputsoff[2]    = { 0.0f, 0.0f };

        result = FMOD_DSPConnection_SetLevels(gDSPLowpassconnection, FMOD_SPEAKER_FRONT_LEFT, leftinputon, 2);
        CHECK_RESULT(result);
        result = FMOD_DSPConnection_SetLevels(gDSPLowpassconnection, FMOD_SPEAKER_FRONT_RIGHT, inputsoff, 2);
        CHECK_RESULT(result);

        result = FMOD_DSPConnection_SetLevels(gDSPChorusconnection, FMOD_SPEAKER_FRONT_LEFT, inputsoff, 2);
        CHECK_RESULT(result);
        result = FMOD_DSPConnection_SetLevels(gDSPChorusconnection, FMOD_SPEAKER_FRONT_RIGHT, rightinputon, 2);
        CHECK_RESULT(result);
    }

    FMOD_DSP_SetBypass(gDSPLowpass, 1);
    FMOD_DSP_SetBypass(gDSPChorus, 1);

    FMOD_DSP_SetActive(gDSPLowpass, 1);
    FMOD_DSP_SetActive(gDSPChorus, 1);
}

void Java_org_fmod_dspeffectperspeaker_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_dspeffectperspeaker_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_Sound_Release(gSound);
    CHECK_RESULT(result);

    result = FMOD_DSP_Release(gDSPLowpass);
    CHECK_RESULT(result);
    result = FMOD_DSP_Release(gDSPChorus);
    CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

jint Java_org_fmod_dspeffectperspeaker_Example_cGetChannelsPlaying(JNIEnv *env, jobject thiz)
{
    int  channelsplaying = 0;

    FMOD_System_GetChannelsPlaying(gSystem, &channelsplaying);

    return channelsplaying;
}

void Java_org_fmod_dspeffectperspeaker_Example_cToggleLowpass(JNIEnv *env, jobject thiz)
{
    static int lowpass = 0;

    FMOD_DSP_SetBypass(gDSPLowpass, lowpass);

    lowpass = !lowpass;
}

void Java_org_fmod_dspeffectperspeaker_Example_cToggleChorus(JNIEnv *env, jobject thiz)
{
    static int chorus = 0;

    FMOD_DSP_SetBypass(gDSPChorus, chorus);

    chorus = !chorus;
}

void Java_org_fmod_dspeffectperspeaker_Example_cSetPan(JNIEnv *env, jobject thiz, float pan)
{
	FMOD_Channel_SetPan(gChannel, pan);
}

