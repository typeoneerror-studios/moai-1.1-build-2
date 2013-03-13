/*===============================================================================================
 3d Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to basic 3d positioning
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <math.h>
#include "fmod.h"
#include "fmod_errors.h"


const int   INTERFACE_UPDATETIME = 50;      // 50ms update for interface
const float DISTANCEFACTOR = 1.0f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

FMOD_SYSTEM  *gSystem   = 0;
FMOD_CHANNEL *gChannel1 = 0;
FMOD_CHANNEL *gChannel2 = 0;
FMOD_CHANNEL *gChannel3 = 0;
FMOD_SOUND	 *gSound1   = 0;
FMOD_SOUND	 *gSound2   = 0;
FMOD_SOUND	 *gSound3   = 0;
int           gListenerFlag = 1;
FMOD_VECTOR   gListenerPos;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_threed_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	/*
		Set initial listener position
	*/
	gListenerPos.x = 0.0f;
	gListenerPos.y = 0.0f;
	gListenerPos.z = -1.0f * DISTANCEFACTOR;

    /*
        Set the distance units. (meters/feet etc).
    */
    result = FMOD_System_Set3DSettings(gSystem, 1.0, DISTANCEFACTOR, 1.0f);
    CHECK_RESULT(result);

    /*
        Load some sounds
    */
    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/drumloop.wav", FMOD_SOFTWARE | FMOD_3D, 0, &gSound1);
    CHECK_RESULT(result);
    result = FMOD_Sound_Set3DMinMaxDistance(gSound1, 0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    CHECK_RESULT(result);
    result = FMOD_Sound_SetMode(gSound1, FMOD_LOOP_NORMAL);
    CHECK_RESULT(result);

    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/jaguar.wav", FMOD_SOFTWARE | FMOD_3D, 0, &gSound2);
    CHECK_RESULT(result);
    result = FMOD_Sound_Set3DMinMaxDistance(gSound2, 0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    CHECK_RESULT(result);
    result = FMOD_Sound_SetMode(gSound2, FMOD_LOOP_NORMAL);
    CHECK_RESULT(result);

    result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/swish.wav", FMOD_SOFTWARE | FMOD_2D, 0, &gSound3);
    CHECK_RESULT(result);

    /*
        Play sounds at certain positions
    */
    {
        FMOD_VECTOR pos = { -10.0f * DISTANCEFACTOR, 0.0f, 0.0f };
        FMOD_VECTOR vel = {  0.0f, 0.0f, 0.0f };

        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound1, 1, &gChannel1);
        CHECK_RESULT(result);
        result = FMOD_Channel_Set3DAttributes(gChannel1, &pos, &vel);
        CHECK_RESULT(result);
        result = FMOD_Channel_SetPaused(gChannel1, 0);
        CHECK_RESULT(result);
    }

    {
        FMOD_VECTOR pos = { 15.0f * DISTANCEFACTOR, 0.0f, 0.0f };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

        result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound2, 1, &gChannel2);
        CHECK_RESULT(result);
        result = FMOD_Channel_Set3DAttributes(gChannel2, &pos, &vel);
        CHECK_RESULT(result);
        result = FMOD_Channel_SetPaused(gChannel2, 0);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_threed_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	// ==========================================================================================
	// UPDATE THE LISTENER
	// ==========================================================================================

	static float t = 0;
	static FMOD_VECTOR lastpos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR forward        = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up             = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR vel;

	if (gListenerFlag)
	{
		gListenerPos.x = (float)sin(t * 0.05f) * 33.0f * DISTANCEFACTOR; // left right pingpong
	}

	// ********* NOTE ******* READ NEXT COMMENT!!!!!
	// vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
	vel.x = (gListenerPos.x - lastpos.x) * (1000 / INTERFACE_UPDATETIME);
	vel.y = (gListenerPos.y - lastpos.y) * (1000 / INTERFACE_UPDATETIME);
	vel.z = (gListenerPos.z - lastpos.z) * (1000 / INTERFACE_UPDATETIME);

	// store pos for next time
	lastpos = gListenerPos;

	result = FMOD_System_Set3DListenerAttributes(gSystem, 0, &gListenerPos, &vel, &forward, &up);
	CHECK_RESULT(result);

	t += (30 * (1.0f / (float)INTERFACE_UPDATETIME));    // t is just a time value .. it increments in 30m/s steps in this example

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_threed_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	unsigned int i = 0;

    result = FMOD_Sound_Release(gSound1);
    CHECK_RESULT(result);
    result = FMOD_Sound_Release(gSound2);
    CHECK_RESULT(result);
    result = FMOD_Sound_Release(gSound3);
    CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_threed_Example_cTogglePause16bit3d(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel2, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel2, !paused);
	CHECK_RESULT(result);
}

void Java_org_fmod_threed_Example_cTogglePause8bit3d(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel1, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel1, !paused);
	CHECK_RESULT(result);
}

void Java_org_fmod_threed_Example_cPlay16bit2d(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound3, 0, &gChannel3);
    CHECK_RESULT(result);
}

void Java_org_fmod_threed_Example_cToggleListenerMovement(JNIEnv *env, jobject thiz)
{
	gListenerFlag = !gListenerFlag;
}

jboolean Java_org_fmod_threed_Example_cGetListenerMovement(JNIEnv *env, jobject thiz)
{
	return gListenerFlag;
}

void Java_org_fmod_threed_Example_cSetListenerPosition(JNIEnv *env, jobject thiz, jfloat position)
{
	gListenerPos.x = position;
}

jfloat Java_org_fmod_threed_Example_cGetListenerPosition(JNIEnv *env, jobject thiz)
{
	return gListenerPos.x;
}
