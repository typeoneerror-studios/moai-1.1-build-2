/*===============================================================================================
 Record example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.

 This example shows how to record a sound.
 It then shows how to play a sound while it is being recorded to.  Because it is recording, the
 sound playback has to be delayed a little bit so that the playback doesn't play part of the
 buffer that is still being written to.
===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel        *recordpos;
    UILabel        *playpos;
    UILabel        *looping;
    UILabel        *status;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Channel  *channel;    
    bool            loopFlag;
}

@property (nonatomic, retain) IBOutlet UILabel *recordpos;
@property (nonatomic, retain) IBOutlet UILabel *playpos;
@property (nonatomic, retain) IBOutlet UILabel *looping;
@property (nonatomic, retain) IBOutlet UILabel *status;

- (IBAction)record:(id)sender;
- (IBAction)play:(id)sender;
- (IBAction)toggleLooping:(id)sender;
- (IBAction)stop:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

