// prefix header, with public (user-code-reachable) struct and typedef declarations
#include "FBtoC.h"
#include "Runtime.h"


//  FilesDollarFunction.c

#if !__LP64__

FBFilesDollarInfo	gFBFilesInfo;


static void FBFilesConvertFSSpecToNoNameVariant( FSSpec * fss )
// for folder FSSpec only; does nothing for file
{
    CInfoPBRec pb;
    
    BlockZero( &pb, sizeof( pb ) );
    pb.dirInfo.ioVRefNum = fss->vRefNum;
    pb.dirInfo.ioDrDirID = fss->parID;
    pb.dirInfo.ioNamePtr = fss->name;
    PBGetCatInfoSync( &pb );
    if ( pb.dirInfo.ioFlAttrib & kioFlAttribDirMask )
    {
        fss->parID = pb.dirInfo.ioDrDirID;
        fss->name[0] = 0;
    }
}


//------------------------------------------------------------------------------
// event proc
//------------------------------------------------------------------------------

typedef struct FBNavEventUserData_priv // temporary malloced struct holding both the FB userCallbackFn and userData
{
    void *		userCallbackFn;
    void *		userData;
} FBNavEventUserData_priv;


typedef long (*Def_Fn_FBFilesUserEventFn_Using)( NavReplyRecord* reply, void* userData );

static void*	sNavDialogFilterFn;


void NavDialog_SetFilterFn( void * filterFn )
{
    sNavDialogFilterFn = filterFn;
}


static void FBFilesEventProc( NavEventCallbackMessage  callBackSelector, NavCBRec* callBackParms, FBNavEventUserData_priv*  userCB )
{
    NavReplyRecord		reply;
    
    switch ( callBackSelector )
    {
        case kNavCBTerminate:
        {
            if ( callBackParms->context ) NavDialogDispose( callBackParms->context );
            free( (void*)userCB );
        }
            break;
            
        case kNavCBUserAction:
        {
            switch ( callBackParms->userAction )
            {
                case kNavUserActionOpen:
                case kNavUserActionSaveAs:
                case kNavUserActionChoose:
                {
                    NavDialogGetReply( callBackParms->context, &reply );
                    (*(Def_Fn_FBFilesUserEventFn_Using)userCB->userCallbackFn)( &reply, userCB->userData ); // call the FB userCallbackFn
                    NavDisposeReply( &reply );
                }
                    break;
            }
        }
            break;
    }
}


//------------------------------------------------------------------------------
// reply
//------------------------------------------------------------------------------

static StringPtr FBGetSaveFileName_priv( NavReplyRecord * reply )
{
    Str255	s = "\p";
    
    if ( reply->validRecord )
    {
        if ( reply->saveFileName ) CFStringGetPascalString( reply->saveFileName, s, 256, kCFStringEncodingMacRoman );
    }
    return PSstrcpy( gReturnedString, s );
}


long NavDialog_GetItemCount( NavReplyRecord * reply )
{
    long	count = 0;
    
    if ( reply->validRecord ) AECountItems( &reply->selection, &count );
    return count;
}

#if !__LP64__

void NavDialog_GetItemFSSpec( NavReplyRecord * reply, long index, FBBoolean noNameVariant, FSSpec * outSpec )
{
    long	count;
    
    if ( reply->validRecord && index > 0 )
    {
        AECountItems( &reply->selection, &count );
        if ( index <= count )
        {
            AEGetNthPtr( &reply->selection, index, typeFSS, NULL, NULL, outSpec, sizeof( FSSpec ), NULL );
            if ( noNameVariant ) FBFilesConvertFSSpecToNoNameVariant( outSpec );
            
            if ( reply->saveFileName ) // convert parent FSSpec to file FSSpec for put file
            {
                Str255		name = "\p:";
                PSstrcat( name, outSpec->name );
                PSstrcat( name, "\p:" );
                gFBStk++; // suggested by Paolo Carminati on 14-June-2016 - might need PSstrcpy( STACK_PUSH() instead. Not investigated. Brian 20160622
                PSstrcat( name, PSleft( FBGetSaveFileName_priv( reply ), 31 ) );
                (void)FSMakeFSSpec( outSpec->vRefNum, outSpec->parID, name, outSpec );
            }
            
        }
    }
}
#endif  /* !__LP64__ */

