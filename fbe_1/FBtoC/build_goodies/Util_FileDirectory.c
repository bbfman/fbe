// this is included by Util_FileDirectory.incl
// it is not part of the regular runtime
/*
 *  Util_FileDirectory.c
 *
 *  bw:20080916
 
 //--------------------------------------------------------------------------------------------
// special Directories  - brian 2012
//--------------------------------------------------------------------------------------------
// Replacement for the three FD_SpecialDirectoryxxxxxx calls that were
// using the deprecated-in-OS X 10.8 FSFindFolder()
// Now calls Cocoa method via SystemDirectoryCopyURL()
//
// Input to function has changed.
// Instead of the old Folder.h constants ( such as kUserDomain, kDesktopFolderTypeetc. which are also deprecated ),
// it uses the 'NS' constants such as NSUserDomainMask, NSApplicationSupportDirectory, NSDownloadsDirectory etc. )
//
// See Util_FileDirectoryII.incl( in Headers ) for more domain and directory constants.
//--------------------------------------------------------------------------------------------
//
// Functions using FSRefs/FSSpecs have been wrapped in #if !__LP64__  Brian 20170930
 */

#include "Util_FileDirectory.h"
#include "SystemDirectoryCopyURL.h"

#pragma mark
#pragma mark Conversion
//--------------------------------------------------------------------------------------------
// conversion
//--------------------------------------------------------------------------------------------
#if !__LP64__

OSStatus FD_FSRefCreateCFURL( FSRef *ref, CFURLRef *url )
{	
	OSStatus	err = noErr;
	
	*url = CFURLCreateFromFSRef( kCFAllocatorDefault, ref );
	if ( *url == NULL ) err = kCFURLFSRefConversionErr;
	
	return err;
}


OSStatus FD_FSRefGetFSSpec( FSRef *ref, FSSpec *spec )
{
	return FSGetCatalogInfo( ref, kFSCatInfoNone, NULL, NULL, spec, NULL );
}


OSStatus FD_CFURLGetFSRef( CFURLRef url, FSRef *ref )
{
	OSStatus	err = noErr;
	
	if ( CFURLGetFSRef( url, ref ) == false ) err = kCFURLFSRefConversionErr;
	
	return err;
}


OSStatus FD_CFURLGetFSSpec( CFURLRef url, FSSpec *spec )
{
	FSRef		ref;
	OSStatus	err = noErr;
	
	if ( CFURLGetFSRef( url, &ref ) == false ) err = kCFURLFSRefConversionErr;
	if ( err == noErr ) err = FD_FSRefGetFSSpec( &ref, spec );
	
	return err;
}


OSStatus FD_FSSpecGetFSRef( FSSpec *spec, FSRef *ref )
{
	return FSpMakeFSRef( spec, ref );
}


OSStatus FD_FSSpecCreateCFURL( FSSpec *spec, CFURLRef *url )
{
	FSRef		ref;
	OSStatus	err;
	
	err = FSpMakeFSRef( spec, &ref );
	if ( err == noErr ) err = FD_FSRefCreateCFURL( &ref, url );
	
	return err;
}

#endif  /* __LP64__ */

#pragma mark
#pragma mark Object exists

//--------------------------------------------------------------------------------------------
// object exists
//--------------------------------------------------------------------------------------------
#if !__LP64__
Boolean FD_ObjectExistsFSRef( CFStringRef name, FSRef *parRef ) {
	HFSUniStr255	nameU;
	FSRef			ref;
	OSStatus		err;
	
	nameU.length = CFStringGetLength( name );
	CFStringGetCharacters( name, CFRangeMake( 0, nameU.length ), nameU.unicode );
	err = FSMakeFSRefUnicode( parRef, nameU.length, nameU.unicode, kTextEncodingUnknown, &ref );
	if ( err == fnfErr ) return false;
	
	return true;
}


Boolean FD_ObjectExistsCFURL( CFURLRef url ) {
	FSRef	ref;
	
	return ( FD_CFURLGetFSRef( url, &ref ) != kCFURLFSRefConversionErr );
}



Boolean FD_ObjectExistsFSSpec( FSSpec *spec ) {
	FSRef	ref;
	return ( FD_FSSpecGetFSRef( spec, &ref ) != fnfErr );
}

