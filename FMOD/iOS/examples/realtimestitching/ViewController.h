/*===============================================================================================
 Real-time stitching example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how you can create your own multi-subsound stream, then in realtime replace
 each the subsound as it plays them.  Using a looping sentence, it will seamlessly stich between
 2 subsounds in this example, and each time it switches to a new sound, it will replace the old
 one with another sound in our list.
 
 These sounds can go on forever as long as they are the same bitdepth (when decoded) and number
 of channels (ie mono / stereo).  The reason for this is the hardware channel cannot change 
 formats mid sentence, and using different hardware channels would mean it wouldn't be gapless.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>


@interface ViewController : UIViewController
{
    UILabel        *status;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Sound    *subsound[2];
    FMOD::Channel  *channel;
    unsigned int    subsoundid;
    unsigned int    sentenceid;
}

@property (nonatomic, retain) IBOutlet UILabel *status;

- (IBAction)pauseResume:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

