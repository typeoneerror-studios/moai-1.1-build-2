#import "ViewController.h"

@implementation ViewController

@synthesize status;
@synthesize frequencyvalue;
@synthesize frequency;
@synthesize volumevalue;
@synthesize volume;
@synthesize panvalue;
@synthesize pan;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


- (void)updateSliders
{
    FMOD_RESULT result              = FMOD_OK;
    float       channelfrequency    = 0.0f;
    float       channelvolume       = 0.0f;
    float       channelpan          = 0.0f;    
    
    result = channel->getVolume(&channelvolume);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }
    result = channel->getFrequency(&channelfrequency);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }
    result = channel->getPan(&channelpan);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }
    
    frequency.value = channelfrequency;
    volume.value    = channelvolume;
    pan.value       = channelpan;
}


- (void)viewDidLoad
{
    system  = NULL;
    dsp     = NULL;
    channel = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT   result        = FMOD_OK;
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

    /*
        Create an oscillator DSP units for the tone.
    */
    result = system->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &dsp);
    ERRCHECK(result);
    result = dsp->setParameter(FMOD_DSP_OSCILLATOR_RATE, 440.0f);   /* Musical note 'A' */
    ERRCHECK(result);

    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    if (dsp)
    {
        dsp->release();
        dsp = NULL;
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
    FMOD_RESULT result              = FMOD_OK;
    float       channelfrequency    = 0.0f;
    float       channelvolume       = 0.0f;
    float       channelpan          = 0.0f;
    bool        channelplaying      = false;
    
    if (channel != NULL)
    {
        result = channel->getFrequency(&channelfrequency);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = channel->getVolume(&channelvolume);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = channel->getPan(&channelpan);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }        
        
        result = channel->isPlaying(&channelplaying);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }
    
    if (system != NULL)
    {        
        result = system->update();
        ERRCHECK(result);
    }
    
    status.text         = channelplaying ? @"Playing" : @"Stopped";
    frequencyvalue.text = [NSString stringWithFormat:@"%.1f", channelfrequency];
    volumevalue.text    = [NSString stringWithFormat:@"%.1f", channelvolume];
    panvalue.text       = [NSString stringWithFormat:@"%.1f", channelpan];
}


- (IBAction)playSineWave:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = dsp->remove();
    ERRCHECK(result);
    result = system->playDSP(FMOD_CHANNEL_REUSE, dsp, true, &channel);
    ERRCHECK(result);
    result = channel->setVolume(0.5f);
    ERRCHECK(result);
    result = dsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 0);
    ERRCHECK(result);
    result = channel->setPaused(false);
    ERRCHECK(result);
    
    [self updateSliders];
}


- (IBAction)playSquareWave:(id)sender
{
    FMOD_RESULT result = FMOD_OK;

    result = dsp->remove();
    ERRCHECK(result);
    result = system->playDSP(FMOD_CHANNEL_REUSE, dsp, true, &channel);
    ERRCHECK(result);
    result = channel->setVolume(0.125f);
    ERRCHECK(result);
    result = dsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 1);
    ERRCHECK(result);
    result = channel->setPaused(false);
    ERRCHECK(result);
    
    [self updateSliders];   
}


- (IBAction)playSawWave:(id)sender
{
    FMOD_RESULT result = FMOD_OK;

    result = dsp->remove();
    ERRCHECK(result);        
    result = system->playDSP(FMOD_CHANNEL_REUSE, dsp, true, &channel);
    ERRCHECK(result);
    result = channel->setVolume(0.125f);
    ERRCHECK(result);
    result = dsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 2);
    ERRCHECK(result);
    result = channel->setPaused(false);
    ERRCHECK(result);
    
    [self updateSliders];
}


- (IBAction)playTriangleWave:(id)sender
{
    FMOD_RESULT result = FMOD_OK;

    result = dsp->remove();
    ERRCHECK(result);    
    result = system->playDSP(FMOD_CHANNEL_REUSE, dsp, true, &channel);
    ERRCHECK(result);
    result = channel->setVolume(0.5f);
    ERRCHECK(result);
    result = dsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 4);
    ERRCHECK(result);
    result = channel->setPaused(false);
    ERRCHECK(result);
    
    [self updateSliders];
}


- (IBAction)playWhiteNoise:(id)sender
{
    FMOD_RESULT result = FMOD_OK;

    result = dsp->remove();
    ERRCHECK(result);    
    result = system->playDSP(FMOD_CHANNEL_REUSE, dsp, true, &channel);
    ERRCHECK(result);
    result = channel->setVolume(0.25f);
    ERRCHECK(result);
    result = dsp->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 5);
    ERRCHECK(result);
    result = channel->setPaused(false);
    ERRCHECK(result);
    
    [self updateSliders];
}


- (IBAction)stop:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (channel)
    {
        result = channel->stop();
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }
    
    [self updateSliders];
}


- (IBAction)frequencyChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (channel)
    {
        result = channel->setFrequency(frequency.value);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }
}


- (IBAction)volumeChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (channel)
    {
        result = channel->setVolume(volume.value);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }    
}


- (IBAction)panChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (channel)
    {
        result = channel->setPan(pan.value);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }    
}


- (void)dealloc 
{
	[status release];
    [frequencyvalue release];
    [frequency release];
    [volumevalue release];
    [volume release];
    [panvalue release];
    [pan release];
	[super dealloc];
}

@end
