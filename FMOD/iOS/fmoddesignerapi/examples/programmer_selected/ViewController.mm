#import "ViewController.h"

@implementation ViewController

@synthesize uiIndexValue;
@synthesize uiIndex;
@synthesize uiStatus;

static int g_sounddef_entry_index = 0;


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
    switch (type)
    {
        case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_SELECTINDEX :
        {
            char    *name   = (char *)param1;       // [in]  (char *) name of sound definition
            int      count  = *(int *)param2;       // [in]  (int *) pointer to number of entries in this sound definition
            int     *index  = (int *)param2;        // [out] (int *) the sounddef entry index to use
            UILabel *status = (UILabel *)userdata;
            
            *index = (g_sounddef_entry_index < count) ? g_sounddef_entry_index : count - 1;
            
            status.text = [NSString stringWithFormat:@"Select index callback '%s': %d", name, *index];
            
            break;
        }
    }
    
    return FMOD_OK;
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
    float       param00_min = 0.0f;
    float       param00_max = 0.0f;
   
    result = FMOD::EventSystem_Create(&eventSystem); 
    ERRCHECK(result);
    
    result = eventSystem->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/examples.fev", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = eventSystem->load(buffer, NULL, NULL);
    ERRCHECK(result);
    
    result = eventSystem->getGroup("examples/FeatureDemonstration/SequencingAndStitching", FMOD_EVENT_DEFAULT, &group);
    ERRCHECK(result);
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
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
}


- (IBAction)playEvent:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = group->getEvent("ProgrammerSelected", FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    result = event->setCallback(eventcallback, uiStatus);
    ERRCHECK(result);
    
    result = event->start();
    ERRCHECK(result);
}


- (IBAction)indexChanged:(id)sender
{
    g_sounddef_entry_index  = (int)uiIndex.value;
    uiIndexValue.text       = [NSString stringWithFormat:@"%d", (int)uiIndex.value];
}


- (void)dealloc 
{
  	[uiIndexValue release];
	[uiIndex release];
    [uiStatus release];
	[super dealloc];
}

@end
