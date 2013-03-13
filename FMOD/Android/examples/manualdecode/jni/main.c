/*===============================================================================================
 ManualDecode Example
 Copyright (c), Firelight Technologies Pty, Ltd 2011.

 Sound played here decoded in realtime by the user with a 'decoder sound'
 The mp3 is created as a stream opened with FMOD_OPENONLY. This is the
 'decoder sound'.  The playback sound is a 16bit PCM FMOD_OPENUSER created
 sound with a pcm read callback.  When the callback happens, we call readData
 on the decoder sound and use the pcmreadcallback data pointer as the parameter.
===============================================================================================*/

#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem  = 0;
FMOD_SOUND	 *gSound  = 0;
FMOD_SOUND	 *gDecodeSound = 0;
FMOD_CHANNEL *gChannel = 0;

pthread_mutex_t gDecodeCrit;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    FMOD_RESULT result;
    unsigned int read;

    pthread_mutex_lock(&gDecodeCrit);

    if (!gDecodeSound)
    {
        return FMOD_ERR_FILE_EOF;
    }

    result = FMOD_Sound_ReadData(gDecodeSound, data, datalen, &read);

    if (result == FMOD_ERR_FILE_EOF)    /* Handle looping. */
    {
        FMOD_Sound_SeekData(gDecodeSound, 0);

        datalen -= read;

        result = FMOD_Sound_ReadData(gDecodeSound, (char *)data + read, datalen, &read);
    }

    pthread_mutex_unlock(&gDecodeCrit);

    return FMOD_OK;
}


FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    /*
        The timeunit will usually be FMOD_TIMEUNIT_PCM, as FMOD will convert the user's timeunit to pcm for the codec.
        If it is not, do the necessary conversion of whatever is coming in, to PCM samples for Sound_seekData.
    */

    FMOD_Sound_SeekData(gDecodeSound, position);

    return FMOD_OK;
}

void Java_org_fmod_manualdecode_Example_cBegin(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
    pthread_mutexattr_t mutexattr;
    unsigned int        decodesound_lengthbytes = 0;
    int                 decodesound_channels;
    float               decodesound_rate;
    FMOD_CREATESOUNDEXINFO  createsoundexinfo;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	pthread_mutexattr_init(&mutexattr);
	pthread_mutex_init(&gDecodeCrit, &mutexattr);

    /*
        First create the 'decoder sound'.  Note it is a stream that does not initially read any data, because FMOD_OPENONLY has been specified.
        We could use createSound instead of createStream but that would allocate memory for the whole sound which is a waste.
    */
	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/wave.mp3", FMOD_OPENONLY | FMOD_LOOP_NORMAL | FMOD_LOWMEM | FMOD_CREATESTREAM, 0, &gDecodeSound);
	CHECK_RESULT(result);

    result = FMOD_Sound_GetLength(gDecodeSound, &decodesound_lengthbytes, FMOD_TIMEUNIT_PCMBYTES);
    CHECK_RESULT(result);

    result = FMOD_Sound_GetFormat(gDecodeSound, 0, 0, &decodesound_channels, 0);
    CHECK_RESULT(result);

    result = FMOD_Sound_GetDefaults(gDecodeSound, &decodesound_rate, 0, 0, 0);
    CHECK_RESULT(result);

    /*
        Now create a user created PCM stream that we will feed data into, and play.
    */
    memset(&createsoundexinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    createsoundexinfo.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);              /* required. */
    createsoundexinfo.decodebuffersize  = 44100;                                       /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
    createsoundexinfo.numchannels       = decodesound_channels;                        /* Number of channels in the sound. */
    createsoundexinfo.length            = decodesound_lengthbytes;                     /* Length of PCM data in bytes of whole song.  -1 = infinite. */
    createsoundexinfo.defaultfrequency  = (int)decodesound_rate;                       /* Default playback rate of sound. */
    createsoundexinfo.format            = FMOD_SOUND_FORMAT_PCM16;                     /* Data format of sound. */
    createsoundexinfo.pcmreadcallback   = pcmreadcallback;                             /* User callback for reading. */
    createsoundexinfo.pcmsetposcallback = pcmsetposcallback;                           /* User callback for seeking. */

    result = FMOD_System_CreateStream(gSystem, 0, FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL, &createsoundexinfo, &gSound);
    CHECK_RESULT(result);

    /*
		Sound played here decoded in realtime by the user with a 'decoder sound'
		The mp3 is created as a stream opened with FMOD_OPENONLY. This is the
		'decoder sound'.  The playback sound is a 16bit PCM FMOD_OPENUSER created
		sound with a pcm read callback.  When the callback happens, we call readData
		on the decoder sound and use the pcmreadcallback data pointer as the parameter.
    */

    /*
        Play the sound.
    */
    result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
    CHECK_RESULT(result);
}

void Java_org_fmod_manualdecode_Example_cUpdate(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT	result = FMOD_OK;

	result = FMOD_System_Update(gSystem);
	CHECK_RESULT(result);
}

void Java_org_fmod_manualdecode_Example_cEnd(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;

    pthread_mutex_lock(&gDecodeCrit);
    {
        /*
            Remove the sound - wait! it might be still in use!
            Instead of releasing the decode sound first we could release it last, but this protection is here to make the issue obvious.
        */
        result = FMOD_Sound_Release(gDecodeSound);
        CHECK_RESULT(result);
        gDecodeSound = 0;    /* This will make the read callback fail from now on. */
    }
    pthread_mutex_unlock(&gDecodeCrit);

	result = FMOD_Sound_Release(gSound);
	CHECK_RESULT(result);

	result = FMOD_System_Release(gSystem);
	CHECK_RESULT(result);

	pthread_mutex_destroy(&gDecodeCrit);
}

void Java_org_fmod_manualdecode_Example_cPause(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	result = FMOD_Channel_SetPaused(gChannel, !paused);
	CHECK_RESULT(result);
}

jboolean Java_org_fmod_manualdecode_Example_cGetPaused(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	FMOD_BOOL paused = 0;

	result = FMOD_Channel_GetPaused(gChannel, &paused);
	CHECK_RESULT(result);

	return paused;
}

jint Java_org_fmod_manualdecode_Example_cGetPosition(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int position = 0;

	result = FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);
	CHECK_RESULT(result);

	return position;
}

jint Java_org_fmod_manualdecode_Example_cGetLength(JNIEnv *env, jobject thiz)
{
	FMOD_RESULT result = FMOD_OK;
	int length = 0;

	result = FMOD_Sound_GetLength(gSound, &length, FMOD_TIMEUNIT_MS);
	CHECK_RESULT(result);

	return length;
}

void Java_org_fmod_manualdecode_Example_cSeekForward(JNIEnv *env, jobject thiz)
{
    unsigned int position;

    FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);
    position += 1000;
    FMOD_Channel_SetPosition(gChannel, position, FMOD_TIMEUNIT_MS);
}

void Java_org_fmod_manualdecode_Example_cSeekBackward(JNIEnv *env, jobject thiz)
{
    unsigned int position;

    FMOD_Channel_GetPosition(gChannel, &position, FMOD_TIMEUNIT_MS);
    if (position >= 1000)
    {
        position -= 1000;
    }
    FMOD_Channel_SetPosition(gChannel, position, FMOD_TIMEUNIT_MS);
}
