/*===============================================================================================
 PlaySound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to simply load and play multiple sounds.  This is about the simplest
 use of FMOD.
 This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
 up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is 
 streamed in realtime as it plays.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel        *time;
    UILabel        *status;
    UILabel        *channels;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound1;
    FMOD::Sound    *sound2;
    FMOD::Sound    *sound3;
    FMOD::Channel  *channel;
}

@property (nonatomic, retain) IBOutlet UILabel *time;
@property (nonatomic, retain) IBOutlet UILabel *status;
@property (nonatomic, retain) IBOutlet UILabel *channels;

- (IBAction)playSound1:(id)sender;
- (IBAction)playSound2:(id)sender;
- (IBAction)playSound3:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

