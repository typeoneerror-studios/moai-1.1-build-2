/*===============================================================================================
 Parameters Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic usage of event parameters
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel                 *uiParamLoadValue;
    UISlider                *uiParamLoad;
    UILabel                 *uiParamRPMValue;
    UISlider                *uiParamRPM;    
    NSTimer                 *timer;

    FMOD::EventSystem       *eventSystem;
    FMOD::Event             *event;
    FMOD::EventParameter    *paramLoad;
    FMOD::EventParameter    *paramRPM;
    FMOD::EventCategory     *masterCategory;
}

@property (nonatomic, retain) IBOutlet UILabel  *uiParamLoadValue;
@property (nonatomic, retain) IBOutlet UISlider *uiParamLoad;
@property (nonatomic, retain) IBOutlet UILabel  *uiParamRPMValue;
@property (nonatomic, retain) IBOutlet UISlider *uiParamRPM;

- (IBAction)paramLoadChanged:(id)sender;
- (IBAction)paramRPMChanged:(id)sender;
- (IBAction)pauseResume:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
