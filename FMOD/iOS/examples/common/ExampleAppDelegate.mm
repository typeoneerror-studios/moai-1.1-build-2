#import "ExampleAppDelegate.h"
#import "ViewController.h"

@implementation ExampleAppDelegate

@synthesize window;
@synthesize controller;


- (void)applicationDidFinishLaunching:(UIApplication *)application 
{   
    // Set up the view controller
	ViewController *controllerInstance = [[ViewController alloc] initWithNibName:@"Example" bundle:[NSBundle mainBundle]];
	self.controller = controllerInstance;
	[controllerInstance release];
    
    [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleDefault];
    
	// Add the view controller's view as a subview of the window
	UIView *view = [controller view];
	[window addSubview:view];
	[window makeKeyAndVisible];
}


- (void)applicationWillTerminate:(UIApplication *)application
{

}


- (void)dealloc
{
	[controller release];
	[window release];
	[super dealloc];
}

@end
