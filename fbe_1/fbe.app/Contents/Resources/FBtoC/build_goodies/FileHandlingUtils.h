#import <AppKit/AppKit.h>
#include "FBtoC.h"
#include "Runtime.h"
//#include "SystemDirectoryCopyURL.h"

///////////////////////////////////////////////////
//  prototypes + translated #defines and records //
///////////////////////////////////////////////////
FBBoolean       FH_FileExistsAtPath( CFStringRef path ) ;
FBBoolean       FH_LastFileExists( CFURLRef url ) ;
CFURLRef        FH_AppendComponentToURL( CFURLRef baseURL, CFStringRef componentToAppend ) ;
CFStringRef     FH_AppendComponentToString( CFStringRef basePath, CFStringRef componentToAppend ) ;
CFURLRef        FH_GetBundleResourceDirectoryURL() ;
CFURLRef        FH_GetSysDirAndPathURL( UInt32 whichDirectory, UInt32 whichDomain, CFStringRef whichAppendComponent ) ;
long            FH_GetFileType( CFURLRef url, OSType * type ) ;
void            FH_SetTypeCreator( CFURLRef fileURL, OSType type, OSType creator ) ;
UInt64          FH_FileLength( int fileDescriptor ) ;
UInt64          FH_CurrentOffsetInFile( int fileDescriptor );
OSStatus        FH_SetFilePointerToEndOfFile( SInt32 fileDescriptor );
NSFileHandle *  FH_InitFileHandleFromFileDescriptor( int fileDescriptor );

NSData *        FH_ReadData( int fd, NSUInteger length, UInt64 *offset );
long            FH_PrintFile( CFURLRef url, CFStringRef string ) ;
CFStringRef     FH_InputFile( CFURLRef url, CFStringRef string ) ;
OSStatus        FH_ReadChar( int fd, UInt64 * offset, UInt8 *value );
void            FH_ReadFile( int fd, void *p, UInt64 length, UInt64 *offset );

UInt64          FH_WriteData( int fd, CFDataRef inData, UInt64 inOffset );

UInt64          FH_WriteFile( int fd, void *p, UInt64 length, UInt64 offset );

UInt64          FH_WriteChar( int fd, char theChar, UInt64 offset ) ;
BOOL            FH_BuildEmptyFile( CFURLRef url );
long            FH_FSpTrashObject( CFStringRef sourceObj ) ;
CFStringRef     FH_URLGetPath( CFURLRef url );
UInt64          FH_TruncateFileAtOffset( NSFileHandle *fh, UInt64 inOffset );
void            FH_SeekToFileOffset( NSFileHandle *fh, UInt64 inOffset  );
OSStatus        FH_IsFileAvailable( CFURLRef url, char openMode, int *fd );
//CFURLRef        OSPanelOpen( UInt32 options, CFStringRef message, CFTypeRef allowedFileTypes, CFStringRef prompt, CFURLRef dirURL );

