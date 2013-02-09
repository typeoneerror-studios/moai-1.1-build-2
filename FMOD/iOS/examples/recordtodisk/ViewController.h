/*===============================================================================================
 Record to disk example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to do a streaming record to disk.
===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel        *status;
    UILabel        *recordBufferPos;
    UILabel        *recordTime;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    unsigned int    bytesPerSample;
    unsigned int    bytesPerSecond;
    unsigned int    dataLength;
    FILE           *outFile;
}

@property (nonatomic, retain) IBOutlet UILabel *status;
@property (nonatomic, retain) IBOutlet UILabel *recordBufferPos;
@property (nonatomic, retain) IBOutlet UILabel *recordTime;

- (IBAction)startRecord:(id)sender;
- (IBAction)stopRecord:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

