/*===============================================================================================
 MusicSystem Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic usage of FMOD's music system.
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import "examples.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *uiRetroRockIntensityValue;
    UISlider            *uiRetroRockIntensity;
    UILabel             *uiStatus;
    NSTimer             *timer;

    FMOD::EventSystem   *eventSystem;
    FMOD::MusicSystem   *musicSystem;
    FMOD::MusicPrompt   *country;
    FMOD::MusicPrompt   *stealth;
    FMOD::MusicPrompt   *retroRock;
}

@property (nonatomic, retain) IBOutlet UILabel  *uiRetroRockIntensityValue;
@property (nonatomic, retain) IBOutlet UISlider *uiRetroRockIntensity;
@property (nonatomic, retain) IBOutlet UILabel  *uiStatus;

- (IBAction)startStopCountry:(id)sender;
- (IBAction)startStopRetroRock:(id)sender;
- (IBAction)startStopStealth:(id)sender;
- (IBAction)retroRockIntensityChange:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