void NavDialog_GetItemFSRef( NavReplyRecord * reply, long index, FSRef * outRef )
{
    long	count;
    
    if ( outRef)
    {
        if (reply->validRecord && index > 0 )
        {
            AECountItems( &reply->selection, &count );
            if ( index <= count ) AEGetNthPtr( &reply->selection, index, typeFSRef, NULL, NULL, outRef, sizeof( FSRef ), NULL );
            // we don't attempt to convert parent FSRef to file FSRef for put file
            // because the file may not exist (in which case we can't make its FSRef)
            // the caller must get the name separately with NavDialog_CopySaveFileName() or NavDialog_GetSaveFileNameAsPascalString() -
        }
    }
}


void NavDialog_CopyItemName( NavReplyRecord * reply, long index, CFStringRef * name )
{
    HFSUniStr255	uniStr255;
    FSRef           ref;
    
    NavDialog_GetItemFSRef( reply, index, &ref );
    if ( FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, &uniStr255, NULL, NULL ) == noErr )
    {
        *name = CFStringCreateWithCharacters( kCFAllocatorDefault, &uniStr255.unicode[0], uniStr255.length );
    }
}


void NavDialog_GetItemNameAsPascalString( NavReplyRecord * reply, long index, Str255 * name )
{
    HFSUniStr255	uniStr255;
    FSRef           ref;
    CFStringRef		string;
    
    NavDialog_GetItemFSRef( reply, index, &ref );
    if ( FSGetCatalogInfo( &ref, kFSCatInfoNone, NULL, &uniStr255, NULL, NULL ) == noErr )
    {
        string = CFStringCreateWithCharacters( kCFAllocatorDefault, &uniStr255.unicode[0], uniStr255.length );
        if ( string )
        {
            
            
            CFStringGetPascalString( string, *name, 256, kCFStringEncodingMacRoman );
            CFRelease( string );
        }
    }
}


void NavDialog_CopyItemCFURLRef( NavReplyRecord * reply, long index, CFURLRef * outUrlRef )
{
    FSRef	ref;
    long	count;
    
    if ( outUrlRef)
    {
        *outUrlRef = NULL;
        if (reply->validRecord && index > 0 )
        {
            AECountItems( &reply->selection, &count );
            if ( index <= count )
            {
                AEGetNthPtr( &reply->selection, index, typeFSRef, NULL, NULL, &ref, sizeof( FSRef ), NULL );
                if ( reply->saveFileName )		// convert parent CFURLRef to file CFURLRef url for put file
                {
                    CFURLRef	tempUrl = CFURLCreateFromFSRef( kCFAllocatorDefault, &ref );
                    if ( tempUrl )
                    {
                        *outUrlRef = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, tempUrl, reply->saveFileName, false );
                        CFRelease( tempUrl );
                    }
                }
                else
                {
                    *outUrlRef = CFURLCreateFromFSRef( kCFAllocatorDefault, &ref );
                }
            }
        }
    }
}


void NavDialog_GetSaveFileNameAsPascalString( NavReplyRecord * reply, Str255 * s )
{
    if ( reply->validRecord )
    {
        if ( reply->saveFileName ) CFStringGetPascalString( reply->saveFileName, *s, 256, kCFStringEncodingMacRoman );
    }
}

CFStringRef NavDialog_CopySaveFileName( NavReplyRecord * reply )
{
    return CFStringCreateCopy( kCFAllocatorDefault, reply->saveFileName );
}



