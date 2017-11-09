// prefix header, with public (user-code-reachable) struct and typedef declarations
#include "FBtoC.h"
#include "Runtime.h"
#include "FileHandlingUtils.h"

//#if !DECARBONATE  should all be 64-bit. Brian 20170930


//  FileHandling.c



#define FBERRORS 13

OSStatus			gFBFileError;
//OSType				gFBSaveType    = 'TEXT';						// used by FBSetOpenType and other
//OSType				gFBSaveCreator = 'ttxt';						// used by FBSetOpenType and other

OSType				gFBSaveType    = 0;						// used by FBSetOpenType and other
OSType				gFBSaveCreator = 0;						// used by FBSetOpenType and other

//FSRef				gFBDefaultFolderRef;
//FSSpec			gFBDefaultFolderSpec;

Str255				gFBLineBuf;
UInt32				gFBLineDev;
UInt32				gFBLinePos;
SInt64              gFBLastFilePos;
char                gFBFileTabMode;
UInt16              gFBTabWidth = 16;

#if !defined NO_SUFFIX_ON_ARRAY_NAMES || NO_SUFFIX_ON_ARRAY_NAMES == 0
#error wrong version of FileHandling.c
#endif


FBInfoForOpenFile	gFBOpenFiles[kMaxFBFiles + kFBMaxSerialFiles]; // suffix matches FB's dim as FBInfoForOpenFile	  gFBOpenFiles(_kFBMaxDiskFiles + _kFBMaxSerialFiles)

//static OSStatus		FBReadFile_Priv( const int fileID, void * dest, ByteCount numBytesRequested, ByteCount * numBytesRead );
static OSStatus		FBReadFile_Priv( const int fileID, void *dest, ByteCount numBytesRequested, ByteCount *numBytesRead );
static int			FBXSerialRead_Priv( const int serialPortID );
static void			FBCloseOneFile_priv( int fileID );

/*
 Set gFBDefaultFolderRef to the FSRef for folder enclosing the application
 This function is no longer called from FBSystemFunction() within General.c Brian 20170930
 
 void FBInitDefaultFolderRef( void  )
 {
 FSRef	appRef;
 ProcessSerialNumber	psn = { 0, kCurrentProcess };
 GetProcessBundleLocation( &psn, &appRef );
 FSGetCatalogInfo( &appRef, kFSCatInfoNone, NULL, NULL, &gFBDefaultFolderSpec, &gFBDefaultFolderRef );
 }
 */

////////////////////////////////////
//      !!!: Error handling        //
////////////////////////////////////



OSStatus FBerror( void ){
    //error function
    return gFBFileError;
}


void FBFileErrorAlert( int fileID, OSStatus err ) {
    Str255	msg;
    
    if ( err ) {
        //PSstrcpy( TO_STACK,  PSltrr( "The program has returned a file error.", "FBFileErrorAlert" ) ), PSstrcat( TO_STACK, PSstring( 1, 13 ) ), PSstrcat( TO_STACK, PSstr( err ) ), PSstrcat( TO_STACK, PSstring( 1, 13 ) );
        //PSstrcpy( msg, TO_STACK );
        //PSstrcpy( TO_STACK, msg ), PSstrcat( TO_STACK, PSltrr( "File ref:", "FBFileErrorAlert" ) ), PSstrcat( TO_STACK, PSstr( fileID ) ), PSstrcat( TO_STACK, PSstring( 1, 13 ) );
        //PSstrcpy( msg, TO_STACK );
        //err = -54;  // testing only
        
        NSError *error   = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSString *string = [NSString stringWithFormat:@"The program returned a file error.\nFile ref:%d\n%@", fileID, [error description]];
        CFStringGetPascalString( (CFStringRef)string, (void*)&msg, sizeof( msg ), kCFStringEncodingMacRoman );
        FBStopMsg( PSstrcpy( TO_STACK, msg ), 0 );
    }
}




// value may be: FBFileErrorAlert (the default), a user-function (from 'on error fn MyErrHandler'), or NULL (from 'on error end').

FBOnXxxxxVector		gFBOnErrorVector = &FBFileErrorAlert; // [fn name marks line to be removed by C runtime stripping] FBFileErrorAlert()



void FBCheckFileError( int fileID, OSStatus err ) {
    /*
     all opens, reads, writes   call though here at exit
     
     if ( err == noErr ) do nothing
     if ( gFBOnErrorVector is 0 ) do nothing  else call the vector - the vector points to either the default FBFileErrorAlert() or the user vector from their ON ERROR fn xxxxx ( if provided ).
     */
    if ( err ){
        gFBFileError = err;
        if ( gFBOnErrorVector ) (*gFBOnErrorVector)( fileID, err );
    }
}


void FBOpenUNIX( unsigned fileID, Str255 command ) {
    /*
     Open "UNIX", fileID, command
     */
    gFBStk--;       // restore missing stack pointer decrement. Brian 20160516
    OSStatus		err = noErr;
    
    if ( fileID > 0 && fileID < kMaxFBFiles ){
        if ( gFBOpenFiles[fileID].isOpen == false ){
            PScstr( command );
            gFBOpenFiles[fileID].pipeUNIX = popen( (char*)command, "r" );
            gFBOpenFiles[fileID].isOpen = true;
        } else {
            CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable(@"Error in Open 'UNIX' for file#",@"Runtime",@"FBOpenUNIX");
            CFStringRef e1 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"), s1, fileID );
            CFStringRef e2 = (CFStringRef)NSLocalizedStringFromTable(@"FileID already in use",@"Runtime",@"FBOpenUNIX");
            FBFullStop( e1, e2, false );
            CFRelease(e1);
        }
    } else {
        CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable(@"FileID not in range ",@"Runtime",@"FBOpenUNIX");
        CFStringRef s2 = (CFStringRef)NSLocalizedStringFromTable(@" for FileID: ",@"Runtime",@"FBOpenUNIX");
        CFStringRef e1 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d%@%d"), s1, (int)err, s2, fileID );
        
        CFStringRef s3 = (CFStringRef)NSLocalizedStringFromTable( @"FileID must be in range 1 to ", @"Runtime", @"FBOpenUNIX" );
        CFStringRef e2 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s3,kMaxFBFiles - 1 );
        
        FBFullStop( e1, e2, false );
        CFRelease(e1);
        CFRelease(e2);
    }
}


