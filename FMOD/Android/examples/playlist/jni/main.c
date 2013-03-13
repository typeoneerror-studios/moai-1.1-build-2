/*===============================================================================================
 PlayList Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to load a playlist and play the sounds in a playlist.
===============================================================================================*/
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem  = 0;
FMOD_SOUND	 *gPlayList  = 0;
FMOD_SOUND	 *gSound  = 0;
FMOD_CHANNEL *gChannel = 0;
int			  gCount = 0;
FMOD_TAG      gTag;
FMOD_BOOL     gIsPlayList = 0;
char		  gTitle[256];

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_playlist_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
    FMOD_SOUND_TYPE   soundtype;
    char              file[128];
    char             *title = NULL;

    gCount = 0;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/playlist.m3u", FMOD_DEFAULT, 0, &gPlayList);
	CHECK_RESULT(result);

    result = FMOD_Sound_GetFormat(gPlayList, &soundtype, 0, 0, 0);
    CHECK_RESULT(result);

    gIsPlayList = (soundtype == FMOD_SOUND_TYPE_PLAYLIST);

    if (gIsPlayList)
    {
    	__android_log_write(ANDROID_LOG_INFO, "fmod_playlist", "PLAYLIST loaded.\n");
        /*
            Get the first song in the playlist, create the sound and then play it.
        */
        result = FMOD_Sound_GetTag(gPlayList, "FILE", gCount, &gTag);
        CHECK_RESULT(result);

        sprintf(file, "/sdcard/fmod/%s", (char *)gTag.data);

        result = FMOD_System_CreateSound(gSystem, file, FMOD_DEFAULT | FMOD_LOOP_OFF, 0, &gSound);
        CHECK_RESULT(result);

        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
        CHECK_RESULT(result);

        FMOD_Sound_GetTag(gPlayList, "TITLE", gCount, &gTag);
        strncpy(gTitle, gTag.data, gTag.datalen);

        gCount++;
    }
    else
    {
    	__android_log_write(ANDROID_LOG_INFO, "fmod_playlist", "SOUND loaded.\n");

        /*
            This is just a normal sound, so just play it.
        */
        gSound = gPlayList;

        result = FMOD_Sound_SetMode(gSound, FMOD_LOOP_NORMAL);
        CHECK_RESULT(result);

        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_playlist_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;
    char file[128];

    if (gChannel && gIsPlayList)
    {
    	FMOD_BOOL isplaying = 0;

        /*
            When sound has finished playing, play the next sound in the playlist
        */
        FMOD_Channel_IsPlaying(gChannel, &isplaying);
        if (!isplaying)
        {
            if (gSound)
            {
                FMOD_Sound_Release(gSound);

                gSound = NULL;
            }

            result = FMOD_Sound_GetTag(gPlayList, "FILE", gCount, &gTag);
            if (result != FMOD_OK)
            {
            	gCount = 0;
            }
            else
            {
            	 __android_log_write(ANDROID_LOG_INFO, "fmod_playlist", "playing next song in playlist...\n");

                sprintf(file, "/sdcard/fmod/%s", (char *)gTag.data);

                result = FMOD_System_CreateSound(gSystem, file, FMOD_DEFAULT | FMOD_LOOP_OFF, 0, &gSound);
                CHECK_RESULT(result);

                result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
                CHECK_RESULT(result);

                FMOD_Sound_GetTag(gPlayList, "TITLE", gCount, &gTag);
                strncpy(gTitle, gTag.data, gTag.datalen);

                gCount++;
            }
        }
    }

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_playlist_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_Sound_Release(gPlayList);
    CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_playlist_Example_cPause(JNIEnv *env, jobject thiz)
{
	FMOD_BOOL paused = 0;

	FMOD_Channel_GetPaused(gChannel, &paused);
	FMOD_Channel_SetPaused(gChannel, !paused);

}

jboolean Java_org_fmod_playlist_Example_cGetPaused(JNIEnv *env, jobject thiz)
{
	FMOD_BOOL paused = 0;

	FMOD_Channel_GetPaused(gChannel, &paused);

	return paused;
}

jint Java_org_fmod_playlist_Example_cGetPosition(JNIEnv *env, jobject thiz)
{
	int position = 0;

	FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);

	return position;
}

jint Java_org_fmod_playlist_Example_cGetLength(JNIEnv *env, jobject thiz)
{
	int length = 0;

	FMOD_Sound_GetLength(gSound, &length, FMOD_TIMEUNIT_MS);

	return length;
}

jstring Java_org_fmod_playlist_Example_cGetTitle(JNIEnv *env, jobject thiz)
{
	return((*env)->NewStringUTF(env, gTitle));
}

void Java_org_fmod_playlist_Example_cNextSound(JNIEnv *env, jobject thiz)
{
    /*
        Play the next song in the playlist
    */
    if (gChannel && gIsPlayList)
    {
        FMOD_Channel_Stop(gChannel);
    }
}
