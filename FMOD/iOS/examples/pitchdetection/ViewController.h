/*===============================================================================================
 Pitch detection example.
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.

 This example combines recording with spectrum analysis to determine the pitch of the sound 
 being recorded.
===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel        *detectedrate;
    UILabel        *detectednote;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Channel  *channel;
}

@property (nonatomic, retain) IBOutlet UILabel *detectedrate;
@property (nonatomic, retain) IBOutlet UILabel *detectednote;

- (void)timerUpdate:(NSTimer *)timer;

@end
