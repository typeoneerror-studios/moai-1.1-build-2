#import "ViewController.h"

@implementation ViewController

@synthesize uiIndexValue;
@synthesize uiIndex;
@synthesize uiStatus;

static const int     SOUND_INDEX_MAX                = 4;
static int           g_sound_index                  = 0;
static int           g_index_map[SOUND_INDEX_MAX]   = {0};
static FMOD::Sound  *g_fsb                          = NULL;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


- (void)initIndexMap
{
    FMOD_RESULT result          = FMOD_OK;
    int         numSubSounds    = 0;
    
    /*
        Sounds aren't in a logical order in the FSB, so we need to iterate through and assign the appropriate filenames to the index map
    */
    result = g_fsb->getNumSubSounds(&numSubSounds);
    ERRCHECK(result);
    
    for (int i = 0; i < SOUND_INDEX_MAX; i++)
    {
        g_index_map[i] = -1;
    }
    
    for (int i = 0; i < numSubSounds; i++)
    {
        FMOD::Sound *sound      = NULL;
        char         name[100]  = {0};
        
        result = g_fsb->getSubSound(i, &sound);
        ERRCHECK(result);
        
        result = sound->getName(name, 100);
        ERRCHECK(result);
        
        if (strncmp(name, "sequence-one.ogg", 100) == 0)
        {
            g_index_map[0] = i;
        }
        else if (strncmp(name, "sequence-two.ogg", 100) == 0)
        {
            g_index_map[1] = i;
        }
        else if (strncmp(name, "sequence-three.ogg", 100) == 0)
        {
            g_index_map[2] = i;
        }
        else if (strncmp(name, "sequence-four.ogg", 100) == 0)
        {
            g_index_map[3] = i;
        }
    }
}


FMOD_RESULT F_CALLBACK eventcallback(FMOD_EVENT *event, FMOD_EVENT_CALLBACKTYPE type, void *param1, void *param2, void *userdata)
{
    FMOD_RESULT  result = FMOD_OK;
    UILabel     *status = (UILabel *)userdata;

    switch (type)
    {
        case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE :
        {
            char             *name              = (char *)param1;           // [in]  (char *) name of sound definition
            int               entryIndex        = *((int *)param2);         // [in]  (int) index of sound definition entry
            FMOD::Sound     **subSound          = (FMOD::Sound **)param2;   // [out] (FMOD::Sound *) a valid lower level API FMOD Sound handle
            char              soundName[128]    = {0};

            result = (g_index_map[g_sound_index] >= 0) ? FMOD_OK : FMOD_ERR_SUBSOUNDS;
            ERRCHECK(result);
            
            result = g_fsb->getSubSound(g_index_map[g_sound_index], subSound);
            ERRCHECK(result);
            
            result = (*subSound)->getName(soundName, 128);
            ERRCHECK(result);
            
            status.text = [NSString stringWithFormat:@"SoundDef create callback '%s' (%d)", soundName, g_index_map[g_sound_index]];
            break;
        }
        case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_RELEASE :
        {
            FMOD::Sound *subSound       = (FMOD::Sound *)param2;   // [in]  (FMOD::Sound *) the FMOD sound handle that was previously created in FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE
            char         soundName[128] = {0};

            result = subSound->getName(soundName, 128);
            ERRCHECK(result);
            
            status.text = [NSString stringWithFormat:@"SoundDef release callback '%s' (%d)", soundName];
            break;
        }
    }

    return FMOD_OK;
}


- (void)viewDidLoad
{
    eventSystem = NULL;
    system      = NULL;
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
    
    result = eventSystem->getSystemObject(&system);
    ERRCHECK(result);

    [[NSString stringWithFormat:@"%@/tutorial_bank.fsb", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createStream(buffer, FMOD_2D | FMOD_SOFTWARE, NULL, &g_fsb);
    ERRCHECK(result);
    
    [self initIndexMap];
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];
}


- (void)viewWillDisappear:(BOOL)animated
{
    [timer invalidate];
    
    if (g_fsb)
    {
        g_fsb->release();
        g_fsb = NULL;
    }
    
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
    
    result = group->getEvent("ProgrammerSounds", FMOD_EVENT_DEFAULT, &event);
    ERRCHECK(result);
    
    result = event->setCallback(eventcallback, uiStatus);
    ERRCHECK(result);
    
    result = event->start();
    ERRCHECK(result);
}


- (IBAction)indexChanged:(id)sender
{
    g_sound_index       = (int)uiIndex.value;
    uiIndexValue.text   = [NSString stringWithFormat:@"%d", (int)uiIndex.value];
}


- (void)dealloc 
{
  	[uiIndexValue release];
	[uiIndex release];
    [uiStatus release];
	[super dealloc];
}

@end
