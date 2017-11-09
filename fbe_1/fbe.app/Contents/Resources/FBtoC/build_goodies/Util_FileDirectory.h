// this is included by Util_FileDirectory.incl
// it is not part of the regular runtime
/*
 *  Util_FileDirectory.h
 */

#ifndef UTIL_FILEDIRECTORY_H
#define UTIL_FILEDIRECTORY_H

#include <Carbon/Carbon.h>

enum {
	kCFURLFSRefConversionErr = -32021,
	kCFURLCreationErr		 = -32022
};


//--------------------------------------------------------------------------------------------
// conversion
//--------------------------------------------------------------------------------------------
OSStatus
FD_FSRefCreateCFURL( FSRef *ref, CFURLRef *url );


OSStatus
FD_FSRefGetFSSpec( FSRef *ref, FSSpec *spec );

OSStatus
FD_CFURLGetFSRef( CFURLRef url, FSRef *ref );

OSStatus
FD_CFURLGetFSSpec( CFURLRef url, FSSpec *spec );

OSStatus
FD_FSSpecGetFSRef( FSSpec *spec, FSRef *ref );

OSStatus
FD_FSSpecCreateCFURL( FSSpec *spec, CFURLRef *url );


//--------------------------------------------------------------------------------------------
// object exists
//--------------------------------------------------------------------------------------------
Boolean
FD_ObjectExistsFSRef( CFStringRef name, FSRef *parRef );


Boolean
FD_ObjectExistsCFURL( CFURLRef url );

Boolean
FD_ObjectExistsFSSpec( FSSpec *spec );

//--------------------------------------------------------------------------------------------
// get object by path
//--------------------------------------------------------------------------------------------
OSStatus
FD_PathGetFSRef( CFStringRef path, FSRef *parRef, FSRef *ref );


OSStatus
FD_PathCreateCFURL( CFStringRef path, CFURLRef parUrl, CFURLRef *url );


OSStatus
FD_PathGetFSSpec( CFStringRef path, FSSpec *parSpec, FSSpec *spec );

//--------------------------------------------------------------------------------------------
// get path
//--------------------------------------------------------------------------------------------
OSStatus
FD_FSRefCreatePath( FSRef *ref, CFStringRef *path );


OSStatus
FD_CFURLCreatePath( CFURLRef url, CFStringRef *path );

OSStatus
FD_FSSpecCreatePath( FSSpec *spec, CFStringRef *path );

//--------------------------------------------------------------------------------------------
// create directory
//--------------------------------------------------------------------------------------------
OSStatus
FD_CreateDirectoryWithFSRef( CFStringRef name, FSRef *parRef, FSRef *dirRef );


OSStatus
FD_CreateDirectoryWithCFURL( CFStringRef name, CFURLRef parUrl, CFURLRef *dirUrl );

OSStatus
FD_CreateDirectoryWithFSSpec( CFStringRef name, FSSpec *parSpec, FSSpec *dirSpec );

//--------------------------------------------------------------------------------------------
// get parent directory
//--------------------------------------------------------------------------------------------
OSStatus
FD_ParentDirectoryGetFSRef( FSRef *ref, FSRef *parRef );

OSStatus
FD_ParentDirectoryCreateCFURL( CFURLRef url, CFURLRef *parUrl );

OSStatus
FD_ParentDirectoryGetFSSpec( FSSpec *spec, FSSpec *parSpec );

//--------------------------------------------------------------------------------------------
// get application directory
//--------------------------------------------------------------------------------------------
OSStatus
FD_ApplicationDirectoryGetFSRef( FSRef *appRef );

OSStatus
FD_ApplicationDirectoryCreateCFURL( CFURLRef *appUrl );

OSStatus
FD_ApplicationDirectoryGetFSSpec( FSSpec *appSpec );

//--------------------------------------------------------------------------------------------
// special Directories
//--------------------------------------------------------------------------------------------
OSStatus FD_SpecialDirectoryGetFSRef( SInt32 domain, UInt32 whichDirectory, FSRef *ref );
OSStatus FD_SpecialDirectoryCreateCFURL( SInt32 domain, UInt32 whichDirectory, CFURLRef *oUrl );
OSStatus FD_SpecialDirectoryGetFSSpec( SInt32 domain, UInt32 whichDirectory, FSSpec *spec );

//--------------------------------------------------------------------------------------------
// rename
//--------------------------------------------------------------------------------------------
OSStatus
FD_RenameObjectFSRef( FSRef *ref, CFStringRef name, FSRef *outRef );


//--------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------
Boolean
FD_IsDirectoryFSRef( FSRef *ref );

#endif /* UTIL_FILEDIRECTORY_H */
