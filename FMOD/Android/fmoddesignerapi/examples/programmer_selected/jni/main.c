/*=============================================================================
 Programmer Selected Sound Definition Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 Demonstrates how to use the "programmer selected sound definition" feature of
 the FMOD event system
=============================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod_event.h"
#include "fmod_errors.h"

FMOD_EVENTSYSTEM *gEventSystem = 0;
FMOD_EVENTGROUP *gEventGroup = 0;
FMOD_EVENT *gEvent = 0;
static int sounddefEntryIndex  = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

FMOD_RESULT F_CALLBACK eventcallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
    (void)event;
    (void)userdata;

    switch (type)
    {
        case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_SELECTINDEX :
        {
            char    *name   = (char *)param1;       // [in]  (char *) name of sound definition
            int     *index  = (int *)param2;        // [out] (int *)  the sounddef entry index to use

            *index = (sounddefEntryIndex < *index) ? sounddefEntryIndex : *index - 1;
            __android_log_print(ANDROID_LOG_INFO, "fmod_programmerselected", "FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_SELECTINDEX '%s': %d\n", name, *index);

            break;
        }
    }

    return FMOD_OK;
}

void Java_org_fmod_programmerselected_Example_cBegin(JNIEnv *env, jobject thiz)
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

    result = FMOD_EventSystem_GetGroup(gEventSystem, "examples/FeatureDemonstration/SequencingAndStitching", 0, &gEventGroup);
    CHECK_RESULT(result);
}

void Java_org_fmod_programmerselected_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_EventSystem_Update(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_programmerselected_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_EventSystem_Release(gEventSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_programmerselected_Example_cStart(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    result = FMOD_EventGroup_GetEvent(gEventGroup, "ProgrammerSelected", FMOD_EVENT_DEFAULT, &gEvent);
    CHECK_RESULT(result);
    result = FMOD_Event_SetCallback(gEvent, eventcallback, NULL);
    CHECK_RESULT(result);
    result = FMOD_Event_Start(gEvent);
    CHECK_RESULT(result);
}

void Java_org_fmod_programmerselected_Example_cIncreaseSounddefEntryIndex(JNIEnv *env, jobject thiz)
{
    sounddefEntryIndex++;
    __android_log_print(ANDROID_LOG_INFO, "fmod_programmerselected", "Sound definition entry index = %d\n", sounddefEntryIndex);
}

void Java_org_fmod_programmerselected_Example_cDecreaseSounddefEntryIndex(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    sounddefEntryIndex--;
    sounddefEntryIndex = (sounddefEntryIndex < 0) ? 0 : sounddefEntryIndex;
    __android_log_print(ANDROID_LOG_INFO, "fmod_programmerselected", "Sound definition entry index = %d\n", sounddefEntryIndex);
}

jint Java_org_fmod_programmerselected_Example_cGetSounddefEntryIndex(JNIEnv *env, jobject thiz)
{
	return sounddefEntryIndex;
}
