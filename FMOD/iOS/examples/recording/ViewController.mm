#import "ViewController.h"

@implementation ViewController

@synthesize recordpos;
@synthesize playpos;
@synthesize looping;
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
    system      = NULL;
    sound       = NULL;
    channel     = NULL;
    loopFlag    = false;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT             result      = FMOD_OK;
    char                    buffer[200] = {0};
    unsigned int            version     = 0;
    FMOD_CREATESOUNDEXINFO  exinfo      = {0};
   
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
    
    exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.numchannels      = 1;
    exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
    exinfo.defaultfrequency = 44100;
    exinfo.length           = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 5;
    
    result = system->createSound(NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
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
    FMOD_RESULT     result          = FMOD_OK;
    bool            recording       = false;
    bool            playing         = false;
    unsigned int    currentrecpos   = 0;
    unsigned int    currentplaypos  = 0;    
       
    result = system->isRecording(0, &recording);
    ERRCHECK(result);
    
    result = system->getRecordPosition(0, &currentrecpos);
    ERRCHECK(result);
    
    if (channel != NULL)
    {
        result = channel->isPlaying(&playing);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        result = channel->getPosition(&currentplaypos, FMOD_TIMEUNIT_PCM);
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
    
    status.text     = recording ? (playing ? @"Recording / Playing" : @"Recording") : (playing ? @"Playing" : @"Idle");
    playpos.text    = [NSString stringWithFormat:@"%d", currentplaypos]; 
    recordpos.text  = [NSString stringWithFormat:@"%d", currentrecpos];
    looping.text    = loopFlag ? @"On" : @"Off";

}


- (IBAction)record:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = system->recordStart(0, sound, loopFlag);
    ERRCHECK(result);
}


- (IBAction)play:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (loopFlag)
    {
        result = sound->setMode(FMOD_LOOP_NORMAL);
        ERRCHECK(result);
    }
    else
    {
        result = sound->setMode(FMOD_LOOP_OFF);
        ERRCHECK(result);        
    }
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);
}


- (IBAction)toggleLooping:(id)sender
{
    loopFlag = !loopFlag;
}


- (IBAction)stop:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = system->recordStop(0);
    ERRCHECK(result);
    
    if (channel)
    {
        channel->stop();
        channel = NULL;
    }
    
}


- (void)dealloc 
{   
	[recordpos release];
  	[playpos release];
    [looping release];
	[status release];
	[super dealloc];
}

@end
