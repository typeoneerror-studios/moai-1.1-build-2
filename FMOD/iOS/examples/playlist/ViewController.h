/*===============================================================================================
 PlayList Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to load a playlist and play the sounds in a playlist.
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UILabel        *time;
    UILabel        *status;
    UILabel        *soundtitle;
    NSTimer        *timer;

    FMOD::System   *system;
    FMOD::Sound    *sound;
    FMOD::Sound    *playlist;
    FMOD::Channel  *channel;
    bool            isplaylist;
    int             listindex;
}

@property (nonatomic, retain) IBOutlet UILabel *time;
@property (nonatomic, retain) IBOutlet UILabel *status;
@property (nonatomic, retain) IBOutlet UILabel *soundtitle;

- (IBAction)playNext:(id)sender;
- (IBAction)pauseResume:(id)sender;
- (void)timerUpdate:(NSTimer *)timer;

@end