void FBOpenOutput( const int fileID, void *fileNamePtr, void *fs ) {
    /*
     Works for data fork only
     Bad fileID gives no errors, just does nothing harmlessly.
     Other errors give no message, but plough on regardless
     
     Only CFURLRef syntax allowed: open "O", fileID, @url
     */
    CFURLRef		url = nil;
    OSStatus		err = noErr;
    int             myFD;
    
    if ( fileID > 0 && fileID < kMaxFBFiles ) {
        switch ( (long)fileNamePtr ) {
            case 2:																// CFURLRef
                BlockMoveData( fs, &url, sizeof(CFURLRef) );
                //if ( CFURLGetFSRef( url, &fileRef ) == false ) // file does not exist; create it
                if ( FH_FileExistsAtPath( (CFStringRef)[(NSURL *)url path] ) == NO ) {
                    if ( FH_BuildEmptyFile( url ) == NO ) {   //  YES create worked, NO it failed
                        err = fnOpnErr; //-38;
                    }
                }
                break;
                
            default: // only CFURLRef allowed. FSRef, FSSpec fails because this file is for 64-bit runtime. Brian 20170113
                err = fnOpnErr; //-38;
                break;
        }
        //        require_noerr( err, Bail );
        __Require_noErr( err, Bail );// 20170926 bw
        
        //int openType = O_WRONLY ; // open with Write privilege
        
        //int myFD = TryUnixOpen( fileID, url, openType, &err );
        err = FH_IsFileAvailable( url, 'O', &myFD );
        
        //        require_noerr( err, Bail );
        __Require_noErr( err, Bail );// 20170926 bw
        
        FH_SetTypeCreator( url, gFBSaveType, gFBSaveCreator );
        
        // Stash for future use
        gFBOpenFiles[fileID].isOpen		    = true;
        gFBOpenFiles[fileID].url            = (CFURLRef)[(NSURL *)url copy];   // make a copy so it is retained.
        NSFileHandle *fh                    = FH_InitFileHandleFromFileDescriptor( myFD );
        gFBOpenFiles[fileID].savedOffset    = FH_TruncateFileAtOffset( fh, 0 );
        [fh release];
        gFBOpenFiles[fileID].recordSize	    = 256;     // default until fn OpenOutputSubStatement enhanced
        gFBOpenFiles[fileID].linePos   	    = 0;       // 20080131
        gFBOpenFiles[fileID].fileDescriptor = myFD;
        
    } else {
        CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable( @"Error in FBOpenOutput for file# ", @"Runtime", @"FBOpenOutput" );
        CFStringRef e1 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s1,fileID );
        CFStringRef s2 = (CFStringRef)NSLocalizedStringFromTable( @"FileID must be in range 1 to ", @"Runtime", @"FBOpenOutput" );
        CFStringRef e2 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s2,kMaxFBFiles - 1 );
        FBFullStop( e1, e2, false );
        CFRelease(e1);
        CFRelease(e2);
    }
Bail:
    FBCheckFileError( fileID, err );
}




void FBOpenRandom( const int fileID, void *fileNamePtr, void *fs, SInt64 recLen, Boolean isOpenR ) {
    /*
     1) Open "R",2,@fs,reclen  or    Open "R",#2,@fs,reclen  or     Open "R",varName,@fs,reclen formats only
     Open "N" formats added rp:20091006
     
     Brian 20091201 -  bug #407 If the user data file does not exist and the user specified the FSSpec or CFURLRef version, the file is created.
     */
    
    OSStatus		err = fnOpnErr;
    CFURLRef		url;
    int             fd; // lockError = 0;
    
    if ( fileID > 0 && fileID < kMaxFBFiles ) {
        BlockMoveData( fs, &url, sizeof(CFURLRef) );
        if ( FH_FileExistsAtPath( (CFStringRef)[(NSURL *)url path] ) == NO ) FH_BuildEmptyFile( url );
        
        if ( isOpenR )
            err = FH_IsFileAvailable( url, 'R', &fd );
        else
            err = FH_IsFileAvailable( url, 'N', &fd );
        
        if( err == noErr || err == opWrErr ) {
            gFBOpenFiles[fileID].isOpen         = true;
            gFBOpenFiles[fileID].url            = (CFURLRef)[(NSURL *)url copy];   // make a copy so it is retained.
            gFBOpenFiles[fileID].savedOffset	= 0;
            gFBOpenFiles[fileID].recordSize     = recLen;
            gFBOpenFiles[fileID].linePos        = 0;  //20080131
            gFBOpenFiles[fileID].fileDescriptor = fd;
        }
    }
Bail:
    FBCheckFileError( fileID, err );
}


void FBOpenAppend( const int fileID, void *fileNamePtr, void *fs, SInt64 recLen ) {
    /*
     1) Open "A",2,@url
     2) Assumes data fork (i.e.  "I") and does not accept "ID" or "IR" for data/resource fork
     
     Only supports this syntax: open "A", fileID, @url
     */
    
    
    switch ( (long)fileNamePtr ) {
        case 2:	 {															// CFURLRef
            CFURLRef		url;
            BlockMoveData( fs, &url, sizeof(CFURLRef) );
            if ( FH_FileExistsAtPath( (CFStringRef)[(NSURL *)url path] ) == NO ) { // if file does not exist, create it.
                FBOpenOutput( fileID, fileNamePtr, &url );
                FBClose( fileID );
            }
            break;
        }
        default: {
            CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable( @"Error in FBOpenAppend for file# ", @"Runtime", @"FBOpenAppend" );
            CFStringRef e1 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s1,fileID );
            CFStringRef s2 = (CFStringRef)NSLocalizedStringFromTable( @"Only CFURLRefs can be used in FB OPEN statement", @"Runtime", @"FBOpenAppend" );
            CFStringRef e2 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s2,kMaxFBFiles - 1 );
            FBFullStop( e1, e2, false );
            CFRelease(e1);
            CFRelease(e2);
        }
            break;
    }
    
    FBOpenRandom( fileID, fileNamePtr, fs, recLen, true );
    //UInt64 posInFile = CurrentOffsetInFile( gFBOpenFiles[fileID].fileDescriptor );
    //NSFileHandle *fh = InitFileHandleFromFileDescriptor( gFBOpenFiles[fileID].fileDescriptor );
    OSStatus err = FH_SetFilePointerToEndOfFile( gFBOpenFiles[fileID].fileDescriptor );
    FBCheckFileError( fileID, err );
    gFBOpenFiles[fileID].savedOffset = FH_CurrentOffsetInFile( gFBOpenFiles[fileID].fileDescriptor );
}


