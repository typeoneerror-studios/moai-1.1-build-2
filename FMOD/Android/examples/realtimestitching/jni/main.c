/*===============================================================================================
 Real-time stitching example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 This example shows how you can create your own multi-subsound stream, then in realtime replace
 each the subsound as it plays them.  Using a looping sentence, it will seamlessly stich between
 2 subsounds in this example, and each time it switches to a new sound, it will replace the old
 one with another sound in our list.

 These sounds can go on forever as long as they are the same bitdepth (when decoded) and number
 of channels (ie mono / stereo).  The reason for this is the hardware channel cannot change
 formats mid sentence, and using different hardware channels would mean it wouldn't be gapless.

===============================================================================================*/
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

#define NUMSOUNDS 32

char s[64];
FMOD_SYSTEM  *gSystem  = 0;
FMOD_SOUND	 *gSound  = 0;
FMOD_SOUND   *gSubSound[2];
FMOD_CHANNEL *gChannel = 0;
unsigned int  gSubSoundID, gSentenceID, gCurrentSubSoundID;
const char   *gSoundName[NUMSOUNDS] =
{
    "/sdcard/fmod/e.ogg",   /* Ma-    */
    "/sdcard/fmod/d.ogg",   /* ry     */
    "/sdcard/fmod/c.ogg",   /* had    */
    "/sdcard/fmod/d.ogg",   /* a      */
    "/sdcard/fmod/e.ogg",   /* lit-   */
    "/sdcard/fmod/e.ogg",   /* tle    */
    "/sdcard/fmod/e.ogg",   /* lamb,  */
    "/sdcard/fmod/e.ogg",   /* .....  */
    "/sdcard/fmod/d.ogg",   /* lit-   */
    "/sdcard/fmod/d.ogg",   /* tle    */
    "/sdcard/fmod/d.ogg",   /* lamb,  */
    "/sdcard/fmod/d.ogg",   /* .....  */
    "/sdcard/fmod/e.ogg",   /* lit-   */
    "/sdcard/fmod/e.ogg",   /* tle    */
    "/sdcard/fmod/e.ogg",   /* lamb,  */
    "/sdcard/fmod/e.ogg",   /* .....  */

    "/sdcard/fmod/e.ogg",   /* Ma-    */
    "/sdcard/fmod/d.ogg",   /* ry     */
    "/sdcard/fmod/c.ogg",   /* had    */
    "/sdcard/fmod/d.ogg",   /* a      */
    "/sdcard/fmod/e.ogg",   /* lit-   */
    "/sdcard/fmod/e.ogg",   /* tle    */
    "/sdcard/fmod/e.ogg",   /* lamb,  */
    "/sdcard/fmod/e.ogg",   /* its    */
    "/sdcard/fmod/d.ogg",   /* fleece */
    "/sdcard/fmod/d.ogg",   /* was    */
    "/sdcard/fmod/e.ogg",   /* white  */
    "/sdcard/fmod/d.ogg",   /* as     */
    "/sdcard/fmod/c.ogg",   /* snow.  */
    "/sdcard/fmod/c.ogg",   /* .....  */
    "/sdcard/fmod/c.ogg",   /* .....  */
    "/sdcard/fmod/c.ogg",   /* .....  */
};

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Java_org_fmod_realtimestitching_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_CREATESOUNDEXINFO exinfo;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 1, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

    /*
        Set up the FMOD_CREATESOUNDEXINFO structure for the user stream with room for 2 subsounds. (our subsound double buffer)
    */
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.defaultfrequency = 44100;
    exinfo.numsubsounds = 2;
    exinfo.numchannels = 1;
    exinfo.format = FMOD_SOUND_FORMAT_PCM16;

    /*
        Create the 'parent' stream that contains the substreams.  Set it to loop so that it loops between subsound 0 and 1.
    */
    result = FMOD_System_CreateStream(gSystem, 0, FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &gSound);
    CHECK_RESULT(result);

    /*
        Add 2 of our streams as children of the parent.  They should be the same format (ie mono/stereo and bitdepth) as the parent sound.
        When subsound 0 has finished and it is playing subsound 1, we will swap subsound 0 with a new sound, and the same for when subsound 1 has finished,
        causing a continual double buffered flip, which means continuous sound.
    */
    result = FMOD_System_CreateStream(gSystem, gSoundName[0], FMOD_DEFAULT, 0, &gSubSound[0]);
    CHECK_RESULT(result);

    result = FMOD_System_CreateStream(gSystem, gSoundName[1], FMOD_DEFAULT, 0, &gSubSound[1]);
    CHECK_RESULT(result);

    result = FMOD_Sound_SetSubSound(gSound, 0, gSubSound[0]);
    CHECK_RESULT(result);

    result = FMOD_Sound_SetSubSound(gSound, 1, gSubSound[1]);
    CHECK_RESULT(result);

    /*
        Set up the gapless sentence to contain these first 2 streams.
    */
    {
        int soundlist[2] = { 0, 1 };

        result = FMOD_Sound_SetSubSoundSentence(gSound, soundlist, 2);
        CHECK_RESULT(result);
    }

    gSubSoundID = 0;
    gSentenceID = 2;     /* The next sound to be appended to the stream. */

    sprintf(s, "Inserted subsound %d / 2 with sound %d / %d\n", 0, 0, NUMSOUNDS);
    __android_log_write(ANDROID_LOG_INFO, "fmod_realtimestitching", s);

    sprintf(s, "Inserted subsound %d / 2 with sound %d / %d\n", 1, 1, NUMSOUNDS);
    __android_log_write(ANDROID_LOG_INFO, "fmod_realtimestitching", s);


    /*
        Play the sound.
    */
    result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
    CHECK_RESULT(result);
}

