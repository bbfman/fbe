#include "FileHandlingUtils.h"
#include "ArrayIndices.h"

///////////////////////////////////////////////////
//       translated globals and fns              //
///////////////////////////////////////////////////

FBBoolean  FH_FileExistsAtPath( CFStringRef path ) {
    FBBoolean  exists = false;
    if( path ) exists = [[NSFileManager defaultManager] fileExistsAtPath:(NSString *)path];
    return exists;
}


FBBoolean  FH_LastFileExists( CFURLRef url ) {
    FBBoolean  exists = false;
    if( url ) exists = FH_FileExistsAtPath((CFStringRef)[(NSURL *)url path] );
    return exists;
}


CFURLRef  FH_AppendComponentToURL( CFURLRef baseURL, CFStringRef componentToAppend ) {
    return(CFURLRef)[(NSURL *)baseURL URLByAppendingPathComponent:(NSString *)componentToAppend];
    return NULL;
}


CFStringRef  FH_AppendComponentToString( CFStringRef basePath, CFStringRef componentToAppend ) {
    return(CFStringRef)[(NSString *)basePath stringByAppendingPathComponent:(NSString *)componentToAppend];
    return NULL;
}


CFURLRef  FH_GetBundleResourceDirectoryURL() {
    return(CFURLRef)[[NSBundle mainBundle] resourceURL];
    return NULL;
}

/*
 CFURLRef  FH_GetSysDirAndPathURL( UInt32 whichDirectory, UInt32 whichDomain, CFStringRef whichAppendComponent ) {
 CFURLRef          url1;
 CFURLRef          url2;
 url1 = SystemDirectoryCopyURL( whichDirectory, whichDomain );
 url2 = FH_AppendComponentToURL( url1, whichAppendComponent );
 CFRelease( url1 );
 return url2;
 }
 */

long  FH_GetFileType( CFURLRef url, OSType * type ) {
    NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:[(NSURL *)url path] error:nil];
    *type = [[attributes objectForKey:NSFileHFSTypeCode] unsignedLongValue];
    //*creator = [[attributes objectForKey:NSFileHFSCreatorCode] unsignedLongValue];
    return 0;
}


void  FH_SetTypeCreator( CFURLRef fileURL, OSType type, OSType creator ) {
    NSString *filePath = [(NSURL *)fileURL path];
    NSNumber *typeNum = [NSNumber numberWithInt:type];
    NSNumber *creatorNum = [NSNumber numberWithInt:creator];
    NSMutableDictionary *attributes = [NSMutableDictionary dictionary];
    [attributes setObject:typeNum forKey:NSFileHFSTypeCode];
    [attributes setObject:creatorNum forKey:NSFileHFSCreatorCode];
    [[NSFileManager defaultManager] setAttributes:attributes ofItemAtPath:filePath error:nil];
}

NSFileHandle * FH_InitFileHandleFromFileDescriptor( int fileDescriptor ) {
    return [[NSFileHandle alloc]initWithFileDescriptor:fileDescriptor closeOnDealloc:NO];
}

OSStatus FH_SetFilePointerToEndOfFile( SInt32 fileDescriptor ) {
    OSStatus err = noErr;
    
    NSFileHandle *fh = FH_InitFileHandleFromFileDescriptor( fileDescriptor );
    @try {
        [fh seekToFileOffset: [fh seekToEndOfFile]];
        
    } @catch (NSException *theException) {
        if (theException.name == NSFileHandleOperationException) {
            err = fnOpnErr;
        }
    }
    
    //@finally {
    
    //}
    [fh release];
    return err;
}

UInt64  FH_FileLength( int fileDescriptor ) {
    NSFileHandle *fh = FH_InitFileHandleFromFileDescriptor( fileDescriptor );
    UInt64    offSet = [fh offsetInFile];     // save current offset
    UInt64    length = [fh seekToEndOfFile];
    [fh seekToFileOffset:offSet];             // restore offset in file
    [fh release];
    return length;
}

UInt64 FH_CurrentOffsetInFile( int fileDescriptor ) {
    NSFileHandle *fh = FH_InitFileHandleFromFileDescriptor( fileDescriptor );
    UInt64    offSet = [fh offsetInFile];
    [fh release];
    return offSet;
}
//NSData *ReadData( CFURLRef url, NSUInteger length, UInt64 *offset )
NSData * FH_ReadData( int fd, NSUInteger length, UInt64 *offset ) {
    NSFileHandle *fh = FH_InitFileHandleFromFileDescriptor( fd );
    [fh seekToFileOffset:*offset];
    NSData *data = [fh readDataOfLength:length];
    *offset = [fh offsetInFile];
    [fh release];
    return data;
}

