/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic usage of event effects
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel                 *uiParamValue;
    UISlider                *uiParam;
    NSTimer                 *timer;

    FMOD::EventSystem       *eventSystem;
    FMOD::Event             *event;
    FMOD::EventParameter    *param00;
}

@property (nonatomic, retain) IBOutlet UILabel  *uiParamValue;
@property (nonatomic, retain) IBOutlet UISlider *uiParam;

- (IBAction)paramChanged:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
