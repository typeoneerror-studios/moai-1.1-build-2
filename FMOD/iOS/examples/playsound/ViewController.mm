#import "ViewController.h"

@implementation ViewController

@synthesize time;
@synthesize status;
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
    system  = NULL;
    sound1  = NULL;
    sound2  = NULL;
    sound3  = NULL;
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
    
    [[NSString stringWithFormat:@"%@/drumloop.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE, NULL, &sound1);
    ERRCHECK(result);
    
    result = sound1->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */
    ERRCHECK(result);                           /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */    
    
    [[NSString stringWithFormat:@"%@/jaguar.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE, NULL, &sound2);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/swish.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE, NULL, &sound3);
    ERRCHECK(result);

    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    if (sound1)
    {
        sound1->release();
        sound1 = NULL;
    }
    
    if (sound2)
    {
        sound2->release();
        sound2 = NULL;
    }
    
    if (sound3)
    {
        sound3->release();
        sound3 = NULL;
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
    int             channelsplaying = 0;
    
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
        
        result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = channel->getCurrentSound(&currentsound);       
        if (currentsound != NULL)
        {
            result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }
        }       
    }
    
    if (system != NULL)
    {
        result = system->getChannelsPlaying(&channelsplaying);
        ERRCHECK(result);
        
        result = system->update();
        ERRCHECK(result);
    }
    
    time.text       = [NSString stringWithFormat:@"%02d:%02d:%02d / %02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100];
    status.text     = (paused ? @"Paused" : (playing ? @"Playing" : @"Stopped"));
    channels.text   = [NSString stringWithFormat:@"%d", channelsplaying]; 
}


- (IBAction)playSound1:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel);
    ERRCHECK(result);
}


- (IBAction)playSound2:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound2, false, &channel);
    ERRCHECK(result);    
}


- (IBAction)playSound3:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound3, false, &channel);
    ERRCHECK(result);    
}


- (void)dealloc 
{   
	[time release];
	[status release];
  	[channels release];
	[super dealloc];
}

@end
