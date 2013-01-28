#import "ViewController.h"

@implementation ViewController

@synthesize status;


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
    system  = NULL;
    sound   = NULL;
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

    result = system->init(1, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL);
    ERRCHECK(result);

    [[NSString stringWithFormat:@"%@/wave.mp3", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_OPENONLY | FMOD_ACCURATETIME, NULL, &sound);
    ERRCHECK(result);
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */       
    if (sound)
    {
        sound->release();
        sound = NULL;
    }
       
    if (system)
    {
        system->release();
        system = NULL;
    }    
}


/*
    Decode the sound and write it to a .raw file.
*/
- (IBAction)decodeSound:(id)sender
{
    const unsigned int  CHUNKSIZE   = 4096;
    FMOD_RESULT         result      = FMOD_OK;    
    char                buffer[200] = {0};
    unsigned int        length      = 0; 
    FILE               *outFile     = NULL;    
    void               *data        = NULL;
    unsigned int        bytesRead   = 0;
    unsigned int        read        = 0;

    result = sound->getLength(&length, FMOD_TIMEUNIT_PCMBYTES);
    ERRCHECK(result);
    
    [[NSString stringWithFormat:@"%@/output.raw", [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    outFile = fopen(buffer, "wb");
    if (!outFile)
    {
        fprintf(stderr, "Error! Could not open output.raw output file.\n");
        return;
    }

    data = malloc(CHUNKSIZE);
    if (!data)
    {
        fprintf(stderr, "Error! Failed to allocate %d bytes.\n", CHUNKSIZE);
        return;
    }

    bytesRead = 0;
    do
    {
        result = sound->readData((char *)data, CHUNKSIZE, &read);
        
        fwrite((char *)data, read, 1, outFile);
                   
        bytesRead   += read;
        status.text  = [NSString stringWithFormat:@"Written %d bytes of %d to output.raw", bytesRead, length];
    }
    while (result == FMOD_OK && read == CHUNKSIZE);

    /*
        Loop terminates when either 
        1. the read function returns an error.  (ie FMOD_ERR_FILE_EOF etc).
        2. the amount requested was different to the amount returned. (somehow got an EOF without the file error, maybe a non stream file format like mod/s3m/xm/it/midi).

        If 'bytesread' is bigger than 'length' then it just means that FMOD miscalculated the size, 
        but this will not usually happen if FMOD_ACCURATETIME is used.  (this will give the correct length for VBR formats)
    */

    if (outFile)
    {
        fclose(outFile);
        outFile = NULL;
    }
}


- (void)dealloc 
{   
	[status release];
	[super dealloc];
}

@end
