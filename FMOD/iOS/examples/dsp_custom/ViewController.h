/*===============================================================================================
 Custom DSP Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to add a user created DSP callback to process audio data.
 A read callback is generated at runtime, and can be added anywhere in the DSP network.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Channel  *channel;
    FMOD::DSP      *mydsp;
}

- (IBAction)toggleActive:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

