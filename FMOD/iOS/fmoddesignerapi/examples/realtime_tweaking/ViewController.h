/*===============================================================================================
 Realtime Tweaking Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic usage of FMOD's network data-driven event library (fmod_event_net.dll)
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_event_net.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *uiEvent;
    UILabel             *uiParamType;
    UILabel             *uiParamValue;
    UILabel             *uiStatus;
    NSTimer             *timer;

    FMOD::EventSystem   *eventSystem;
    FMOD::EventGroup    *group;
    FMOD::Event         *event[3];
}

@property (nonatomic, retain) IBOutlet UILabel *uiEvent;
@property (nonatomic, retain) IBOutlet UILabel *uiParamType;
@property (nonatomic, retain) IBOutlet UILabel *uiParamValue;
@property (nonatomic, retain) IBOutlet UILabel *uiStatus;

- (IBAction)playEvent:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