void FBOpenInput( const int fileID, void *fileNamePtr, void *fs, SInt64 recLen, Boolean isResourceOpen  ) {
    
    //CFURLRef: open "I", fileID, @url
    CFURLRef		url;
    OSStatus        err = noErr;
    int             myFD;
    
    if ( fileID > 0 && fileID < kMaxFBFiles ) {
        switch ( (long)fileNamePtr ) {
            case 2:																// CFURLRef
                BlockMoveData( fs, &url, sizeof(CFURLRef) );
                break;
        }
        //int openType = O_RDONLY ; // open with Read privilege
        //int myFD = TryUnixOpen( fileID, url, openType, &err );
        
        err = FH_IsFileAvailable( url, 'I', &myFD );
        
        //        require_noerr( err, Bail );
        __Require_noErr( err, Bail );// 20170926 bw
        
        gFBOpenFiles[fileID].isOpen         = true;
        gFBOpenFiles[fileID].url            = (CFURLRef)[(NSURL *)url copy];   // make a copy so it is retained.
        gFBOpenFiles[fileID].savedOffset	= 0;
        gFBOpenFiles[fileID].recordSize     = recLen;
        gFBOpenFiles[fileID].linePos        = 0;
        gFBOpenFiles[fileID].fileDescriptor = myFD;
    } else {
        CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable( @"Error in FBOpenInput for file# ", @"Runtime", @"FBOpenInput" );
        CFStringRef e1 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s1,fileID );
        CFStringRef s2 = (CFStringRef)NSLocalizedStringFromTable( @"FileID must be in range 1 to ", @"Runtime", @"FBOpenInput" );
        CFStringRef e2 = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d"),s2,kMaxFBFiles - 1 );
        FBFullStop( e1, e2, false );
        CFRelease(e1);
        CFRelease(e2);
    }
Bail:
    FBCheckFileError( fileID, err );
}




static void FBCloseOneFile_priv( int fileID ) {
    if ( gFBOpenFiles[fileID].pipeUNIX ) {           // UNIX
        pclose( gFBOpenFiles[fileID].pipeUNIX );
    } else if ( gFBOpenFiles[fileID].savedOffset ) {    // file
        gFBLinePos = 0;
        //FSCloseFork( gFBOpenFiles[fileID].savedOffset );
        close( gFBOpenFiles[fileID].fileDescriptor );
        CFRelease( gFBOpenFiles[fileID].url );          // url retained when file was opened
    } else if ( gFBOpenFiles[fileID].fileDescriptor ) { // serial
        gFBLinePos = 0;
        close( gFBOpenFiles[fileID].fileDescriptor );
        free( gFBOpenFiles[fileID].buff );
    }
    BlockZero( &gFBOpenFiles[fileID], sizeof( FBInfoForOpenFile ) );
}


void FBClose( int fileID ) {
    /*
     close fileID
     */
    if ( fileID == 0 ) {          // close all
        for ( fileID = 1; fileID < (kMaxFBFiles + kFBMaxSerialFiles); fileID++ ) {
            FBCloseOneFile_priv ( fileID );
        }
    }
    else if ( fileID >= -kFBMaxSerialFiles && fileID < kMaxFBFiles ) {
        if ( fileID < 0 ) fileID = kMaxFBFiles - fileID;
        FBCloseOneFile_priv( fileID );
    }
}



////////////////////////////////////
//     !!!: Enquiries / commands  //
////////////////////////////////////


long FBeof( int fileID ) {
    FBBoolean		result = ZTRUE;
    
    if ( fileID > 0 && fileID < kMaxFBFiles && gFBOpenFiles[fileID].isOpen ){
        if ( gFBOpenFiles[fileID].pipeUNIX ) {   // UNIX
            result = gFBOpenFiles[fileID].eof;
        } else	{							// file
            
            SInt64   size     = FH_FileLength( gFBOpenFiles[fileID].fileDescriptor );
            SInt64   position = FH_CurrentOffsetInFile( gFBOpenFiles[fileID].fileDescriptor );
            //if ( noErr != FSGetForkPosition( gFBOpenFiles[fileID].savedOffset, &forkPos) ) { forkPos = 0; }
            //if ( noErr != FSGetForkSize( gFBOpenFiles[fileID].savedOffset, &size ) ) { size = 0; }
            //printf( "FBeof  fileID %d  size  %d, forkPos %d\n", fileID, (int)size, (int)forkPos );
            if ( position < size ) result = false;
        }
    }
    return result;
}


