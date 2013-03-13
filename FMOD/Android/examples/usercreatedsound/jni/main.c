/*===============================================================================================
 User Created Sound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2010-2011.

 This example shows how create a sound with data filled by the user.
 It shows a user created static sample, followed by a user created stream.
 The former allocates all memory needed for the sound and is played back as a static sample,
 while the latter streams the data in chunks as it plays, using far less memory.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <math.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM *gSystem  = 0;
FMOD_CHANNEL *gChannel = 0;
FMOD_SOUND *gSound = 0;
FMOD_CREATESOUNDEXINFO gExInfo;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

FMOD_RESULT pcmreadcallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    static float  t1 = 0, t2 = 0;        // time
    static float  v1 = 0, v2 = 0;        // velocity
    signed short *stereo16bitbuffer = (signed short *)data;

    for (count = 0; count < (datalen >> 2); count++)        // >>2 = 16bit stereo (4 bytes per sample)
    {
        *stereo16bitbuffer++ = (signed short)(sin(t1) * 32767.0f);    // left channel
        *stereo16bitbuffer++ = (signed short)(sin(t2) * 32767.0f);    // right channel

        t1 += 0.01f   + v1;
        t2 += 0.0142f + v2;
        v1 += (float)(sin(t1) * 0.002f);
        v2 += (float)(sin(t2) * 0.002f);
    }

    return FMOD_OK;
}

FMOD_RESULT pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    /*
        This is useful if the user calls Channel::setPosition and you want to seek your data accordingly
    */
    return FMOD_OK;
}

void Java_org_fmod_usercreatedsound_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	memset(&gExInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	gExInfo.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);              /* Required. */
	gExInfo.decodebuffersize  = 44100;                                       /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
	gExInfo.length            = 44100 * 2 * sizeof(signed short) * 5;        /* Length of PCM data in bytes of whole song (for Sound::getLength) */
	gExInfo.numchannels       = 2;                                           /* Number of channels in the sound. */
	gExInfo.defaultfrequency  = 44100;                                       /* Default playback rate of sound. */
	gExInfo.format            = FMOD_SOUND_FORMAT_PCM16;                     /* Data format of sound. */
	gExInfo.pcmreadcallback   = pcmreadcallback;                             /* User callback for reading. */
	gExInfo.pcmsetposcallback = pcmsetposcallback;                           /* User callback for seeking. */
}

void Java_org_fmod_usercreatedsound_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_usercreatedsound_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	if (gSound != 0)
	{
		result = FMOD_Sound_Release(gSound);
		CHECK_RESULT(result);
	}

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_usercreatedsound_Example_cPlaySample(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	if (gSound != 0)
	{
		result = FMOD_Channel_Stop(gChannel);
		CHECK_RESULT(result);

		FMOD_Sound_Release(gSound);
		gSound = 0;
	}

    result = FMOD_System_CreateSound(gSystem, 0, FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL, &gExInfo, &gSound);
    CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);
}

void Java_org_fmod_usercreatedsound_Example_cPlayStream(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	if (gSound != 0)
	{
		result = FMOD_Channel_Stop(gChannel);
		CHECK_RESULT(result);

		FMOD_Sound_Release(gSound);
		gSound = 0;
	}

    result = FMOD_System_CreateStream(gSystem, 0, FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL, &gExInfo, &gSound);
    CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);
}

void Java_org_fmod_usercreatedsound_Example_cTogglePause(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	if (gChannel)
	{
		result = FMOD_Channel_GetPaused(gChannel, &paused);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}

		result = FMOD_Channel_SetPaused(gChannel, !paused);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}
	}
}

jboolean Java_org_fmod_usercreatedsound_Example_cGetPlaying(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL playing = 0;

	if (gChannel)
	{
		result = FMOD_Channel_IsPlaying(gChannel, &playing);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}
	}

	return playing;
}

jboolean Java_org_fmod_usercreatedsound_Example_cGetPaused(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	if (gChannel)
	{
		result = FMOD_Channel_GetPaused(gChannel, &paused);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}
	}

	return paused;
}

jint Java_org_fmod_usercreatedsound_Example_cGetPosition(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int position = 0;

	if (gChannel)
	{
		result = FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}
	}

	return position;
}

jint Java_org_fmod_usercreatedsound_Example_cGetLength(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int length = 0;

	if (gSound)
	{
		result = FMOD_Sound_GetLength(gSound, &length, FMOD_TIMEUNIT_MS);
		CHECK_RESULT(result);
	}

	return length;
}


