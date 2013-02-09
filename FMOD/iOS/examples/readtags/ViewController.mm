#import "ViewController.h"

@implementation ViewController

@synthesize outputtext;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


- (void)viewWillAppear:(BOOL)animated
{
    FMOD_RESULT      result         = FMOD_OK;
    FMOD::System    *system         = NULL;
    FMOD::Sound     *sound          = NULL;
    char             buffer[200]    = {0};
    unsigned int     version        = 0;
    int              numtags        = 0;
    int              numtagsupdated = 0;
    FMOD_TAG         tag;

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
    
    /*
        Open the specified file. Use FMOD_CREATESTREAM and FMOD_OPENONLY so it opens quickly
    */
    [[NSString stringWithFormat:@"%@/wave.mp3", [[NSBundle mainBundle] resourcePath]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];
    result = system->createSound(buffer, FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_OPENONLY, NULL, &sound);
    ERRCHECK(result);

    /*
        Read and display all tags associated with this file
    */
    while (true)
    {
        /*
            An index of -1 means "get the first tag that's new or updated".
            If no tags are new or updated then getTag will return FMOD_ERR_TAGNOTFOUND.
            This is the first time we've read any tags so they'll all be new but after we've read them, 
            they won't be new any more
        */
        if (sound->getTag(NULL, -1, &tag) != FMOD_OK)
        {
            break;
        }
        if (tag.datatype == FMOD_TAGDATATYPE_STRING)
        {
            outputtext.text = [outputtext.text stringByAppendingFormat:@"%s = %s (%d bytes)\n\n", tag.name, tag.data, tag.datalen];
        }
        else
        {
            outputtext.text = [outputtext.text stringByAppendingFormat:@"%s = <binary> (%d bytes)\n\n", tag.name, tag.datalen];
        }
    }
    outputtext.text = [outputtext.text stringByAppendingFormat:@"\n\n"];

    /*
        Read all the tags regardless of whether they're updated or not. Also show the tag type
    */
    result = sound->getNumTags(&numtags, &numtagsupdated);
    ERRCHECK(result);
    
    for (int i = 0; i < numtags; i++)
    {
        result = sound->getTag(NULL, i, &tag);
        ERRCHECK(result);

        switch (tag.type)
        {
            case FMOD_TAGTYPE_UNKNOWN :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_UNKNOWN  "];
                break;

            case FMOD_TAGTYPE_ID3V1 :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_ID3V1  "];
                break;

            case FMOD_TAGTYPE_ID3V2 :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_ID3V2  "];
                break;

            case FMOD_TAGTYPE_VORBISCOMMENT :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_VORBISCOMMENT  "];
                break;

            case FMOD_TAGTYPE_SHOUTCAST :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_SHOUTCAST  "];
                break;

            case FMOD_TAGTYPE_ICECAST :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_ICECAST  "];
                break;

            case FMOD_TAGTYPE_ASF :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_ASF  "];
                break;

            case FMOD_TAGTYPE_FMOD :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_FMOD  "];
                break;

            case FMOD_TAGTYPE_USER :
                outputtext.text = [outputtext.text stringByAppendingFormat:@"FMOD_TAGTYPE_USER  "];
                break;
        }

        if (tag.datatype == FMOD_TAGDATATYPE_STRING)
        {
            outputtext.text = [outputtext.text stringByAppendingFormat:@"%s = %s (%d bytes)\n\n", tag.name, tag.data, tag.datalen];
        }
        else
        {
            outputtext.text = [outputtext.text stringByAppendingFormat:@"%s = ??? (%d bytes)\n\n", tag.name, tag.datalen];
        }
    }
    outputtext.text = [outputtext.text stringByAppendingFormat:@"\n\n"];

    /*
        Find a specific tag by name. Specify an index > 0 to get access to multiple tags of the same name
    */
    result = sound->getTag("ARTIST", 0, &tag);
    ERRCHECK(result);
    
    outputtext.text = [outputtext.text stringByAppendingFormat:@"%s = %s (%d bytes)\n\n", tag.name, tag.data, tag.datalen];
    outputtext.text = [outputtext.text stringByAppendingFormat:@"\n\n"];
    
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


- (void)dealloc 
{   
  	[outputtext release];
	[super dealloc];
}

@end
