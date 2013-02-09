/*===============================================================================================
 DSP Effect per speaker Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to manipulate a DSP network and as an example, creates 2 dsp effects,
 and splits a single sound into 2 audio paths, which it then filters seperately.
 To only have each audio path come out of one speaker each, DSPConnection::setLevels is used just
 before the 2 branches merge back together again.
 
 For more speakers:
 1. Use System::setSpeakerMode or System::setOutputFormat.
 2. Create more effects, currently 2 for stereo (lowpass and chorus), create one per speaker.
 3. Under the 'Now connect the 2 effects to channeldsp head.' section, connect the extra effects
 by duplicating the code more times.
 4. Filter each effect to each speaker by calling DSP::setInputLevels.  Expand the existing code
 by extending the level arrays from 2 to the number of speakers you require, and change the
 numlevels parameter in DSP::setInputLevels from 2 to the correct number accordingly.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel        *channels;
    UILabel        *panvalue;
    UISlider       *pan;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Channel  *channel;
    FMOD::DSP      *dsplowpass;
    FMOD::DSP      *dspchorus;
}

@property (nonatomic, retain) IBOutlet UILabel  *channels;
@property (nonatomic, retain) IBOutlet UILabel  *panvalue;
@property (nonatomic, retain) IBOutlet UISlider *pan;

- (IBAction)toggleLowpass:(id)sender;
- (IBAction)toggleChorus:(id)sender;
- (IBAction)panValueChanged:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