long  FH_PrintFile( CFURLRef url, CFStringRef string ) {
    [(NSString *)string writeToURL:(NSURL *)url atomically:NO encoding:NSUTF8StringEncoding error:nil];
    return 0;
}


CFStringRef  FH_InputFile( CFURLRef url, CFStringRef string ) {
    string = (CFStringRef)[NSString stringWithContentsOfURL:(NSURL *)url encoding:NSUTF8StringEncoding error:nil];
    return string;
}


OSStatus  FH_ReadChar( int fd, UInt64 * offset, UInt8 *value ) {
    OSStatus err = 0;
    NSData *data = FH_ReadData( fd, sizeof(char), offset );
    if( data != nil ) {
        [data getBytes:value length:[data length]];
    } else {
        err = ioErr;
    }
    return err;
}

void  FH_ReadFile( int fd, void *p, UInt64 length, UInt64 *offset ) {
    NSData *data = FH_ReadData( fd, length, offset ); // allow for file already being read and this is a subsequent read.
    [data getBytes:p length: [data length]];
}

void FH_SeekToFileOffset( NSFileHandle *fh, UInt64 inOffset  ) {
    [fh seekToFileOffset:inOffset];
}

UInt64 FH_TruncateFileAtOffset( NSFileHandle *fh,  UInt64 inOffset ) {
    FH_SeekToFileOffset( fh, inOffset  );
    [fh truncateFileAtOffset: inOffset];
    //[fh closeFile];
    return inOffset;
}

OSStatus FH_CheckFileLockStatus( CFURLRef url, int *fileLockStat ) {
    NSError *fhError = nil;
    OSStatus err = noErr;
    int localFD;
    struct flock fileLockRec = { 0, 0, 0, 0, 0 };
    
    NSFileHandle *fh = [NSFileHandle fileHandleForUpdatingURL: (NSURL *)url error:&fhError];   // just to check to lock. Not passed back for FBer
    
    if ( fhError ) {
        err = [fhError code];
    } else {
        localFD = [fh fileDescriptor];
        fileLockRec.l_type = F_WRLCK;
        int errNo = fcntl( localFD, F_GETLK, &fileLockRec );
        if ( errNo >= 0 ) {
            *fileLockStat = (int)fileLockRec.l_type;
        } else {
            err = errNo;
            if ( err == ENOTSUP ) {                 // possibly a network file...
                err = flock( localFD, LOCK_UN );    //...if no error, we supposing file is unlocked...
                if ( err == noErr )
                    *fileLockStat = F_UNLCK;
                else
                    err = errno;
            }
        }
        close( localFD );
    }
    return err;
}


// 'Updateable' means available for reading and writing
// 'Writable'   menas available only for writing.
OSStatus FH_IsFileAvailable( CFURLRef url, char modeRequested, int *fd  ) {
    OSStatus err  = noErr;
    NSError *fhError = nil;
    int fileLockStat = 0,lockError = 0, localFD = -1;
    
    err = FH_CheckFileLockStatus( url, &fileLockStat );
    if ( err == 0 ) {
        switch ( fileLockStat ) {
            case F_RDLCK:                            // at least one process has a shared lock
                switch ( modeRequested ) {
                    case 'N':
                        err = opWrErr; // -49 *file already open with with write permission NOTE: lack of 'break' here is intentional.
                    case 'I': {
                        NSFileHandle *fh = [NSFileHandle fileHandleForReadingFromURL:(NSURL *)url error:&fhError];   // just to check to lock. Not passed back for FBer
                        if ( fhError ) {
                            err = [fhError code];
                        } else {
                            localFD = [fh fileDescriptor];
                            lockError = flock( localFD, LOCK_SH | LOCK_NB );   //    create shared lock
                        }
                        break;
                    }
                    case 'R':                        // OPEN "R" "O", "A" all require exclusive so this request fails
                    case 'O':
                    case 'A':err = wrPermErr; break;  // Is already a shared lock, so we can't allow this request to have exclusive write access
                }
                break;
            case F_UNLCK:  // means file is unlocked and available. "N" "I" get shared locks and "O" "A" "R" an exclusive lock
                switch ( modeRequested ) {
                    case 'N':{
                        if ( FH_FileExistsAtPath( (CFStringRef)[(NSURL*)url path] ) ) {
                            NSFileHandle *fh = [NSFileHandle fileHandleForUpdatingURL:(NSURL *)url error:&fhError];   // passed back for FBer
                            if ( fhError ) {
                                err = [fhError code];
                            } else {
                                localFD = [fh fileDescriptor];
                                lockError = flock( localFD, LOCK_SH | LOCK_NB );   //    create shared lock
                            }
                        } else {
                            err = fnOpnErr;
                        }
                        break;     //    create shared lock
                    }
                    case 'I':{
                        NSFileHandle *fh = [NSFileHandle fileHandleForReadingFromURL:(NSURL *)url error:&fhError];   // passed back for FBer
                        if ( fhError ) {
                            err = [fhError code];
                        } else {
                            localFD = [fh fileDescriptor];
                            lockError = flock( localFD, LOCK_SH | LOCK_NB );   //    create shared lock
                        }
                    }
                        break;     //    create shared lock
                    case 'O':
                    case 'A':
                    case 'R': {
                        NSFileHandle *fh = [NSFileHandle fileHandleForUpdatingURL: (NSURL *)url error:&fhError];   // passed back for FBer
                        if ( fhError ) {
                            err = [fhError code];
                        } else {
                            localFD = [fh fileDescriptor];
                            lockError = flock( localFD, LOCK_EX | LOCK_NB );   //    create exclusive lock
                        }
                        break;
                        
                    }
                }
                break;
            case F_WRLCK:  // one process has an exclusive lock
                err = wrPermErr;
                break;
            default:
                err = wrPermErr;
                break;
        }
    } else {
        err = fnOpnErr;
    }
    if ( localFD >= 0 )          *fd = localFD;
    if ( lockError && err == 0 ) err = fnOpnErr;
    
    return err;
}

