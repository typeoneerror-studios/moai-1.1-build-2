/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

This demonstrates the use of FMOD event effects. It simply plays an event with effects
applied and allows the user to adjust the parameters.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

FMOD_EVENTSYSTEM *gEventSystem  = 0;
FMOD_EVENT *gEvent = 0;
FMOD_EVENTPARAMETER *gParam00 = NULL;
float gParam00_val = 0.0f;
float gParam00_min = 0.0f;
float gParam00_max = 0.0f;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_effects_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Create(&gEventSystem);
	CHECK_RESULT(result);

	result = FMOD_EventSystem_Init(gEventSystem, 64, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL);
	CHECK_RESULT(result);

	result = FMOD_EventSystem_SetMediaPath(gEventSystem, "/sdcard/fmod/");
	CHECK_RESULT(result);

	result = FMOD_EventSystem_Load(gEventSystem, "examples.fev", 0, 0);
	CHECK_RESULT(result);

    result = FMOD_EventSystem_GetEvent(gEventSystem, "examples/FeatureDemonstration/Effects/PropertyAutomationEffectEnvelope", FMOD_EVENT_DEFAULT, &gEvent);
    CHECK_RESULT(result);

    result = FMOD_Event_GetParameter(gEvent, "param00", &gParam00);
    CHECK_RESULT(result);

    result = FMOD_EventParameter_GetRange(gParam00, &gParam00_min, &gParam00_max);
    CHECK_RESULT(result);

    result = FMOD_EventParameter_SetValue(gParam00, 0.0f);
    CHECK_RESULT(result);

    result = FMOD_Event_Start(gEvent);
    CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_effects_Example_cSetParamValue(JNIEnv *env, jobject thiz, jfloat value)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventParameter_SetValue(gParam00, value);
    CHECK_RESULT(result);
}

jfloat Java_org_fmod_effects_Example_cGetParamValue(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	float value;

    result = FMOD_EventParameter_GetValue(gParam00, &value);
    CHECK_RESULT(result);

    return value;
}

jfloat Java_org_fmod_effects_Example_cGetParamMaxValue(JNIEnv *env, jobject thiz)
{
    return gParam00_max;
}

jfloat Java_org_fmod_effects_Example_cGetParamMinValue(JNIEnv *env, jobject thiz)
{
    return gParam00_min;
}
