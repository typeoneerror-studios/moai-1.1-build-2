#import <UIKit/UIKit.h>

@class ViewController;

@interface ExampleAppDelegate : NSObject <UIApplicationDelegate>
{
	IBOutlet UIWindow  *window;
	ViewController     *controller;
}

@property (nonatomic, retain) UIWindow        *window;
@property (nonatomic, retain) ViewController  *controller;

@end
