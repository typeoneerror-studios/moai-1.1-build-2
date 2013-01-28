#import "ViewController.h"

@implementation ViewController

@synthesize status;
@synthesize recordBufferPos;
@synthesize recordTime;

#define __PACKED __attribute__((packed))    /* gcc packed */

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
    system          = NULL;
    sound           = NULL;
    bytesPerSample  = 0;
    bytesPerSecond  = 0;
    dataLength      = 0;
    outFile         = NULL;
}


- (void)viewWillAppear:(BOOL)animated
{
    const unsigned int      RECORD_LENGTH_SECS  = 3;
    FMOD_RESULT             result              = FMOD_OK;
    unsigned int            version             = 0;
    FMOD_CREATESOUNDEXINFO  exinfo              = {0};

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
    
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

    exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.numchannels      = 1;
    exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
    exinfo.defaultfrequency = 8000;
    exinfo.length           = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * RECORD_LENGTH_SECS;

    bytesPerSample = sizeof(short) * exinfo.numchannels;
    bytesPerSecond = exinfo.defaultfrequency * bytesPerSample;
    
    result = system->createSound(NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
    ERRCHECK(result);
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerUpdate:) userInfo:nil repeats:YES];    
}


- (void)viewWillDisappear:(BOOL)animated
{
    /*
        Shut down
    */
    [self stopRecord:self];    
    [timer invalidate];
                  
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
    Write a wav file header
*/
void writeWavHeader(FILE *file, FMOD::Sound *sound, int length)
{
    int               channels;
    int               bits;
    float             rate;
    FMOD_SOUND_FORMAT format;

    if (!sound)
    {
        return;
    }

    fseek(file, 0, SEEK_SET);

    sound->getFormat(NULL, &format, &channels, &bits);
    sound->getDefaults(&rate, NULL, NULL, NULL);

    {
        /*
            WAV Structures
        */
        typedef struct
        {
	        signed char id[4];
	        int 		size;
        } RiffChunk;
    
        struct
        {
            RiffChunk       chunk           __PACKED;
            unsigned short	wFormatTag      __PACKED;    /* format type  */
            unsigned short	nChannels       __PACKED;    /* number of channels (i.e. mono, stereo...)  */
            unsigned int	nSamplesPerSec  __PACKED;    /* sample rate  */
            unsigned int	nAvgBytesPerSec __PACKED;    /* for buffer estimation  */
            unsigned short	nBlockAlign     __PACKED;    /* block size of data  */
            unsigned short	wBitsPerSample  __PACKED;    /* number of bits per sample of mono data */
        } __PACKED FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };

        if (format == FMOD_SOUND_FORMAT_PCMFLOAT)
        {
            FmtChunk.wFormatTag = 3;
        }

        struct
        {
            RiffChunk   chunk;
        } DataChunk = { {{'d','a','t','a'}, length } };

        struct
        {
            RiffChunk   chunk;
	        signed char rifftype[4];
        } WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + length }, {'W','A','V','E'} };

        /*
            Write out the WAV header
        */
        fwrite(&WavHeader, sizeof(WavHeader), 1, file);
        fwrite(&FmtChunk, sizeof(FmtChunk), 1, file);
        fwrite(&DataChunk, sizeof(DataChunk), 1, file);
    }
}


/*
    Start recording to record.wav
*/
- (IBAction)startRecord:(id)sender
{
    FMOD_RESULT result      = FMOD_OK;    
    char        buffer[200] = {0};

    [self stopRecord:self];
    
    result = system->recordStart(0, sound, true);
    ERRCHECK(result);

    [[NSString stringWithFormat:@"%@/record.wav", [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]] getCString:buffer maxLength:200 encoding:NSASCIIStringEncoding];    
    outFile = fopen(buffer, "wb");
    if (!outFile)
    {
        fprintf(stderr, "Error! Could not open record.wavw output file.\n");
        return;
    }

    /*
        Write out the wav header.  As we don't know the length yet it will be 0
    */
    writeWavHeader(outFile, sound, dataLength);
}


/*
    Stop recording
*/
- (IBAction)stopRecord:(id)sender
{
    if (outFile != NULL)
    {
        /*
            Write back the wav header now that we know its length
        */
        writeWavHeader(outFile, sound, dataLength);

        fclose(outFile);
        outFile = NULL;
        dataLength = 0;
    }
}


- (void)timerUpdate:(NSTimer *)timer 
{
    /*
        Main loop
    */
    static unsigned int lastRecordPos   = 0;
    FMOD_RESULT         result          = FMOD_OK;
    unsigned int        recordPos       = 0;
    unsigned int        soundLength     = 0;

    if (outFile != NULL)
    {
        result = sound->getLength(&soundLength, FMOD_TIMEUNIT_PCM);
        ERRCHECK(result);

        result = system->getRecordPosition(0, &recordPos);
        ERRCHECK(result);

        if (recordPos != lastRecordPos)
        {
            void           *ptr1        = NULL;
            void           *ptr2        = NULL;
            int             blockLength = 0;
            unsigned int    len1        = 0;
            unsigned int    len2        = 0;
            
            blockLength = (int)recordPos - (int)lastRecordPos;
            if (blockLength < 0)
            {
                blockLength += soundLength;
            }

            /*
                Lock the sound to get access to the raw data
            */
            result = sound->lock(lastRecordPos * bytesPerSample, blockLength * bytesPerSample, &ptr1, &ptr2, &len1, &len2);
            ERRCHECK(result);

            /*
                Write it to disk
            */
            if (ptr1 && len1)
            {
                dataLength += fwrite(ptr1, 1, len1, outFile);
            }
            if (ptr2 && len2)
            {
                dataLength += fwrite(ptr2, 1, len2, outFile);
            }

            /*
                Unlock the sound to allow FMOD to use it again
            */
            result = sound->unlock(ptr1, ptr2, len1, len2);
            ERRCHECK(result);
        }

        lastRecordPos = recordPos;
    }

    result = system->update();
    ERRCHECK(result);
    
    status.text             = (outFile == NULL ? @"Not Recording" : @"Recording");    
    recordBufferPos.text    = [NSString stringWithFormat:@"%d", recordPos];
    recordTime.text         = [NSString stringWithFormat:@"%02d:%02d", dataLength / bytesPerSecond / 60, (dataLength / bytesPerSecond) % 60];
}


- (void)dealloc 
{
	[recordBufferPos release];
	[recordTime release];   
	[status release];
	[super dealloc];
}

@end
