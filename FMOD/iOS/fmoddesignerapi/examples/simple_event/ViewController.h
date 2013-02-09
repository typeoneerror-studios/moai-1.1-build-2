/*===============================================================================================
 SimpleEvent Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic usage of FMOD's data-driven event library.
 ===============================================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *uiStatus;
    NSTimer             *timer;

    FMOD::EventSystem   *eventSystem;
    FMOD::Event         *event;
}

@property (nonatomic, retain) IBOutlet UILabel *uiStatus;

- (IBAction)playEvent:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

