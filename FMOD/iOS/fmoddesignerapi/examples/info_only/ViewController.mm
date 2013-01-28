#import "ViewController.h"

@implementation ViewController

@synthesize uiMemCurrent;
@synthesize uiMemMaximum;
@synthesize uiVolume;
@synthesize uiVolumeValue;
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
    float       volume          = 0;
        
    result = FMOD::Memory_GetStats(&memoryCurrent, &memoryMaximum);
    ERRCHECK(result);
            
    if (event)
    {
        /*
            Attempt to get the event's volume, this will succeed on both info-only events and instances.
        */
        result = event->getVolume(&volume);
        if (result != FMOD_ERR_INVALID_HANDLE)
        {
            ERRCHECK(result);
        }
    }
    
    uiMemCurrent.text  = [NSString stringWithFormat:@"%d", memoryCurrent];
    uiMemMaximum.text  = [NSString stringWithFormat:@"%d", memoryMaximum];
    uiVolumeValue.text = [NSString stringWithFormat:@"%.2f", volume];
    
    result = eventSystem->update();
    ERRCHECK(result);
}


- (IBAction)getInfoOnlyEvent:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    /*
        Get an info-only event.
        - This is the parent from which all instances of the specified event are derived.
        - This call will not allocate memory for event instances or load sample data.
        - The resulting event cannot be played, it simply allows querying and setting of event properties.
        - Any properties which are set on the info-only event will be copied to all instances which are retrieved from then on.
    */
    result = group->getEvent("SimpleEventWithLooping", FMOD_EVENT_INFOONLY, &event);
    ERRCHECK(result);
    
    uiStatus.text = @"getEvent(FMOD_EVENT_INFOONLY) succeeded.";
}


- (IBAction)getEventInstance:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    /*
        Get an event instance.
        - This call will allocate memory for event instances and load sample data if required.
        - The resulting event can be played.
        - Any properties which have been set on the info-only event will be copied to this instance.
    */
    result = group->getEvent("SimpleEventWithLooping", FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    uiStatus.text = @"getEvent(FMOD_EVENT_DEFAULT) succeeded.";
}


- (IBAction)startStopEvent:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    if (event)
    {
        FMOD_EVENT_STATE state;
        
        result = event->getState(&state);
        if (result != FMOD_ERR_INVALID_HANDLE)
        {
            ERRCHECK(result);
        }
        
        if (state & FMOD_EVENT_STATE_PLAYING)
        {
            /* 
                Attempt to stop the event, this will fail if the event is info-only.
            */
            result = event->stop();
            if (result == FMOD_OK)
            {
                uiStatus.text = @"event->stop() succeeded.";
            }
            else
            {
                uiStatus.text = [NSString stringWithFormat:@"event->stop() returned an error:  (%d) %s", result, FMOD_ErrorString(result)];
            }
        }
        else
        {
            /* 
                Attempt to start the event, this will fail if the event is info-only.
            */
            result = event->start();
            if (result == FMOD_OK)
            {
                uiStatus.text = @"event->start() succeeded.";
            }
            else
            {
                uiStatus.text = [NSString stringWithFormat:@"event->start() returned an error:  (%d) %s", result, FMOD_ErrorString(result)];
            }
        }
    }
}


- (IBAction)unloadEventData:(id)sender
{
    FMOD_RESULT result = FMOD_OK;

    result = group->freeEventData();
    ERRCHECK(result);
    
    uiStatus.text = @"Event data unloaded.";
}


- (IBAction)volumeChanged:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    /* 
        Attempt to set the event's volume.
        - This will succeed on both info-only events and instances.
        - Volume set on the info-only event will be copied to all instances retrieved from now on.
        - Volume set on an instance will only apply to that instance.
    */   
    result = event->setVolume(uiVolume.value);
    if (result == FMOD_OK)
    {
        uiStatus.text = @"event->setVolume() succeeded.";
    }
    else
    {
        uiStatus.text = [NSString stringWithFormat:@"event->setVolume() returned an error:  (%d) %s", result, FMOD_ErrorString(result)];
    }   
}


- (void)dealloc 
{
	[uiMemCurrent release];
	[uiMemMaximum release];
	[uiVolume release];
	[uiVolumeValue release];
	[uiStatus release];    
	[super dealloc];
}

@end