#endif  /* __LP64__ */

#pragma mark
#pragma mark Get object by path

//--------------------------------------------------------------------------------------------
// get object by path
//--------------------------------------------------------------------------------------------

OSStatus FD_PathCreateCFURL( CFStringRef path, CFURLRef parUrl, CFURLRef *url ) {
    OSStatus	err = noErr;
    
    if ( path != NULL ) {
        if ( parUrl == NULL ) {			// full path
            *url = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, path, kCFURLPOSIXPathStyle, false ); // was true rdp:20110608
        } else {	                    // partial path
            *url = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, parUrl, path, false ); // was true rdp:20110608
        }
        if ( *url == NULL ) err = kCFURLCreationErr;
    } else {
        err = kCFURLCreationErr;
    }
    return err;
}

#if !__LP64__

OSStatus FD_PathGetFSSpec( CFStringRef path, FSSpec *parSpec, FSSpec *spec )
{
	FSRef		ref;
	CFURLRef	parUrl, url;
	OSStatus	err;
	
	if ( parSpec == NULL )														// full path
	{
		err = FD_PathGetFSRef( path, NULL, &ref );
		if ( err == noErr ) err = FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, NULL, spec, NULL );
	}
	else																		// partial path
	{
		err = FD_FSSpecCreateCFURL( parSpec, &parUrl );
		if ( err == noErr )
		{
			url = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, parUrl, path, true );
			if ( url == NULL )
			{
				err = kCFURLCreationErr;
			}
			else
			{
				err = FD_CFURLGetFSSpec( url, spec );
				CFRelease( url );
			}
			CFRelease( parUrl );
		}
	}
	
	return err;
}
OSStatus FD_PathGetFSRef( CFStringRef path, FSRef *parRef, FSRef *ref )
{
    char		*buffer;
    CFURLRef	parUrl, dirUrl;
    CFIndex		length;
    OSStatus	err = noErr;
    
    if ( parRef == NULL )														// full path
    {
        length = CFStringGetMaximumSizeForEncoding( CFStringGetLength( path ), kCFStringEncodingUTF8 ) + 1;
        buffer = malloc( length );
        if ( buffer == NULL ) return mFulErr;
        CFStringGetCString( path, buffer, length, kCFStringEncodingUTF8 );
        err = FSPathMakeRef( (UInt8 *)buffer, ref, NULL );
        free( buffer );
    }
    else																		// partial path
    {
        parUrl = CFURLCreateFromFSRef( kCFAllocatorDefault, parRef );
        if ( parUrl == NULL ) err = kCFURLFSRefConversionErr;
        if ( err == noErr )
        {
            dirUrl = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, parUrl, path, true );
            if ( dirUrl == NULL ) err = kCFURLCreationErr;
            if ( err == noErr )
            {
                if ( CFURLGetFSRef( dirUrl, ref ) == false ) err = kCFURLFSRefConversionErr;
                CFRelease( dirUrl );
            }
            CFRelease( parUrl );
        }
    }
    return err;
}


#endif  /* __LP64__ */


#pragma mark
#pragma mark Get path

//--------------------------------------------------------------------------------------------
// get path
//--------------------------------------------------------------------------------------------
#if !__LP64__
OSStatus FD_FSRefCreatePath( FSRef *ref, CFStringRef *path )
{
	CFURLRef	url;
	
	OSStatus err = FD_FSRefCreateCFURL( ref, &url );	
	if ( err == noErr ) 
	{
		*path = CFURLCopyFileSystemPath( url, kCFURLPOSIXPathStyle );
		CFRelease( url );
	}
	return err;
}


OSStatus FD_CFURLCreatePath( CFURLRef url, CFStringRef *path )
{
	FSRef		ref;
	OSStatus	err;
	
	err = FD_CFURLGetFSRef( url, &ref );
	if ( err == noErr ) err = FD_FSRefCreatePath( &ref, path );
	
	return err;	
}



OSStatus FD_FSSpecCreatePath( FSSpec *spec, CFStringRef *path )
{
	FSRef		ref;
	OSStatus	err;
	
	err = FD_FSSpecGetFSRef( spec, &ref );
	if ( err == noErr ) err = FD_FSRefCreatePath( &ref, path );
	
	return err;
}

