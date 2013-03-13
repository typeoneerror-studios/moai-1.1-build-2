/*===============================================================================================
 SimpleEvent Example
 Copyright (c), Firelight Technologies Pty, Ltd 2010-2011.

 Demonstrates basic usage of FMOD's data-driven event library
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

FMOD_EVENTSYSTEM *gEventSystem  = 0;
FMOD_EVENT *gEvent = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_simpleevent_Example_cBegin(JNIEnv *env, jobject thiz)
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

	result = FMOD_EventSystem_GetEvent(gEventSystem, "examples/FeatureDemonstration/Basics/SimpleEvent", FMOD_EVENT_DEFAULT, &gEvent);
	CHECK_RESULT(result);
}

void Java_org_fmod_simpleevent_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_simpleevent_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_simpleevent_Example_cStart(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Event_Start(gEvent);
	CHECK_RESULT(result);
}

jboolean Java_org_fmod_simpleevent_Example_cGetPlaying(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_EVENT_STATE state;

	result = FMOD_Event_GetState(gEvent, &state);
	CHECK_RESULT(result);

	return state & FMOD_EVENT_STATE_PLAYING;
}
