#import "ViewController.h"

@implementation ViewController

@synthesize uiMemCurrent;
@synthesize uiMemMaximum;
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
    group       = NULL;
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
    
    result = eventSystem->getGroup("examples/FeatureDemonstration/Basics", false, &group);
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
    FMOD_RESULT result          = FMOD_OK;
    int         memoryCurrent   = 0;
    int         memoryMaximum   = 0;
        
    result = FMOD::Memory_GetStats(&memoryCurrent, &memoryMaximum);
    ERRCHECK(result);
    
    uiMemCurrent.text  = [NSString stringWithFormat:@"%d", memoryCurrent];
    uiMemMaximum.text  = [NSString stringWithFormat:@"%d", memoryMaximum];
    
    result = eventSystem->update();
    ERRCHECK(result);
}


- (IBAction)getEvent:(id)sender
{
    FMOD_RESULT result = FMOD_OK;

    result = group->getEvent("SimpleEvent", FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    uiStatus.text = @"Event data loaded.";
}


- (IBAction)freeEventData:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = group->freeEventData(event);
    ERRCHECK(result);
    
    event = NULL;
    
    uiStatus.text = @"Event data unloaded.";
}


- (IBAction)loadGroupData:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = group->loadEventData();
    ERRCHECK(result);
    
    uiStatus.text = @"Event group data unloaded.";
}


- (IBAction)freeGroupData:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = group->freeEventData();
    ERRCHECK(result);
    
    uiStatus.text = @"Event group data unloaded.";
}


- (void)dealloc 
{
	[uiMemCurrent release];
	[uiMemMaximum release];
	[uiStatus release];    
	[super dealloc];
}

@end
