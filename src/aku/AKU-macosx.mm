#import <aku/AKU-macosx.h>
#import <moaiext-macosx/moaiext-macosx.h>

#include <moaicore/moaicore.h>

void AKUMacOSXInit () {

	MOAIEnvironment& environment = MOAIEnvironment::Get ();

	environment.SetValue ( MOAI_ENV_cacheDirectory,	[ [ NSSearchPathForDirectoriesInDomains ( NSCachesDirectory, NSUserDomainMask, YES ) objectAtIndex:0 ] UTF8String ]);
	environment.SetValue ( MOAI_ENV_documentDirectory, [ [ NSSearchPathForDirectoriesInDomains ( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex:0 ] UTF8String ]);

}