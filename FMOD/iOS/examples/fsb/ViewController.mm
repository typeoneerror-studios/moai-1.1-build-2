#import "ViewController.h"

@implementation ViewController

@synthesize time;
@synthesize status;
@synthesize channels;
@synthesize subsoundpicker;


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
    system          = NULL;
    fsb             = NULL;
    channel         = NULL;
    numsubsounds    = 0;
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
    
    [[NSString stringWithFormat:@"%@/example.fsb", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE, NULL, &fsb);
    ERRCHECK(result);
    
    result = fsb->getNumSubSounds(&numsubsounds);
    ERRCHECK(result);
    
    subsoundpickerdata = [[NSMutableArray arrayWithCapacity:10] retain];
    for (int subsoundindex = 0; subsoundindex < numsubsounds; subsoundindex++)
    {
        FMOD::Sound *subsound  = NULL;
        char         name[256] = {0};
        
        result = fsb->getSubSound(subsoundindex, &subsound);
        ERRCHECK(result);
        
        result = subsound->getName(name, 256);
        ERRCHECK(result);
        
        [subsoundpickerdata addObject:[NSString stringWithFormat:@"%s", name]];
    }
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    [subsoundpickerdata release];
    
    if (fsb)
    {
        fsb->release();
        fsb = NULL;
    }

    if (system)
    {
        system->release();
        system = NULL;
    }    
}


- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [subsoundpickerdata objectAtIndex:row];
}


- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
	return [subsoundpickerdata count];
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


- (IBAction)playSound:(id)sender
{
    FMOD_RESULT  result     = FMOD_OK;
    FMOD::Sound *subsound   = NULL;
    NSInteger    row        = [subsoundpicker selectedRowInComponent:0];
    
    if (row >= 0 && row < numsubsounds)
    {
        result = fsb->getSubSound(row, &subsound);
        ERRCHECK(result);        
        
        result = system->playSound(FMOD_CHANNEL_FREE, subsound, false, &channel);
        ERRCHECK(result);
    }
}


- (void)dealloc 
{   
	[time release];
	[status release];
  	[channels release];
	[super dealloc];
}

@end
