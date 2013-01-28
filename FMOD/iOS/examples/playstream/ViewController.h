/*===============================================================================================
 PlayStream Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to simply play a stream, such as an mp3 or wav.
 The stream behaviour is achieved by specifying FMOD_CREATESTREAM in the call to 
 System::createSound.
 This makes FMOD decode the file in realtime as it plays, instead of loading it all at once.
 This uses far less memory, in exchange for a small runtime cpu hit.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel        *time;
    UILabel        *status;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Channel  *channel;
}

@property (nonatomic, retain) IBOutlet UILabel *time;
@property (nonatomic, retain) IBOutlet UILabel *status;

- (IBAction)playPause:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

