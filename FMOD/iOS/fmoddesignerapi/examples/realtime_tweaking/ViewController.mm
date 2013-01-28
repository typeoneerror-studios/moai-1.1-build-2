#import "ViewController.h"

@implementation ViewController

@synthesize uiEvent;
@synthesize uiParamType;
@synthesize uiParamValue;
@synthesize uiStatus;

static const char *g_eventName[] = { "Basics/SimpleEventWithLooping", "SequencingAndStitching/LoopLogic", "3D Events/2D-3DPanMorph" };


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


FMOD_RESULT F_CALLBACK eventcallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
    if (type == FMOD_EVENT_CALLBACKTYPE_NET_MODIFIED)
    {
        FMOD_EVENT_PROPERTY  property   = *((FMOD_EVENT_PROPERTY *)&param1);
        float                value      = *((float *)&param2);
        ViewController      *view       = (ViewController*)userdata;
        
        view.uiEvent.text        = [NSString stringWithFormat:@"%p", event];
        view.uiParamType.text    = (property == FMOD_EVENTPROPERTY_VOLUME) ? @"volume" : ((property == FMOD_EVENTPROPERTY_PITCH) ? @"pitch" : @"???");
        view.uiParamValue.text   = [NSString stringWithFormat:@"%.2f", value];
    }
    
    return FMOD_OK;
}


- (void)viewDidLoad
{
    eventSystem = NULL;
    group       = NULL;
    event[0]    = NULL;
    event[1]    = NULL;
    event[2]    = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT result      = FMOD_OK;
    char        buffer[200] = {0};

    result = FMOD::EventSystem_Create(&eventSystem); 
    ERRCHECK(result);
    
    result = eventSystem->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    
    result = FMOD::NetEventSystem_Init(eventSystem);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/examples.fev", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = eventSystem->load(buffer, NULL, NULL);
    ERRCHECK(result);
    
    result = eventSystem->getGroup("examples/FeatureDemonstration", FMOD_EVENT_DEFAULT, &group);
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
    
    FMOD::NetEventSystem_Shutdown();
}


- (void)timerUpdate:(NSTimer *)timer 
{
    FMOD_RESULT result = FMOD_OK;
    
    result = eventSystem->update();
    ERRCHECK(result);
    
    result = FMOD::NetEventSystem_Update();
    ERRCHECK(result);
}


- (IBAction)playEvent:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    int         index   = [sender tag];
    
    if (event[index])
    {
        result = event[index]->stop();
        ERRCHECK(result);
        
        event[index] = NULL;
        
        uiStatus.text = [NSString stringWithFormat:@"Stopping '%s'", g_eventName[index]];
    }
    else
    {
        result = group->getEvent(g_eventName[index], FMOD_EVENT_DEFAULT, &event[index]);
        ERRCHECK(result);
        
        result = event[index]->setCallback(eventcallback, self);
        ERRCHECK(result);
        
        result = event[index]->start();
        ERRCHECK(result);
        
        uiStatus.text = [NSString stringWithFormat:@"Starting '%s'", g_eventName[index]];
    }
}


- (void)dealloc 
{   
	[uiEvent release];
    [uiParamType release];
    [uiParamValue release];
    [uiStatus release];
	[super dealloc];
}

@end