#endif  /* __LP64__ */

#pragma mark
#pragma mark Create directory

//--------------------------------------------------------------------------------------------
// create directory
//--------------------------------------------------------------------------------------------
#if !__LP64__
OSStatus FD_CreateDirectoryWithFSRef( CFStringRef name, FSRef *parRef, FSRef *dirRef )
{
	HFSUniStr255	nameU;
	OSStatus		err;
	
	nameU.length = CFStringGetLength( name );
	CFStringGetCharacters( name, CFRangeMake( 0, nameU.length ), nameU.unicode );
	err = FSCreateDirectoryUnicode( parRef, nameU.length, nameU.unicode, kFSCatInfoNone, NULL, dirRef, NULL, NULL );
	if ( err == dupFNErr )
	{
		err = FD_PathGetFSRef( name, parRef, dirRef );
		if ( err == noErr ) err = dupFNErr;
	}
	
	return err;
}


OSStatus FD_CreateDirectoryWithCFURL( CFStringRef name, CFURLRef parUrl, CFURLRef *dirUrl )
{
	FSRef		dirRef, parRef;
	OSStatus	err;
	Boolean		dup = false;
	
	err = FD_CFURLGetFSRef( parUrl, &parRef );
	if ( err == noErr ) err = FD_CreateDirectoryWithFSRef( name, &parRef, &dirRef );
	if ( err == dupFNErr )
	{
		err = noErr;
		dup = true;
	}
	if ( err == noErr ) err = FD_FSRefCreateCFURL( &dirRef, dirUrl );
	if ( err != noErr ) return err;
	
	*dirUrl = CFURLCreateFromFSRef( kCFAllocatorDefault, &dirRef );
	if ( *dirUrl == NULL ) err = kCFURLFSRefConversionErr;
	if ( err == noErr && dup == true ) err = dupFNErr;
	
	return err;
}



OSStatus FD_CreateDirectoryWithFSSpec( CFStringRef name, FSSpec *parSpec, FSSpec *dirSpec )
{
	FSRef		parRef, dirRef;
	OSStatus	err;
	Boolean		dup = false;
	
	err = FSpMakeFSRef( parSpec, &parRef );
	if ( err == noErr ) err = FD_CreateDirectoryWithFSRef( name, &parRef, &dirRef );
	if ( err == dupFNErr )
	{ 
		err = noErr;
		dup = true;
	}
	if ( err == noErr ) err = FSGetCatalogInfo( &dirRef, kFSCatInfoNone, NULL, NULL, dirSpec, NULL );
	if ( err == noErr && dup == true ) err = dupFNErr;
	
	return err;
}

#endif  /* __LP64__ */

#pragma mark
#pragma mark Get parent directory
//--------------------------------------------------------------------------------------------
// get parent directory
//--------------------------------------------------------------------------------------------
#if !__LP64__
OSStatus FD_ParentDirectoryGetFSRef( FSRef *ref, FSRef *parRef )
{	
	return FSGetCatalogInfo( ref, kFSCatInfoNone, NULL, NULL, NULL, parRef );
}
#endif  /* __LP64__ */

OSStatus FD_ParentDirectoryCreateCFURL( CFURLRef url, CFURLRef *parUrl )
{
	OSStatus	err = noErr;
	
	*parUrl = CFURLCreateCopyDeletingLastPathComponent( kCFAllocatorDefault, url );
	if ( *parUrl == NULL ) err = kCFURLCreationErr;
	
	return err;
}

#if !__LP64__

OSStatus FD_ParentDirectoryGetFSSpec( FSSpec *spec, FSSpec *parSpec )
{
	FSRef		ref, parRef;
	OSStatus	err;
	
	err = FSpMakeFSRef( spec, &ref );
	if ( err == noErr ) err = FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, NULL, NULL, &parRef );
	if ( err == noErr ) err = FSGetCatalogInfo( &parRef, kFSCatInfoNone, NULL, NULL, parSpec, NULL );
	
	return err;
}
#endif  /* __LP64__ */

