/*===============================================================================================
 NetStream Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to play streaming audio from the internet
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController <UITextFieldDelegate>
{
	UILabel        *time;
    UILabel        *status;
	UILabel        *buffered;
    UILabel        *lasttag;
    UITextField    *urltext;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Channel  *channel;
}

@property (nonatomic, retain) IBOutlet UILabel      *time;
@property (nonatomic, retain) IBOutlet UILabel      *status;
@property (nonatomic, retain) IBOutlet UILabel      *buffered;
@property (nonatomic, retain) IBOutlet UILabel      *lasttag;
@property (nonatomic, retain) IBOutlet UITextField  *urltext;

- (IBAction)pauseResume:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

