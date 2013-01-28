/*===============================================================================================
 ChannelGroups Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to put channels into channel groups, so that you can affect a group
 of channels at a time instead of just one channel at a time.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel             *channels;
    NSTimer             *timer;

    FMOD::System        *system;
    FMOD::Sound         *sound[6];
    FMOD::Channel       *channel[6];
    FMOD::ChannelGroup  *groupA;
    FMOD::ChannelGroup  *groupB;
    FMOD::ChannelGroup  *masterGroup;
}

@property (nonatomic, retain) IBOutlet UILabel *channels;

- (IBAction)toggleMuteA:(id)sender;
- (IBAction)toggleMuteB:(id)sender;
- (IBAction)toggleMuteMaster:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end
