#import "ViewController.h"

@implementation ViewController

@synthesize time;
@synthesize status;
@synthesize buffered;
@synthesize lasttag;
@synthesize urltext;


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

    result = system->init(1, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL);
    ERRCHECK(result);
    
    /*
        Bump up the file buffer size a little bit for netstreams (to account for lag)
    */
    result = system->setStreamBufferSize(64 * 1024, FMOD_TIMEUNIT_RAWBYTES);
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


- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    FMOD_RESULT result      = FMOD_OK;
    char        buffer[200] = {0};
    
    if (textField == urltext)
    {
        [textField resignFirstResponder];
        
        if (channel != NULL)
        {
            channel->stop();
            channel = NULL;
        }
        
        if (sound != NULL)
        {
            sound->release();
            sound = NULL;
        }
        
        [urltext.text getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];     
        result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_NONBLOCKING, NULL, &sound);
        ERRCHECK(result);       
    }
    
    return YES;
}


- (void)timerUpdate:(NSTimer *)timer 
{
    /*
        Main loop
    */    
    FMOD_RESULT     result          = FMOD_OK;
    FMOD_OPENSTATE  openstate       = (FMOD_OPENSTATE)0;
    unsigned int    percentbuffered = 0;
    bool            starving        = false;
    bool            playing         = false;
    bool            paused          = false;
    unsigned int    ms              = 0;    
    
    if (sound != NULL)
    {
        result = sound->getOpenState(&openstate, &percentbuffered, &starving);
        if (result == FMOD_ERR_FILE_NOTFOUND)
        {
            sound->release();
            sound = NULL;
            
            UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:@"Location Not Found" message:@"The URL specified could not be found." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [errorAlert show];
            [errorAlert release];
        }
        else
        {
            ERRCHECK(result);
        }
        
        if ((openstate == FMOD_OPENSTATE_READY) && (channel == NULL))
        {
            result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
            ERRCHECK(result);
        }
    }
    
    if (channel != NULL)
    {
        while (true)
        {
            FMOD_TAG tag;
            
            result = sound->getTag(NULL, -1, &tag);
            if (result == FMOD_ERR_TAGNOTFOUND)
            {
                break;
            }
            ERRCHECK(result);            
            
            if (tag.datatype == FMOD_TAGDATATYPE_STRING)
            {
                lasttag.text = [NSString stringWithFormat:@"%s = %s (%d bytes)", tag.name, tag.data, tag.datalen];
            }
        }
        
        result = channel->isPlaying(&playing);
        ERRCHECK(result);
        
        result = channel->getPaused(&paused);
        ERRCHECK(result);

        result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
        ERRCHECK(result);
        
        result = channel->setMute(starving);
        ERRCHECK(result);
    } 
          
    result = system->update();
    ERRCHECK(result);
    
    time.text       = [NSString stringWithFormat:@"%02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100];
    status.text     = (starving ? @"Starving!" : (openstate == FMOD_OPENSTATE_BUFFERING ? @"Buffering" : (openstate == FMOD_OPENSTATE_CONNECTING ? @"Connecting" : (paused ? @"Paused" : (playing ? @"Playing" : @"Stopped")))));
    buffered.text   = [NSString stringWithFormat:@"%d%%", percentbuffered];
}


- (IBAction)pauseResume:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        paused  = false;
    
    if (channel != NULL)
    {
        result = channel->getPaused(&paused);
        ERRCHECK(result);
        
        result = channel->setPaused(!paused);
        ERRCHECK(result);
    }
}


- (void)dealloc 
{  
	[time release];
	[status release];
	[buffered release];
	[lasttag release];
	[urltext release];    
	[super dealloc];
}

@end
