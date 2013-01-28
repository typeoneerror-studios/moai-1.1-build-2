/*===============================================================================================
 Load Event Data Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic loading and unloading of event data per event
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *uiMemCurrent;
    UILabel             *uiMemMaximum;
    UILabel             *uiStatus;
    NSTimer             *timer;

    FMOD::EventSystem   *eventSystem;
    FMOD::EventGroup    *group;
    FMOD::Event         *event;
}

@property (nonatomic, retain) IBOutlet UILabel  *uiMemCurrent;
@property (nonatomic, retain) IBOutlet UILabel  *uiMemMaximum;
@property (nonatomic, retain) IBOutlet UILabel  *uiStatus;

- (IBAction)getEvent:(id)sender;
- (IBAction)freeEventData:(id)sender;
- (IBAction)loadGroupData:(id)sender;
- (IBAction)freeGroupData:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
