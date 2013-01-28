/*===============================================================================================
 GenerateTone Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.

 This example shows how simply play generated tones using FMOD::System::payDSP instead of 
 manually connecting and disconnecting DSP units.
===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel        *status;
    UILabel        *frequencyvalue;
    UISlider       *frequency;
    UILabel        *volumevalue;
    UISlider       *volume;
    UILabel        *panvalue;
    UISlider       *pan;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::DSP      *dsp;
    FMOD::Channel  *channel;
}

@property (nonatomic, retain) IBOutlet UILabel  *status;
@property (nonatomic, retain) IBOutlet UILabel  *frequencyvalue;
@property (nonatomic, retain) IBOutlet UISlider *frequency;
@property (nonatomic, retain) IBOutlet UILabel  *volumevalue;
@property (nonatomic, retain) IBOutlet UISlider *volume;
@property (nonatomic, retain) IBOutlet UILabel  *panvalue;
@property (nonatomic, retain) IBOutlet UISlider *pan;

- (IBAction)playSineWave:(id)sender;
- (IBAction)playSquareWave:(id)sender;
- (IBAction)playSawWave:(id)sender;
- (IBAction)playTriangleWave:(id)sender;
- (IBAction)playWhiteNoise:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)frequencyChanged:(id)sender;
- (IBAction)volumeChanged:(id)sender;
- (IBAction)panChanged:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;
- (void)updateSliders;

@end

