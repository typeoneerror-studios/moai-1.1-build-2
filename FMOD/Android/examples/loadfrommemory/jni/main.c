/*===============================================================================================
 Load from memory example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example is simply a variant of the play sound example, but it loads the data into memory
 then uses the 'load from memory' feature of System::createSound.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include "fmod.h"
#include "fmod_errors.h"

#define NUM_SOUNDS 3

FMOD_SYSTEM  *gSystem  = 0;
FMOD_CHANNEL *gChannel = 0;
FMOD_SOUND	 *gSound[NUM_SOUNDS];

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void LoadFileIntoMemory(const char *name, void **buff, int *length)
{
    FILE *fp = fopen(name, "rb");

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buff = malloc(*length);
    fread(*buff, *length, 1, fp);

    fclose(fp);
}

void Java_org_fmod_loadfrommemory_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
    void             *buff = 0;
    int               length = 0;
    FMOD_CREATESOUNDEXINFO exinfo;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

    LoadFileIntoMemory("/sdcard/fmod/drumloop.wav", &buff, &length);
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = length;

	result = FMOD_System_CreateSound(gSystem, (const char *)buff, FMOD_OPENMEMORY | FMOD_LOOP_OFF, &exinfo, &gSound[0]);
	CHECK_RESULT(result);

	free(buff); // don't need the original memory any more.  Note!  If loading as a stream, the memory must stay active so do not free it!

    LoadFileIntoMemory("/sdcard/fmod/jaguar.wav", &buff, &length);
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = length;

	result = FMOD_System_CreateSound(gSystem, (const char *)buff, FMOD_OPENMEMORY, &exinfo, &gSound[1]);
	CHECK_RESULT(result);

	free(buff); // don't need the original memory any more.  Note!  If loading as a stream, the memory must stay active so do not free it!

    LoadFileIntoMemory("/sdcard/fmod/swish.wav", &buff, &length);
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = length;

	result = FMOD_System_CreateSound(gSystem, (const char *)buff, FMOD_OPENMEMORY, &exinfo, &gSound[2]);
	CHECK_RESULT(result);

	free(buff); // don't need the original memory any more.  Note!  If loading as a stream, the memory must stay active so do not free it!

}

void Java_org_fmod_loadfrommemory_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_loadfrommemory_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	unsigned int i = 0;

	for (i = 0; i < NUM_SOUNDS; i++)
	{
		result = FMOD_Sound_Release(gSound[i]);
		CHECK_RESULT(result);
	}

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_loadfrommemory_Example_cPlaySound(JNIEnv *env, jobject thiz, int id)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[id], 0, &gChannel);
	CHECK_RESULT(result);
}

jint Java_org_fmod_loadfrommemory_Example_cGetChannelsPlaying(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int chans = 0;

	result = FMOD_System_GetChannelsPlaying(gSystem, &chans);
	CHECK_RESULT(result);

	return chans;
}

jboolean Java_org_fmod_loadfrommemory_Example_cGetPlaying(JNIEnv *env, jobject thiz)
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

jint Java_org_fmod_loadfrommemory_Example_cGetPosition(JNIEnv *env, jobject thiz)
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

jint Java_org_fmod_loadfrommemory_Example_cGetLength(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_SOUND *sound = 0;
	int length = 0;

	if (gChannel)
	{
		result = FMOD_Channel_GetCurrentSound(gChannel, &sound);
		if (result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
		{
			CHECK_RESULT(result);
		}
	}

	if (sound)
	{
		result = FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_MS);
		CHECK_RESULT(result);
	}

	return length;
}


