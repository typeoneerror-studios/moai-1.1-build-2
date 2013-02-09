#import "ViewController.h"

@implementation ViewController

const int   INTERFACE_UPDATETIME    = 50;       // 50ms update for interface
const float DISTANCEFACTOR          = 1.0f;     // Units per meter, i.e feet would = 3.28, centimeters would = 100

@synthesize listener;


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
    system              = NULL;
    sound1              = NULL;
    sound2              = NULL;
    sound3              = NULL;
    channel1            = NULL;
    channel2            = NULL;
    channel3            = NULL;
    listenermoveflag    = true;
    listenerpos.x       = 0.0f;
    listenerpos.y       = 0.0f;
    listenerpos.z       = -1.0f * DISTANCEFACTOR;
    
    [listener setEnabled:!listenermoveflag];
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
    
    /*
        Set the distance units. (meters/feet etc)
    */
    result = system->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
    ERRCHECK(result);    
    
    
    /*
        Load some sounds
    */    
    [[NSString stringWithFormat:@"%@/drumloop.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_3D | FMOD_LOOP_NORMAL, NULL, &sound1);
    ERRCHECK(result);
    result = sound1->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    ERRCHECK(result);    
    
    [[NSString stringWithFormat:@"%@/jaguar.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_3D | FMOD_LOOP_NORMAL, NULL, &sound2);
    ERRCHECK(result);
    result = sound2->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    ERRCHECK(result);

    [[NSString stringWithFormat:@"%@/swish.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_2D, NULL, &sound3);
    ERRCHECK(result);
    
    /*
        Play sounds at certain positions
    */
    {
        FMOD_VECTOR pos = { -10.0f * DISTANCEFACTOR, 0.0f, 0.0f };
        FMOD_VECTOR vel = {  0.0f, 0.0f, 0.0f };

        result = system->playSound(FMOD_CHANNEL_FREE, sound1, true, &channel1);
        ERRCHECK(result);
        result = channel1->set3DAttributes(&pos, &vel);
        ERRCHECK(result);
        result = channel1->setPaused(false);
        ERRCHECK(result);
    }

    {
        FMOD_VECTOR pos = { 15.0f * DISTANCEFACTOR, 0.0f, 0.0f };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

        result = system->playSound(FMOD_CHANNEL_FREE, sound2, true, &channel2);
        ERRCHECK(result);
        result = channel2->set3DAttributes(&pos, &vel);
        ERRCHECK(result);
        result = channel2->setPaused(false);
        ERRCHECK(result);
    }    

    timer = [NSTimer scheduledTimerWithTimeInterval:(INTERFACE_UPDATETIME / 1000.0f) target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
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
    FMOD_RESULT result = FMOD_OK;

    // ==========================================================================================
    // UPDATE THE LISTENER
    // ==========================================================================================
    {
        static float        time            = 0;
        static FMOD_VECTOR  lastlistenerpos = { 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR         forward         = { 0.0f, 0.0f, 1.0f };
        FMOD_VECTOR         up              = { 0.0f, 1.0f, 0.0f };
        FMOD_VECTOR         vel             = { 0.0f, 0.0f, 0.0f };
        
        if (listenermoveflag)
        {
            listenerpos.x = (float)sin(time * 0.05f) * 33.0f * DISTANCEFACTOR;     // left right pingpong
            [listener setValue:listenerpos.x];
        }
        
        // ********* NOTE ******* READ NEXT COMMENT!!!!!
        // vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
        vel.x = (listenerpos.x - lastlistenerpos.x) * (1000 / INTERFACE_UPDATETIME);
        vel.y = (listenerpos.y - lastlistenerpos.y) * (1000 / INTERFACE_UPDATETIME);
        vel.z = (listenerpos.z - lastlistenerpos.z) * (1000 / INTERFACE_UPDATETIME);
        
        // Store pos for next time
        lastlistenerpos = listenerpos;
        
        result = system->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up);
        ERRCHECK(result);
        
        time += (30 * (1.0f / (float)INTERFACE_UPDATETIME));    // time value .. it increments in 30m/s steps in this example
    }    

    if (system != NULL)
    {       
        result = system->update();
        ERRCHECK(result);
    }
}


- (IBAction)togglePauseSound1:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        paused  = false;
    
    result = channel1->getPaused(&paused);
    ERRCHECK(result);
    
    result = channel1->setPaused(!paused);
    ERRCHECK(result);  
}


- (IBAction)togglePauseSound2:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    bool        paused  = false;
    
    result = channel2->getPaused(&paused);
    ERRCHECK(result);
    
    result = channel2->setPaused(!paused);
    ERRCHECK(result);   
}


- (IBAction)playSound3:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = system->playSound(FMOD_CHANNEL_FREE, sound3, false, &channel3);
    ERRCHECK(result);    
}


- (IBAction)startStopListenerMove:(id)sender
{
    [listener setEnabled:listenermoveflag];
    listenermoveflag = !listenermoveflag;
}


- (IBAction)listenerMove:(id)sender
{
    listenerpos.x = listener.value * DISTANCEFACTOR;
}


- (void)dealloc 
{   
	[listener release];
	[super dealloc];
}

@end
