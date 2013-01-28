/*===============================================================================================
 User Created Sound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how create a sound with data filled by the user.
 It shows a user created static sample, followed by a user created stream.
 The former allocates all memory needed for the sound and is played back as a static sample, 
 while the latter streams the data in chunks as it plays, using far less memory.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel                 *time;
    UILabel                 *status;
    NSTimer                 *timer;

    FMOD::System            *system;
    FMOD::Sound             *sound;
    FMOD::Channel           *channel;
    FMOD_CREATESOUNDEXINFO   exinfo;
}

@property (nonatomic, retain) IBOutlet UILabel *time;
@property (nonatomic, retain) IBOutlet UILabel *status;

- (IBAction)playStream:(id)sender;
- (IBAction)playSample:(id)sender;
- (IBAction)pauseResume:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

