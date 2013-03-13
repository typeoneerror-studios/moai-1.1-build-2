/*===============================================================================================
 Parameters Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 Demonstrates basic usage of event parameters
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

FMOD_EVENTSYSTEM      *gEventSystem    = NULL;
FMOD_EVENTGROUP       *gEventGroup     = NULL;
FMOD_EVENTCATEGORY    *gMasterCategory = NULL;
FMOD_EVENT            *car             = NULL;
FMOD_EVENTPARAMETER   *rpm             = NULL;
FMOD_EVENTPARAMETER   *load            = NULL;
float                  rpmVal          = 0.0f;
float                  rpmMin          = 0.0f;
float                  rpmMax          = 0.0f;
float                  loadVal         = 0.0f;
float                  loadMin         = 0.0f;
float                  loadMax         = 0.0f;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_parameters_Example_cBegin(JNIEnv *env, jobject thiz)
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

    result = FMOD_EventSystem_GetGroup(gEventSystem, "examples/AdvancedTechniques", 0, &gEventGroup);
	CHECK_RESULT(result);

    result = FMOD_EventGroup_GetEvent(gEventGroup, "car", FMOD_EVENT_DEFAULT, &car);
	CHECK_RESULT(result);
    result = FMOD_EventSystem_GetCategory(gEventSystem, "master", &gMasterCategory);
	CHECK_RESULT(result);

    result = FMOD_Event_GetParameter(car, "load", &load);
	CHECK_RESULT(result);
    result = FMOD_EventParameter_GetRange(load, &loadMin, &loadMax);
	CHECK_RESULT(result);
    result = FMOD_EventParameter_SetValue(load, loadMax);
	CHECK_RESULT(result);

    result = FMOD_Event_GetParameterByIndex(car, 0, &rpm);
	CHECK_RESULT(result);
    result = FMOD_EventParameter_GetRange(rpm, &rpmMin, &rpmMax);
	CHECK_RESULT(result);
    result = FMOD_EventParameter_SetValue(rpm, 1000.0f);
	CHECK_RESULT(result);

    result = FMOD_Event_Start(car);
	CHECK_RESULT(result);
}

void Java_org_fmod_parameters_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_parameters_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_parameters_Example_cSetRPM(JNIEnv *env, jobject thiz, float value)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventParameter_SetValue(rpm, value);
	CHECK_RESULT(result);
}

jfloat Java_org_fmod_parameters_Example_cGetRPM(JNIEnv *env, jobject thiz)
{
	float value;
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventParameter_GetValue(rpm, &value);
	CHECK_RESULT(result);

	return value;
}

jfloat Java_org_fmod_parameters_Example_cGetRPMMin(JNIEnv *env, jobject thiz)
{
	return rpmMin;
}

jfloat Java_org_fmod_parameters_Example_cGetRPMMax(JNIEnv *env, jobject thiz)
{
	return rpmMax;
}

void Java_org_fmod_parameters_Example_cSetLoad(JNIEnv *env, jobject thiz, float value)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventParameter_SetValue(load, value);
	CHECK_RESULT(result);
}

jfloat Java_org_fmod_parameters_Example_cGetLoad(JNIEnv *env, jobject thiz)
{
	float value;
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventParameter_GetValue(load, &value);
	CHECK_RESULT(result);

	return value;
}

jfloat Java_org_fmod_parameters_Example_cGetLoadMin(JNIEnv *env, jobject thiz)
{
	return loadMin;
}

jfloat Java_org_fmod_parameters_Example_cGetLoadMax(JNIEnv *env, jobject thiz)
{
	return loadMax;
}

void Java_org_fmod_parameters_Example_cTogglePause(JNIEnv *env, jobject thiz, float value)
{
	FMOD_RESULT result = FMOD_OK;
    int paused = 0;

    result = FMOD_EventCategory_GetPaused(gMasterCategory, &paused);
    CHECK_RESULT(result);
    result = FMOD_EventCategory_SetPaused(gMasterCategory, !paused);
    CHECK_RESULT(result);
}