void Java_org_fmod_realtimestitching_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

    /*
        Replace the subsound that just finished with a new subsound, to create endless seamless stitching!

        Note that this polls the currently playing subsound using the FMOD_TIMEUNIT_BUFFERED flag.
        Remember streams are decoded / buffered ahead in advance!
        Don't use the 'audible time' which is FMOD_TIMEUNIT_SENTENCE_SUBSOUND by itself.  When streaming, sound is
        processed ahead of time, and things like stream buffer / sentence manipulation (as done below) is required
        to be in 'buffered time', or else there will be synchronization problems and you might end up releasing a
        sub-sound that is still playing!
    */
    result = FMOD_Channel_GetPosition(gChannel, &gCurrentSubSoundID, (FMOD_TIMEUNIT)(FMOD_TIMEUNIT_SENTENCE_SUBSOUND | FMOD_TIMEUNIT_BUFFERED));
    CHECK_RESULT(result);

    if (gCurrentSubSoundID != gSubSoundID)
    {
        /*
            Release the sound that isn't playing any more.
        */
        result = FMOD_Sound_Release(gSubSound[gSubSoundID]);
        CHECK_RESULT(result);

        /*
            Replace it with a new sound in our list.
        */
        result = FMOD_System_CreateStream(gSystem, gSoundName[gSentenceID], FMOD_DEFAULT, 0, &gSubSound[gSubSoundID]);
        CHECK_RESULT(result);

        result = FMOD_Sound_SetSubSound(gSound, gSubSoundID, gSubSound[gSubSoundID]);
        CHECK_RESULT(result);

        sprintf(s, "Replacing subsound %d / 2 with sound %d / %d\n", gSubSoundID, gSentenceID, NUMSOUNDS);
        __android_log_write(ANDROID_LOG_INFO, "fmod_realtimestitching", s);

        gSentenceID++;
        if (gSentenceID >= NUMSOUNDS)
        {
            gSentenceID = 0;
        }

        gSubSoundID = gCurrentSubSoundID;
    }

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_realtimestitching_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_Sound_Release(gSound); /* Freeing a parent subsound also frees its children. */
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_realtimestitching_Example_cPause(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel, !paused);
	CHECK_RESULT(result);
}