static void FBFilesSetLastType_priv( NavReplyRecord * reply )
{
    FSRef			ref;
    FSCatalogInfo	catalogInfo;
    FInfo			*fInfo;
    
    if ( reply->validRecord )
    {
        AEGetNthPtr( &reply->selection, 1, typeFSRef, NULL, NULL, &ref, sizeof( FSRef ), NULL );
        FSGetCatalogInfo( &ref, kFSCatInfoFinderInfo, &catalogInfo, NULL, NULL, NULL );
        fInfo = (FInfo *)catalogInfo.finderInfo;
        gFBFilesInfo.lastType = CFSwapInt32HostToBig( fInfo->fdType ); // endian swap
    }
}


//------------------------------------------------------------------------------
// type list
//------------------------------------------------------------------------------
static void FBFilesDisposeTypeList_priv( void )
{
    if ( gFBFilesInfo.typeListH ) DisposeHandle( (Handle)gFBFilesInfo.typeListH );
    gFBFilesInfo.typeListH = NULL;
}


static NavTypeListHandle FBFilesNewTypeListHandle_priv( short numTypes )
{
    if ( gFBFilesInfo.typeListH == NULL ) gFBFilesInfo.typeListH = (NavTypeListHandle)NewHandle( sizeof(NavTypeList) + ( numTypes * sizeof(OSType)) );
    return gFBFilesInfo.typeListH;
}


static void FBFilesSetTypeList( ConstStr255Param typeListStr )
{
    NavTypeListHandle	typeListH = NULL;
    short				numTypes = 0;
    
    numTypes = typeListStr[0]/4;
    if ( numTypes > 0 )
    {
        typeListH = FBFilesNewTypeListHandle_priv( numTypes );
        (**typeListH).componentSignature	= kNavGenericSignature;
        (**typeListH).osTypeCount			= numTypes;
#if TARGET_RT_BIG_ENDIAN
        BlockMoveData( &typeListStr[1], (**typeListH).osType, numTypes * sizeof(OSType) );
#else
        OSType typeList[64];
        int i;
        BlockMoveData( &typeListStr[1], &typeList, numTypes * sizeof(OSType));
        for ( i = 0; i < numTypes; i++ )
        {
            typeList[i] = CFSwapInt32BigToHost( typeList[i] ); // endian swap
            BlockMoveData( &typeList, (**typeListH).osType, numTypes * sizeof(OSType) );
        }
#endif
    }
}


static NavTypeListHandle FBFilesTypeListHandle( void )
{
    return gFBFilesInfo.typeListH;
}

// UTIs
static CFMutableArrayRef NavDialog_UTIArrayRef( Boolean create, Boolean release )
{
    static CFMutableArrayRef	sUTIArrayRef;
    
    if ( release )
    {
        if ( sUTIArrayRef ) CFRelease( sUTIArrayRef );
        sUTIArrayRef = NULL;
    }
    else
    {
        if ( create )
        {
            if ( sUTIArrayRef == NULL ) sUTIArrayRef = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks );
        }
    }
    return sUTIArrayRef;
}


void NavDialog_AddUTIPascalString( ConstStr255Param uti )
{
    CFStringRef	string = CFStringCreateWithPascalString( kCFAllocatorDefault, uti, kCFStringEncodingMacRoman );
    if ( string )
    {
        CFArrayAppendValue( NavDialog_UTIArrayRef( true, false ), string );
        CFRelease( string );
    }
}

void NavDialog_AddUTICFString( CFStringRef uti )
{
    CFArrayAppendValue( NavDialog_UTIArrayRef( true, false ), uti );
}



// option setters
static NavDialogCreationOptions sNavDialog_CreationOptions;

static void NavDialog_GetCreationOptions( NavDialogCreationOptions * options )
{
    if ( sNavDialog_CreationOptions.optionFlags == 0 )
    {
        NavGetDefaultDialogCreationOptions( &sNavDialog_CreationOptions );
        sNavDialog_CreationOptions.optionFlags ^= kNavAllowMultipleFiles;		// default to single file
    }
    BlockMoveData( &sNavDialog_CreationOptions, options, sizeof( NavDialogCreationOptions ) );
}

