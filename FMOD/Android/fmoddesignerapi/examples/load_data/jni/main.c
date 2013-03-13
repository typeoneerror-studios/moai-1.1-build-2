/*===============================================================================================
 Load Event Data Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 Demonstrates basic loading and unloading of event data per event
===============================================================================================*/
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

FMOD_EVENTSYSTEM *gEventSystem  = 0;
FMOD_EVENTGROUP *gEventGroup;
FMOD_EVENT *gEvent = 0;
int gMemoryChanged = 1;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_loaddata_Example_cBegin(JNIEnv *env, jobject thiz)
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

    result = FMOD_EventSystem_GetGroup(gEventSystem, "examples/FeatureDemonstration/Basics", 0, &gEventGroup);
    CHECK_RESULT(result);
}

void Java_org_fmod_loaddata_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_loaddata_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_loaddata_Example_cLoadEventData(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_GetEvent(gEventGroup, "SimpleEvent", FMOD_EVENT_DEFAULT, &gEvent);
    CHECK_RESULT(result);

    __android_log_print(ANDROID_LOG_INFO, "fmod_loaddata", "Event data loaded\n");
}

void Java_org_fmod_loaddata_Example_cUnLoadEventData(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    if (gEvent)
    {
        result = FMOD_EventGroup_FreeEventData(gEventGroup, gEvent, 0);
        if (result != FMOD_ERR_INVALID_HANDLE)
        {
            CHECK_RESULT(result);
            __android_log_print(ANDROID_LOG_INFO, "fmod_loaddata", "Event data unloaded\n");
        }
        gEvent = 0;
    }
}

void Java_org_fmod_loaddata_Example_cLoadGroupData(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_LoadEventData(gEventGroup, 0, 0);
    CHECK_RESULT(result);

    __android_log_print(ANDROID_LOG_INFO, "fmod_loaddata", "Event group data loaded\n");
}

void Java_org_fmod_loaddata_Example_cUnLoadGroupData(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_FreeEventData(gEventGroup, 0, 0);
    CHECK_RESULT(result);

    __android_log_print(ANDROID_LOG_INFO, "fmod_loaddata", "Event group data unloaded\n");
}

jint Java_org_fmod_loaddata_Example_cGetMemoryCurrent(JNIEnv *env, jobject thiz)
{
	int currentmem;

	FMOD_Memory_GetStats(&currentmem, 0, 0);

	return currentmem;
}

jint Java_org_fmod_loaddata_Example_cGetMemoryMax(JNIEnv *env, jobject thiz)
{
	int maxmem;

	FMOD_Memory_GetStats(0, &maxmem, 0);

	return maxmem;
}
