/*===============================================================================================
 Offline Decoding Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how decode a file to PCM, without playing it.
 It writes out the data as a raw data file.
 The System::createSound function uses FMOD_OPENONLY so that FMOD does not read any data 
 itself.
 If this is uses then it is up to the user to use Sound::readData to get the data out of 
 the file and into the destination buffer.
===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
    UILabel        *status;

    FMOD::System   *system;
    FMOD::Sound    *sound;
}

@property (nonatomic, retain) IBOutlet UILabel *status;

- (IBAction)decodeSound:(id)sender;

@end

