/*===============================================================================================
 ReadTags Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to read tags from sound files
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
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

void Java_org_fmod_readtags_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int numtags, numtagsupdated, count;
	FMOD_TAG tag;
	char s[256];

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

    /*
        Open the specified file. Use FMOD_CREATESTREAM and FMOD_OPENONLY so it opens quickly
    */
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/wave.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_OPENONLY, 0, &gSound);
	CHECK_RESULT(result);

    /*
        Read and display all tags associated with this file
    */
    for (;;)
    {
        /*
            An index of -1 means "get the first tag that's new or updated".
            If no tags are new or updated then getTag will return FMOD_ERR_TAGNOTFOUND.
            This is the first time we've read any tags so they'll all be new but after we've read them,
            they won't be new any more.
        */
        if (FMOD_Sound_GetTag(gSound, 0, -1, &tag) != FMOD_OK)
        {
            break;
        }
        if (tag.datatype == FMOD_TAGDATATYPE_STRING)
        {
        	sprintf(s, "%s = %s (%d bytes)\n", tag.name, tag.data, tag.datalen);
            __android_log_write(ANDROID_LOG_INFO, "fmod_readtags", s);
        }
        else
        {
        	sprintf(s, "%s = <binary> (%d bytes)\n", tag.name, tag.datalen);
        	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", s);
        }
    }
    __android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "");

    /*
        Read all the tags regardless of whether they're updated or not. Also show the tag type.
    */
    result = FMOD_Sound_GetNumTags(gSound, &numtags, &numtagsupdated);
    CHECK_RESULT(result);
    for (count=0; count < numtags; count++)
    {
        result = FMOD_Sound_GetTag(gSound, 0, count, &tag);
        CHECK_RESULT(result);

        switch (tag.type)
        {
            case FMOD_TAGTYPE_UNKNOWN :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_UNKNOWN  ");
                break;

            case FMOD_TAGTYPE_ID3V1 :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_ID3V1  ");
                break;

            case FMOD_TAGTYPE_ID3V2 :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_ID3V2  ");
                break;

            case FMOD_TAGTYPE_VORBISCOMMENT :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_VORBISCOMMENT  ");
                break;

            case FMOD_TAGTYPE_SHOUTCAST :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_SHOUTCAST  ");
                break;

            case FMOD_TAGTYPE_ICECAST :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_ICECAST  ");
                break;

            case FMOD_TAGTYPE_ASF :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_ASF  ");
                break;

            case FMOD_TAGTYPE_FMOD :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_FMOD  ");
                break;

            case FMOD_TAGTYPE_USER :
            	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", "FMOD_TAGTYPE_USER  ");
                break;
        }

        if (tag.datatype == FMOD_TAGDATATYPE_STRING)
        {
        	sprintf(s, "%s = %s (%d bytes)\n", tag.name, tag.data, tag.datalen);
        	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", s);
        }
        else
        {
        	sprintf(s, "%s = ??? (%d bytes)\n", tag.name, tag.datalen);
        	__android_log_write(ANDROID_LOG_INFO, "fmod_readtags", s);
        }
    }
}

void Java_org_fmod_readtags_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_readtags_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

jstring Java_org_fmod_readtags_Example_cGetArtist(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_TAG tag;

    result = FMOD_Sound_GetTag(gSound, "ARTIST", 0, &tag);
    CHECK_RESULT(result);

    return((*env)->NewStringUTF(env, tag.data));
}

jstring Java_org_fmod_readtags_Example_cGetTitle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_TAG tag;

    result = FMOD_Sound_GetTag(gSound, "TITLE", 0, &tag);
    CHECK_RESULT(result);

    return((*env)->NewStringUTF(env, tag.data));
}
