/*===============================================================================================
 File Callbacks Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example is a modified version of the playstream example.
 It shows the correct way declare and handle fmod file callbacks with System::setFileSystem.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
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

/*
    TIPS:

    1. use F_CALLBACK.  Do NOT force cast your own function to fmod's callback type.
    2. return FMOD_ERR_FILE_NOTFOUND in open as required.
    3. return number of bytes read in read callback.  Do not get the size and count
       around the wrong way in fread for example, this would return 1 instead of the number of bytes read.

    QUESTIONS:

    1. Why does fmod seek to the end and read?  Because it is looking for ID3V1 tags.
       Use FMOD_IGNORETAGS in FMOD_System_CreateSound / FMOD_System_CreateStream if you don't like this behaviour.
*/

FMOD_RESULT F_CALLBACK myopen(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
{
    if (name)
    {
        FILE *fp;

        fp = fopen(name, "rb");
        if (!fp)
        {
            return FMOD_ERR_FILE_NOTFOUND;
        }

        fseek(fp, 0, SEEK_END);
        *filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *userdata = (void *)0x12345678;
        *handle = fp;
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myclose(void *handle, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    fclose((FILE *)handle);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myread(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (bytesread)
    {
        *bytesread = (int)fread(buffer, 1, sizebytes, (FILE *)handle);

        if (*bytesread < sizebytes)
        {
            return FMOD_ERR_FILE_EOF;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myseek(void *handle, unsigned int pos, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    fseek((FILE *)handle, pos, SEEK_SET);

    return FMOD_OK;
}

void Java_org_fmod_filecallbacks_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

    result = FMOD_System_SetFileSystem(gSystem, myopen, myclose, myread, myseek, 0, 0, 2048);
    CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/wave.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &gSound);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);
}

void Java_org_fmod_filecallbacks_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_filecallbacks_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_filecallbacks_Example_cPause(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel, !paused);
	CHECK_RESULT(result);
}

jboolean Java_org_fmod_filecallbacks_Example_cGetPaused(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	return paused;
}

jint Java_org_fmod_filecallbacks_Example_cGetPosition(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int position = 0;

	result = FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);
	CHECK_RESULT(result);

	return position;
}

jint Java_org_fmod_filecallbacks_Example_cGetLength(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int length = 0;

	result = FMOD_Sound_GetLength(gSound, &length, FMOD_TIMEUNIT_MS);
	CHECK_RESULT(result);

	return length;
}
