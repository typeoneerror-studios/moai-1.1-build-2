/*===============================================================================================
 ChannelGroups Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to put channels into channel groups, so that you can affect a group
 of channels at a time instead of just one channel at a time.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  		*gSystem  = 0;
FMOD_SOUND	 		*gSound[6];
FMOD_CHANNEL 		*gChannel[6];
FMOD_CHANNELGROUP 	*gGroupA, *gGroupB, *gMasterGroup;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_channelgroups_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int count = 0;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/drumloop.wav", FMOD_LOOP_NORMAL, 0, &gSound[0]);
	CHECK_RESULT(result);
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/jaguar.wav", FMOD_LOOP_NORMAL, 0, &gSound[1]);
	CHECK_RESULT(result);
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/swish.wav", FMOD_LOOP_NORMAL, 0, &gSound[2]);
	CHECK_RESULT(result);
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/c.ogg", FMOD_LOOP_NORMAL, 0, &gSound[3]);
	CHECK_RESULT(result);
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/d.ogg", FMOD_LOOP_NORMAL, 0, &gSound[4]);
	CHECK_RESULT(result);
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/e.ogg", FMOD_LOOP_NORMAL, 0, &gSound[5]);
	CHECK_RESULT(result);

	result = FMOD_System_CreateChannelGroup(gSystem, "Group A", &gGroupA);
	CHECK_RESULT(result);

	result = FMOD_System_CreateChannelGroup(gSystem, "Group B", &gGroupB);
	CHECK_RESULT(result);

	result = FMOD_System_GetMasterChannelGroup(gSystem, &gMasterGroup);
	CHECK_RESULT(result);

    /*
        Instead of being independent, set the group A and B to be children of the master group.
    */
    result = FMOD_ChannelGroup_AddGroup(gMasterGroup, gGroupA);
    CHECK_RESULT(result);

    result = FMOD_ChannelGroup_AddGroup(gMasterGroup, gGroupB);
    CHECK_RESULT(result);

    /*
        Start all the sounds!
    */
    for (count = 0; count < 6; count++)
    {
        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[count], 1, &gChannel[count]);
        CHECK_RESULT(result);
        if (count < 3)
        {
            result = FMOD_Channel_SetChannelGroup(gChannel[count], gGroupA);
        }
        else
        {
            result = FMOD_Channel_SetChannelGroup(gChannel[count], gGroupB);
        }
        CHECK_RESULT(result);
        result = FMOD_Channel_SetPaused(gChannel[count], 0);
        CHECK_RESULT(result);
    }

    /*
        Change the volume of each group, just because we can!  (And makes it less of a loud noise).
    */
    result = FMOD_ChannelGroup_SetVolume(gGroupA, 0.5f);
    CHECK_RESULT(result);
    result = FMOD_ChannelGroup_SetVolume(gGroupB, 0.5f);
    CHECK_RESULT(result);
}

void Java_org_fmod_channelgroups_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_channelgroups_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int count = 0;

    /*
        Shut down
    */
    for (count = 0; count < 6; count++)
    {
        result = FMOD_Sound_Release(gSound[count]);
        CHECK_RESULT(result);
    }

    result = FMOD_ChannelGroup_Release(gGroupA);
    CHECK_RESULT(result);
    result = FMOD_ChannelGroup_Release(gGroupB);
    CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_channelgroups_Example_cMuteGroupA(JNIEnv *env, jobject thiz)
{
    static int mute = 1;

    FMOD_ChannelGroup_SetMute(gGroupA, mute);

    mute = !mute;
}

void Java_org_fmod_channelgroups_Example_cMuteGroupB(JNIEnv *env, jobject thiz)
{
    static int mute = 1;

    FMOD_ChannelGroup_SetMute(gGroupB, mute);

    mute = !mute;

}

void Java_org_fmod_channelgroups_Example_cMuteMasterGroup(JNIEnv *env, jobject thiz)
{
    static int mute = 1;

    FMOD_ChannelGroup_SetMute(gMasterGroup, mute);

    mute = !mute;
}

jint Java_org_fmod_channelgroups_Example_cGetChannelsPlaying(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int chans = 0;

	result = FMOD_System_GetChannelsPlaying(gSystem, &chans);
	CHECK_RESULT(result);

	return chans;
}



