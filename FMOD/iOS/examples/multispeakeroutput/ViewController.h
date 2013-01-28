/*===============================================================================================
 Multi Speaker Output Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to play sounds in multiple speakers, and also how to even assign 
 sound subchannels, such as those in a stereo sound to different individual speakers.
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
    FMOD::Channel  *channel;
}

@property (nonatomic, retain) IBOutlet UILabel *time;
@property (nonatomic, retain) IBOutlet UILabel *status;
@property (nonatomic, retain) IBOutlet UILabel *channels;

- (IBAction)playLeft:(id)sender;
- (IBAction)playRight:(id)sender;
- (IBAction)playStereo:(id)sender;
- (IBAction)playStereoSwapped:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

