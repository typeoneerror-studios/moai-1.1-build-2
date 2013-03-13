/*===============================================================================================
 soundgroups Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to use the sound group behavior modes
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

#define NUM_SOUNDS 3

FMOD_SYSTEM  *gSystem  = 0;
FMOD_CHANNEL *gChannel[NUM_SOUNDS];
FMOD_SOUND	 *gSound[NUM_SOUNDS];
FMOD_SOUNDGROUP *gSoundGroup = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_soundgroups_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

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

    /*
        Create the sound group with the following attributes:
          Name       = MyGroup
          MaxAudible = 1
          Behavior   = Mute
    */
	result = FMOD_System_CreateSoundGroup(gSystem, "MyGroup", &gSoundGroup);
	CHECK_RESULT(result);

    result = FMOD_SoundGroup_SetMaxAudible(gSoundGroup, 1);
    CHECK_RESULT(result);

    result = FMOD_SoundGroup_SetMaxAudibleBehavior(gSoundGroup, FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
    CHECK_RESULT(result);

    result = FMOD_SoundGroup_SetMuteFadeSpeed(gSoundGroup, 2);
    CHECK_RESULT(result);

    /*
        Put the sounds in the sound group
    */
	result = FMOD_Sound_SetSoundGroup(gSound[0], gSoundGroup);
	CHECK_RESULT(result);

	result = FMOD_Sound_SetSoundGroup(gSound[1], gSoundGroup);
	CHECK_RESULT(result);

	result = FMOD_Sound_SetSoundGroup(gSound[2], gSoundGroup);
	CHECK_RESULT(result);

	/*
        Play the sounds (two will be muted because of the behavior mode)
    */
	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[0], 0, &gChannel[0]);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[1], 0, &gChannel[1]);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[2], 0, &gChannel[2]);
	CHECK_RESULT(result);

}

void Java_org_fmod_soundgroups_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_soundgroups_Example_cEnd(JNIEnv *env, jobject thiz)
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

void Java_org_fmod_soundgroups_Example_cSetBehaviourFail(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_SoundGroup_SetMaxAudibleBehavior(gSoundGroup, FMOD_SOUNDGROUP_BEHAVIOR_FAIL);
    CHECK_RESULT(result);
}

void Java_org_fmod_soundgroups_Example_cSetBehaviourMute(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_SoundGroup_SetMaxAudibleBehavior(gSoundGroup, FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
    CHECK_RESULT(result);
}

void Java_org_fmod_soundgroups_Example_cSetBehaviourSteal(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_SoundGroup_SetMaxAudibleBehavior(gSoundGroup, FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST);
    CHECK_RESULT(result);
}

void Java_org_fmod_soundgroups_Example_cPlayDrumloop(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int index = 0;

    FMOD_Channel_GetIndex(gChannel[0], &index);

    if (!index)
    {
        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[0], 0, &gChannel[0]);
        if (result!=FMOD_ERR_MAXAUDIBLE)
        {
        	CHECK_RESULT(result);
        }
    }
    else
    {
        result = FMOD_Channel_Stop(gChannel[0]);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_soundgroups_Example_cPlayJaguar(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int index = 0;

    FMOD_Channel_GetIndex(gChannel[1], &index);

    if (!index)
    {
        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[1], 0, &gChannel[1]);
        if (result!=FMOD_ERR_MAXAUDIBLE)
        {
        	CHECK_RESULT(result);
        }
    }
    else
    {
        result = FMOD_Channel_Stop(gChannel[1]);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_soundgroups_Example_cPlaySwish(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int index = 0;

    FMOD_Channel_GetIndex(gChannel[2], &index);

    if (!index)
    {
        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound[2], 0, &gChannel[2]);
        if (result!=FMOD_ERR_MAXAUDIBLE)
        {
        	CHECK_RESULT(result);
        }
    }
    else
    {
        result = FMOD_Channel_Stop(gChannel[2]);
        CHECK_RESULT(result);
    }
}

int Java_org_fmod_soundgroups_Example_cGetBehaviour(JNIEnv *env, jobject thiz)
{
    FMOD_SOUNDGROUP_BEHAVIOR behavior;
    FMOD_SoundGroup_GetMaxAudibleBehavior(gSoundGroup, &behavior);

    return behavior;
}

jstring Java_org_fmod_soundgroups_Example_cGetChannelState(JNIEnv *env, jobject thiz, int channelnumber)
{
	FMOD_RESULT result;
	char s[8];
	int index;
	float audibility;

    FMOD_Channel_GetAudibility(gChannel[channelnumber], &audibility);

    if (!audibility)
    {
        result = FMOD_Channel_GetIndex(gChannel[channelnumber], &index);

        if (!index)
        {
            sprintf(s, "STOP");
        }
        else
        {
            sprintf(s, "MUTE");
        }
    }
    else
    {
        sprintf(s, "PLAY");
    }

    return((*env)->NewStringUTF(env, s));
}
