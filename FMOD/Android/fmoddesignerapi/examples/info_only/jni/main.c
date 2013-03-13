/*===============================================================================================
 Info-only Event Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 Demonstrates usage and functionality of info-only events
===============================================================================================*/
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

FMOD_EVENTSYSTEM *gEventSystem  = 0;
FMOD_EVENTGROUP *gEventGroup;
FMOD_EVENT *gEvent = 0;
int gStateChanged = 1;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_infoonly_Example_cBegin(JNIEnv *env, jobject thiz)
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

void Java_org_fmod_infoonly_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);

    if (gStateChanged)
    {
        int memoryCurrent   = 0;
        int memoryMax       = 0;

        FMOD_Memory_GetStats(&memoryCurrent, &memoryMax, 0);

        __android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "Memory usage: current = %10d, max = %10d\n", memoryCurrent, memoryMax);

        if (gEvent)
        {
            float volume = 0.0f;

            /* Attempt to get the event's volume.
               - This will succeed on both info-only events and instances.
            */
            result = FMOD_Event_GetVolume(gEvent, &volume);
            if (result != FMOD_ERR_INVALID_HANDLE)
            {
            	CHECK_RESULT(result);
                __android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "Volume: %.2f\n", volume);
            }
        }

        gStateChanged = 0;
    }
}

void Java_org_fmod_infoonly_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_FreeEventData(gEventGroup, 0, 0);
    CHECK_RESULT(result);

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_infoonly_Example_cGetInfoOnlyEvent(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    /* Get an info-only event.
       - This is the parent from which all instances of the specified event are derived.
       - This call will not allocate memory for event instances or load sample data.
       - The resulting event cannot be played, it simply allows querying and setting of event properties.
       - Any properties which are set on the info-only event will be copied to all instances which are retrieved from then on.
    */
    result = FMOD_EventGroup_GetEvent(gEventGroup, "BasicEventWithLooping", FMOD_EVENT_INFOONLY, &gEvent);
    CHECK_RESULT(result);

    __android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "getEvent(FMOD_EVENT_INFOONLY) succeeded\n");
    gStateChanged = 1;
}

void Java_org_fmod_infoonly_Example_cGetEvent(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    /* Get an event instance.
       - This call will allocate memory for event instances and load sample data if required.
       - The resulting event can be played.
       - Any properties which have been set on the info-only event will be copied to this instance.
    */
    result = FMOD_EventGroup_GetEvent(gEventGroup, "BasicEventWithLooping", FMOD_EVENT_DEFAULT, &gEvent);
    CHECK_RESULT(result);

    __android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "getEvent(FMOD_EVENT_DEFAULT) succeeded\n");
    gStateChanged = 1;
}

void Java_org_fmod_infoonly_Example_cStartStopEvent(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    if (gEvent)
    {
        FMOD_EVENT_STATE state;

        result = FMOD_Event_GetState(gEvent, &state);
        if (result != FMOD_ERR_INVALID_HANDLE)
        {
            CHECK_RESULT(result);
        }

        if (state & FMOD_EVENT_STATE_PLAYING)
        {
            /* Attempt to stop the event.
               - This will fail if the event is info-only.
            */
            result = FMOD_Event_Stop(gEvent, 0);
            if (result == FMOD_OK)
            {
            	__android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "event->stop() succeeded\n");
                gStateChanged = 1;
            }
            else
            {
            	__android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "event->stop() returned an error:\n\t(%d) %s\n", result, FMOD_ErrorString(result));
            }
        }
        else
        {
            /* Attempt to start the event.
               - This will fail if the event is info-only.
            */
            result = FMOD_Event_Start(gEvent);
            if (result == FMOD_OK)
            {
            	__android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "event->start() succeeded\n");
                gStateChanged = 1;
            }
            else
            {
            	__android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "event->start() returned an error:\n\t(%d) %s\n", result, FMOD_ErrorString(result));
            }
        }
    }
}

void Java_org_fmod_infoonly_Example_cUnloadEventData(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_FreeEventData(gEventGroup, 0, 0);
    CHECK_RESULT(result);

    __android_log_print(ANDROID_LOG_INFO, "fmod_infoonly", "Event data unloaded\n");
    gStateChanged = 1;
}

void Java_org_fmod_infoonly_Example_cSetVolume(JNIEnv *env, jobject thiz, jfloat volume)
{
	FMOD_RESULT result = FMOD_OK;

    /* Attempt to set the event's volume.
       - This will succeed on both info-only events and instances.
       - Volume set on the info-only event will be copied to all instances retrieved from now on.
       - Volume set on an instance will only apply to that instance.
    */
    FMOD_Event_SetVolume(gEvent, volume);
}

jfloat Java_org_fmod_infoonly_Example_cGetVolume(JNIEnv *env, jobject thiz)
{
	float volume;

	FMOD_Event_GetVolume(gEvent, &volume);

	return volume;
}
