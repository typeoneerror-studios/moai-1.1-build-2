#include <android/log.h>
#include <android_native_app_glue.h>
#include <stdlib.h>
#include "fmod.h"
#include "fmod_errors.h"

FMOD_SYSTEM  *gSystem  = 0;
FMOD_SOUND	 *gSound  = 0;
FMOD_CHANNEL *gChannel = 0;

#define CHECK_RESULT(x) \
{ \
	FMOD_RESULT _result = x; \
	if (_result != FMOD_OK) \
	{ \
		__android_log_print(ANDROID_LOG_ERROR, "fmod", "FMOD error! (%d) %s\n%s:%d", _result, FMOD_ErrorString(_result), __FILE__, __LINE__); \
		exit(-1); \
	} \
}

void Begin()
{
	FMOD_RESULT result = FMOD_OK;

	result = FMOD_System_Create(&gSystem);
	CHECK_RESULT(result);

	result = FMOD_System_Init(gSystem, 32, FMOD_INIT_NORMAL, 0);
	CHECK_RESULT(result);

	result = FMOD_System_CreateSound(gSystem, "/sdcard/fmod/wave.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &gSound);
	CHECK_RESULT(result);

	result = FMOD_System_PlaySound(gSystem, FMOD_CHANNEL_FREE, gSound, 0, &gChannel);
	CHECK_RESULT(result);
}

void Update()
{
	FMOD_RESULT result = FMOD_OK;

	if (gSystem)
	{
		result = FMOD_System_Update(gSystem);
		CHECK_RESULT(result);
	}
}

void End()
{
	FMOD_RESULT result = FMOD_OK;

	if (gSound)
	{
		result = FMOD_Sound_Release(gSound);
		CHECK_RESULT(result);

		gSound = 0;
	}

	if (gSystem)
	{
		result = FMOD_System_Release(gSystem);
		CHECK_RESULT(result);

		gSystem = 0;
	}
}

static void HandleCMD(struct android_app *app, int32_t cmd)
{
    switch (cmd)
    {
    	case APP_CMD_START:
    	{
    		Begin();
    		break;
    	}
    	case APP_CMD_STOP:
    	{
    		End();
    		break;
    	}
    }
}

void android_main(struct android_app *state)
{
	app_dummy(); // Make sure glue isn't stripped
    state->onAppCmd = HandleCMD;

    while (!state->destroyRequested)
    {
        struct android_poll_source *source;
        while ((ALooper_pollAll(50, NULL, NULL, (void **)&source)) >= 0)
        {
            if (source)
            {
                source->process(state, source);
            }
        }

        Update();
    }
}
