/*===============================================================================================
 PlayStream Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to simply play a stream, such as an mp3 or wav.
 The stream behaviour is achieved by specifying FMOD_CREATESTREAM in the call to
 System::createSound, or by calling System::createStream.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem  = 0;
FMOD_SOUND	 *gSound  = 0;
FMOD_CHANNEL *gChannel = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_playstream_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/wave.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &gSound);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);
}

void Java_org_fmod_playstream_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_playstream_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_playstream_Example_cPause(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel, !paused);
	CHECK_RESULT(result);
}

jboolean Java_org_fmod_playstream_Example_cGetPaused(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	return paused;
}

jint Java_org_fmod_playstream_Example_cGetPosition(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int position = 0;

	result = FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);
	CHECK_RESULT(result);

	return position;
}

jint Java_org_fmod_playstream_Example_cGetLength(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int length = 0;

	result = FMOD_Sound_GetLength(gSound, &length, FMOD_TIMEUNIT_MS);
	CHECK_RESULT(result);

	return length;
}


