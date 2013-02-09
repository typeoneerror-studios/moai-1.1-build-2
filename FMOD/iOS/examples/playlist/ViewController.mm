#import "ViewController.h"

@implementation ViewController

@synthesize time;
@synthesize status;
@synthesize soundtitle;


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
    playlist    = NULL;
    channel     = NULL;
    isplaylist  = false;
    listindex   = 0;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT     result          = FMOD_OK;
    char            buffer[200]     = {0};
    unsigned int    version         = 0;
    FMOD_SOUND_TYPE soundtype       = (FMOD_SOUND_TYPE)0; 

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
    
    [[NSString stringWithFormat:@"%@/playlist.m3u", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_DEFAULT, NULL, &playlist);
    ERRCHECK(result);
    
    result = playlist->getFormat(&soundtype, NULL, NULL, NULL);
    ERRCHECK(result);

    isplaylist = (soundtype == FMOD_SOUND_TYPE_PLAYLIST);

    if (isplaylist)
    {
        FMOD_TAG tag;
        
        /*
            Get the first song in the playlist, create the sound and then play it
        */
        result = playlist->getTag("FILE", listindex, &tag);
        ERRCHECK(result);

        [[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], (char*)tag.data] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
        result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_OFF, NULL, &sound);
        ERRCHECK(result);

        result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
        ERRCHECK(result);

        result = playlist->getTag("TITLE", listindex, &tag);
        ERRCHECK(result);
        
        soundtitle.text = [NSString stringWithFormat:@"%s", (char *)tag.data];
        listindex++;
    }
    else
    {
        /*
            This is just a normal sound, so just play it
        */
        sound = playlist;

        result = sound->setMode(FMOD_LOOP_NORMAL);
        ERRCHECK(result);

        result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
        ERRCHECK(result);
    }    

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
    
    if (isplaylist && playlist)
    {
        playlist->release();
        playlist = NULL;
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
    FMOD_RESULT     result  = FMOD_OK;
    bool            playing = false;
    bool            paused  = false;
    unsigned int    ms      = 0;
    unsigned int    lenms   = 0;
    
    if (channel && isplaylist)
    {
        /*
            When sound has finished playing, play the next sound in the playlist
        */
        result = channel->isPlaying(&playing);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        if (!playing)
        {
            FMOD_TAG tag;
            
            if (sound)
            {
                sound->release();
                sound = NULL;
            }

            result = playlist->getTag("FILE", listindex, &tag);
            if (result == FMOD_OK)
            {
                char buffer[200] = {0};
                
                [[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], (char*)tag.data] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
                result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_OFF, NULL, &sound);
                ERRCHECK(result);
                
                result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
                ERRCHECK(result);
                
                result = playlist->getTag("TITLE", listindex, &tag);
                ERRCHECK(result);
                
                soundtitle.text = [NSString stringWithFormat:@"%s", (char *)tag.data];
                listindex++;
                
            }
            else if (result == FMOD_ERR_TAGNOTFOUND)
            {
                listindex = 0;
            }
            else
            {
                ERRCHECK(result);
            }
        }
    }    
    
    
    if (channel)
    {
        if (sound)
        {
            result = sound->getLength(&lenms, FMOD_TIMEUNIT_MS);
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
    }
    
    if (system != NULL)
    {        
        result = system->update();
        ERRCHECK(result);
    }
    
    time.text   = [NSString stringWithFormat:@"%02d:%02d:%02d / %02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100];
    status.text = (paused ? @"Paused" : (playing ? @"Playing" : @"Stopped"));
}


- (IBAction)playNext:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    /*
        Play the next song in the playlist
    */
    if (channel && isplaylist)
    {
        result = channel->stop();
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }
}


- (IBAction)pauseResume:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    bool        paused = false;
    
    result = channel->getPaused(&paused);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }
    
    result = channel->setPaused(!paused);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }
}


- (void)dealloc 
{   
	[time release];
	[status release];
  	[soundtitle release];
	[super dealloc];
}

@end
