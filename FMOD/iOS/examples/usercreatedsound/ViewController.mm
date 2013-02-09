#import "ViewController.h"

@implementation ViewController

@synthesize time;
@synthesize status;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    static float  t1 = 0, t2 = 0;        // time
    static float  v1 = 0, v2 = 0;        // velocity
    signed short *stereo16bitbuffer = (signed short *)data;
    
    for (count = 0; count < (datalen >> 2); count++)        // >>2 = 16bit stereo (4 bytes per sample)
    {
        *stereo16bitbuffer++ = (signed short)(sin(t1) * 32767.0f);    // left channel
        *stereo16bitbuffer++ = (signed short)(sin(t2) * 32767.0f);    // right channel
        
        t1 += 0.01f   + v1;
        t2 += 0.0142f + v2;
        v1 += (float)(sin(t1) * 0.002f);
        v2 += (float)(sin(t2) * 0.002f);
    }
    
    return FMOD_OK;
}


FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    /*
        This is useful if the user calls Channel::setPosition and you want to seek your data accordingly
    */
    return FMOD_OK;
}


- (void)viewDidLoad
{
    system  = NULL;
    sound   = NULL;
    channel = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT   result        = FMOD_OK;
    char          buffer[200]   = {0};
    unsigned int  version       = 0;

    /*
        Create a System object and initialize
    */    
    result = FMOD::System_Create(&system); 
    ERRCHECK(result);
    
    result = system->getVersion(&version);
    ERRCHECK(result);
    
    if (version < FMOD_VERSION)
    {
        fprintf(stderr, "You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }

    result = system->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL);
    ERRCHECK(result);
    
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);              /* Required. */
    exinfo.decodebuffersize  = 44100;                                       /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
    exinfo.length            = 44100 * 2 * sizeof(signed short) * 5;        /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    exinfo.numchannels       = 2       ;                                    /* Number of channels in the sound. */
    exinfo.defaultfrequency  = 44100;                                       /* Default playback rate of sound. */
    exinfo.format            = FMOD_SOUND_FORMAT_PCM16;                     /* Data format of sound. */
    exinfo.pcmreadcallback   = pcmreadcallback;                             /* User callback for reading. */
    exinfo.pcmsetposcallback = pcmsetposcallback;                           /* User callback for seeking. */

    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    if (sound)
    {
        sound->release();
        sound = NULL;
    }
    
    if (system)
    {
        system->release();
        system = NULL;
    }    
}


- (void)timerUpdate:(NSTimer *)timer 
{
    /*
        Main loop
    */
    FMOD_RESULT     result          = FMOD_OK;
    bool            playing         = false;
    bool            paused          = false;
    unsigned int    ms              = 0;
    unsigned int    lenms           = 0;
    
    if (channel != NULL)
    {
        result = channel->isPlaying(&playing);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = channel->getPaused(&paused);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = sound->getLength(&lenms, FMOD_TIMEUNIT_MS);
        ERRCHECK(result);
    }
    
    if (system != NULL)
    {
        result = system->update();
        ERRCHECK(result);
    }
    
    time.text   = [NSString stringWithFormat:@"%02d:%02d:%02d / %02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100];
    status.text = (paused ? @"Paused" : (playing ? @"Playing" : @"Stopped"));
}


- (IBAction)playStream:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (sound != NULL)
    {
        result = channel->stop();
        ERRCHECK(result);
        
        sound->release();
        sound = NULL;
    }
    
    result = system->createSound(NULL, FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_SOFTWARE, &exinfo, &sound);
    ERRCHECK(result);    
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);
}


- (IBAction)playSample:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (sound != NULL)
    {
        result = channel->stop();
        ERRCHECK(result);
        
        sound->release();
        sound = NULL;
    }
    
    result = system->createStream(NULL, FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_SOFTWARE, &exinfo, &sound);
    ERRCHECK(result);    
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);  
}


- (IBAction)pauseResume:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    bool        paused = false;
    
    result = channel->getPaused(&paused);
    ERRCHECK(result);  

    result = channel->setPaused(!paused);
    ERRCHECK(result);
}


- (void)dealloc 
{   
	[time release];
	[status release];
	[super dealloc];
}

@end
