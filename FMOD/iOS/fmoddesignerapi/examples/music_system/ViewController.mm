#import "ViewController.h"

@implementation ViewController

@synthesize uiRetroRockIntensityValue;
@synthesize uiRetroRockIntensity;
@synthesize uiStatus;


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
    eventSystem = NULL;
    musicSystem = NULL;
    country     = NULL;
    stealth     = NULL;
    retroRock    = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT result      = FMOD_OK;
    char        buffer[200] = {0};
   
    result = FMOD::EventSystem_Create(&eventSystem); 
    ERRCHECK(result);
    
    result = eventSystem->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/examples.fev", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = eventSystem->load(buffer, NULL, NULL);
    ERRCHECK(result);
    
    result = eventSystem->getMusicSystem(&musicSystem);
    ERRCHECK(result);

    result = musicSystem->prepareCue(MUSICCUE_EXAMPLES_COUNTRY_ARENA,    &country);
    ERRCHECK(result);
    result = musicSystem->prepareCue(MUSICCUE_EXAMPLES_STEALTH_ARENA,    &stealth);
    ERRCHECK(result);             
    result = musicSystem->prepareCue(MUSICCUE_EXAMPLES_RETRO_ROCK_ARENA, &retroRock);    
    ERRCHECK(result);
    
    /*
        Start off with country cue active
    */
    result = country->begin();
    ERRCHECK(result);    
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    [timer invalidate];
    
    if (eventSystem)
    {
        eventSystem->release();
        eventSystem = NULL;
    }    
}


- (void)timerUpdate:(NSTimer *)timer 
{
    FMOD_RESULT result = FMOD_OK;
    
    result = eventSystem->update();
    ERRCHECK(result);

    uiRetroRockIntensityValue.text = [NSString stringWithFormat:@"%.1f", uiRetroRockIntensity.value];
}


- (IBAction)startStopCountry:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    bool        active = false;
    
    result = country->isActive(&active);
    ERRCHECK(result);
    
    if (!active)
    {
        result = country->begin();
        ERRCHECK(result);
        
        uiStatus.text = @"Country cue begin.";
    }
    else
    {
        result = country->end();
        ERRCHECK(result);
        
        uiStatus.text = @"Country cue end.";
    }
}


- (IBAction)startStopStealth:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    bool        active = false;
    
    result = stealth->isActive(&active);
    ERRCHECK(result);
    
    if (!active)
    {
        result = stealth->begin();
        ERRCHECK(result);
        
        uiStatus.text = @"Stealth cue begin.";
    }
    else
    {
        result = stealth->end();
        ERRCHECK(result);
        
        uiStatus.text = @"Stealth cue end.";
    }
}


- (IBAction)startStopRetroRock:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    bool        active = false;
    
    result = retroRock->isActive(&active);
    ERRCHECK(result);
    
    if (!active)
    {
        result = retroRock->begin();
        ERRCHECK(result);
        
        uiStatus.text = @"Retro rock cue begin.";
    }
    else
    {
        result = retroRock->end();
        ERRCHECK(result);
        
        uiStatus.text = @"Retro rock cue end.";
    }
}


- (IBAction)retroRockIntensityChange:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = musicSystem->setParameterValue(MUSICPARAM_EXAMPLES_RETRO_ROCK_INTENSITY, uiRetroRockIntensity.value);
    ERRCHECK(result);
}


- (void)dealloc 
{
    [uiRetroRockIntensityValue release];
    [uiRetroRockIntensity release];
	[uiStatus release];
	[super dealloc];
}

@end
