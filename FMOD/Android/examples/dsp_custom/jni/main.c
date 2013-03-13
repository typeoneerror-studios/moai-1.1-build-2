/*===============================================================================================
 Custom DSP Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how to add a user created DSP callback to process audio data.
 A read callback is generated at runtime, and can be added anywhere in the DSP network.

===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_errors.h"

#define NUM_SOUNDS 3

FMOD_SYSTEM  *gSystem  = 0;
FMOD_CHANNEL *gChannel = 0;
FMOD_SOUND	 *gSound   = 0;
FMOD_DSP	 *gMyDSP   = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count, userdata;
    int count2;
    char name[256];
    FMOD_DSP *thisdsp = dsp_state->instance;

    /*
        This redundant call just shows using the instance parameter of FMOD_DSP_STATE and using it to
        call a DSP information function.
    */
    FMOD_DSP_GetInfo(thisdsp, name, 0, 0, 0, 0);

    FMOD_DSP_GetUserData(thisdsp, (void **)&userdata);

    /*
        This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0'
        as the number of channels in FMOD_DSP_DESCRIPTION.
        Specifying an actual channel count will mean you have to take care of any number of channels coming in,
        but outputting the number of channels specified.  Generally it is best to keep the channel
        count at 0 for maximum compatibility.
    */
    for (count = 0; count < length; count++)
    {
        /*
            Feel free to unroll this.
        */
        for (count2 = 0; count2 < outchannels; count2++)
        {
            /*
                This DSP filter just halves the volume!
                Input is modified, and sent to output.
            */
            outbuffer[(count * outchannels) + count2] = inbuffer[(count * inchannels) + count2] * 0.2f;
        }
    }

    return FMOD_OK;
}

void Java_org_fmod_dspcustom_Example_cBegin(JNIEnv *env, jobject thiz)
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
        Create the DSP effects.
    */
    {
        FMOD_DSP_DESCRIPTION  dspdesc;

        memset(&dspdesc, 0, sizeof(FMOD_DSP_DESCRIPTION));

        strcpy(dspdesc.name, "My first DSP unit");
        dspdesc.channels     = 0;                   // 0 = whatever comes in, else specify.
        dspdesc.read         = myDSPCallback;
        dspdesc.userdata     = (void *)0x12345678;

        result = FMOD_System_CreateDSP(gSystem, &dspdesc, &gMyDSP);
        CHECK_RESULT(result);
    }

    /*
        Inactive by default.
    */
    FMOD_DSP_SetBypass(gMyDSP, 1);

    /*
    	Add to system
    */
    result = FMOD_System_AddDSP(gSystem, gMyDSP, 0);
    CHECK_RESULT(result);
}

void Java_org_fmod_dspcustom_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_dspcustom_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

    result = FMOD_DSP_Release(gMyDSP);
    CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_dspcustom_Example_cToggleFilter(JNIEnv *env, jobject thiz)
{
    static int active = 0;

    FMOD_DSP_SetBypass(gMyDSP, active);

    active = !active;
}
