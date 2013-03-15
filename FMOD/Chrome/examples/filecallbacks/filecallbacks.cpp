/*===============================================================================================
 File Callbacks Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example is a modified version of the playstream example.
 It shows the correct way declare and handle fmod file callbacks with System::setFileSystem.
===============================================================================================*/
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/url_loader.h"
#include "ppapi/cpp/url_request_info.h"
#include "ppapi/cpp/url_response_info.h"
#include "ppapi/cpp/file_ref.h"
#include "ppapi/cpp/file_io.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/audio.h"
#include "ppapi/cpp/audio_config.h"
#include "ppapi/c/ppb.h"

#include "../common/nacl_file.h"

#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmodnacl.h"

#define ERRCHECK(result) if (result != FMOD_OK) { SetStatus("FMOD error! (%d) %s", result, FMOD_ErrorString(result)); assert(0); }

namespace fileCallbacks 
{

static const char* PauseSoundMessage = "pauseSound";

/*
    TIPS:

    1. use F_CALLBACK.  Do NOT force cast your own function to fmod's callback type.
    2. return FMOD_ERR_FILE_NOTFOUND in open as required.
    3. return number of bytes read in read callback.  Do not get the size and count 
       around the wrong way in fread for example, this would return 1 instead of the number of bytes read.

    QUESTIONS:

    1. Why does fmod seek to the end and read?  Because it is looking for ID3V1 tags.  
       Use FMOD_IGNORETAGS in System::createSound / System::createStream if you don't like this behaviour.

*/
FMOD_RESULT F_CALLBACK myopen(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
{
    if (name)
    {
		int fd = nacl_file::FileManager::GetFD(name, false);
		if (fd < 0)
		{
			return FMOD_ERR_FILE_NOTFOUND;
		}
		nacl_file::FileHandle* fp = nacl_file::FileManager::GetFileHandle(fd);
		
		*filesize = (unsigned) fp->Length();
		
        *userdata = (void *)0xDEADBEEF;
        *handle = (void *) fd;
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myclose(void *handle, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
	
	nacl_file::FileManager::Close((int) handle);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myread(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (bytesread)
    {	
		nacl_file::FileHandle* fp = nacl_file::FileManager::GetFileHandle((int)handle);
		*bytesread = fp->Read(buffer, sizebytes);
    
        if (*bytesread < sizebytes)
        {
            return FMOD_ERR_FILE_EOF;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myseek(void *handle, unsigned int pos, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	nacl_file::FileHandle* fp = nacl_file::FileManager::GetFileHandle((int)handle);
	fp->Seek(pos, SEEK_SET);

    return FMOD_OK;
}


class FileCallbacksInstance : public pp::Instance
{
	public:
	explicit FileCallbacksInstance(PP_Instance instance);
	virtual ~FileCallbacksInstance();
	
	virtual bool Init(uint32_t argc, const char *argn[], const char * argv[]);
	virtual void HandleMessage(const pp::Var& var_message);
	void Update(int32_t unused);
	
	void FilesLoaded();
	
	void SetStatus(const char* format, ...);

	private:	
	FMOD::System    *mSystem;
	FMOD::Channel   *mChannel;
	FMOD::Sound     *mSound;
	enum LOADSTATE
	{
		LOADSTATE_FETCHING,
		LOADSTATE_LOADING,
		LOADSTATE_DONE,
	};
	LOADSTATE		mLoadState;
};

FileCallbacksInstance::FileCallbacksInstance(PP_Instance instance) :
pp::Instance(instance),
mSystem(0),
mChannel(0),
mSound(0)
{
}

bool FileCallbacksInstance::Init(uint32_t argc, const char *argn[], const char * argv[])	
{
    FMOD_RESULT      result;
    unsigned int     version;
	
	SetStatus("Starting FMOD");
	
    /*
        Create a System object and initialize.
    */
	//FMOD::Debug_SetLevel(FMOD_DEBUG_ALL);
    result = FMOD::System_Create(&mSystem);
    ERRCHECK(result);

    result = mSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		assert(0);
    }

	/*
		Pass a pointer to the module instance as the platform-specific third argument
	*/
		
	FMOD_NACL_EXTRADRIVERDATA extraDriverData;
	memset(&extraDriverData, 0, sizeof(FMOD_NACL_EXTRADRIVERDATA));
	extraDriverData.instance = pp_instance();
		
    result = mSystem->init(32, FMOD_INIT_NORMAL, &extraDriverData);
    ERRCHECK(result);	
	
    result = mSystem->setFileSystem(myopen, myclose, myread, myseek, 0, 0, 2048);
    ERRCHECK(result);
	
	SetStatus("Downloading sound files");
	mLoadState = LOADSTATE_FETCHING;
	nacl_file::FileManager::Init(this, 20*1024*1024);
	nacl_file::FileManager::set_ready_func(std::tr1::bind(&FileCallbacksInstance::FilesLoaded, this));
	nacl_file::FileManager::Fetch("/media/wave.mp3");
	
	Update(0);
}

FileCallbacksInstance::~FileCallbacksInstance()
{
	FMOD_RESULT result;
	result = mSystem->release();
	ERRCHECK(result);
}


void FileCallbacksInstance::SetStatus(const char* format, ...)
{
	char status[1024] = "status:";
	
    va_list argptr;
    va_start(argptr, format);
    vsprintf(status + strlen(status), format, argptr);
    va_end(argptr);
	
	printf(status);
	printf("\n");
	
	PostMessage(pp::Var(status));
}

void FileCallbacksInstance::FilesLoaded()
{
	SetStatus("Reading sound files");
	FMOD_RESULT result;
	result = mSystem->createSound("/media/wave.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D | FMOD_NONBLOCKING, 0, &mSound);
	ERRCHECK(result);              
	mLoadState = LOADSTATE_LOADING;
}

void UpdateCallback(void *data, int32_t unused)
{
	((FileCallbacksInstance*)data)->Update(unused);
}

void FileCallbacksInstance::Update(int32_t unused)
{
	mSystem->update();
	
	FMOD_RESULT	 result;
	unsigned int ms = 0;
	unsigned int lenms = 0;
	bool         playing = 0;
	bool         paused = 0;
	int          channelsplaying = 0;
	
	if (mLoadState == LOADSTATE_LOADING)
	{	
		bool allLoaded = true;
		FMOD_OPENSTATE openState;
		
		mSound->getOpenState(&openState, NULL, NULL, NULL);
		if (openState == FMOD_OPENSTATE_READY)
		{			
			result = mSystem->playSound(FMOD_CHANNEL_FREE, mSound, false, &mChannel);
			ERRCHECK(result);
			mLoadState = LOADSTATE_DONE;
			SetStatus("Sound Loaded");
		}
	}

	if (mChannel)
	{
		FMOD::Sound *currentsound = 0;

		result = mChannel->isPlaying(&playing);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		result = mChannel->getPaused(&paused);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		result = mChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		mChannel->getCurrentSound(&currentsound);
		if (currentsound)
		{
			result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
			if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
			{
				ERRCHECK(result);
			}
		}
	}

	mSystem->getChannelsPlaying(&channelsplaying);

	char buf[256];
	sprintf(buf, "%d:%d:%s:%d", ms , lenms , paused ? "Paused " : playing ? "Playing" : "Stopped", channelsplaying);
	
	PostMessage(pp::Var(buf));

	pp::CompletionCallback nexttick(UpdateCallback, this);
	pp::Module::Get()->core()->CallOnMainThread(10, nexttick, 0);
}

void FileCallbacksInstance::HandleMessage(const pp::Var& var_message) 
{
	if (!var_message.is_string()) 
	{
		return;
	}
	if (mLoadState != LOADSTATE_DONE)
	{
		return;
	}
	std::string message = var_message.AsString();
  
	if (message == PauseSoundMessage) 
	{	
		bool paused;
		mChannel->getPaused(&paused);
		mChannel->setPaused(!paused);
	}  
}

class FileCallbacksModule : public pp::Module 
{
	public:
	FileCallbacksModule() : pp::Module() {}
	virtual ~FileCallbacksModule() {}
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new FileCallbacksInstance(instance);
	}
};

}  // namespace fileCallbacks


namespace pp 
{

Module* CreateModule()
{
	return new fileCallbacks::FileCallbacksModule();
}

}  // namespace pp
