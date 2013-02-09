#import "ViewController.h"

@implementation ViewController

@synthesize uiMaxPlayBehavior;
@synthesize uiDistanceValue;
@synthesize uiDistance;
@synthesize uiSound;
@synthesize uiStatus;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


- (void)setupEvent
{
    FMOD_RESULT              result = FMOD_OK;
    FMOD::EventParameter    *param  = NULL;
    FMOD_VECTOR              pos    = { 0.0f, 0.0f, uiDistance.value };
    
    result = event->set3DAttributes(&pos, NULL, NULL);
    ERRCHECK(result);
    
    result = event->getParameter("sound", &param);
    ERRCHECK(result);
    
    result = param->setValue(sound);
    ERRCHECK(result);
}


- (void)viewDidLoad
{
    eventSystem = NULL;
    group       = NULL;
    event       = NULL;
    sound       = 0;
    
  	maxPlayBehaviorData = [[NSArray arrayWithObjects: @"MaxPlay-StealOldest", @"MaxPlay-StealNewest", @"MaxPlay-StealQuietest", @"MaxPlay-JustFail", @"MaxPlay-JustFailIfQuietest", nil] retain];    
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
    
    result = eventSystem->getGroup("examples/FeatureDemonstration/MaxPlayback", FMOD_EVENT_DEFAULT, &group);
    ERRCHECK(result);
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    [timer invalidate];
    
    [maxPlayBehaviorData release];
    
    if (eventSystem)
    {
        eventSystem->release();
        eventSystem = NULL;
    }    
}


- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [maxPlayBehaviorData objectAtIndex:row];
}


- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
	return [maxPlayBehaviorData count];
}


- (void)timerUpdate:(NSTimer *)timer 
{
    FMOD_RESULT result = FMOD_OK;
    
    result = eventSystem->update();
    ERRCHECK(result);
    
    uiDistanceValue.text    = [NSString stringWithFormat:@"%.2f", uiDistance.value];
    uiSound.text            = [NSString stringWithFormat:@"%d", sound];
}


- (IBAction)startEvent:(id)sender
{
    FMOD_RESULT result      = FMOD_OK;
    char        name[30]    = {0};
    
    [[maxPlayBehaviorData objectAtIndex:[uiMaxPlayBehavior selectedRowInComponent:0]] getCString:name maxLength:200 encoding:NSASCIIStringEncoding];    

    if ([uiMaxPlayBehavior selectedRowInComponent:0] == JustFailIfQuietest)
    {
        /*
            The 'Just fail if quietest' behaviour calculates the expected volume of the event based on the properties of the info-only
            event, so we have to get the info-only event first and set it up appropriately.
        */

        // Get the info-only event to set up for volume calculation
        result = group->getEvent(name, FMOD_EVENT_INFOONLY, &event);
        ERRCHECK(result);

        /*
            Set the desired properties on the info-only event
            - distances below the event's 3D Min Distance all give the same volume; in this case, getEvent will just fail
            - we could set other volume-affecting properties here as well (e.g. orientation if the event has a falloff cone)
        */
        [self setupEvent];

        // Attempt to get a real event instance
        result = group->getEvent(name, FMOD_EVENT_DEFAULT, &event);
        if (result == FMOD_OK)
        {
            FMOD::EventParameter *param = NULL;
            
            uiStatus.text = [NSString stringWithFormat:@"getEvent(\"%s\") succeeded.", name];

            /*
                We don't need to set the position of the instance, as it is copied from the info-only event, but we
                do need to set the parameter value
            */
            result = event->getParameter("sound", &param);
            ERRCHECK(result);
            
            result = param->setValue(sound);
            ERRCHECK(result);
            
            result = event->start();
            ERRCHECK(result);
        }
        else
        {
            uiStatus.text = [NSString stringWithFormat:@"getEvent(\"%s\") failed.", name];
        }
    }
    else
    {
        result = group->getEvent(name, FMOD_EVENT_DEFAULT, &event);
        if (result == FMOD_OK)
        {
            uiStatus.text = [NSString stringWithFormat:@"getEvent(\"%s\") succeeded.", name];
            
            [self setupEvent];
            
            result = event->start();
            ERRCHECK(result);
        }
        else
        {
            uiStatus.text = [NSString stringWithFormat:@"getEvent(\"%s\") failed.", name];
        }
    }

    sound++;
    if (sound > 3)
    {
        sound = 0;
    }
}


- (IBAction)stopAllEvents:(id)sender
{
    FMOD_RESULT result = FMOD_OK;
    
    result = group->freeEventData();
    ERRCHECK(result);
}


- (void)dealloc 
{
	[uiMaxPlayBehavior release];
    [uiDistanceValue release];
    [uiDistance release];
    [uiSound release];
    [uiStatus release];
	[super dealloc];
}

@end