#pragma mark
#pragma mark Get application directory
//--------------------------------------------------------------------------------------------
// get application directory
//--------------------------------------------------------------------------------------------
#if !__LP64__
OSStatus FD_ApplicationDirectoryGetFSRef( FSRef *appRef )
{
	FSRef		ref;
	OSStatus	err;
	
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	err = GetProcessBundleLocation( &psn, &ref );
	if ( err == noErr ) err = FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, NULL, NULL, appRef );
	
	return err;
}


OSStatus FD_ApplicationDirectoryCreateCFURL( CFURLRef *appUrl )
{
	FSRef		ref;
	OSStatus	err;
	
	err = FD_ApplicationDirectoryGetFSRef( &ref );
	if ( err == noErr ) err = FD_FSRefCreateCFURL( &ref, appUrl );
	
	return err;
}


OSStatus FD_ApplicationDirectoryGetFSSpec( FSSpec *appSpec )
{
	FSRef		ref;
	OSStatus	err;
	
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	err = GetProcessBundleLocation( &psn, &ref );
	if ( err == noErr ) err = FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, NULL, NULL, &ref );
	if ( err == noErr ) err = FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, NULL, appSpec, NULL );
	
	return err;
}

#endif  /* __LP64__ */

#pragma mark
#pragma mark Special directories

//--------------------------------------------------------------------------------------------
// special Directories
//--------------------------------------------------------------------------------------------
// Replacement for the three FD_SpecialDirectoryxxxxxx calls that were
// using the deprecated-in-OS X 10.8 FSFindFolder()
// Now calls Cocoa method via SystemDirectoryCopyURL()
//
// Input to function has changed.
// Instead of the old Folder.h constants ( such as kUserDomain, kDesktopFolderTypeetc. which are also deprecated ),
// it uses the 'NS' constants such as NSUserDomainMask, NSApplicationSupportDirectory, NSDownloadsDirectory etc. )
//
// See Util_FileDirectoryII.incl( in Headers ) for more domain and directory constants.
//--------------------------------------------------------------------------------------------
#if !__LP64__
OSStatus FD_SpecialDirectoryGetFSRef( SInt32 domain, UInt32 whichDirectory, FSRef *ref )
{
    OSStatus err = kCFURLCreationErr;
    CFURLRef url  = SystemDirectoryCopyURL( whichDirectory, domain );
    if ( url ) {
        err  = FD_CFURLGetFSRef( url, ref );
        CFRelease( url );
    }
    return err;
}


OSStatus FD_SpecialDirectoryCreateCFURL( SInt32 domain, UInt32 whichDirectory, CFURLRef *oUrl )
{
    OSStatus err = fnfErr;
    *oUrl  = SystemDirectoryCopyURL( whichDirectory, domain );
    if ( oUrl ) err = noErr;
    return err;
}



OSStatus FD_SpecialDirectoryGetFSSpec( SInt32 domain, UInt32 whichDirectory, FSSpec *spec )
{
    OSStatus err = kCFURLCreationErr;
    CFURLRef url = SystemDirectoryCopyURL( whichDirectory, domain );
    if ( url ) {
        err = FD_CFURLGetFSSpec( url, spec );
        CFRelease( url );
    }
	return err;
}

#endif  /* __LP64__ */
#pragma mark
#pragma mark Rename

//--------------------------------------------------------------------------------------------
// rename
//--------------------------------------------------------------------------------------------
#if !__LP64__
OSStatus FD_RenameObjectFSRef( FSRef *ref, CFStringRef name, FSRef *outRef )
{
	HFSUniStr255	nameU;
	
	nameU.length = CFStringGetLength( name );
	CFStringGetCharacters( name, CFRangeMake( 0, nameU.length ), nameU.unicode );
	return FSRenameUnicode( ref, nameU.length, nameU.unicode, kTextEncodingUnknown, outRef );
}

#pragma mark
#pragma mark Query

//--------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------
Boolean FD_IsDirectoryFSRef( FSRef *ref )
{
	FSCatalogInfo	info;
	
	FSGetCatalogInfo( ref, kFSCatInfoNodeFlags, &info, NULL, NULL, NULL );
	return ( (info.nodeFlags & kFSNodeIsDirectoryMask) != 0 );
}
#endif  /* __LP64__ */


