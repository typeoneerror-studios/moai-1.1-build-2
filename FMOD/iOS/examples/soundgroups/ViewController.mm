#import "ViewController.h"

@implementation ViewController

@synthesize sound1state;
@synthesize sound2state;
@synthesize sound3state;


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
    system      = NULL;
    sound1      = NULL;
    sound2      = NULL;
    sound3      = NULL;
    soundgroup  = NULL;
    channel1    = NULL;
    channel2    = NULL;
    channel3    = NULL;
    
	groupbehaviordata = [[NSArray arrayWithObjects: @"BEHAVIOR_MUTE", @"BEHAVIOR_FAIL", @"BEHAVIOR_STEALLOWEST", nil] retain];    
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
    
    [[NSString stringWithFormat:@"%@/drumloop.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound1);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/jaguar.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound2);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/swish.wav", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &sound3);
    ERRCHECK(result);
    
    /* 
        Create the sound group with the following attributes:
          Name       = MyGroup
          MaxAudible = 1
          Behavior   = Mute 
    */
	result = system->createSoundGroup("MyGroup", &soundgroup);
	ERRCHECK(result);

    result = soundgroup->setMaxAudible(1);
    ERRCHECK(result);

    result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
    ERRCHECK(result);

    result = soundgroup->setMuteFadeSpeed(2);
    ERRCHECK(result);

    /*
        Put the sounds in the sound group
    */
	result = sound1->setSoundGroup(soundgroup);
	ERRCHECK(result);

	result = sound2->setSoundGroup(soundgroup);
	ERRCHECK(result);

	result = sound3->setSoundGroup(soundgroup);
	ERRCHECK(result);
	
	/*
        Play the sounds (two will be muted because of the behavior mode)
    */
	result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel1);
	ERRCHECK(result);	

	result = system->playSound(FMOD_CHANNEL_FREE, sound2, false, &channel2);
	ERRCHECK(result);

	result = system->playSound(FMOD_CHANNEL_FREE, sound3, false, &channel3);
	ERRCHECK(result);    

    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */    
    [timer invalidate];
    
    [groupbehaviordata release];
    
    if (sound1)
    {
        sound1->release();
        sound1 = NULL;
    }
    
    if (sound2)
    {
        sound2->release();
        sound2 = NULL;
    }
    
    if (sound3)
    {
        sound3->release();
        sound3 = NULL;
    }
    
    if (soundgroup)
    {
        soundgroup->release();
        soundgroup = NULL;
    }
    
    if (system)
    {
        system->release();
        system = NULL;
    }    
}


- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    FMOD_RESULT result = FMOD_OK;
    
    if (row == 0)  // Mute
    {
        result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
        ERRCHECK(result);        
    }
    else if (row == 1)   // Fail
    {
        result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_FAIL);
        ERRCHECK(result);        
    }    
    else if (row == 2)  // Steal Lowest
    {
        result = soundgroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST);
        ERRCHECK(result);        
    }
}


- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [groupbehaviordata objectAtIndex:row];
}


- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
	return [groupbehaviordata count];
}


- (void)timerUpdate:(NSTimer *)timer 
{
    /*
        Main loop
    */
    FMOD_RESULT result      = FMOD_OK;
    float       audibility  = 0.0f;
    int         index       = 0;

    channel1->getAudibility(&audibility);
    channel1->getIndex(&index);
    sound1state.text = (audibility ? @"PLAY" : (index ? @"MUTE" : @"STOP"));
    
    channel2->getAudibility(&audibility);
    channel2->getIndex(&index);
    sound2state.text = (audibility ? @"PLAY" : (index ? @"MUTE" : @"STOP"));
    
    channel3->getAudibility(&audibility);
    channel3->getIndex(&index);
    sound3state.text = (audibility ? @"PLAY" : (index ? @"MUTE" : @"STOP"));

    if (system != NULL)
    {        
        result = system->update();
        ERRCHECK(result);
    }
}


- (IBAction)playSound1:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    int         index   = 0;
    
    channel1->getIndex(&index);   
    if (!index)
    {
        result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel1);
        if (result != FMOD_ERR_MAXAUDIBLE)
        {
            ERRCHECK(result);
        }
    }
    else
    {
        result = channel1->stop();
        ERRCHECK(result);
    }
}


- (IBAction)playSound2:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    int         index   = 0;
    
    channel2->getIndex(&index);
    if (!index)
    {
        result = system->playSound(FMOD_CHANNEL_FREE, sound2, false, &channel2);
        if (result != FMOD_ERR_MAXAUDIBLE)
        {
            ERRCHECK(result);
        }
    }
    else
    {
        result = channel2->stop();
        ERRCHECK(result);
    }    
}


- (IBAction)playSound3:(id)sender
{
    FMOD_RESULT result  = FMOD_OK;
    int         index   = 0;
    
    channel3->getIndex(&index);
    if (!index)
    {
        result = system->playSound(FMOD_CHANNEL_FREE, sound3, false, &channel3);
        if (result != FMOD_ERR_MAXAUDIBLE)
        {
            ERRCHECK(result);
        }
    }
    else
    {
        result = channel3->stop();
        ERRCHECK(result);
    }    
}


- (void)dealloc 
{   
	[sound1state release];
	[sound2state release];
  	[sound3state release];
	[super dealloc];
}

@end
