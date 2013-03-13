/*===============================================================================================
 Max Playbacks Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 Demonstrates basic usage of event max playbacks behaviour.
===============================================================================================*/
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

#define StealOldest 0
#define StealNewest 1
#define StealQuietest 2
#define JustFail 3
#define JustFailIfQuietest 4

FMOD_EVENTSYSTEM *gEventSystem  = 0;
FMOD_EVENTGROUP *gEventGroup;
FMOD_EVENT *gEvent = 0;
float gDistance = 2.0f;
float gSound = 0.0f;
int gEventBehaviour = StealOldest;


#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void setupEvent(FMOD_EVENT *event)
{
    FMOD_RESULT           result = FMOD_OK;
    FMOD_EVENTPARAMETER  *param;
    FMOD_VECTOR           pos = {0, 0, gDistance};

    result = FMOD_Event_Set3DAttributes(event, &pos, 0, 0);
    CHECK_RESULT(result);

    result = FMOD_Event_GetParameter(event, "sound", &param);
    CHECK_RESULT(result);

    result = FMOD_EventParameter_SetValue(param, gSound);
    CHECK_RESULT(result);
}

void stopAllEvents()
{
    FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_FreeEventData(gEventGroup, 0, 0);
    CHECK_RESULT(result);
}

void Java_org_fmod_maxplaybacks_Example_cBegin(JNIEnv *env, jobject thiz)
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

    result = FMOD_EventSystem_GetGroup(gEventSystem, "examples/FeatureDemonstration/MaxPlayback", 0, &gEventGroup);
    CHECK_RESULT(result);
}

void Java_org_fmod_maxplaybacks_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_maxplaybacks_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_maxplaybacks_Example_cChangeEventBehaviour(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	gEventBehaviour++;
    if (gEventBehaviour > JustFailIfQuietest)
    {
    	gEventBehaviour = StealOldest;
    }
    stopAllEvents();
}

void Java_org_fmod_maxplaybacks_Example_cStartEvent(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
    const char *name;

    switch(gEventBehaviour)
    {
        case StealOldest :
            name = "MaxPlay-StealOldest";
            break;

        case StealNewest :
            name = "MaxPlay-StealNewest";
            break;

        case StealQuietest :
            name = "MaxPlay-StealQuietest";
            break;

        case JustFail :
            name = "MaxPlay-JustFail";
            break;

        case JustFailIfQuietest :
            name = "MaxPlay-JustFailIfQuietest";
            break;
    }

    if (gEventBehaviour == JustFailIfQuietest)
    {
        /* The 'Just fail if quietest' behaviour calculates the expected
           volume of the event based on the properties of the info-only
           event, so we have to get the info-only event first and set it
           up appropriately.
         */

        // get the info-only event to set up for volume calculation
        result = FMOD_EventGroup_GetEvent(gEventGroup, name, FMOD_EVENT_INFOONLY, &gEvent);
        CHECK_RESULT(result);

        /* set the desired properties on the info-only event
           Notes:
           - distances below the event's 3D Min Distance all give the
             same volume; in this case, getEvent will just fail
           - we could set other volume-affecting properties here as
             well (e.g. orientation if the event has a falloff cone)
         */
        setupEvent(gEvent);

        // attempt to get a real event instance
        result = FMOD_EventGroup_GetEvent(gEventGroup, name, FMOD_EVENT_DEFAULT, &gEvent);
        if (result == FMOD_OK)
        {
        	__android_log_print(ANDROID_LOG_INFO, "fmod_maxplaybacks", "getEvent(\"%s\") succeeded\n", name);

            /* we don't need to set the position of the instance,
               as it is copied from the info-only event, but we
               do need to set the parameter value.
             */
            FMOD_EVENTPARAMETER *param;
            result = FMOD_Event_GetParameter(gEvent, "sound", &param);
            CHECK_RESULT(result);

            result = FMOD_EventParameter_SetValue(param, gSound);
            CHECK_RESULT(result);

            result = FMOD_Event_Start(gEvent);
            CHECK_RESULT(result);
        }
        else
        {
        	__android_log_print(ANDROID_LOG_INFO, "fmod_maxplaybacks", "getEvent(\"%s\") failed\n", name);
        }
    }
    else
    {
        result = FMOD_EventGroup_GetEvent(gEventGroup, name, FMOD_EVENT_DEFAULT, &gEvent);
        if (result == FMOD_OK)
        {
        	__android_log_print(ANDROID_LOG_INFO, "fmod_maxplaybacks", "getEvent(\"%s\") succeeded\n", name);
            setupEvent(gEvent);
            result = FMOD_Event_Start(gEvent);
            CHECK_RESULT(result);
        }
        else
        {
        	__android_log_print(ANDROID_LOG_INFO, "fmod_maxplaybacks", "getEvent(\"%s\") failed\n", name);
        }
    }

    ++gSound;
    if(gSound > 3)
    {
        gSound = 0;
    }
}

void Java_org_fmod_maxplaybacks_Example_cStopAllEvents(JNIEnv *env, jobject thiz)
{
	stopAllEvents();
}

void Java_org_fmod_maxplaybacks_Example_cIncreaseEventDistance(JNIEnv *env, jobject thiz)
{
	gDistance += 0.1f;
}

void Java_org_fmod_maxplaybacks_Example_cDecreaseEventDistance(JNIEnv *env, jobject thiz)
{
    gDistance -= 0.1f;
    gDistance = (gDistance < 0.0f) ? 0.0f : gDistance;
}

jint Java_org_fmod_maxplaybacks_Example_cGetSound(JNIEnv *env, jobject thiz)
{
	return (int)gSound;
}

jfloat Java_org_fmod_maxplaybacks_Example_cGetDistance(JNIEnv *env, jobject thiz)
{
	return gDistance;
}

jstring Java_org_fmod_maxplaybacks_Example_cGetEventBehaviour(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    const char *name;

    switch(gEventBehaviour)
    {
        case StealOldest :
            name = "StealOldest";
            break;

        case StealNewest :
            name = "StealNewest";
            break;

        case StealQuietest :
            name = "StealQuietest";
            break;

        case JustFail :
            name = "JustFail";
            break;

        case JustFailIfQuietest :
            name = "JustFailIfQuietest";
            break;
    }

    return((*env)->NewStringUTF(env, name));
}

