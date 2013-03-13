/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to apply some of the built in software effects to sounds.
 This example filters the global mix.  All software sounds played here would be filtered in the
 same way.
 To filter per channel, and not have other channels affected, simply replace system->addDSP with
 channel->addDSP.
 Note in this example you don't have to add and remove units each time, you could simply add them
 all at the start then use DSP::setActive to toggle them on and off.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem        = 0;
FMOD_SOUND	 *gSound         = 0;
FMOD_CHANNEL *gChannel       = 0;
FMOD_DSP     *gDSPLowpass    = 0;
FMOD_DSP     *gDSPHighpass   = 0;
FMOD_DSP     *gDSPEcho       = 0;
FMOD_DSP     *gDSPFlange     = 0;
FMOD_DSP     *gDSPDistortion = 0;
FMOD_DSP     *gDSPChorus     = 0;
FMOD_DSP     *gDSPParameq    = 0;

void CHECK_RESULT(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s", result, FMOD_ErrorString(result));
		exit(-1);
	}
}

void Java_org_fmod_effects_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/drumloop.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &gSound);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);

    /*
        Create some effects to play with.
    */
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_LOWPASS, &gDSPLowpass);
    CHECK_RESULT(result);
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_HIGHPASS, &gDSPHighpass);
    CHECK_RESULT(result);
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_ECHO, &gDSPEcho);
    CHECK_RESULT(result);
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_FLANGE, &gDSPFlange);
    CHECK_RESULT(result);
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_DISTORTION, &gDSPDistortion);
    CHECK_RESULT(result);
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_CHORUS, &gDSPChorus);
    CHECK_RESULT(result);
    result = FMOD_System_CreateDSPByType(gSystem, FMOD_DSP_TYPE_PARAMEQ, &gDSPParameq);
    CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_DSP_Release(gDSPLowpass);
	CHECK_RESULT(result);
	result = FMOD_DSP_Release(gDSPHighpass);
	CHECK_RESULT(result);
	result = FMOD_DSP_Release(gDSPEcho);
	CHECK_RESULT(result);
	result = FMOD_DSP_Release(gDSPFlange);
	CHECK_RESULT(result);
	result = FMOD_DSP_Release(gDSPDistortion);
	CHECK_RESULT(result);
	result = FMOD_DSP_Release(gDSPChorus);
	CHECK_RESULT(result);
	result = FMOD_DSP_Release(gDSPParameq);
	CHECK_RESULT(result);

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cPause(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel, !paused);
	CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cLowpassToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPLowpass, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPLowpass);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPLowpass, 0);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_effects_Example_cHighpassToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPHighpass, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPHighpass);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPHighpass, 0);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_effects_Example_cEchoToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPEcho, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPEcho);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPEcho, 0);
        CHECK_RESULT(result);

        result = FMOD_DSP_SetParameter(gDSPEcho, FMOD_DSP_ECHO_DELAY, 50.0f);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_effects_Example_cFlangeToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPFlange, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPFlange);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPFlange, 0);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_effects_Example_cDistortionToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPDistortion, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPDistortion);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPDistortion, 0);
        CHECK_RESULT(result);

        result = FMOD_DSP_SetParameter(gDSPDistortion, FMOD_DSP_DISTORTION_LEVEL, 0.8f);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_effects_Example_cChorusToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPChorus, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPChorus);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPChorus, 0);
        CHECK_RESULT(result);
    }
}

void Java_org_fmod_effects_Example_cParameqToggle(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL active = 0;

    result = FMOD_DSP_GetActive(gDSPParameq, &active);
    CHECK_RESULT(result);

    if (active)
    {
        result = FMOD_DSP_Remove(gDSPParameq);
        CHECK_RESULT(result);
    }
    else
    {
        result = FMOD_System_AddDSP(gSystem, gDSPParameq, 0);
        CHECK_RESULT(result);

		result = FMOD_DSP_SetParameter(gDSPParameq, FMOD_DSP_PARAMEQ_CENTER, 5000.0f);
		CHECK_RESULT(result);

		result = FMOD_DSP_SetParameter(gDSPParameq, FMOD_DSP_PARAMEQ_GAIN, 0.0f);
		CHECK_RESULT(result);
    }
}

jboolean Java_org_fmod_effects_Example_cGetPaused(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	return paused;
}



