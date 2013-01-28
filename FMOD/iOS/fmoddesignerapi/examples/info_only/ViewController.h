/*===============================================================================================
 Info-only Event Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates usage and functionality of info-only events
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *uiMemCurrent;
    UILabel             *uiMemMaximum;
    UISlider            *uiVolume;
    UILabel             *uiVolumeValue;
    UILabel             *uiStatus;
    NSTimer             *timer;

    FMOD::EventSystem   *eventSystem;
    FMOD::EventGroup    *group;
    FMOD::Event         *event;
}

@property (nonatomic, retain) IBOutlet UILabel  *uiMemCurrent;
@property (nonatomic, retain) IBOutlet UILabel  *uiMemMaximum;
@property (nonatomic, retain) IBOutlet UISlider *uiVolume;
@property (nonatomic, retain) IBOutlet UILabel  *uiVolumeValue;
@property (nonatomic, retain) IBOutlet UILabel  *uiStatus;

- (IBAction)getInfoOnlyEvent:(id)sender;
- (IBAction)getEventInstance:(id)sender;
- (IBAction)startStopEvent:(id)sender;
- (IBAction)unloadEventData:(id)sender;
- (IBAction)volumeChanged:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

