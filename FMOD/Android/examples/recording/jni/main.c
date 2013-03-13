/*===============================================================================================
 Record example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.

 This example shows how to record a sound.
 It then shows how to play a sound while it is being recorded to.  Because it is recording, the
 sound playback has to be delayed a little bit so that the playback doesn't play part of the
 buffer that is still being written to.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem   = 0;
FMOD_CHANNEL *gChannel  = 0;
FMOD_SOUND	 *gSound    = 0;
FMOD_BOOL     gLoopFlag = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_recording_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	FMOD_CREATESOUNDEXINFO exinfo;
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

	exinfo.cbsize 			= sizeof(exinfo);
	exinfo.numchannels      = 1;
	exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
    exinfo.defaultfrequency = 44100;
    exinfo.length           = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 5;

	result = FMOD_System_CreateSound(gSystem, NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &gSound);
	CHECK_RESULT(result);
}

void Java_org_fmod_recording_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_recording_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_recording_Example_cRecordSound(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_RecordStart(gSystem, 0, gSound, gLoopFlag);
	CHECK_RESULT(result);
}

void Java_org_fmod_recording_Example_cPlaySound(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_SetMode(gSound, gLoopFlag ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);
}

void Java_org_fmod_recording_Example_cToggleLoop(JNIEnv *env, jobject thiz)
{
	gLoopFlag = !gLoopFlag;
}

void Java_org_fmod_recording_Example_cStopAll(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_RecordStop(gSystem, 0);
	CHECK_RESULT(result);

	if (gChannel)
	{
		FMOD_Channel_Stop(gChannel);
		gChannel = 0;
	}
}

jboolean Java_org_fmod_recording_Example_cGetRecording(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL recording = 0;

	result = FMOD_System_IsRecording(gSystem, 0, &recording);
	CHECK_RESULT(result);

	return recording;
}

jboolean Java_org_fmod_recording_Example_cGetPlaying(JNIEnv *env, jobject thiz)
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

jboolean Java_org_fmod_recording_Example_cGetLooping(JNIEnv *env, jobject thiz)
{
	return gLoopFlag;
}

jint Java_org_fmod_recording_Example_cGetRecordPosition(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int position = 0;

	result = FMOD_System_GetRecordPosition(gSystem, 0, &position);
	CHECK_RESULT(result);

	return position;
}

jint Java_org_fmod_recording_Example_cGetPlayPosition(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int position = 0;

	if (gChannel)
	{
		result = FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_PCM);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}
	}

	return position;
}
