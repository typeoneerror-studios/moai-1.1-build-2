#import "ViewController.h"

@implementation ViewController


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels) 
{ 
    unsigned int    userdata    = 0;
    char            name[256]   = {0}; 
    FMOD::DSP      *thisdsp     = (FMOD::DSP *)dsp_state->instance; 

    /* 
        This redundant call just shows using the instance parameter of FMOD_DSP_STATE and using it to 
        call a DSP information function. 
    */
    thisdsp->getInfo(name, 0, 0, 0, 0);

    thisdsp->getUserData((void **)&userdata);

    /*
        This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0' 
        as the number of channels in FMOD_DSP_DESCRIPTION.  
        Specifying an actual channel count will mean you have to take care of any number of channels coming in,
        but outputting the number of channels specified.  Generally it is best to keep the channel 
        count at 0 for maximum compatibility.
    */
    for (unsigned int sample = 0; sample < length; sample++) 
    { 
        /*
            Feel free to unroll this.
        */
        for (int chan = 0; chan < outchannels; chan++)
        {
            /* 
                This DSP filter just halves the volume! 
                Input is modified, and sent to output.
            */
            outbuffer[(sample * outchannels) + chan] = inbuffer[(sample * inchannels) + chan] * 0.2f;
        }
    } 

    return FMOD_OK; 
}


- (void)viewDidLoad
{
    system  = NULL;
    sound   = NULL;
    channel = NULL;
    mydsp   = NULL;
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
    
    [[NSString stringWithFormat:@"%@/wave.mp3", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound);
    ERRCHECK(result);

    /*
        Play the sound
    */    
    result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    ERRCHECK(result);
    
    /*
        Create the DSP effects
    */  
    { 
        FMOD_DSP_DESCRIPTION dspdesc; 

        memset(&dspdesc, 0, sizeof(FMOD_DSP_DESCRIPTION)); 

        strcpy(dspdesc.name, "My first DSP unit"); 
        dspdesc.channels     = 0;                   // 0 = whatever comes in, else specify. 
        dspdesc.read         = myDSPCallback; 
        dspdesc.userdata     = (void *)0x12345678; 

        result = system->createDSP(&dspdesc, &mydsp); 
        ERRCHECK(result); 
    } 

    /*
        Inactive by default
    */
    result = mydsp->setBypass(true);
    ERRCHECK(result); 

    result = system->addDSP(mydsp, NULL);
    ERRCHECK(result); 
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    if (mydsp)
    {
        mydsp->release();
        mydsp = NULL;
    }
    
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
    FMOD_RESULT result = FMOD_OK;
           
    result = system->update();
    ERRCHECK(result);
}


- (IBAction)toggleActive:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    static bool active  = false;
    
    result = mydsp->setBypass(active);
    ERRCHECK(result);
    
    active = !active;
}


- (void)dealloc 
{   
	[super dealloc];
}

@end
