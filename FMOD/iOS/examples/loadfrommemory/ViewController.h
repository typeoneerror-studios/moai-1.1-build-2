/*===============================================================================================
 Load from memory example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example is simply a variant of the play sound example, but it loads the data into memory
 then uses the 'load from memory' feature of System::createSound.
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

