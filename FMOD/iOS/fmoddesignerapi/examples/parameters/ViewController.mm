#import "ViewController.h"

@implementation ViewController

@synthesize uiParamLoadValue;
@synthesize uiParamLoad;
@synthesize uiParamRPMValue;
@synthesize uiParamRPM;


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
    eventSystem     = NULL;
    event           = NULL;
    paramLoad       = NULL;
    paramRPM        = NULL;
    masterCategory  = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT result          = FMOD_OK;
    char        buffer[200]     = {0};
    float       paramLoadMin    = 0.0f;
    float       paramLoadMax    = 0.0f;
    float       paramRPMMin     = 0.0f;
    float       paramRPMMax     = 0.0f; 
   
    result = FMOD::EventSystem_Create(&eventSystem); 
    ERRCHECK(result);
    
    result = eventSystem->init(32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL, FMOD_EVENT_INIT_NORMAL);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/examples.fev", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = eventSystem->load(buffer, NULL, NULL);
    ERRCHECK(result);
    
    result = eventSystem->getEvent("examples/AdvancedTechniques/car", FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    result = eventSystem->getCategory("master", &masterCategory);
    ERRCHECK(result);
    
    result = event->getParameter("load", &paramLoad);
    ERRCHECK(result);    
    result = paramLoad->getRange(&paramLoadMin, &paramLoadMax);
    ERRCHECK(result);    
    result = paramLoad->setValue(paramLoadMax);
    ERRCHECK(result);
    
    result = event->getParameter("rpm", &paramRPM);
    ERRCHECK(result);    
    result = paramRPM->getRange(&paramRPMMin, &paramRPMMax);
    ERRCHECK(result);    
    result = paramRPM->setValue(1000.0f);
    ERRCHECK(result);    
    
    result = event->start();
    ERRCHECK(result);

    uiParamLoad.maximumValue    = paramLoadMax;
    uiParamLoad.minimumValue    = paramLoadMin;
    uiParamLoad.value           = 1.0f;
    uiParamRPM.maximumValue     = paramRPMMax;
    uiParamRPM.minimumValue     = paramRPMMin;
    uiParamRPM.value            = 1000.0f;    
    
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
    float       load    = 0.0f;
    float       rpm     = 0.0f;
    
    result = eventSystem->update();
    ERRCHECK(result);
    
    result = paramLoad->getValue(&load);
    ERRCHECK(result);
    result = paramRPM->getValue(&rpm);
    ERRCHECK(result);

    uiParamLoadValue.text   = [NSString stringWithFormat:@"%.2f", load];
    uiParamRPMValue.text    = [NSString stringWithFormat:@"%.2f", rpm];
}


- (IBAction)paramLoadChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = paramLoad->setValue(uiParamLoad.value);
    ERRCHECK(result);
}


- (IBAction)paramRPMChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = paramRPM->setValue(uiParamRPM.value);
    ERRCHECK(result);
}


- (IBAction)pauseResume:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    bool        paused = false;
    
    result = masterCategory->getPaused(&paused);
    ERRCHECK(result);

    result = masterCategory->setPaused(!paused);
    ERRCHECK(result);
}


- (void)dealloc 
{
  	[uiParamLoadValue release];
	[uiParamLoad release];
  	[uiParamRPMValue release];
	[uiParamRPM release];    
	[super dealloc];
}

@end
