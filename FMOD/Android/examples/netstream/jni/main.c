/*===============================================================================================
 NetStream Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to play streaming audio from the internet
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

void Java_org_fmod_netstream_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 1, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

    /*
        Bump up the file buffer size a little bit for netstreams (to account for lag).
    */
    result = FMOD_System_SetStreamBufferSize(gSystem, 64*1024, FMOD_TIMEUNIT_RAWBYTES);
    CHECK_RESULT(result);
}

void Java_org_fmod_netstream_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);

	if (!gChannel)
	{
	    FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	}
}

void Java_org_fmod_netstream_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_netstream_Example_cPlayNetStream(JNIEnv *env, jobject thiz, jstring urlstring)
{
	FMOD_RESULT result = FMOD_OK;

	char *url = (char *)(*env)->GetStringUTFChars(env, urlstring, 0);

	FMOD_Sound_Release(gSound);
	gSound = 0;

	FMOD_Channel_Stop(gChannel);
	gChannel = 0;

    result = FMOD_System_CreateSound(gSystem, url, FMOD_2D | FMOD_CREATESTREAM | FMOD_NONBLOCKING,  0, &gSound);
    CHECK_RESULT(result);
}

jint Java_org_fmod_netstream_Example_cGetOpenState(JNIEnv *env, jobject thiz)
{
	FMOD_OPENSTATE openstate = 0;

	FMOD_Sound_GetOpenState(gSound, &openstate, 0, 0, 0);

	return openstate;
}

jint Java_org_fmod_netstream_Example_cGetBuffered(JNIEnv *env, jobject thiz)
{
	int percent = 0;

	FMOD_Sound_GetOpenState(gSound, 0, &percent, 0, 0);

	return percent;
}

jboolean Java_org_fmod_netstream_Example_cGetStarving(JNIEnv *env, jobject thiz)
{
	int starving;

	FMOD_Sound_GetOpenState(gSound, 0, 0, &starving, 0);

	return starving;
}


jint Java_org_fmod_netstream_Example_cGetPosition(JNIEnv *env, jobject thiz)
{
	int position = 0;

	FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);

	return position;
}

jboolean Java_org_fmod_netstream_Example_cGetIsPlaying(JNIEnv *env, jobject thiz)
{
	int playing = 0;

	FMOD_Channel_IsPlaying(gChannel, &playing);

	return playing;
}


