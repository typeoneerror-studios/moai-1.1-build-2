#import "ViewController.h"

const int   NUMSOUNDS               = 32;
const char *soundname[NUMSOUNDS]    = { "e.ogg",   /* Ma-    */
                                        "d.ogg",   /* ry     */
                                        "c.ogg",   /* had    */
                                        "d.ogg",   /* a      */
                                        "e.ogg",   /* lit-   */
                                        "e.ogg",   /* tle    */
                                        "e.ogg",   /* lamb,  */
                                        "e.ogg",   /* .....  */
                                        "d.ogg",   /* lit-   */
                                        "d.ogg",   /* tle    */
                                        "d.ogg",   /* lamb,  */
                                        "d.ogg",   /* .....  */
                                        "e.ogg",   /* lit-   */
                                        "e.ogg",   /* tle    */
                                        "e.ogg",   /* lamb,  */
                                        "e.ogg",   /* .....  */
                                        
                                        "e.ogg",   /* Ma-    */
                                        "d.ogg",   /* ry     */
                                        "c.ogg",   /* had    */
                                        "d.ogg",   /* a      */
                                        "e.ogg",   /* lit-   */
                                        "e.ogg",   /* tle    */
                                        "e.ogg",   /* lamb,  */
                                        "e.ogg",   /* its    */
                                        "d.ogg",   /* fleece */
                                        "d.ogg",   /* was    */
                                        "e.ogg",   /* white  */
                                        "d.ogg",   /* as     */
                                        "c.ogg",   /* snow.  */
                                        "c.ogg",   /* .....  */
                                        "c.ogg",   /* .....  */
                                        "c.ogg",   /* .....  */ };

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
    system      = NULL;
    sound       = NULL;
    subsound[0] = NULL;
    subsound[1] = NULL;
    channel     = NULL;
    subsoundid  = 0;
    sentenceid  = 0;
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

    result = system->init(1, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL);
    ERRCHECK(result);
    
    /*
        Set up the FMOD_CREATESOUNDEXINFO structure for the user stream with room for 2 subsounds (our subsound double buffer)
    */
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.defaultfrequency = 44100;
    exinfo.numsubsounds     = 2;
    exinfo.numchannels      = 1;
    exinfo.format           = FMOD_SOUND_FORMAT_PCM16;

    /*
        Create the 'parent' stream that contains the substreams.  Set it to loop so that it loops between subsound 0 and 1
    */
    result = system->createStream(NULL, FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &sound);
    ERRCHECK(result);

    /*
        Add 2 of our streams as children of the parent.  They should be the same format (ie mono/stereo and bitdepth) as the parent sound.
        When subsound 0 has finished and it is playing subsound 1, we will swap subsound 0 with a new sound, and the same for when subsound 1 has finished,
        causing a continual double buffered flip, which means continuous sound
    */
    [[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], soundname[0]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createStream(buffer, FMOD_DEFAULT, 0, &subsound[0]);
    ERRCHECK(result);

    [[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], soundname[1]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createStream(buffer, FMOD_DEFAULT, 0, &subsound[1]);
    ERRCHECK(result);

    result = sound->setSubSound(0, subsound[0]);
    ERRCHECK(result);

    result = sound->setSubSound(1, subsound[1]);
    ERRCHECK(result);

    /*
        Set up the gapless sentence to contain these first 2 streams
    */
    {
        int soundlist[2] = { 0, 1 };
     
        result = sound->setSubSoundSentence(soundlist, 2);
        ERRCHECK(result);
    }

    subsoundid = 0;     
    sentenceid = 2;     /* The next sound to be appeneded to the stream */

    /*
        Play the sound
    */
    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
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
    FMOD_RESULT     result              = FMOD_OK;
    unsigned int    currentsubsoundid   = 0;
    char            buffer[200]         = {0};

    /*
        Replace the subsound that just finished with a new subsound, to create endless seamless stitching!

        Note that this polls the currently playing subsound using the FMOD_TIMEUNIT_BUFFERED flag.  
        Remember streams are decoded / buffered ahead in advance! 
        Don't use the 'audible time' which is FMOD_TIMEUNIT_SENTENCE_SUBSOUND by itself.  When streaming, sound is 
        processed ahead of time, and things like stream buffer / sentence manipulation (as done below) is required 
        to be in 'buffered time', or else there will be synchronization problems and you might end up releasing a
        sub-sound that is still playing!
    */
    result = channel->getPosition(&currentsubsoundid, (FMOD_TIMEUNIT)(FMOD_TIMEUNIT_SENTENCE_SUBSOUND | FMOD_TIMEUNIT_BUFFERED));
    ERRCHECK(result);

    if (currentsubsoundid != subsoundid)
    {
        /* 
            Release the sound that isn't playing any more. 
        */
        result = subsound[subsoundid]->release();       
        ERRCHECK(result);

        /* 
            Replace it with a new sound in our list.
        */
        [[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], soundname[sentenceid]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
        result = system->createStream(buffer, FMOD_DEFAULT, 0, &subsound[subsoundid]);
        ERRCHECK(result);

        result = sound->setSubSound(subsoundid, subsound[subsoundid]);
        ERRCHECK(result);

        status.text = [NSString stringWithFormat:@"Replacing subsound %d / 2 with sound %d / %d", subsoundid, sentenceid, NUMSOUNDS];
        
        sentenceid++;
        if (sentenceid >= NUMSOUNDS)
        {
            sentenceid = 0;
        }

        subsoundid = currentsubsoundid;
    }
     
    if (system != NULL)
    {
        result = system->update();
        ERRCHECK(result);
    }
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
	[status release];
	[super dealloc];
}

@end
