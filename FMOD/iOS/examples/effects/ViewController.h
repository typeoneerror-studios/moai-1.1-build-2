/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to apply some of the built in software effects to sounds. 
 This example filters the global mix.  All software sounds played here would be filtered in the 
 same way.
 To filter per channel, and not have other channels affected, simply replace system->addDSP with
 channel->addDSP.
 Note in this example you don't have to add and remove units each time, you could simply add them 
 all at the start then use DSP::setActive to toggle them on and off.
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
    FMOD::Channel  *channel;
    FMOD::DSP      *dsplowpass;
    FMOD::DSP      *dsphighpass;
    FMOD::DSP      *dspecho;
    FMOD::DSP      *dspflange;
    FMOD::DSP      *dspdistortion;
    FMOD::DSP      *dspchorus;
    FMOD::DSP      *dspparameq;
}

@property (nonatomic, retain) IBOutlet UILabel *status;

- (IBAction)playPause:(id)sender;
- (IBAction)toggleLowpass:(id)sender;
- (IBAction)toggleHighpass:(id)sender;
- (IBAction)toggleEcho:(id)sender;
- (IBAction)toggleFlange:(id)sender;
- (IBAction)toggleDistortion:(id)sender;
- (IBAction)toggleChorus:(id)sender;
- (IBAction)toggleParamEq:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

