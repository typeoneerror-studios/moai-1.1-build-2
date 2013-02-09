#import "ViewController.h"

@implementation ViewController

@synthesize uiParamValue;
@synthesize uiParam;


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
    param00     = NULL;
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
    
    result = eventSystem->getEvent("examples/FeatureDemonstration/Effects/PropertyAutomationEffectEnvelope", FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    result = event->getParameter("param00", &param00);
    ERRCHECK(result);
    
    result = param00->getRange(&param00_min, &param00_max);
    ERRCHECK(result);
    
    result = param00->setValue(0.0f);
    ERRCHECK(result);
    
    result = event->start();
    ERRCHECK(result);

    uiParam.maximumValue = param00_max;
    uiParam.minimumValue = param00_min;
    uiParam.value        = 0.0f;
    
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
    FMOD_RESULT result  = FMOD_OK;
    float       value   = 0.0f;
    
    result = eventSystem->update();
    ERRCHECK(result);
    
    result = param00->getValue(&value);
    ERRCHECK(result);

    uiParamValue.text = [NSString stringWithFormat:@"%.2f", value];
}


- (IBAction)paramChanged:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    
    result = param00->setValue(uiParam.value);
    ERRCHECK(result);
}


- (void)dealloc 
{
  	[uiParamValue release];
	[uiParam release];
	[super dealloc];
}

@end
