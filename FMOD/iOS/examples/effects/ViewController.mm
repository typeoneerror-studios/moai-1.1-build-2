#import "ViewController.h"

@implementation ViewController

@synthesize status;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


- (void)viewDidLoad
{
    system  = NULL;
    sound   = NULL;
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
    
    [[NSString stringWithFormat:@"%@/drumloop.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE, NULL, &sound);
    ERRCHECK(result);
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);
    
    result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsplowpass);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &dsphighpass);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dspecho);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &dspflange);
    ERRCHECK(result);    
    result = system->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &dspdistortion);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &dspchorus);
    ERRCHECK(result);
    result = system->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &dspparameq);
    ERRCHECK(result);
    
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
    FMOD_RESULT result  = FMOD_OK;
    bool        playing = false;
    bool        paused  = false;
    
    if (channel != NULL)
    {
        FMOD::Sound *currentsound = NULL;
        
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
    }
    
    if (system != NULL)
    {       
        result = system->update();
        ERRCHECK(result);
    }
    
    status.text = (paused ? @"Paused" : (playing ? @"Playing" : @"Stopped"));
}


- (IBAction)playPause:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        paused  = false;
    
    result = channel->getPaused(&paused);
    ERRCHECK(result);
    
    result = channel->setPaused(!paused);
    ERRCHECK(result);
}


- (IBAction)toggleLowpass:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dsplowpass->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dsplowpass->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dsplowpass, NULL);
        ERRCHECK(result);        
    }    
}


- (IBAction)toggleHighpass:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dsphighpass->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dsphighpass->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dsphighpass, NULL);
        ERRCHECK(result);        
    }    
}


- (IBAction)toggleEcho:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dspecho->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dspecho->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dspecho, NULL);
        ERRCHECK(result);
        
        result = dspecho->setParameter(FMOD_DSP_ECHO_DELAY, 50.0f);
        ERRCHECK(result);        
    }    
}


- (IBAction)toggleFlange:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dspflange->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dspflange->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dspflange, NULL);
        ERRCHECK(result);        
    }    
}


- (IBAction)toggleDistortion:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dspdistortion->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dspdistortion->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dspdistortion, NULL);
        ERRCHECK(result);
        
        result = dspdistortion->setParameter(FMOD_DSP_DISTORTION_LEVEL, 0.8f);
        ERRCHECK(result);         
    }    
}


- (IBAction)toggleChorus:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dspchorus->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dspchorus->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dspchorus, NULL);
        ERRCHECK(result);        
    }    
}


- (IBAction)toggleParamEq:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        active  = false;
    
    result = dspparameq->getActive(&active);
    ERRCHECK(result);
    
    if (active)
    {
        result = dspparameq->remove();
        ERRCHECK(result);        
    }
    else
    {
        result = system->addDSP(dspparameq, NULL);
        ERRCHECK(result);
        
        result = dspparameq->setParameter(FMOD_DSP_PARAMEQ_CENTER, 5000.0f);
        ERRCHECK(result); 
        
        result = dspparameq->setParameter(FMOD_DSP_PARAMEQ_GAIN, 0.0f);
        ERRCHECK(result);         
    }    
}


- (void)dealloc 
{   
	[status release];
	[super dealloc];
}

@end