static void NavDialog_SetCreationOptions( NavDialogCreationOptions * options )
{
    BlockMoveData( options, &sNavDialog_CreationOptions, sizeof( NavDialogCreationOptions ) );
}


static void NavDialog_ClearCreationOptions( void )
{
    if (sNavDialog_CreationOptions.clientName ) CFRelease( sNavDialog_CreationOptions.clientName ); // #582...
    if (sNavDialog_CreationOptions.windowTitle ) CFRelease( sNavDialog_CreationOptions.windowTitle );
    if (sNavDialog_CreationOptions.actionButtonLabel ) CFRelease( sNavDialog_CreationOptions.actionButtonLabel );
    if (sNavDialog_CreationOptions.cancelButtonLabel ) CFRelease( sNavDialog_CreationOptions.cancelButtonLabel );
    if (sNavDialog_CreationOptions.saveFileName ) CFRelease( sNavDialog_CreationOptions.saveFileName );
    if (sNavDialog_CreationOptions.message ) CFRelease( sNavDialog_CreationOptions.message ); //... #582
    BlockZero( &sNavDialog_CreationOptions, sizeof( NavDialogCreationOptions ) );
}


void NavDialog_ChangeOptionFlags( NavDialogOptionFlags flagsToSet, NavDialogOptionFlags flagsToClear )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.optionFlags |= flagsToSet;
    options.optionFlags ^= flagsToClear;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetPreserveSaveFileExtension( void )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.optionFlags |= kNavPreserveSaveFileExtension;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetSaveFileName( CFStringRef string )
{
    NavDialogCreationOptions	options;
    
    CFRetain( string );
    NavDialog_GetCreationOptions( &options );
    options.saveFileName = string;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetLocation( Point pt )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.location = pt;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetClientName( CFStringRef string )
{
    NavDialogCreationOptions	options;
    
    CFRetain( string );
    NavDialog_GetCreationOptions( &options );
    options.clientName = string;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetWindowTitle( CFStringRef string )
{
    NavDialogCreationOptions	options;
    
    CFRetain( string );
    NavDialog_GetCreationOptions( &options );
    options.windowTitle = string;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetActionButtonLabel( CFStringRef string )
{
    NavDialogCreationOptions	options;
    
    CFRetain( string );
    NavDialog_GetCreationOptions( &options );
    options.actionButtonLabel = string;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetCancelButtonLabel( CFStringRef string )
{
    NavDialogCreationOptions	options;
    
    CFRetain( string );
    NavDialog_GetCreationOptions( &options );
    options.cancelButtonLabel = string;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetMessage( CFStringRef string )
{
    NavDialogCreationOptions	options;
    
    CFRetain( string );
    NavDialog_GetCreationOptions( &options );
    options.message = string;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetPreferenceKey( UInt32 key )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.preferenceKey = key;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetModality( WindowModality modality )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.modality = modality;
    NavDialog_SetCreationOptions( &options );
}


void NavDialog_SetParentWindow( WindowRef w )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.parentWindow = w;
    options.modality = kWindowModalityWindowModal;
    NavDialog_SetCreationOptions( &options );
}


static CFMutableArrayRef	sPopupExtension;

void NavDialog_AddPopupExtension( CFStringRef string )
{
    if ( sPopupExtension == NULL ) sPopupExtension = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks );
    CFArrayAppendValue( sPopupExtension, string );
}

void NavDialog_SetPopupExtension( CFArrayRef array )
{
    NavDialogCreationOptions	options;
    
    NavDialog_GetCreationOptions( &options );
    options.popupExtension = array;
    NavDialog_SetCreationOptions( &options );
}


// ... more option setters ...



// filterFn helper
#if MAC_OS_X_VERSION_10_3 < MAC_OS_X_VERSION_MAX_ALLOWED

Boolean NavDialog_UTIConformsTo( AEDesc * theItem, CFStringRef inUTI )
{
    FSRef			ref;
    CFStringRef		itemUTI = NULL;
    Boolean			result = false;
    AEDesc          coercedItem;
    
    if ( AECoerceDesc( theItem, typeFSRef, &coercedItem ) == noErr )
    {
        if ( AEGetDescData( &coercedItem, &ref, sizeof( FSRef ) ) == noErr )
        {
            if ( LSCopyItemAttribute( &ref, kLSRolesAll, kLSItemContentType, (CFTypeRef*)&itemUTI ) == noErr )
            {
                result = UTTypeConformsTo( itemUTI, inUTI );
                CFRelease( itemUTI );
            }
        }
        AEDisposeDesc( &coercedItem ); // don't leak
    }
    return result;
}


#endif



void NavDialog( UInt32 type, ConstStr255Param prompt, ConstStr255Param typeListOrDefaultSaveName, void * callbackFunction, void * userData )
/*
 replacement (FBtoC-only) for files$
 */
{
    NavDialogCreationOptions	options;
    NavDialogRef				dialogRef = NULL;
    
    if ( callbackFunction )
    {
        FBNavEventUserData_priv * userCBStruct = malloc( sizeof( FBNavEventUserData_priv ) ); // will free in FBFilesEventProc() case kNavCBTerminate
        userCBStruct->userCallbackFn = callbackFunction;
        userCBStruct->userData		 = userData;
        
        // options
        NavDialog_GetCreationOptions( &options );
        
        if ( type & kNavDialogSheet )
        {
            if ( options.parentWindow == NULL ) options.parentWindow = GetFrontWindowOfClass( kDocumentWindowClass, true ); // :bw:20080902
            options.modality = kWindowModalityWindowModal;
        }
        if ( type & kNavDialogMultiple )		options.optionFlags |= kNavAllowMultipleFiles;
        if ( type & kNavDialogInvisible )		options.optionFlags |= kNavAllowInvisibleFiles;
        if ( type & kNavDialogSupportPackages )	options.optionFlags |= kNavSupportPackages;
        if ( type & kNavDialogOpenPackages )	options.optionFlags |= kNavAllowOpenPackages;
        if ( type & kNavDialogPreserveFileExt )	options.optionFlags |= kNavPreserveSaveFileExtension;
        if ( prompt[0] && !options.message )    options.message = CFStringCreateWithPascalString( NULL, prompt, kCFStringEncodingMacRoman ); // #582
        if ( sPopupExtension ) options.popupExtension = sPopupExtension;
        
        // strip off options to get dialog type
        type &= 0xff;
        switch ( type )
        {
            case kNavDialogGetFile:
                if ( typeListOrDefaultSaveName[0] ) FBFilesSetTypeList( typeListOrDefaultSaveName );
                NavCreateGetFileDialog( &options, FBFilesTypeListHandle(), (NavEventProcPtr)&FBFilesEventProc, NULL, sNavDialogFilterFn, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogPutFile:
                if ( typeListOrDefaultSaveName[0] && !options.saveFileName ) options.saveFileName = CFStringCreateWithPascalString( kCFAllocatorDefault, typeListOrDefaultSaveName, kCFStringEncodingMacRoman ); // #582
                NavCreatePutFileDialog( &options, 0, 0, (NavEventProcPtr)&FBFilesEventProc, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogChooseFile:
                if ( typeListOrDefaultSaveName[0] ) FBFilesSetTypeList( typeListOrDefaultSaveName );
                NavCreateChooseFileDialog( &options, FBFilesTypeListHandle(), (NavEventProcPtr)&FBFilesEventProc, NULL, sNavDialogFilterFn, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogChooseFolder:
                NavCreateChooseFolderDialog( &options, (NavEventProcPtr)&FBFilesEventProc, sNavDialogFilterFn, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogChooseVolume:
                NavCreateChooseVolumeDialog( &options, (NavEventProcPtr)&FBFilesEventProc, sNavDialogFilterFn, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogChooseObject:
                NavCreateChooseObjectDialog( &options, (NavEventProcPtr)&FBFilesEventProc, NULL, sNavDialogFilterFn, userCBStruct, &dialogRef );
                break;
        }
        //if ( options.message != NULL ) CFRelease( options.message ); // Brian 20100726 // #582
        //if ( options.saveFileName != NULL ) CFRelease( options.saveFileName ); // Brian 20100726 // #582
        
        if ( dialogRef )
        {
            //UTIs
            // NavDialogSetFilterTypeIdentifiers weak linked < 10.4
#if MAC_OS_X_VERSION_10_3 < MAC_OS_X_VERSION_MAX_ALLOWED
            CFMutableArrayRef			fileUTIs;
            fileUTIs = NavDialog_UTIArrayRef( false, false );		// get array ref
            if ( (&NavDialogSetFilterTypeIdentifiers != NULL) && fileUTIs ) NavDialogSetFilterTypeIdentifiers( dialogRef, fileUTIs );
            (void)NavDialog_UTIArrayRef( false, true );				// release, expire and zero
#endif
            NavDialogRun( dialogRef );
        }
        NavDialog_ClearCreationOptions();
        sNavDialogFilterFn = NULL; // expire
        sPopupExtension = NULL;
    }
}


//------------------------------------------------------------------------------
// fileName = files$( type, [typeList], [prompt], *FSSpec | *FSRef | *CFURLRef )
//------------------------------------------------------------------------------

StringPtr FBFiles( UInt32 dialogType, Str255 typeList, Str255 prompt, void *fs ) {
    NavDialogCreationOptions	options;
    NavReplyRecord				reply;
    NavDialogRef				dialogRef = NULL;
    CFURLRef					url;
    Str255						name;
    
    // dialog options
    NavGetDefaultDialogCreationOptions( &options );
    options.optionFlags = ( options.optionFlags ^ kNavAllowMultipleFiles );		// default to single file
    
    switch ( dialogType ) {
        case kFBCFURLRefOpen:
            if ( typeList[0] ) FBFilesSetTypeList( typeList );
            if ( prompt[0] ) options.message = CFStringCreateWithPascalString( NULL, prompt, kCFStringEncodingMacRoman );
            NavCreateGetFileDialog( &options, FBFilesTypeListHandle(), NULL, NULL, NULL, NULL, &dialogRef );
            break;
            
        case kFBCFURLRefSave:
            if ( typeList[0] ) options.message = CFStringCreateWithPascalString( NULL, typeList, kCFStringEncodingMacRoman );
            if ( prompt[0] ) options.saveFileName = CFStringCreateWithPascalString( NULL, prompt, kCFStringEncodingMacRoman );
            NavCreatePutFileDialog( &options, 0, 0, NULL, NULL, &dialogRef );
            break;
            
        case kFBCFURLRefFolder:
            if ( typeList[0] ) options.message = CFStringCreateWithPascalString( NULL, typeList, kCFStringEncodingMacRoman );
            NavCreateChooseFolderDialog( &options, NULL, NULL, NULL, &dialogRef );
            break;
            
        default:
            PSstrcpy( gReturnedString, "\p" );
            break;
    }
    
    if ( options.message != NULL ) CFRelease( options.message ); // 20100725
    if ( options.saveFileName != NULL ) CFRelease( options.saveFileName ); // 20100725
    
    if ( dialogRef ) {
        NavDialogRun( dialogRef );
        NavDialogGetReply( dialogRef, &reply );
        switch ( NavDialogGetUserAction( dialogRef ) ) {
            case kNavUserActionOpen:
            case kNavUserActionChoose:
                switch ( dialogType ) {
                    case kFBCFURLRefOpen:
                    case kFBCFURLRefFolder:
                        NavDialog_CopyItemCFURLRef( &reply, 1, &url );
                        GetNameAsPascalStringFromCFURLRef( url, &name );
                        PSstrcpy( gReturnedString, name );
                        if ( url ) BlockMoveData( &url, fs, sizeof( CFURLRef ) ); // return file url (for kFBCFURLRefOpen) or folder url (for kFBCFURLRefFolder)
                        break;
                }
                
                FBFilesSetLastType_priv( &reply );
                break;
                
            case kNavUserActionSaveAs:
                switch ( dialogType ) {
                    case kFBCFURLRefSave:
                        NavDialog_CopyItemCFURLRef( &reply, 1, &url ); // file url
                        BlockMoveData( &url, fs, sizeof( CFURLRef ) );
                        PSstrcpy( gReturnedString, FBGetSaveFileName_priv( &reply ) );
                        break;
                    default:
                        PSstrcpy( gReturnedString, "\p" );
                        break;
                }
                break;
                
            case kNavUserActionCancel:
                PSstrcpy( gReturnedString, "\p" );
                break;
        }
        NavDisposeReply( &reply );
        NavDialogDispose( dialogRef );
    }	
    FBFilesDisposeTypeList_priv();
    return  gReturnedString;
}



//------------------------------------------------------------------------------
// NavDialogAsk      			bw:20081205
//------------------------------------------------------------------------------ 
typedef long (*Def_Fn_NavDialogAskUserEventFn_Using)( NavUserAction userAction, void* userData );


static void NavDialogAskEventProc( NavEventCallbackMessage  callBackSelector, NavCBRec* callBackParms, FBNavEventUserData_priv*  userCB )
{	
    switch ( callBackSelector )
    {
        case kNavCBTerminate:
        {
            if ( callBackParms->context ) NavDialogDispose( callBackParms->context );
            free( (void*)userCB );
        }
            break;
            
        case kNavCBUserAction:
        {
            switch ( callBackParms->userAction )
            {	case kNavUserActionCancel:
                case kNavUserActionSaveChanges:
                case kNavUserActionDontSaveChanges:
                case kNavUserActionDiscardChanges:
                case kNavUserActionReviewDocuments:
                case kNavUserActionDiscardDocuments:
                {					
                    (*(Def_Fn_NavDialogAskUserEventFn_Using)userCB->userCallbackFn)( callBackParms->userAction, userCB->userData ); // call the FB userCallbackFn
                }
                    break;
            }
        }
            break;
    }
}


void NavDialogAsk( UInt32 type, UInt32 actionDocCount, void * callbackFunction, void * userData )
{
    NavDialogCreationOptions	options;
    NavDialogRef				dialogRef = NULL;
    
    if ( callbackFunction )
    {
        FBNavEventUserData_priv * userCBStruct = malloc( sizeof( FBNavEventUserData_priv ) ); // will free in FBFilesEventProc() case kNavCBTerminate
        userCBStruct->userCallbackFn = callbackFunction;
        userCBStruct->userData		 = userData;
        
        // options		
        NavDialog_GetCreationOptions( &options );
        
        switch ( type )
        {
            case kNavDialogAskSaveChanges:
                NavCreateAskSaveChangesDialog( &options, actionDocCount, (NavEventProcPtr)&NavDialogAskEventProc, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogAskDiscardChanges:
                NavCreateAskDiscardChangesDialog( &options, (NavEventProcPtr)&NavDialogAskEventProc, userCBStruct, &dialogRef );
                break;
                
            case kNavDialogAskReviewDocuments:
                NavCreateAskReviewDocumentsDialog( &options, actionDocCount,(NavEventProcPtr)&NavDialogAskEventProc, userCBStruct, &dialogRef );
                break;
        }
        
        if ( dialogRef ) NavDialogRun( dialogRef );
        NavDialog_ClearCreationOptions();
    }		
}

#endif  /* !__LP64__ */

