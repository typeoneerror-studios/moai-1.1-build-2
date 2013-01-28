#import "ViewController.h"

@implementation ViewController

@synthesize channels;


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
    groupA      = NULL;
    groupB      = NULL;
    masterGroup = NULL;
    dspreverb   = NULL;
    dspflange   = NULL;
    dsplowpass  = NULL;
    
    memset(sound, 0, sizeof(sound));
    memset(channel, 0, sizeof(sound));
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
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }
    
    result = system->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/drumloop.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[0]);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/jaguar.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[1]);
    ERRCHECK(result);  
    
    [[NSString stringWithFormat:@"%@/c.ogg", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[2]);
    ERRCHECK(result);  
    
    [[NSString stringWithFormat:@"%@/d.ogg", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[3]);
    ERRCHECK(result);  
    
    [[NSString stringWithFormat:@"%@/e.ogg", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[4]);
    ERRCHECK(result);      
        
    result = system->createChannelGroup("Group A", &groupA);
    ERRCHECK(result);
    
    result = system->createChannelGroup("Group B", &groupB);
    ERRCHECK(result);
    
    result = system->getMasterChannelGroup(&masterGroup);
    ERRCHECK(result);
    
    result = masterGroup->addGroup(groupA);
    ERRCHECK(result);
    
    result = masterGroup->addGroup(groupB);
    ERRCHECK(result);    

    /*
        Start all the sounds!
    */
    for (int i = 0; i < 5; i++)
    {
        result = system->playSound(FMOD_CHANNEL_FREE, sound[i], true, &channel[i]);
        ERRCHECK(result);
        
        if (i < 2)
        {
            result = channel[i]->setChannelGroup(groupA);
            ERRCHECK(result);            
        }
        else
        {
            result = channel[i]->setChannelGroup(groupB);
            ERRCHECK(result);
        }

        result = channel[i]->setPaused(false);
        ERRCHECK(result);
    }

    /*
        Create the DSP effects we want to apply to our submixes
    */  
    result = system->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &dspreverb);
    ERRCHECK(result);

    result = system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &dspflange);
    ERRCHECK(result);
    result = dspflange->setParameter(FMOD_DSP_FLANGE_RATE, 1.0f);
    ERRCHECK(result);

    result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsplowpass);
    ERRCHECK(result);
    result = dsplowpass->setParameter(FMOD_DSP_LOWPASS_CUTOFF, 500.0f);
    ERRCHECK(result);    
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    for (int i = 0; i < 6; i++)
    {
        if (sound[i])
        {
            sound[i]->release();           
            sound[i] = NULL;
        }
    }
    
    if (dspreverb)
    {
        dspreverb->release();
        dspreverb = NULL;
    }
    
    if (dspflange)
    {
        dspflange->release();
        dspflange = NULL;
    }
    
    if (dsplowpass)
    {
        dsplowpass->release();
        dsplowpass = NULL;
    }
    
    if (groupA)
    {
        groupA->release();
        groupA = NULL;
    }
    
    if (groupB)
    {
        groupB->release();
        groupB = NULL;
    }
    
    if (system)
    {
        system->release();
        system = NULL;
    }    
}


- (void)timerUpdate:(NSTimer *)timer 
{
    FMOD_RESULT result          = FMOD_OK;
    int         channelsplaying = 0;
    
    if (system != NULL)
    {
        result = system->getChannelsPlaying(&channelsplaying);
        ERRCHECK(result);
        
        result = system->update();
        ERRCHECK(result);
    }
    
    channels.text = [NSString stringWithFormat:@"%d", channelsplaying]; 
}


- (IBAction)toggleMuteA:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    static bool mute    = true;
    
    result = groupA->setMute(mute);
    ERRCHECK(result);
    
    mute = !mute;
}


- (IBAction)toggleMuteB:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    static bool mute    = true;
    
    result = groupB->setMute(mute);
    ERRCHECK(result);
    
    mute = !mute;
}


- (IBAction)toggleReverb:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    static bool reverb  = true;
    
    if (reverb)
    {
        groupA->addDSP(dspreverb, NULL);
        ERRCHECK(result);
    }
    else
    {
        dspreverb->remove();
        ERRCHECK(result);
    }
    
    reverb = !reverb;
}


- (IBAction)toggleFlange:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    static bool flange  = true;
    
    if (flange)
    {
        groupB->addDSP(dspflange, NULL);
        ERRCHECK(result);
    }
    else
    {
        dspflange->remove();
        ERRCHECK(result);
    }
    
    flange = !flange;
}


- (IBAction)toggleLowpass:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    static bool lowpass = true;
    
    if (lowpass)
    {
        masterGroup->addDSP(dsplowpass, NULL);
        ERRCHECK(result);
    }
    else
    {
        dsplowpass->remove();
        ERRCHECK(result);
    }
    
    lowpass = !lowpass;
}


- (void)dealloc 
{   
  	[channels release];
	[super dealloc];
}

@end
