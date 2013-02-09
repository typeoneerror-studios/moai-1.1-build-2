/*==============================================================================
 Programmer Sound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2006-2011.
 
 Demonstrates how to use the "programmer sound" feature of the FMOD event system
 ==============================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *uiIndexValue;
    UISlider            *uiIndex;
    UILabel             *uiStatus;
    NSTimer             *timer;

    FMOD::EventSystem   *eventSystem;
    FMOD::System        *system;
    FMOD::EventGroup    *group;
    FMOD::Event         *event;
}

@property (nonatomic, retain) IBOutlet UILabel  *uiIndexValue;
@property (nonatomic, retain) IBOutlet UISlider *uiIndex;
@property (nonatomic, retain) IBOutlet UILabel  *uiStatus;

- (IBAction)playEvent:(id)sender;
- (IBAction)indexChanged:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
