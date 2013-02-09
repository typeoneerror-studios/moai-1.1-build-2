/*===============================================================================================
 ReadTags Example
 Copyright (c), Firelight Technologies Pty, Ltd 2008-2011.
 
 This example shows how to read tags from sound files
 ===============================================================================================*/

#import "fmod.hpp"
#import "fmod_errors.h"
#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
	UITextView *outputtext;
}

@property (nonatomic, retain) IBOutlet UITextView *outputtext;

@end
