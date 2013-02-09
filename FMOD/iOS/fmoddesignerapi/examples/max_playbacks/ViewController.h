/*=============================================================================
 Max Playbacks Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 Demonstrates basic usage of event max playbacks behaviour.
 =============================================================================*/

#import "fmod_event.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController <UIPickerViewDelegate>
{
    UIPickerView        *uiMaxPlayBehavior;
    UILabel             *uiDistanceValue;
    UISlider            *uiDistance;
    UILabel             *uiSound;
    UILabel             *uiStatus;
    NSTimer             *timer;
    NSArray             *maxPlayBehaviorData;

    FMOD::EventSystem   *eventSystem;
    FMOD::EventGroup    *group;
    FMOD::Event         *event;
    int                  sound;
    
    enum eventBehavior
    {
        StealOldest,
        StealNewest,
        StealQuietest,
        JustFail,
        JustFailIfQuietest
    };    
}

@property (nonatomic, retain) IBOutlet UIPickerView *uiMaxPlayBehavior;
@property (nonatomic, retain) IBOutlet UILabel      *uiDistanceValue;
@property (nonatomic, retain) IBOutlet UISlider     *uiDistance;
@property (nonatomic, retain) IBOutlet UILabel      *uiSound;
@property (nonatomic, retain) IBOutlet UILabel      *uiStatus;

- (IBAction)startEvent:(id)sender;
- (IBAction)stopAllEvents:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
