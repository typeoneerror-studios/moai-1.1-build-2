/*===============================================================================================
 3d Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to basic 3d positioning
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UISlider       *listener;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound1;
    FMOD::Sound    *sound2;
    FMOD::Sound    *sound3;
    FMOD::Channel  *channel1;
    FMOD::Channel  *channel2;
    FMOD::Channel  *channel3;
    bool            listenermoveflag;
    FMOD_VECTOR     listenerpos;
}

@property (nonatomic, retain) IBOutlet UISlider *listener;

- (IBAction)togglePauseSound1:(id)sender;
- (IBAction)togglePauseSound2:(id)sender;
- (IBAction)playSound3:(id)sender;
- (IBAction)startStopListenerMove:(id)sender;
- (IBAction)listenerMove:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

