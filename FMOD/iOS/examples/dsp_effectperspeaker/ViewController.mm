#import "ViewController.h"

@implementation ViewController

@synthesize channels;
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


- (void)viewDidLoad
{
    system      = NULL;
    sound       = NULL;
    channel     = NULL;
    dsplowpass  = NULL;
    dspchorus   = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT          result                 = FMOD_OK;
    char                 buffer[200]            = {0};
    unsigned int         version                = 0;
    FMOD::DSP           *dsphead                = 0;
    FMOD::DSP           *dspchannelmixer        = 0;
    FMOD::DSPConnection *dsplowpassconnection   = NULL;
    FMOD::DSPConnection *dspchorusconnection    = NULL;

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
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound);
    ERRCHECK(result);

    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);

    /*
        Create the DSP effects.
    */  
    result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsplowpass);
    ERRCHECK(result);

    result = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &dspchorus);
    ERRCHECK(result);

    /*
        Connect up the DSP network
    */

    /*
        When a sound is played, a subnetwork is set up in the DSP network which looks like this.
        Wavetable is the drumloop sound, and it feeds its data from right to left.

        [DSPHEAD]<------------[DSPCHANNELMIXER]
    */  
    result = system->getDSPHead(&dsphead);
    ERRCHECK(result);

    result = dsphead->getInput(0, &dspchannelmixer, NULL);
    ERRCHECK(result);

    /*
        Now disconnect channeldsp head from wavetable to look like this.

        [DSPHEAD]             [DSPCHANNELMIXER]
    */
    result = dsphead->disconnectFrom(dspchannelmixer);
    ERRCHECK(result);

    /*
        Now connect the 2 effects to channeldsp head.  
		Store the 2 connections this makes so we can set their speakerlevels later.

                  [DSPLOWPASS]
                 /x           
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y           
                  [DSPCHORUS]
    */
    result = dsphead->addInput(dsplowpass, &dsplowpassconnection);  /* x = dsplowpassconnection */
    ERRCHECK(result);
    result = dsphead->addInput(dspchorus, &dspchorusconnection);    /* y = dspchorusconnection */
    ERRCHECK(result);
    
    /*
        Now connect the wavetable to the 2 effects

                  [DSPLOWPASS]
                 /x          \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y          /
                  [DSPCHORUS]
    */
    result = dsplowpass->addInput(dspchannelmixer, NULL);  /* Null for connection - we dont care about it. */
    ERRCHECK(result);
    result = dspchorus->addInput(dspchannelmixer, NULL);   /* Null for connection - we dont care about it. */
    ERRCHECK(result);

    /*
        Now the drumloop will be twice as loud, because it is being split into 2, then recombined at the end.
        What we really want is to only feed the dspchannelmixer->dsplowpass through the left speaker, and 
        dspchannelmixer->dspchorus to the right speaker.
        We can do that simply by setting the pan, or speaker levels of the connections.

                  [DSPLOWPASS]
                 /x=1,0      \
        [DSPHEAD]             [DSPCHANNELMIXER]
                 \y=0,1      /
                  [DSPCHORUS]
    */
    {
        float leftinputon[2]  = { 1.0f, 0.0f };
        float rightinputon[2] = { 0.0f, 1.0f };
        float inputsoff[2]    = { 0.0f, 0.0f };

        result = dsplowpassconnection->setLevels(FMOD_SPEAKER_FRONT_LEFT, leftinputon, 2);
        ERRCHECK(result);
        result = dsplowpassconnection->setLevels(FMOD_SPEAKER_FRONT_RIGHT, inputsoff, 2);
        ERRCHECK(result);

        result = dspchorusconnection->setLevels(FMOD_SPEAKER_FRONT_LEFT, inputsoff, 2);
        ERRCHECK(result);
        result = dspchorusconnection->setLevels(FMOD_SPEAKER_FRONT_RIGHT, rightinputon, 2);
        ERRCHECK(result);
    }

    result = dsplowpass->setBypass(true);
    result = dspchorus->setBypass(true);

    result = dsplowpass->setActive(true);
    result = dspchorus->setActive(true);    
    
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
    
    if (dsplowpass)
    {
        dsplowpass->release();
        dsplowpass = NULL;
    }
    
    if (dspchorus)
    {
        dspchorus->release();
        dspchorus = NULL;
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
    FMOD_RESULT result          = FMOD_OK;
    float       currentpan      = 0.0f;
    int         channelsplaying = 0;
    
    if (channel != NULL)
    {
        result = channel->getPan(&currentpan);
        ERRCHECK(result);
    }
    
    if (system != NULL)
    {
        result = system->getChannelsPlaying(&channelsplaying);
        ERRCHECK(result);
        
        result = system->update();
        ERRCHECK(result);
    }
    
    channels.text = [NSString stringWithFormat:@"%d", channelsplaying];
    panvalue.text = [NSString stringWithFormat:@"%.2f", currentpan];
}


- (IBAction)toggleLowpass:(id)sender
{
    FMOD_RESULT result = FMOD_OK;  
    static bool bypass = false;
    
    result = dsplowpass->setBypass(bypass);
    ERRCHECK(result);    
    
    bypass = !bypass;
}


- (IBAction)toggleChorus:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    static bool bypass = false;
    
    result = dspchorus->setBypass(bypass);
    ERRCHECK(result);
    
    bypass = !bypass;
}


- (IBAction)panValueChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = channel->setPan(pan.value);
    ERRCHECK(result);    
}


- (void)dealloc 
{   
  	[channels release];
  	[pan release];
	[super dealloc];
}

@end
