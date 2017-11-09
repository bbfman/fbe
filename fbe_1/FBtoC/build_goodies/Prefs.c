// prefix header, with public (user-code-reachable) struct and typedef declarations
#include "FBtoC.h"
#include "Runtime.h"

/*
 *  Prefs.c
 conditionally included runtime support for get preferences, put preferences, kill preferences
 */


void FBPutPreferences( Str255 name, void *prefsPtr, CFIndex size ) {
    CFDataRef	dataRef = CFDataCreate( 0, prefsPtr, size );
    if ( dataRef ) {
        CFStringRef	string = CFStringCreateWithPascalString( NULL, name, kCFStringEncodingMacRoman );
        if ( string ) {
            CFPreferencesSetAppValue( CFSTR( "FBGetPutPreferences" ), dataRef, string );
            CFPreferencesAppSynchronize( string );
            CFRelease( string );
        }
        CFRelease( dataRef );
    }
}


void FBGetPreferences( Str255 name, void *prefsPtr ) {
    CFStringRef	string = CFStringCreateWithPascalString( NULL, name, kCFStringEncodingMacRoman );
    if ( string ) {
        CFDataRef	dataRef = CFPreferencesCopyAppValue( CFSTR( "FBGetPutPreferences" ), string );
        if ( dataRef ) {
            if ( CFGetTypeID( dataRef) == CFDataGetTypeID() ) {
                CFRange	range = CFRangeMake( 0, CFDataGetLength( dataRef ) );
                CFDataGetBytes( dataRef, range, prefsPtr );
            }
            CFRelease( dataRef );
        }
        CFRelease( string );
    }
}



void FBKillPreferences( Str255 name ) {
    
    CFStringRef	cfName = CFStringCreateWithPascalString( NULL, name, kCFStringEncodingMacRoman );
    if ( cfName ) {
        CFURLRef prefsDirURL  = (CFURLRef)[NSURL fileURLWithPath:[@"~/Library/Preferences" stringByExpandingTildeInPath]];
        CFStringRef plistName = CFStringCreateWithFormat( NULL, NULL, CFSTR( "%@.plist" ), cfName );
        CFURLRef url          = (CFURLRef)[(NSURL *)prefsDirURL URLByAppendingPathComponent:(NSString *)plistName];
        if ( url ) {
            short		p = 0;
            FBPutPreferences( name, &p, 0 ); // null out the existing prefs file and (more importantly) the CFPreferences cache
            [[NSFileManager defaultManager] removeItemAtURL:(NSURL *)url error:nil];
        }
        CFRelease( plistName );
        CFRelease( cfName );
    }
}





