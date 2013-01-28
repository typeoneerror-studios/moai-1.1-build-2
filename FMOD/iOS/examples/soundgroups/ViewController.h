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

@interface ViewController : UIViewController <UIPickerViewDelegate>
{
	UILabel             *sound1state;
    UILabel             *sound2state;
    UILabel             *sound3state;
    UIPickerView        *groupbehavior;
  	NSArray				*groupbehaviordata;
    NSTimer             *timer;

    FMOD::System        *system;
    FMOD::Sound         *sound1;
    FMOD::Sound         *sound2;
    FMOD::Sound         *sound3;
    FMOD::SoundGroup    *soundgroup;
    FMOD::Channel       *channel1;
    FMOD::Channel       *channel2;
    FMOD::Channel       *channel3;
}

@property (nonatomic, retain) IBOutlet UILabel *sound1state;
@property (nonatomic, retain) IBOutlet UILabel *sound2state;
@property (nonatomic, retain) IBOutlet UILabel *sound3state;

- (IBAction)playSound1:(id)sender;
- (IBAction)playSound2:(id)sender;
- (IBAction)playSound3:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

