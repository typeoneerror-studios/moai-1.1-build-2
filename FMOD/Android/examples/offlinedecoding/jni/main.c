/*===============================================================================================
 Offline Decoding Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how decode a file to PCM, without playing it.
 It writes out the data as a raw data file.
 The FMOD_System_CreateSound function uses FMOD_OPENONLY so that FMOD does not read any data
 itself.
 If this is uses then it is up to the user to use FMOD_Sound_ReadData to get the data out of
 the file and into the destination buffer.
===============================================================================================*/
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem  = 0;
FMOD_SOUND	 *gSound  = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_offlinedecoding_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 1, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);
}

void Java_org_fmod_offlinedecoding_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_offlinedecoding_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_offlinedecoding_Example_cDecode(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result;

    /*
        Decode the sound and write it to a .raw file.
    */

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/wave.mp3", FMOD_OPENONLY | FMOD_ACCURATETIME, 0, &gSound);
	CHECK_RESULT(result);

    __android_log_write(ANDROID_LOG_INFO, "fmod_offlinedecoding", "decoding...\n");
    {
        void *data;
        unsigned int length = 0, read;
        unsigned int bytesread;
        FILE *outfp;

        #define CHUNKSIZE 4096

        result = FMOD_Sound_GetLength(gSound, &length, FMOD_TIMEUNIT_PCMBYTES);
        CHECK_RESULT(result);

        outfp = fopen("/sdcard/fmod/output.raw", "wb");
        if (!outfp)
        {
        	__android_log_write(ANDROID_LOG_INFO, "fmod_offlinedecoding","Error!  Could not open output.raw output file.\n");
            exit(-1);
        }

        data = malloc(CHUNKSIZE);
        if (!data)
        {
        	__android_log_write(ANDROID_LOG_INFO, "fmod_offlinedecoding", "Error!  Failed to allocate memory.\n");
            exit(-1);
        }

        bytesread = 0;
        do
        {
        	char s[64];

            result = FMOD_Sound_ReadData(gSound, (char *)data, CHUNKSIZE, &read);

            fwrite((char *)data, read, 1, outfp);

            bytesread += read;

            sprintf(s, "writing %d bytes of %d to output.raw\r", bytesread, length);

            __android_log_write(ANDROID_LOG_INFO, "fmod_offlinedecoding", s);
        }
        while (result == FMOD_OK && read == CHUNKSIZE);

        /*
            Loop terminates when either
            1. the read function returns an error.  (ie FMOD_ERR_FILE_EOF etc).
            2. the amount requested was different to the amount returned. (somehow got an EOF without the file error, maybe a non stream file format like mod/s3m/xm/it/midi).

            If 'bytesread' is bigger than 'length' then it just means that FMOD miscalculated the size,
            but this will not usually happen if FMOD_ACCURATETIME is used.  (this will give the correct length for VBR formats)
        */

        if (outfp)
        {
            fclose(outfp);
        }
    }

    __android_log_write(ANDROID_LOG_INFO, "fmod_offlinedecoding", "done.\n");

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);
}



