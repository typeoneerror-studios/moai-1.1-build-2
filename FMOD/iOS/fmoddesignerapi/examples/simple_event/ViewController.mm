#import "ViewController.h"

@implementation ViewController

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
    event       = NULL;
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
    
    result = eventSystem->getEvent("examples/FeatureDemonstration/Basics/SimpleEvent", FMOD_EVENT_DEFAULT, &event);
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
    FMOD_RESULT         result  = FMOD_OK;
    FMOD_EVENT_STATE    state   = {0};
    
    result = eventSystem->update();
    ERRCHECK(result);

    result = event->getState(&state);
    ERRCHECK(result);
    
    uiStatus.text = (state & FMOD_EVENT_STATE_PLAYING) ? @"Playing" : @"Stopped";
}


- (IBAction)playEvent:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = event->start();
    ERRCHECK(result);
}


- (void)dealloc 
{   
	[uiStatus release];
	[super dealloc];
}

@end
