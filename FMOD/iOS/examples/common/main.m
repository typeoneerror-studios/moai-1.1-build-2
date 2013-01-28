#import <UIKit/UIKit.h>

int main(int argc, char *argv[])
{
    NSAutoreleasePool  *pool    = NULL;
    int                 retVal  = 0;
    
    pool    = [[NSAutoreleasePool alloc] init];
    retVal  = UIApplicationMain(argc, argv, nil, nil);
    
    [pool release];
    return retVal;
}