//UInt64  WriteData( CFURLRef url, CFDataRef inData, UInt64 inOffset )
UInt64  FH_WriteData( int fd, CFDataRef inData, UInt64 inOffset ) {
    //NSFileHandle *fh = [NSFileHandle fileHandleForWritingToURL:(NSURL *)url error:nil];
    NSFileHandle *fh = FH_InitFileHandleFromFileDescriptor( fd );
    [fh seekToFileOffset:inOffset];
    [fh writeData:(NSData *)inData];
    UInt64 outOffset = [fh offsetInFile];
    //[fh truncateFileAtOffset: outOffset];
    [fh release];
    return outOffset;
}

//UInt64  WriteFile( CFURLRef url, void *p, UInt64 length, UInt64 offset )
UInt64  FH_WriteFile( int fd, void *p, UInt64 length, UInt64 offset ) {
    //UInt64  WriteFile( CFURLRef url, CFStringRef mutInString, UInt64 length, UInt64 offset ) {
    //UInt64            outOffset;
    //UInt64 outOffset = WriteData( url,(CFDataRef)[NSData dataWithBytes:p length:length], offset );
    //outOffset = WriteData( url,(CFDataRef)[(NSString *)mutInString dataUsingEncoding:NSUTF8StringEncoding], offset );
    //NSData* data     = [[(id)mutInString dataUsingEncoding:NSUTF8StringEncoding]retain];
    //UInt64 outOffset = WriteData( url,(CFDataRef)data, offset );
    //[data release];
    return FH_WriteData( fd,(CFDataRef)[NSData dataWithBytes:p length:length], offset );
}


UInt64  FH_WriteChar( int fd, char theChar, UInt64 offset ) {
    //UInt64            outOffset;
    //outOffset = WriteData( url,(CFDataRef)[NSData dataWithBytes:&theChar length:sizeof(theChar)], offset );
    return FH_WriteData( fd, (CFDataRef)[NSData dataWithBytes:&theChar length:sizeof(theChar)], offset );;
}

CFStringRef FH_URLGetPath( CFURLRef url ) {
    return (CFStringRef)[(NSURL *)url path];
}

BOOL  FH_BuildEmptyFile( CFURLRef url ) {
    NSFileManager *fm = [NSFileManager defaultManager];
    return [fm createFileAtPath: (NSString *)FH_URLGetPath( url ) contents:nil attributes:nil];
}



long  FH_FSpTrashObject( CFStringRef sourceObj ) {
    OSErr             err;
    err = noErr;
    if ( sourceObj ) {
        //NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
        NSString *src = [(NSString *)sourceObj stringByDeletingLastPathComponent];
        NSString *file = [(NSString *)sourceObj lastPathComponent];
        [[NSWorkspace sharedWorkspace] performFileOperation:NSWorkspaceRecycleOperation source:src destination:@"" files:[NSArray arrayWithObject:file] tag:nil];
        NSSound *systemSound = [[NSSound alloc] initWithContentsOfFile:@"/System/Library/Components/CoreAudio.component/Contents/SharedSupport/SystemSounds/dock/drag to trash.aif" byReference:YES];
        if(systemSound) {
            [systemSound play];
            [systemSound release];
        }
        //[pool drain];
    } else {
        err = 999;
    }
    return err;
}
/*
CFURLRef OSPanelOpen( UInt32 options, CFStringRef message, CFTypeRef allowedFileTypes, CFStringRef prompt, CFURLRef dirURL ) {
    return NULL;
}
*/
