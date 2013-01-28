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
        fprintf(stderr, "You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
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
    
    [[NSString stringWithFormat:@"%@/swish.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[2]);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/c.ogg", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[3]);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/d.ogg", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[4]);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/e.ogg", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound[5]);
    ERRCHECK(result);
    
    /*
        Instead of being independent, set the group A and B to be children of the master group
    */    
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
    for (int i = 0; i < 6; i++)
    {
        result = system->playSound(FMOD_CHANNEL_FREE, sound[i], true, &channel[i]);
        ERRCHECK(result);
        
        if (i < 3)
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
        Change the volume of each group, just because we can!  (And makes it less noise)
    */
    result = groupA->setVolume(0.5f);
    ERRCHECK(result);
    
    result = groupB->setVolume(0.5f);
    ERRCHECK(result);    
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        A little fade out (over 2 seconds)
    */
    {
        FMOD_RESULT result  = FMOD_OK;
        float       pitch   = 1.0f;
        float       vol     = 1.0f;

        for (int i = 0; i < 200; i++)
        {
            result = masterGroup->setPitch(pitch);
            ERRCHECK(result);
            
            result = masterGroup->setVolume(vol);
            ERRCHECK(result);

            vol   -= (1.0f / 200.0f);
            pitch -= (0.5f / 200.0f);

            usleep(1000 * 10);
        }
    }    
    
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
    bool        mute    = false;
    
    result = groupA->getMute(&mute);
    ERRCHECK(result);
    
    result = groupA->setMute(!mute);
    ERRCHECK(result);
}


- (IBAction)toggleMuteB:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        mute    = false;
    
    result = groupB->getMute(&mute);
    ERRCHECK(result);
    
    result = groupB->setMute(!mute);
    ERRCHECK(result);    
}


- (IBAction)toggleMuteMaster:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        mute    = false;
    
    result = masterGroup->getMute(&mute);
    ERRCHECK(result);
    
    result = masterGroup->setMute(!mute);
    ERRCHECK(result);   
}


- (void)dealloc 
{   
  	[channels release];
	[super dealloc];
}

@end