#if __LP64__
SInt64 FBlof( int fileID, SInt64 recLen ) {
    SInt64			size = 0;
#else
UInt32 FBlof( int fileID, UInt32 recLen ) {
UInt32			size = 0;
#endif
    int				serialPortID;
    
    if ( fileID > 0 && fileID < kMaxFBFiles && (gFBOpenFiles[fileID].fileDescriptor) ) {
        //if ( noErr != FSGetForkSize( gFBOpenFiles[fileID].fileDescriptor, &size ) ) { size = 0; }
        size = FH_FileLength( gFBOpenFiles[fileID].fileDescriptor );
    } else if ( fileID < 0 && fileID >= -kFBMaxSerialFiles ) { // serial
        serialPortID = kMaxFBFiles - fileID;
        FBXSerialRead_Priv( serialPortID ); // grab any chars pending
        size = gFBOpenFiles[serialPortID].bytesInBuff;
    }
    return size/recLen;
}


void FBAppend( const int fileID ) {
    // FB 'append' statement
    OSStatus err = FH_SetFilePointerToEndOfFile( gFBOpenFiles[fileID].fileDescriptor );
    if ( !err )  gFBOpenFiles[fileID].savedOffset = FH_CurrentOffsetInFile( gFBOpenFiles[fileID].fileDescriptor );
    //err = FSSetForkPosition( gFBOpenFiles[fileID].savedOffset, fsFromLEOF, 0 );
    FBCheckFileError( fileID, err );
}


void FBRecord( const int fileID, SInt64 recNum, SInt64 posInRec ) {
    /*
     Record [#]fileID, recordNum [,positionInRecord]
     */
    //SInt64			newPos;
    OSStatus		err  = noErr;
    
    if ( fileID > 0 && fileID < kMaxFBFiles && (gFBOpenFiles[fileID].fileDescriptor) ) {
        gFBOpenFiles[fileID].savedOffset = recNum * gFBOpenFiles[fileID].recordSize + posInRec;   // calculate new file position and store in global record
                                                                                                  //err = FSSetForkPosition( gFBOpenFiles[fileID].savedOffset, fsFromStart, newPos );
        NSFileHandle *fh = FH_InitFileHandleFromFileDescriptor( gFBOpenFiles[fileID].fileDescriptor );
        FH_SeekToFileOffset( fh, gFBOpenFiles[fileID].savedOffset  );
    }
    if ( err == eofErr ) { // extend the file
        FSForkIOParam    pb;
        BlockZero( &pb, sizeof( pb ) );
        pb.forkRefNum     = gFBOpenFiles[fileID].savedOffset;
        pb.positionMode   = fsFromStart;
        //pb.positionOffset = newPos;
        err = PBSetForkSizeSync( &pb );
        if ( err == noErr ) {
            //err = FSSetForkPosition( gFBOpenFiles[fileID].savedOffset, fsFromStart, newPos );    //Paolo 4-02-09
        }
    }
    FBCheckFileError( fileID, err );
}


// returns current offset in file divided by the record size used in the FB Open statement
SInt64 FBrec( int fileID ) {
    // FB 'rec' statement
    SInt64			position = 0;
    
    if ( fileID > 0 && fileID < kMaxFBFiles && gFBOpenFiles[fileID].isOpen ) {
        //if ( noErr != FSGetForkPosition( gFBOpenFiles[fileID].savedOffset, &forkPos) ) { forkPos = 0; }
        position = FH_CurrentOffsetInFile( gFBOpenFiles[fileID].fileDescriptor );
    }
    return position/gFBOpenFiles[fileID].recordSize;
}


SInt64 FBloc( int fileID ) {
    // FB 'loc' function
    SInt64			position = 0;
    
    if ( fileID > 0 && fileID < kMaxFBFiles && gFBOpenFiles[fileID].isOpen ) {
        //if ( noErr != FSGetForkPosition( gFBOpenFiles[fileID].savedOffset, &forkPos) ) { forkPos = 0; }
        position = FH_CurrentOffsetInFile( gFBOpenFiles[fileID].fileDescriptor );
    }
    return position % gFBOpenFiles[fileID].recordSize;
}



////////////////////////////////////
//            !!!: Input          //
////////////////////////////////////


void FBLineInputHash( int fileID, StringPtr s ) {
    /*
     line input #fileID, stringVar$
     */
    long			len;
    signed char		theChar;
    Boolean			eol;
    OSStatus        err = noErr;
    
    *(char*)s = 0; // null string if error
    
    //	if ( fileID == 0 )
    //	{
    //		//FBLineInput( "\p", s ); // antique Terminal.app-hosted stdin stdout thing
    //	}
    //	else
    if ( fileID < 0 )  { // serial
        do {
            int bytesRead = FBReadFile( fileID, &theChar, false, 1 );
            if ( bytesRead == 0 ) break;
            if ( theChar != 13  ) {
                s[0]++;
                s[s[0]] = theChar;
            }
        }
        while ( theChar != 13 && s[0] < 255 );
    } else {
        if ( fileID > 0 && fileID < kMaxFBFiles ) { // UNIX or file
            if (gFBOpenFiles[fileID].pipeUNIX) { // UNIX
                eol = false;
                len = 0;
                do {
                    theChar = fgetc( gFBOpenFiles[fileID].pipeUNIX );
                    switch ( theChar ) {
                        case 10:
                        case 13:
                            eol = true;
                            break;
                        case EOF:
                            eol = true;
                            gFBOpenFiles[fileID].eof = ZTRUE;
                            break;
                        default:
                            len++;
                            s[len] = theChar;
                    }
                }
                while ( eol == false && len < 255 );
                s[0] = len;
            } else {                                    // file
                if ( gFBOpenFiles[fileID].fileDescriptor ) {
                    
                    unsigned char	ch;
                    int				count = 0;
                    
                    do {
                        //err = FSReadFork( gFBOpenFiles[fileID].savedOffset, fsAtMark + pleaseCacheMask, 0, 1, &ch, NULL );  // 0,1 means start at position zero and read 1 byte
                        
                        err =  FH_ReadChar( gFBOpenFiles[fileID].fileDescriptor, &gFBOpenFiles[fileID].savedOffset, &ch ); //
                        if ( err ) break;
                        count++;
                        *(char*)(s + count) = ch;
                        //#define LINE_INPUT_TERMINATED_BY_LINEFEED 1
                        //#if LINE_INPUT_TERMINATED_BY_LINEFEED
                    } while ( count < 255 && ch != 13 && ch != 10 );
                    
                    if ( !err && (ch == 13 || ch == 10) ) { count--; }			// strip <cr> or <lf>
                                                                                //#else
                                                                                //} while ( count < 255 && ch != 13 );
                                                                                //if ( !err && ch == 13 ) { count--; }			// strip <cr>
                                                                                //#endif
                    *(char*)s = count;								// length byte
                    if ( count && (err == eofErr) ) err = noErr;	// if we read any char at all, don't signal eof error
                } else {
                    err = fnOpnErr; //  file not open
                }
            }
        }
    }
    FBCheckFileError( fileID, err );
}



static OSStatus FBReadFile_Priv( const int fileID, void *dest, ByteCount numBytesRequested, ByteCount * numBytesRead ) {
    //static OSStatus FBReadFile_Priv( const int fileID, CFMutableStringRef dest, ByteCount numBytesRequested, ByteCount * numBytesRead ) {
    OSStatus		err = noErr;
    int				serialPortID;
    
    *numBytesRead = 0; // default if error
    if ( fileID > 0 && fileID < kMaxFBFiles )  {   // disk file
        if ( gFBOpenFiles[fileID].isOpen ) {
            //err = FSReadFork( gFBOpenFiles[fileID].forkRefNum, fsAtMark + pleaseCacheMask, 0, numBytesRequested, dest, numBytesRead );
            //CFIndex  size = CFStringGetLength( dest );
            //void*       p = malloc( numBytesRequested );
            //CFStringGetBytes( dest, CFRangeMake( 0, size ), kCFStringEncodingMacRoman, 0, false, p, size, NULL );
            //ReadFile( gFBOpenFiles[fileID].url, dest, numBytesRequested, &(gFBOpenFiles[fileID].savedOffset) );
            FH_ReadFile( gFBOpenFiles[fileID].fileDescriptor, dest, numBytesRequested, &(gFBOpenFiles[fileID].savedOffset) );
            //CFStringRef string = CFStringCreateWithBytes( kCFAllocatorDefault, p, numBytesRequested, kCFStringEncodingUTF8, false );
            //CFStringAppend( dest, string );
            //CFRelease( string );
            //free( p );
            *numBytesRead = numBytesRequested;
        } else {
            err = fnOpnErr; }
    }
    
    else if ( fileID < 0 && fileID >= -kFBMaxSerialFiles ) {   // serial
        serialPortID = kMaxFBFiles - fileID;
        if ( gFBOpenFiles[serialPortID].isOpen ) {
            FBXSerialRead_Priv( serialPortID );
            if ( gFBOpenFiles[serialPortID].bytesInBuff >= numBytesRequested ) {
                BlockMoveData( gFBOpenFiles[serialPortID].buff, dest, numBytesRequested ); // copy from serial buffer to dest
                gFBOpenFiles[serialPortID].bytesInBuff -= numBytesRequested;
                BlockMoveData( gFBOpenFiles[serialPortID].buff + numBytesRequested, gFBOpenFiles[serialPortID].buff, gFBOpenFiles[serialPortID].bytesInBuff ); // 'left-justify' buffer contents
                *numBytesRead = numBytesRequested;
            } else {
                err = eofErr;  // asked for too many
            }
        } else {
            err = fnOpnErr;
        }
    }
    return err;
}



ByteCount FBReadFile( const int fileID, void * dest, Boolean isInputHashString, ByteCount numBytesRequested ) {
    //ByteCount FBReadFile( const int fileID, CFMutableStringRef dest, Boolean isInputHashString, ByteCount numBytesRequested ) {
    OSStatus	err;
    ByteCount	numBytesRead;
    
    if ( isInputHashString && (numBytesRequested == 0) ) numBytesRequested = 1; // read #serialPort, s$; 0   behaves like  inkey$( serialPort )
    err = FBReadFile_Priv( fileID, dest, numBytesRequested, &numBytesRead );
    if ( isInputHashString == false ) FBCheckFileError( fileID, err );
    return numBytesRead;
}


StringPtr FBInkeyFromFile( const int fileID )
{
    // inkey$() return 1 character on success, null string if failure; failure is silent
    ByteCount	numBytesRead;
    
    FBReadFile_Priv( fileID, &gReturnedString[1], 1, &numBytesRead );
    gReturnedString[0] = numBytesRead;
    return gReturnedString;
}

/*
 read# numeric var    big-endian on disk
 */
short FBReadSwapShort( const unsigned fileID )
{
    short myShort   ;
    FBReadFile( fileID, &myShort, false, 2 );
    return CFSwapInt16BigToHost( myShort );
}

long FBReadSwapLong( const unsigned fileID ) {
    
    long myLongVar;
    FBReadFile( fileID, &myLongVar, false, 4 );
    return CFSwapInt32BigToHost( myLongVar );
}

float FBReadSwapSingle( const unsigned fileID ) {
    CFSwappedFloat32 myFloatSingle;
    FBReadFile( fileID, &myFloatSingle, false, sizeof( myFloatSingle ) );
    return CFConvertFloatSwappedToHost( myFloatSingle );
}

double FBReadSwapDouble( const unsigned fileID ) {
    CFSwappedFloat64 myDouble;
    FBReadFile( fileID, &myDouble, false, sizeof( myDouble ) );
    return CFConvertDoubleSwappedToHost( myDouble );
}



static SInt64 FBStringToSInt64( ConstStr255Param s )
{
    // fixes bug #358
    return (SInt64)PSval( PSstrcpy( STACK_PUSH(), s ) );
}


void FBParseInput( const short  varType, void * vPtr )
{
    void *            textPtr;
    long              linePos;
    Str255            parsedStr;
    
    textPtr = &gFBLineBuf;
    linePos = gFBLinePos;
    
    PSstrcpy( parsedStr, "\p" );
    while ( (-(linePos <= *(unsigned char*)(textPtr) ) & ( ((-(*(unsigned char*)(textPtr + linePos) == (unsigned char)' ' ) | ( -(*(unsigned char*)(textPtr + linePos) == 9 ) ) )) ) ) )
    {
        linePos++;
    }
    if ( -(*(unsigned char*)(textPtr + linePos) == PSasc( PSstrcpy( STACK_PUSH(), "\p\"" ) ) ) )
    {
        linePos++;
        while ( (-(linePos <= *(unsigned char*)(textPtr) ) & ( -(*(unsigned char*)(textPtr + linePos) != PSasc( PSstrcpy( STACK_PUSH(), "\p\"" ) ) ) ) ) )
        {
            (PSstrcpy( TO_STACK, parsedStr ), PSstrcat( TO_STACK, PSstring( 1, *(unsigned char*)(textPtr + linePos) ) )); PSstrcpy( parsedStr, TO_STACK );
            linePos++;
        }
        if ( (-(linePos <= *(unsigned char*)(textPtr) ) & ( -(*(unsigned char*)(textPtr + linePos) == PSasc( PSstrcpy( STACK_PUSH(), "\p\"" ) ) ) ) ) )
        {
            linePos++;
        }
    }
    else
    {
        while ( (-(linePos <= *(unsigned char*)(textPtr) ) & ( (-(*(unsigned char*)(textPtr + linePos) != (unsigned char)',' ) & ( -(*(unsigned char*)(textPtr + linePos) != 9 ) ) ) ) ) )
        {
            (PSstrcpy( TO_STACK, parsedStr ), PSstrcat( TO_STACK, PSstring( 1, *(unsigned char*)(textPtr + linePos) ) )); PSstrcpy( parsedStr, TO_STACK );
            linePos++;
        }
    }
    while ( (-(linePos <= *(unsigned char*)(textPtr) ) & ( ((-(*(unsigned char*)(textPtr + linePos) == (unsigned char)' ' ) | ( (-(*(unsigned char*)(textPtr + linePos) == 9 ) | ( -(*(unsigned char*)(textPtr + linePos) == 13 ) ) ) ) )) ) ) )
    {
        linePos++;
    }
    if ( (-(linePos <= *(unsigned char*)(textPtr) ) & ( -(*(unsigned char*)(textPtr + linePos) == (unsigned char)',' ) ) ) )
    {
        linePos++;
    }
    
    // magic numbers here are varTypes coded in InitCTypetable
    
    if ( varType == 0 || varType == 1 || varType == 2 ) // _longVarType, _fixedVarType, _unsignedLongVarType
    {
        *(SInt32*)vPtr = (SInt32)FBStringToSInt64( parsedStr );
    }
    else if ( varType == 3 || varType == 4 ) // _shortVarType, _unsignedShortVarType
    {
        *(SInt16*)vPtr = (SInt16)FBStringToSInt64( parsedStr );
    }
    else if ( varType == 5 || varType == 6 || varType == 7 ) // _byteVarType, _unsignedByteVarType, _booleanVarType
    {
        *(SInt8*)vPtr = (SInt8)FBStringToSInt64( parsedStr );
    }
    else if ( varType == 8 || varType == 9 ) // _UInt64VarType, _SInt64VarType
    {
        *(SInt64*)vPtr = FBStringToSInt64( parsedStr );
    }
    else if ( varType == 12 ) //_floatVarType, _singleVarType
    {
        *(float*)vPtr = PSval( PSstrcpy( STACK_PUSH(), parsedStr ) );
    }
    else if ( varType == 13 ) // _doubleVarType
    {
        *(double*)vPtr = PSval( PSstrcpy( STACK_PUSH(), parsedStr ) );
    }
    else if ( varType == 14 ) // _ldoubleVarType
    {
        *(long double*)vPtr = PSval( PSstrcpy( STACK_PUSH(), parsedStr ) );
        
    }
    // 15 is _doubleComplexType
    // 16 is _constStr255VarType
    else if ( varType >= 17 && varType <= 144 ) // _str1VarType to _str255VarType
    {
        PSstrcpy( (StringPtr)vPtr, parsedStr );
    }
    else if ( varType == 145 ) // _containerVarType
    {
        CtnrStoreString( (void **)vPtr, parsedStr );
    }
    gFBLinePos = linePos;
}


void FBToCInputFile( const int fileID, void *target, short varType ) {
    UInt32 lastDevice;
    lastDevice = gFBLineDev;
    gFBLineDev = fileID;
    
    if ( fileID ) {
        if ( lastDevice != gFBLineDev || gFBLinePos == 0 || gFBLinePos > gFBLineBuf[0] ) {
            gFBLinePos = 1;
            FBLineInputHash( fileID, (StringPtr)&gFBLineBuf );
        }
    }
    FBParseInput( varType, target );
}

static int FBXSerialRead_Priv( const int serialPortID )
{
    int bytesRead;
    
    bytesRead = read( gFBOpenFiles[serialPortID].fileDescriptor, gFBOpenFiles[serialPortID].buff + gFBOpenFiles[serialPortID].bytesInBuff, gFBOpenFiles[serialPortID].buffSize - gFBOpenFiles[serialPortID].bytesInBuff );
    gFBOpenFiles[serialPortID].bytesInBuff += bytesRead;
    return bytesRead;
}



void FBReadString( const int fileID, void *dest, ByteCount numBytesRequested )
{
    // read #fileID, s; numBytes
    OSStatus	err;
    ByteCount	numBytesRead;
    
    *(char*)dest = 0;
    if ( (numBytesRequested == 0) && (FBlof( fileID, 1 ) > 0) ) numBytesRequested = 1; // read #fileID, s; 0  used to poll serial port
    
    if ( numBytesRequested ) {
        //NSMutableString *mutStr = [[NSMutableString alloc]initWithString:@""];
        //err = FBReadFile_Priv( fileID, (CFMutableStringRef)mutStr, numBytesRequested, &numBytesRead );
        err = FBReadFile_Priv( fileID, dest + 1, numBytesRequested, &numBytesRead );
        *(char*)dest= numBytesRead; // set caller's pascal length byte
        FBCheckFileError( fileID, err );
    }
}



////////////////////////////////////
//              !!!: Output       //
////////////////////////////////////

//void FBWriteFile( const int fileID, CFStringRef s, ByteCount numBytes ) __attribute__ ((noinline));
//void FBWriteFile( const int fileID, CFStringRef s, ByteCount numBytes ) {

void FBWriteFile( const int fileID, void * src, ByteCount numBytes ) __attribute__ ((noinline));
void FBWriteFile( const int fileID, void * src, ByteCount numBytes ) {
    
    // write file[#] deviceID, address&, numBytes&
    OSStatus		err = noErr;
    
    if ( fileID > 0 && fileID < kMaxFBFiles ) { // disk file
        if ( gFBOpenFiles[fileID].isOpen ) {
            //err = FSWriteFork( gFBOpenFiles[fileID].savedOffset, fsAtMark + pleaseCacheMask, 0, numBytes, src, nil );
            // the input pointer ( src ) should be changed to a CFStringRef to avoid the below conversion
            // but for now is ok. - Brian 20170113
            //CFStringRef s = CFStringCreateWithBytes( NULL, src, numBytes, kCFStringEncodingMacRoman, false );
            //UInt64  WriteFile( int fd, void *p, UInt64 length, UInt64 offset )
            //gFBOpenFiles[fileID].savedOffset = WriteFile( gFBOpenFiles[fileID].url, src , numBytes, gFBOpenFiles[fileID].savedOffset );
            gFBOpenFiles[fileID].savedOffset = FH_WriteFile( gFBOpenFiles[fileID].fileDescriptor, src , numBytes, gFBOpenFiles[fileID].savedOffset );
        } else {
            err = fnOpnErr;
        }
    } else if ( fileID < 0 && fileID >= -kFBMaxSerialFiles ) { // serial
        int				serialPortID;
        ByteCount		bytesSent;
        
        serialPortID = kMaxFBFiles - fileID;
        if ( gFBOpenFiles[serialPortID].isOpen ){
            //CFIndex dataSize;
            //CFStringGetBytes( s, CFRangeMake( 0, CFStringGetLength( s ) ), kCFStringEncodingMacRoman, 0, false, NULL, 0, (void*)&dataSize ); // only gets string length
            //void *p          = malloc( dataSize );
            //CFStringGetBytes( s, CFRangeMake( 0, dataSize ), kCFStringEncodingMacRoman, 0, false, p, dataSize, NULL );
            bytesSent        = write( gFBOpenFiles[serialPortID].fileDescriptor, src, numBytes );  // 'man 2 write' for documentation
            if ( bytesSent != numBytes ) err = writErr;
            //free( p );
        } else {
            err = fnOpnErr;
        }
    }
    FBCheckFileError( fileID, err );
}


void FBWriteString( const int fileID, char * src, ByteCount varLength ) __attribute__ ((noinline));
void FBWriteString( const int fileID, char * src, ByteCount varLength ) {
    /*
     write# stringVar[;length]
     */
    int		writeBytes = varLength;
    int     lengthOfPstr = *(unsigned char*)src;  // picks off the leading length byte of the pascal string - Brian 20170113
    char	temp[256];
    
    
    if ( writeBytes > lengthOfPstr ) writeBytes = lengthOfPstr;
    FBWriteFile( fileID, src + 1, writeBytes );   // write the string sent
    
    // if necessary, pad the string to the length specified in the WRITE# statement. e.g. WRITE #1, stringVar;3   <<<<<== the '3' is passed to this function in varLength
    // e.g. if the string length provided in the WRITE# statement is longer than the string's actual length, this code pads to the string length provided.
    // So, if the string has 1 character in it ( s = "X" ) and WRITE# specifies more than one ( WRITE #1, string;3 ), it will be padded with 2 characters ( 2 + 1 = 3 )
    
    // Two rules cover the behavior:
    // (1) when the programmer-supplied varLength on the WRITE# statement is LESS than the string length, the string is truncated to the varLength.
    // (2) when the programmer-supplied varLength on the WRITE# statement is GREATER than the string length, the string is padded to the varLength with spaces.
    
    
    writeBytes = varLength - lengthOfPstr;
    if ( writeBytes > 0 ) {
        if ( writeBytes > 255 ) writeBytes = 255;
        //CFMutableStringRef mutStr = CFStringCreateMutable(kCFAllocatorDefault, 0);
        for ( int i = 0; i < writeBytes; i++ ) {
            temp[i] = ' ';
            //CFStringAppend(mutStr, CFSTR(" ") );
        }
        //s = CFStringCreateWithBytes( NULL, &temp[0], writeBytes, kCFStringEncodingMacRoman, false );
        FBWriteFile( fileID, &temp[0], writeBytes );
    }
}



/*
 write# numeric var    big-endian on disk
 */
void FBWriteSwapShort( short myShort, const unsigned fileID ) {
    myShort = CFSwapInt16HostToBig( myShort );
    FBWriteFile( fileID, &myShort, 2 );
}

void FBWriteSwapLong( long aLongVar, const unsigned fileID ) {
    aLongVar = CFSwapInt32HostToBig( aLongVar );
    FBWriteFile( fileID, &aLongVar, 4 );
    
}

void FBWriteSwapSingle( float f, const unsigned fileID ) {
    CFSwappedFloat32 mySwappedFloat = CFConvertFloatHostToSwapped( f );
    FBWriteFile( fileID, &mySwappedFloat, sizeof( mySwappedFloat ) );
}

void FBWriteSwapDouble( double d, const unsigned fileID ) {
    CFSwappedFloat64 mySwappedDouble = CFConvertDoubleHostToSwapped( d );
    FBWriteFile( fileID, &mySwappedDouble, sizeof( mySwappedDouble ) );
}


void FBFilePrintChar( char  theChar ){
    if ( theChar == 13){
        gFBOpenFiles[gFBRoute].linePos = 0;
    } else {
        gFBOpenFiles[gFBRoute].linePos++;
    }
    gFBLastFilePos = gFBOpenFiles[gFBRoute].linePos;
    FBWriteFile( gFBRoute, &theChar, 1 );
}


SInt64 FBFileGetPos()
{
    return gFBLastFilePos;
}


void FBFilePrintTab()
{
    int             spaceCount;
    int             i;
    
    if ( gFBFileTabMode )
    {
        FBFilePrintChar( gFBFileTabMode );
    }
    else
    {
        spaceCount = gFBTabWidth - (gFBOpenFiles[gFBRoute].linePos % gFBTabWidth);
        if ( spaceCount < 100 )
        {
            for ( i = 0; i < spaceCount; i++ ) { FBFilePrintChar( 32 ); }
        }
    }
}


void FBFilePrintCR()
{
    FBFilePrintChar( 13 );
}


void FBFilePrintString( Str255  s_p )
{
    Str255 s;  PSstrcpy( s, s_p );
    gFBStk -= 1;
    gFBOpenFiles[gFBRoute].linePos += s[0];
    gFBLastFilePos = gFBOpenFiles[gFBRoute].linePos;
    FBWriteFile( gFBRoute, (void*)&s + 1, s[0] );
}





////////////////////////////////////
//            !!!: Misc stuff     //
////////////////////////////////////


//#if !__LP64__
/*
 Not used
 
 StringPtr FBFilesLastType( void )
 {
 
 // lastFileType$ = files$
 
 return PSmki( gFBFilesInfo.lastType );
 }
*/
//#endif  /* !__LP64__ */


void FBSetOpenType( Str15  opentype$_p ) {
    Str255 opentype$;  PSstrcpy( opentype$, opentype$_p );
    gFBStk -= 1;
    UInt8             j;
    if ( opentype$[0] == 0 ) {  // 20170611 bw
        gFBSaveType = 0;
        gFBSaveCreator = 0;
    } else {     // bail remainder of function if opentype$[0] == 0    Brian 20170611
        if ( opentype$[0] >= 4 ) {
            gFBSaveType = 0;
            j = 1;
            do {
                gFBSaveType = gFBSaveType << 8;
                gFBSaveType += opentype$[j];
                j++;
            }
            while ( j <= 4 );
        }
        if ( opentype$[0] >= 8 ) {
            gFBSaveCreator = 0;
            j = 5;
            do {
                gFBSaveCreator = gFBSaveCreator << 8;
                gFBSaveCreator += opentype$[j];
                j++;
            }
            while ( j <= 8 );
        }
    }
}



void FBRename( CFURLRef oldName, CFURLRef newName  ) {
    [[NSFileManager defaultManager] moveItemAtURL:(NSURL * )oldName
                                            toURL:(NSURL * )newName
                                            error: nil];
}

void FBKill( CFURLRef fileName  ) {
    [[NSFileManager defaultManager] removeItemAtURL:(NSURL * )fileName
                                              error:nil];
}

//////////////////////////////////////////
//  !!!: FSRef and CFURLRef helpers     //
//////////////////////////////////////////


static OSStatus Str255ToHFSUniStr_Priv( ConstStr255Param name, TextEncoding encoding, HFSUniStr255 * oUniName )
{
    OSStatus					err;
    ByteCount					uniLength;
    static TextToUnicodeInfo	sTextToUnicodeInfo;
    
    if ( sTextToUnicodeInfo == 0 )
    {
        err = CreateTextToUnicodeInfoByEncoding( encoding, &sTextToUnicodeInfo );
        if ( err ) return err;
    }
    err = ConvertFromPStringToUnicode( sTextToUnicodeInfo, name, 510, &uniLength, oUniName->unicode );
    oUniName->length = uniLength / sizeof( UniChar );
    return err;
}


OSStatus GetFileFSRef( FSRef * parRef, ConstStr255Param name, FSRef * fileRef )
{
    HFSUniStr255	uniName;
    OSStatus		err;
    
    err = Str255ToHFSUniStr_Priv( name, kTextEncodingMacRoman,  &uniName );
    if ( err ) return err;
    err = FSMakeFSRefUnicode( parRef, uniName.length, &uniName.unicode[0], kTextEncodingUnknown, fileRef );
    return err;
}

void CopyNameFromCFURLRef( CFURLRef url, CFStringRef * string ) {
    
    *string = (CFStringRef)[[(NSURL *)url lastPathComponent]copy];
    
    //*string = NULL;
    //if ( CFURLGetFSRef( url, &ref ) ) CopyNameFromFSRef( &ref, string );
}


void GetNameAsPascalStringFromCFURLRef( CFURLRef url, Str255 *name ) {
    CFStringRef string;
    
    CopyNameFromCFURLRef( url, &string );
    CFStringGetPascalString( string, *name, 256, kCFStringEncodingMacRoman );
    CFRelease( string );
    //if ( CFURLGetFSRef( url, &ref ) ) GetNameAsPascalStringFromFSRef( &ref, name );
}

/*
 
 // this function moved into FB Editor source to avoid having FSGetCatalogInfo in FileHandling.c Brian 20170310
 
 void CopyNameFromFSRef( FSRef * ref, CFStringRef * string )
 {
 HFSUniStr255	uniStr255;
 
 *string = NULL;
 if ( FSGetCatalogInfo( ref, kFSCatInfoNone, NULL, &uniStr255, NULL, NULL ) == noErr )
 {
 *string = CFStringCreateWithCharacters( kCFAllocatorDefault, &uniStr255.unicode[0], uniStr255.length );
 }
 }
 */


/*
 void GetNameAsPascalStringFromFSRef( FSRef * ref, Str255 * name )
 {
 HFSUniStr255	uniStr255;
 CFStringRef		string = NULL;
 
 if ( FSGetCatalogInfo( ref, kFSCatInfoNone, NULL, &uniStr255, NULL, NULL ) == noErr )
 {
 string = CFStringCreateWithCharacters( kCFAllocatorDefault, &uniStr255.unicode[0], uniStr255.length );
 if ( string )
 {
 CFStringGetPascalString( string, *name, 256, kCFStringEncodingMacRoman );
 CFRelease( string );
 }
 }
 }
 */




//#endif //  !DECARBONATE should all be 64-bit. Brian 20170930


