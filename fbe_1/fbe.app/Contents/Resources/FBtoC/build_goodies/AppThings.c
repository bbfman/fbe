#include "FBtoC.h"
#include "Runtime.h"

#if 1   // cocoaui 20170704 bw
#include "CocoaUI.h"
#endif

#define kMajorRel 10
#define kMinorRel 7

/*
 AppThings.c
 */

FBBoolean			gFBQuit;
unsigned char		gFBFloatMaxDigits = 10;

FBOnXxxxxVector		gFBOnEventVector, gFBOnDialogVector, gFBOnMouseVector, gFBOnMenuVector, gFBOnTimerVector; // gFBOnAppleEventVector;

FBOnXxxxxVector     gFBOnFinderInfoVector;			// bw:20081117


FBOnXxxxxVector     gFBOnAppEventVector;    // cocoaui 20170704 bw
FBAppEventInfo      gFBAppEventInfo;        // cocoaui 20170704 bw


#if 1// cocoaui 20170704 bw

FBDialogEventInfo    gFBDialogEventInfo;
FBMenuEventInfo      gFBMenuEventInfo;

#if __LP64__

FBEventThing        gFBEventThing = { 0 };

void WindowSetTitle( SInt32 wndNum, CFStringRef title )
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060
    if ( WindowIsCocoa( wndNum ) ) {// 20170923 bw
        CocoaWindowSetTitle( wndNum, title );
    }
#endif // __MAC_OS_X_VERSION_MAX_ALLOWED > 1060
}

#endif



#if !__LP64__

FBEventThing        gFBEventThing = { 0xFFFFFFFF, 0, {0, 0, 0, {0, 0}, 0} }; // default 0xFFFFFFFF ticks WNE emulation

//FBMenuEventInfo        gFBMenuEventInfo;

UInt32                gFBMouseClickCount;
Point                gFBLastMousePt;

short                gFBAplActive;

long                gFBLastCursor, gFBLastCursType, gFBLastCrsrWnd;
static long            sFBPrevScrollValue;

LongDateRec            gFBControlLongDate;
SInt64                gFBControlSeconds;
Str255                gFBControlText$;

CGrafPtr            gFBBadPort;

const UInt32        kFBEFClassTag            = 'Clas';
const OSType        kFBWindowCategoryTag    = 'wCat';


static OSStatus FBHandleEvents_Sub( EventHandlerCallRef  inCaller __attribute__ ((__unused__)), EventRef inEvent __attribute__ ((__unused__)), void* inUserData __attribute__ ((__unused__)) );
static void GetMenuItemInfo( CFMutableStringRef string, MenuItemInfo *info );
static void FBInsertMenu( MenuRef theMenu, MenuID menuID );
static OSStatus FBMouseHandler( EventHandlerCallRef inCaller __attribute__ ((__unused__)), EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) );
static OSStatus FBDialogEventHandler( EventHandlerCallRef inCaller, EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) );
static OSStatus FBAppCommandProcessHandler( EventHandlerCallRef inCaller __attribute__ ((__unused__)), EventRef theEvent, void* unusedRefcon __attribute__ ((__unused__)) );
static void SetCBCInfo( SInt16 contentType, SInt32 id, ControlButtonContentInfo *cbcInfo );        // bw:20080801

#endif  /* !__LP64__ */


/*
 FBDialogFunction
 */
long FBDialogFunction( long val )
{
    if ( val == 0 ) return gFBDialogEventInfo.fbEventKind;
    if ( val < 0 ) return gFBDialogEventInfo.fbWndNum; // 20120207
    return gFBDialogEventInfo.fbEventID;
}

void CallUserDefinedOnDialogFunction( long fbEventKind, long fbEventID, long fbWndNum )
{
    // call user's DoDialog function, if defined
    if ( gFBOnDialogVector )
    {
        gFBDialogEventInfo.fbEventID   = fbEventID;
        gFBDialogEventInfo.fbEventKind = fbEventKind;
        gFBDialogEventInfo.fbWndNum = fbWndNum;
        (*gFBOnDialogVector)();
        
        
        // do we need this line? bw 20170716
        if ( gFBDialogEventInfo.fbEventKind == 1 ) FBDelay( 8 ); // rate-limit generation of button clicks FBtoC 1.3 build 152 rp:20090702   1 == kFBBtnClick
    }
}

void InstallFBAppDialogHandler( void )
{
#if !__LP64__
    static const EventTypeSpec  kAppEvents[] = {
        { kEventClassCommand, kEventCommandUpdateStatus },
        { kEventClassApplication, kEventAppActivated },
        { kEventClassApplication, kEventAppDeactivated }
    };
    InstallApplicationEventHandler( &FBDialogEventHandler, GetEventTypeCount( kAppEvents ), kAppEvents, 0, NULL );
#endif// !__LP64__
}

void InstallFBAppCommandProcessHandler( void )
// called only if 'on menu fn DoMenu' is in the user's source
{
#if !__LP64__
    static const EventTypeSpec  kFBAppEvent = { kEventClassCommand, kEventCommandProcess };
    InstallApplicationEventHandler( &FBAppCommandProcessHandler, 1, &kFBAppEvent, 0, NULL );
#endif// !__LP64__
}

long FBMenuFunction( long val )
{
    if ( val == 0 ) return gFBMenuEventInfo.menu;
    return gFBMenuEventInfo.item;
}

#else// cocoaui 20170704 bw

#if !__LP64__

FBEventThing        gFBEventThing = { 0xFFFFFFFF, 0, {0, 0, 0, {0, 0}, 0} }; // default 0xFFFFFFFF ticks WNE emulation

FBMenuEventInfo        gFBMenuEventInfo;
FBDialogEventInfo    gFBDialogEventInfo;

UInt32                gFBMouseClickCount;
Point                gFBLastMousePt;

short                gFBAplActive;

long                gFBLastCursor, gFBLastCursType, gFBLastCrsrWnd;
static long            sFBPrevScrollValue;

LongDateRec            gFBControlLongDate;
SInt64                gFBControlSeconds;
Str255                gFBControlText$;

CGrafPtr            gFBBadPort;

const UInt32        kFBEFClassTag            = 'Clas';
const OSType        kFBWindowCategoryTag    = 'wCat';


static OSStatus FBHandleEvents_Sub( EventHandlerCallRef  inCaller __attribute__ ((__unused__)), EventRef inEvent __attribute__ ((__unused__)), void* inUserData __attribute__ ((__unused__)) );
static void GetMenuItemInfo( CFMutableStringRef string, MenuItemInfo *info );
static void FBInsertMenu( MenuRef theMenu, MenuID menuID );
static OSStatus FBMouseHandler( EventHandlerCallRef inCaller __attribute__ ((__unused__)), EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) );
static OSStatus FBDialogEventHandler( EventHandlerCallRef inCaller, EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) );
static OSStatus FBAppCommandProcessHandler( EventHandlerCallRef inCaller __attribute__ ((__unused__)), EventRef theEvent, void* unusedRefcon __attribute__ ((__unused__)) );
static void SetCBCInfo( SInt16 contentType, SInt32 id, ControlButtonContentInfo *cbcInfo );        // bw:20080801

#endif  /* !__LP64__ */

#endif// cocoaui 20170704 bw



///////////////////////////////////////////////////
//                AppleEvents                    //
///////////////////////////////////////////////////


OSErr FBAEHandleQuit( AppleEvent __attribute__ ((__unused__)) *theAppleEvent, AppleEvent __attribute__ ((__unused__)) *reply, long __attribute__ ((__unused__))  myAEFlags )
{
    gFBQuit = ZTRUE;
#if !__LP64__
    QuitApplicationEventLoop();
    return noErr;
#else
    exit( 0 );
#endif  /* !__LP64__ */
}


void FBInitAEEvents( void )
{
    AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, (AEEventHandlerProcPtr)FBAEHandleQuit, 0, false );
}


///////////////////////////////////////////////////
//            HandleEvents emulation             //
///////////////////////////////////////////////////

#if __LP64__    // cocoaui 20170814 bw
void FBHandleEvents( void )
{ CocoaAppRun(); }
#endif

/*
 This allows the user to write:
 do
 HandleEvents
 until ( gFBQuit )
 
 The reason for stacking our RNE loop on top of RAEL is explained in
 http://developer.apple.com/qa/qa2001/qa1061.html
 and
 ADC Home > Reference Library > Guides > Carbon > Events & Other Input > Carbon Event Manager Programming Guide > Carbon Event Manager Tasks
 and
 http://developer.apple.com/documentation/Carbon/Conceptual/Carbon_Event_Manager/Tasks/chapter_3_section_12.html#//apple_ref/doc/uid/TP30000989-CH203-CIHFIHIF
 and
 http://developer.apple.com/samplecode/GrabBag/listing4.html
 
 RunApplicationEventLoop() gives us a lot of goodies for free. It installs handlers to:
 Allow clicks in the menu bar to begin menu tracking
 Dispatch Apple events by calling AEProcessAppleEvent
 Respond to quit Apple events by quitting RunApplicationEventLoop.
 
 But: (see bug #341)
 Our pingpong RAEL/QUAEL with stacked RNE in FBHandleEvents()/FBHandleEvents_Sub()
 is evidently incompatible with one of the many HIToolbox changes in 10.5.
 See http://developer.apple.com/releasenotes/Carbon/%20HIToolbox.html which says
 "The EditUnicodeText and HITextView controls now support drag and drop".
 This would explain why http://developer.apple.com/qa/qa2001/qa1061.html
 is deprecated in 10.5
 
 */

#if !__LP64__

static OSStatus FBHandleEvents_Sub( EventHandlerCallRef  inCaller __attribute__ ((__unused__)), EventRef inEvent __attribute__ ((__unused__)), void *  inUserData __attribute__ ((__unused__)) )
{
    EventTargetRef  theTarget = GetEventDispatcherTarget();
    EventTimeout    waitTil, timeout;
    EventRef        theEvent;
    OSStatus        err;
    
    timeout = TicksToEventTime( gFBEventThing.eventTicks );
    waitTil = GetCurrentEventTime() + timeout;
    do
    {
        // thread suppport within the event handler
        if ( gThreadsUsed ) YieldToAnyThread();
        
        err = ReceiveNextEvent( 0, NULL, timeout, true, &theEvent ); // receive all
        if ( err == noErr ) // dispatch event
        {
            EventKind	oldWhat;
            ConvertEventRefToEventRecord ( theEvent, &gFBEventThing.theEvent ); // WNE emulation
            oldWhat = gFBEventThing.theEvent.what;
            // call user's 'on event fn DoEvent', unless we have mouseMovedMessage (which does not occur in FB event handling)
            if ( gFBOnEventVector && ( (gFBEventThing.theEvent.message >> 24) != mouseMovedMessage ) )
            {
                (*gFBOnEventVector)();
                if ( gFBQuit )  waitTil = 0.0; // user set gFBQuit non-zero in Do Events(); fall out of loop
            }
            
            if ( oldWhat == gFBEventThing.theEvent.what ) SendEventToEventTarget( theEvent, theTarget ); // send on unless cancelled by user (ev.what = 0)
            ReleaseEvent( theEvent );
        }
        
    } while ( err == noErr && GetCurrentEventTime() < waitTil );
    // fall out of the loop either when RNE times out or events are being processed but the elapsed EventTime exceeded timeout
    QuitApplicationEventLoop(); // quit RAEL (in FBHandleEvents)
    return err;
}



void FBHandleEvents( void )
{
    if ( IsCocoaApplication() ) {
        CocoaAppRun();
    } else {
        static const EventTypeSpec eventSpec = { 'KWIN', 'KWIN' };
        OSStatus                err;
        static EventRef            sDummyEvent;
        static EventHandlerUPP    sEventLoopEventHandlerUPP;
        // Install FBHandleEvents_Sub, post a dummy event, then call RunApplicationEventLoop().
        if ( sEventLoopEventHandlerUPP == NULL )
        {
            sEventLoopEventHandlerUPP = NewEventHandlerUPP( FBHandleEvents_Sub );
            InstallEventHandler( GetApplicationEventTarget(), sEventLoopEventHandlerUPP, 1, &eventSpec, NULL, NULL );
            CreateEvent( NULL, 'KWIN', 'KWIN', GetCurrentEventTime(), kEventAttributeNone, &sDummyEvent );
        }
        gFBEventThing.theEvent.what = nullEvent; // WNE emulation defaults to nullEvent
        err = PostEventToQueue( GetMainEventQueue(), sDummyEvent, kEventPriorityHigh );
        if ( err == noErr ) RunApplicationEventLoop();
        // get here when FBHandleEvents_Sub() calls QuitApplicationEventLoop() and returns
    }
}


///////////////////////////////////////////////////
//                !!!:  menu stuff               //
///////////////////////////////////////////////////


enum {
    kFBAppleMenu = 254
};



static MenuRef GetAboutMenuRef( void )
{
    MenuRef m = GetMenuHandle( kFBAppleMenu );
    if ( m == NULL )
    {
        GetIndMenuItemWithCommandID( 0, kHICommandAbout, 1, &m, NULL );
        if ( m == NULL ) GetIndMenuItemWithCommandID( 0, kHICommandQuit, 1, &m, NULL );
    }
    return m;
}



static OSStatus  FBAppCommandProcessHandler( EventHandlerCallRef inCaller __attribute__ ((__unused__)), EventRef theEvent, void* unusedRefcon  __attribute__ ((__unused__)) )
/*
 FBMenuEventHandler
 Installed only if 'on menu fn DoMenu' is in the user's source.
 We must decide whether
 - to call (*gFBOnMenuVector)()
 - to return noErr or eventNotHandledErr
 */
{
    HICommandExtended	cmd;
    MenuRef				applicationMenu;
    MenuID				menuID = 0;
    MenuItemIndex		itemID = 0, hideItemID = 9999;
    UInt32				menuContext;
    
    GetEventParameter( theEvent, kEventParamMenuContext, typeUInt32, NULL, sizeof( menuContext ), NULL, &menuContext );
    // if event is from pop-up button, we don't want to call DoMenu. Returning eventNotHandledErr allows the button to accept the new state
    if ( (menuContext & kMenuContextMenuBar) == 0 )		return eventNotHandledErr;
    
    GetEventParameter( theEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( HICommandExtended ), NULL, &cmd );
    
    OSStatus	result = noErr;
    switch ( cmd.commandID )
    {
        case kHICommandPreferences: // 'relocated' app menu <--> Edit menu
            menuID = gFBMenuEventInfo.prefsMenu;
            itemID = gFBMenuEventInfo.prefsItem;
            break;
            
        case kHICommandQuit:  // 'relocated' app menu <--> File menu
            menuID = gFBMenuEventInfo.quitMenu;
            itemID = gFBMenuEventInfo.quitItem;
            result = eventNotHandledErr; // needed for Quit menu to work when app has no null events (poke long event - 8, 0xffffffff)
            break;
            
        default:
            itemID = cmd.source.menu.menuItemIndex;
            GetIndMenuItemWithCommandID( 0, kHICommandHide, 1, &applicationMenu, &hideItemID );
            if ( applicationMenu == cmd.source.menu.menuRef )
            {
                if ( itemID >= hideItemID )		return eventNotHandledErr; // Hide <appname>, Hide Others, Show All don't need the user's DoMenu
                menuID = kFBAppleMenu;	// bw:20080922
            }
            else
            {
                menuID = GetMenuID( cmd.source.menu.menuRef );
            }
            break;
    }
    if ( menuID == 0 || itemID == 0 )	return eventNotHandledErr; // sanity check; some weird thing not requiring user's DoMenu
    gFBMenuEventInfo.menu = menuID;
    gFBMenuEventInfo.item = itemID;
    (*gFBOnMenuVector)(); // call user's DoMenu function
    return result;
}

#if 0// cocoaui 20170704 bw

void InstallFBAppCommandProcessHandler( void )
// called only if 'on menu fn DoMenu' is in the user's source
{
    static const EventTypeSpec  kFBAppEvent = { kEventClassCommand, kEventCommandProcess };
    InstallApplicationEventHandler( &FBAppCommandProcessHandler, 1, &kFBAppEvent, 0, NULL );
}

long FBMenuFunction( long val )
{
    if ( val == 0 ) return gFBMenuEventInfo.menu;
    return gFBMenuEventInfo.item;
}

#endif// cocoaui 20170704 bw


static void GetMenuItemInfo( CFMutableStringRef string, MenuItemInfo *info )
{
    /*
     - Called from FBAppleMenu() and FBMenu() -
     - Picks the first menu item from string and fills info record: info.title, info.state, info.cmdKey, info.markChar
     - Some fields are not changed for the first item (this is FB's behaviour) so we increment counter (info.item)
     - The item is removed from the string before return
     
     Bugs:
     - doesn't handle '^' and '<' macros
     */
    CFIndex		strLen, index;
    UniChar		indexChar;
    
    CFStringDelete( info->title, CFRangeMake( 0, CFStringGetLength( info->title ) ) );
    
    if ( info->item ) info->state = kFBEnable;
    info->cmdKey = 0;
    info->markChar  = 0;
    index = 0;
    strLen = CFStringGetLength( string );
    while ( index < strLen )
    {
        indexChar = CFStringGetCharacterAtIndex( string, index );
        switch ( indexChar ) {
            case '^':			// not handled
            case '<':			// not handled
                index++;
                break;
                
            case '(':
                if ( info->item ) info->state = kFBDisable;
                break;
                
            case ';':
                index++;
                goto EndOfItemText;
                
            case '!':
                index++;
                if ( info->item > 0 && index < strLen ) info->markChar = CFStringGetCharacterAtIndex( string, index );
                break;
                
            case '/':
                index++;
                if ( index < strLen ) info->cmdKey = CFStringGetCharacterAtIndex( string, index );
                break;
                
            default:
                CFStringAppendCharacters( info->title, &indexChar, 1 );
        }
        index++;
    }
    
EndOfItemText:
    if ( index > strLen ) index = strLen;
    CFStringDelete( string, CFRangeMake( 0, index ) );
}




static void FBInsertMenu( MenuRef theMenu, MenuID menuID )
{
    //  insert menu in ID order (left to right)
    menuID++;
    while ( GetMenuHandle( menuID ) == 0 && menuID < 32 ) menuID++;
    if ( menuID > 31 ) menuID = 0;
    InsertMenu( theMenu, menuID );
}


#if 1   // kCFAppWndBtn

void FBAppleMenu( void *titlePtr, Boolean cfTitle )
{
    MenuItemInfo		info;
    CFStringRef			string1;
    CFMutableStringRef	string2;
    MenuRef				theMenu;
    
    if ( cfTitle ) {
        //string1 = CFStringCreateCopy( kCFAllocatorDefault, titlePtr );
        
        if ( CFGetTypeID(titlePtr) == CFStringGetTypeID() ) {
            string1 = CFStringCreateCopy( kCFAllocatorDefault, titlePtr );
        }
        if ( CFGetTypeID(titlePtr) == CFArrayGetTypeID() ) {
            string1 = CFStringCreateByCombiningStrings( kCFAllocatorDefault, titlePtr, CFSTR(";") );
        }
        
    } else {
        string1 = CFStringCreateWithPascalString( NULL, titlePtr, kCFStringEncodingMacRoman );
    }
    
    string2 = CFStringCreateMutableCopy( NULL, CFStringGetLength( string1 ), string1 );
    CFRelease( string1 );
    info.title = CFStringCreateMutable( NULL, CFStringGetLength( string2 ) );
    info.item = 1;
    theMenu = GetAboutMenuRef();
    DeleteMenuItems( theMenu, 1, gFBMenuEventInfo.lastAppleItem );					// remove previous 'apple menu' installed items (FB feature)
    InsertMenuItemTextWithCFString( theMenu, CFSTR( "-" ), 0, 0, 0 );
    while ( CFStringGetLength( string2 ) )
    {
        GetMenuItemInfo( string2, &info );
        InsertMenuItemTextWithCFString( theMenu, info.title, info.item - 1, kMenuItemAttrAutoRepeat, 0 );
        if ( info.state ) EnableMenuItem( theMenu, info.item ); else DisableMenuItem( theMenu, info.item );
        SetMenuItemCommandKey( theMenu, info.item, false, info.cmdKey );
        SetItemMark( theMenu, info.item, info.markChar );
        info.item++;
    }
    SetMenuItemCommandID( theMenu, 1, kHICommandAbout );
    gFBMenuEventInfo.lastAppleItem = info.item;
    CFRelease( string2 );
    CFRelease( info.title );
}

#else


#endif



void FBEditMenu( MenuID menuID )
{
    /*
     FBEditMenu
     new version that calls CFSTR instead of CFStr. Not localizable.
     */
    MenuRef			theMenu;
    MenuItemIndex	theItem;
    
    CreateNewMenu( menuID, 0, &theMenu );
    SetMenuTitleWithCFString( theMenu, CFSTR( "Edit" ) );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Undo" ), kMenuItemAttrDisabled | kMenuItemAttrAutoRepeat, kHICommandUndo, &theItem );
    SetItemCmd( theMenu, theItem, 'Z' );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Redo" ), kMenuItemAttrDisabled | kMenuItemAttrAutoRepeat, kHICommandRedo, &theItem );
    SetItemCmd( theMenu, theItem, 'Z' );
    SetMenuItemModifiers( theMenu, theItem, kMenuShiftModifier );
    AppendMenuItemTextWithCFString( theMenu, 0, kMenuItemAttrSeparator, 0, 0 );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Cut" ), kMenuItemAttrAutoRepeat, kHICommandCut, &theItem );
    SetItemCmd( theMenu, theItem, 'X' );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Copy" ), kMenuItemAttrAutoRepeat, kHICommandCopy, &theItem );
    SetItemCmd( theMenu, theItem, 'C' );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Paste" ), kMenuItemAttrAutoRepeat, kHICommandPaste, &theItem );
    SetItemCmd( theMenu, theItem, 'V' );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Delete" ), kMenuItemAttrAutoRepeat, kHICommandClear, &theItem );
    AppendMenuItemTextWithCFString( theMenu, CFSTR( "Select All" ), kMenuItemAttrAutoRepeat, kHICommandSelectAll, &theItem );
    SetItemCmd( theMenu, theItem, 'A' );
    FBInsertMenu( theMenu, menuID );
}



void FBMenuPreferences( MenuID menuID, MenuItemIndex itemID )
{
    CFStringRef			string = 0;
    MenuRef				theMenu;
    MenuItemAttributes	itemAttributes;
    UniChar				cmdKey = 0;
    
    theMenu = GetMenuHandle( menuID );
    if ( theMenu )
    {
        if ( itemID == CountMenuItems( theMenu ) )												// only if it's the last item in the menu
        {
            gFBMenuEventInfo.prefsMenu = menuID;
            gFBMenuEventInfo.prefsItem = itemID;
            
            CopyMenuItemTextAsCFString( theMenu, itemID, &string );
            GetMenuItemCommandKey( theMenu, itemID, false, &cmdKey );
            DeleteMenuItem( theMenu, itemID );
            itemID--;
            GetMenuItemAttributes( theMenu, itemID, &itemAttributes );
            if ( itemAttributes & kMenuItemAttrSeparator ) DeleteMenuItem( theMenu, itemID );	// if previous item is separator, remove it
        }
    }
    EnableMenuCommand( 0, kHICommandPreferences );
    
    GetIndMenuItemWithCommandID( 0, kHICommandPreferences, 1, &theMenu, &itemID );
    if ( string )
    {
        SetMenuItemTextWithCFString( theMenu, itemID, string );
        CFRelease( string );
    }
    else
    {
        SetMenuItemTextWithCFString( theMenu, itemID, CFSTR( "" ) );
        cmdKey = 0;
        SetMenuItemCommandKey( theMenu, itemID, false, cmdKey );
    }
    if ( cmdKey ) SetMenuItemCommandKey( theMenu, itemID, false, cmdKey );
}



#if 1   // _kFBMenuCF

void FBMenu( MenuID menuID, MenuItemIndex itemID, long state, void *titlePtr, MenuCommand commandID, Boolean cfType )
{
    /*
     Bugs:
     - '^'  '<' macros not handled
     - menu resources not handled
     */
    MenuItemInfo		info;
    CFStringRef			string1 = 0;
    CFMutableStringRef  string2 = 0;
    MenuItemAttributes	itemAttributes;
    MenuRef				theMenu;
    MenuItemIndex		quitItem = 0;
    UInt16				itemCount;
    
    if ( menuID == kFBAppleMenu )	// bw:20080922
    {
        theMenu = GetAboutMenuRef();
    }
    else
    {
        theMenu = GetMenuHandle( menuID );
    }
    
    if ( menuID == gFBMenuEventInfo.prefsMenu && itemID == gFBMenuEventInfo.prefsItem )
    {
        GetIndMenuItemWithCommandID( 0, kHICommandPreferences, 1, &theMenu, &itemID );
    }
    
    if ( titlePtr )
    {
        if ( cfType ) {
            if ( CFGetTypeID(titlePtr) == CFStringGetTypeID() ) {
                string1 = CFStringCreateCopy( kCFAllocatorDefault, titlePtr );
            }
            if ( CFGetTypeID(titlePtr) == CFArrayGetTypeID() ) {
                string1 = CFStringCreateByCombiningStrings( kCFAllocatorDefault, titlePtr, CFSTR(";") );
            }
        } else {
            string1 = CFStringCreateWithPascalString( kCFAllocatorDefault, titlePtr, kCFStringEncodingMacRoman );
            gFBStk--;
        }
    }
    
    if ( itemID == 0 )
    {
        if ( titlePtr )
        {
            if ( theMenu == 0 )												// add new menu
            {
                CreateNewMenu( menuID, 0, &theMenu );
                FBInsertMenu( theMenu, menuID );
            }
            if ( theMenu == 0 ) goto CantCreateMenu;
            SetMenuTitleWithCFString( theMenu, string1 );
            DeleteMenuItems( theMenu, 1, CountMenuItems( theMenu ) );		// if menu title changed, remove all menu items (FB rule)
        }
        if ( state ) EnableMenuItem( theMenu, 0 ); else DisableMenuItem( theMenu, 0 );
    }
    else
    {																	// add/amend item
        if ( theMenu == 0 ) goto CantCreateMenu;
        itemCount = CountMenuItems( theMenu );
        
        if ( itemID > itemCount )
            // adding
        {
            if ( titlePtr == 0 ) goto CantCreateMenuItem;
            
            string2 = CFStringCreateMutableCopy( NULL, CFStringGetLength( string1 ), string1 );
            info.title = CFStringCreateMutable( NULL, CFStringGetLength( string2 ) );
            info.state = state;
            info.item = 0;
            
            // fill unused items with separators
            while ( itemID > itemCount + 1 )
            {
                AppendMenuItemTextWithCFString( theMenu, CFSTR( "-" ), kMenuItemAttrSeparator, 0, 0 );
                itemCount++;
            }
            
            // pick items from string (could be ";" delimited)
            while ( CFStringGetLength( string2 ) )
            {
                GetMenuItemInfo( string2, &info );					// title, state, cmdKey, markChar
                if ( CFStringGetLength( info.title ) )
                {
                    AppendMenuItemTextWithCFString( theMenu, info.title, kMenuItemAttrAutoRepeat, commandID, &itemID ); // bw:20081031
                    if ( info.state ) EnableMenuItem( theMenu, itemID ); else DisableMenuItem( theMenu, itemID );
                    SetMenuItemCommandKey( theMenu, itemID, false, info.cmdKey );
                    if ( info.cmdKey == 'Q' ) quitItem = itemID;
                    SetItemMark( theMenu, itemID, info.markChar );
                }
                info.item++;
            }
            CFRelease( string2 );
            CFRelease( info.title );
            
            if ( quitItem )
            {
                EnableMenuCommand( 0, kHICommandQuit );
                gFBMenuEventInfo.quitMenu = menuID;
                gFBMenuEventInfo.quitItem = quitItem;
                DeleteMenuItem( theMenu, quitItem );
                quitItem--;
                GetMenuItemAttributes( theMenu, quitItem, &itemAttributes );
                if ( itemAttributes & kMenuItemAttrSeparator ) DeleteMenuItem( theMenu, quitItem );	// if previous item is separator, remove it
            }
            
        }
        else
            // amending
        {
            if ( titlePtr ) SetMenuItemTextWithCFString( theMenu, itemID, string1 );
        }
        
        switch ( state )														// {bw:20071118}
        {
            case kFBEnable:
                EnableMenuItem( theMenu, itemID );
                SetItemMark( theMenu, itemID, 0 );
                break;
                
            case kFBDisable:
                DisableMenuItem( theMenu, itemID );
                SetItemMark( theMenu, itemID, 0 );
                break;
                
            case kFBChecked:
                EnableMenuItem( theMenu, itemID );
                SetItemMark( theMenu, itemID, kMenuCheckmarkGlyph );
                break;
        }
    }
    
CantCreateMenu:
CantCreateMenuItem:
    if ( string1 ) CFRelease( string1 );
}

void MenuSetTitle( MenuID menuID, CFStringRef title )
{
    MenuRef m = GetMenuHandle( menuID );
    if ( m ) {
        SetMenuTitleWithCFString( m, title );
    }
}

CFStringRef MenuCopyTitle( MenuID menuID )
{
    CFStringRef title = NULL;
    MenuRef m = GetMenuHandle( menuID );
    if ( m ) {
        CopyMenuTitleAsCFString( m, &title );
    }
    return title;
}

void MenuItemSetText( MenuID menuID, MenuItemIndex item, CFStringRef string )
{
    MenuRef m = GetMenuHandle( menuID );
    if ( m ) {
        SetMenuItemTextWithCFString( m, item, string );
    }
}

CFStringRef MenuItemCopyText( MenuID menuID, MenuItemIndex item )
{
    CFStringRef string = NULL;
    MenuRef m = GetMenuHandle( menuID );
    if ( m ) {
        CopyMenuItemTextAsCFString( m, item, &string );
    }
    return string;
}

#else



#endif // _kFBMenuCF



///////////////////////////////////////////////////
//           !!!:  window stuff                  //
///////////////////////////////////////////////////


extern void InstallFBWndDialogHandler( WindowRef w );


//enum
//{
//	kFBtoCAppSignature = 'FB>C',
//	kFBWindowIDProperty = 'wNum',
//	kFBWindowHScrlProperty = 'hScl',
//	kFBWindowVScrlProperty = 'vScl',
//	kFBWindowStandardHandlerRequestedProperty = 'SHnd'
//};

void HideGrowBox( WindowRef w ) {
    short  minor, major;
    
    OSMajorMinor( &major, &minor );
    if ( ( major == kMajorRel && minor >= kMinorRel ) || major > kMajorRel ) {
        WindowAttributes  attributes;
        if ( GetWindowAttributes( w, &attributes ) == noErr ) {
            if ( ( attributes & kWindowCompositingAttribute ) != 0 ) {
                HIViewRef growBox;
                HIViewFindByID( HIViewGetRoot( w ), kHIViewWindowGrowBoxID, &growBox );
                HIViewSetVisible( growBox, false );
            }
        }
    }
}

void FBWindowSetID( WindowRef w, long id )
{
    SetWindowProperty( w, kFBtoCAppSignature, kFBWindowIDProperty, sizeof( id ), &id );
}



long FBGetWndNumber( WindowRef w )
{
    long	id = 0;
    
    GetWindowProperty( w, kFBtoCAppSignature, kFBWindowIDProperty, sizeof( id ), 0, &id );
    return id;
}




WindowRef FBWindowFindByID( long id )
{
    WindowRef	w = GetWindowList();
    while ( w )
    {
        if ( FBGetWndNumber( w ) == id ) return w;
        w = GetNextWindow( w );
    }
    return 0;
}





long FBSetBestWindow()
{
    WindowRef         frontDoc;
    WindowRef         frontWnd;
    WindowRef         w;
    long              wNum;
    
    frontDoc = FrontNonFloatingWindow();
    frontWnd = FrontWindow();
    if ( FBGetWndNumber( frontDoc ) == 0 )
    {
        frontDoc = NULL;
    }
    if ( FBGetWndNumber( frontWnd ) == 0 )
    {
        frontWnd = NULL;
    }
    if ( frontDoc )
    {
        w = frontDoc;
    }
    else
    {
        w = frontWnd;
    }
    wNum = FBGetWndNumber( w );
    if ( wNum )
    {
        SetPortWindowPort( w );
    }
    return wNum;
}

void FBMakeSureItsAWindow()
{
    CGrafPtr          thisPort;
    Boolean           portOK;
    
    GetPort( &thisPort );
    portOK = ( thisPort != gFBBadPort );
    if ( portOK )
    {
        portOK = IsValidPort( thisPort );
    }
    if ( portOK == false )
    {
        if ( FBSetBestWindow() == 0 )
        {
            FBWindow( 1, CFSTR("Printing without a Window!"), CGRectMake(0, 0, 400, 500), 0, NULL );
            QDSwapTextFlags( kQDUseCGTextRendering );
            FBText( 0, 12, -1, 1 );
        }
    }
}

WindowRef FBGetActiveWindow( void )
{
    WindowRef frontDoc = FrontNonFloatingWindow();
    WindowRef frontWnd = FrontWindow();
    if ( FBGetWndNumber( frontDoc ) == 0 ) frontDoc = 0;
    if ( FBGetWndNumber( frontWnd ) == 0 ) frontWnd = 0;
    if ( frontDoc ) return frontDoc;
    return frontWnd;
}

long FBWindowFunction( long expr )
{
    Rect							rContent, rStructure;
    HIViewID						viewID = { 0, 0 };
    ControlEditTextSelectionRec		sel = { 0, 0 };
    WindowClass						wc = 0;
    WindowAttributes				wa = 0;
    WindowRef						outputWnd;
    HIViewRef						view = 0;
    CGrafPtr						port;
    long							class = 0, category = 0;
    Point							tempPt;
    
    if ( expr < 0 ) return (FBWindowFindByID( -expr ) == NULL) ? 0: -1; // window( -wNum)
    outputWnd = FBGetCurOutputWindow();
    if ( gFBRoute == 128 )
    {
        GetPortBounds( gFBPRPort, &rContent );
        rStructure = rContent;
    }
    else
    {
        GetWindowBounds( outputWnd, kWindowContentRgn, &rContent );
        GetWindowBounds( outputWnd, kWindowStructureRgn, &rStructure );
    }
    
    ControlRef        c;
    Rect              scrollRect;
    
    c = GetFBWindowScrlH( outputWnd, kFBScrollVert );
    if ( c )
    {
        GetControlBounds( c, &scrollRect );
        rContent.right -= (scrollRect.right - scrollRect.left - 1);
    }
    c = GetFBWindowScrlH( outputWnd, kFBScrollHorz );
    if ( c )
    {
        GetControlBounds( c, &scrollRect );
        rContent.bottom -= (scrollRect.bottom - scrollRect.top - 1);
    }
    
    switch ( expr ) {
        case kFBActiveWnd:
            return FBGetWndNumber( FBGetActiveWindow() );
            
        case kFBOutputWnd:
            return FBGetWndNumber( outputWnd );
            
        case kFBWidth:
        case kFBContentWidth:
            return rContent.right - rContent.left;
            
        case kFBHeight:
        case kFBContentHeight:
            return rContent.bottom - rContent.top;
            
        case kFBToLeft:
            return -rContent.left;
            
        case kFBToTop:
            return -rContent.top;
            
        case kFBToRight:
            return CGDisplayPixelsWide( kCGDirectMainDisplay ) - rContent.right;
            
        case kFBToBottom:
            return CGDisplayPixelsHigh( kCGDirectMainDisplay ) - rContent.bottom;
            
        case kFBWndRef:	// and WNDPOINTER
            return (long)outputWnd;
            
        case kFBEfNum:
            GetKeyboardFocus( outputWnd, &view );
            if ( view ) GetControlID( view, &viewID );
            return (long)viewID.id;
            
        case kFBSelStart:
            GetKeyboardFocus( outputWnd, &view );
            if ( view ) GetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel, NULL );
            return sel.selStart;
            
        case kFBSelEnd:
            GetKeyboardFocus( outputWnd, &view );
            if ( view ) GetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel, NULL );
            return sel.selEnd;
            
        case kFBEfClass:
            GetKeyboardFocus( outputWnd, &view );
            if ( view ) GetControlProperty( view, 0, kFBEFClassTag, sizeof(long), NULL, &class );
            return (long)class;
            
        case kFBActiveDoc:
            return FBGetWndNumber( GetFrontWindowOfClass( kDocumentWindowClass, true ) );
            
        case kFBActivePlt:
            return FBGetWndNumber( GetFrontWindowOfClass( kFloatingWindowClass, false ) );
            
        case kFBWndPort:
            GetPort( &port );
            return (long)port;
            
        case kFBStructureTop:
            return rStructure.top;
            
        case kFBStructureLeft:
            return rStructure.left;
            
        case kFBStructureWidth:
            return rStructure.right - rStructure.left;
            
        case kFBStructureHeight:
            return rStructure.bottom - rStructure.top;
            
        case kFBContentTop:
            return rContent.top;
            
        case kFBContentLeft:
            return rContent.left;
            
        case kFBMacWClass:
            GetWindowClass( outputWnd, &wc );
            return wc;
            
        case kFBMacWAttributes:
            GetWindowAttributes( outputWnd, &wa );
            return wa;
            
        case kFBOutputWCategory:
            GetWindowProperty( outputWnd, 0, kFBWindowCategoryTag, sizeof(long), NULL, &category );
            return category;
            
        case kFBActiveWCategory:
            GetWindowProperty( FBGetActiveWindow(), 0, kFBWindowCategoryTag, sizeof(long), NULL, &category );
            return category;
            
        case kFBFloatingPtr:
            return (long)GetFrontWindowOfClass( kFloatingWindowClass, true );
            
        case kFBPenH:
        case kFBPenV:
            GetPen( &tempPt );
            return (expr == kFBPenH) ? tempPt.h : tempPt.v;
            
        default:
            break;
    }
    return 0;
}




void FBWindowClose( long id )
{
    WindowRef	w;
    
    w = FBWindowFindByID( id );
    if ( w ) DisposeWindow( w ); // prevent crash if called for non-existent window
    
    // from here on fixes bug #144
    SetUserFocusWindow( kUserFocusAuto );
    w = GetUserFocusWindow();
    if ( w )
    {
        SetPortWindowPort( w );
    }
    else
    {
        SetPort( NULL );
    }
}




void FBWindowOutput( long id )
{
    WindowRef	w;
    Boolean		makeVis = ( id > 0 );
    
    if ( id == 0 ) return; // #703
    w = FBWindowFindByID( abs( id ) );
    SetPortWindowPort( w );
    
    if ( makeVis )
    {
        if ( IsWindowVisible( w ) == false ) ShowWindow( w );
    }
    else
    {
        if ( IsWindowVisible( w ) ) HideWindow( w );
    }
}



//static UInt32 kWindowMouseTrackingTag = 'Trak';
enum { kWindowMouseTrackingTag = 'Trak' };

static void CreateWindowTrackingRegion( WindowRef w )
{
    MouseTrackingRef	trackingRef;
    RgnHandle			rgn;
    
    rgn = NewRgn();
    if ( rgn )
    {
        if ( GetWindowRegion( w, kWindowContentRgn, rgn ) == noErr )
        {
            MouseTrackingRegionID trackingID = { kWindowMouseTrackingTag, 0 };
            CreateMouseTrackingRegion( w, rgn, 0, kMouseTrackingOptionsGlobalClip, trackingID, NULL, 0, &trackingRef );
            SetWindowProperty( w, 0, kWindowMouseTrackingTag, sizeof( MouseTrackingRef ), &trackingRef );
        }
        DisposeRgn( rgn );
    }
}


static void UpdateWindowTrackingRegion( WindowRef w )
{
    RgnHandle			rgn;
    MouseTrackingRef	trackingRef;
    
    rgn = NewRgn();
    if ( rgn )
    {
        if ( GetWindowRegion( w, kWindowContentRgn, rgn ) == noErr )
        {
            if ( GetWindowProperty( w, 0, kWindowMouseTrackingTag, sizeof( MouseTrackingRef ), NULL, &trackingRef ) == noErr )
            {
                ChangeMouseTrackingRegion( trackingRef, rgn, 0 );
            }
        }
        DisposeRgn( rgn );
    }
}


WindowRef FBAppearanceWindow( long id, void *titlePtr, Rect *rectPtr, WindowClass wc, SInt32 attributes, long fbAttributes __attribute__ ((__unused__)), Boolean cfTitle )
{
    /*
     FBAppearanceWindow
     
     Bugs:
     - ignores fbAttributes param
     */
    CFStringRef				titleStr;
    Rect					r;
    WindowRef				w = NULL;   // 20170728 bw
    WindowPositionMethod	pMethod = 0;
    WindowAttributes        wa = (WindowAttributes)attributes;
    Boolean					show, initText;
    
    //if ( titlePtr ) gFBStk--; #692
    Boolean popStackOnExit = ( titlePtr != NULL ); // #692
    
    show = ( id > 0 );
    id = abs( id );
    
#if 1   // cocoaui 20170704 bw
    
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060   // 20170807 bw

#if !__LP64__
    if ( FBWindowIsCocoaWindow( id ) ) goto CantCreateWindow;
#endif  //!__LP64__
    
#endif // __MAC_OS_X_VERSION_MAX_ALLOWED > 1060   // 20170807 bw

#endif  // cocoaui 20170704 bw

    w = FBWindowFindByID( id );
    initText = ( w == NULL );
    if ( w ) {																// window exists
        if ( rectPtr ) SetWindowBounds( w, kWindowContentRgn, rectPtr );
    } else	{																// new window
        
        if ( titlePtr == 0 ) {
            if ( cfTitle ) {                    // 20160227 bw
                titlePtr = (void *)CFSTR("Untitled");
            } else {
                titlePtr = PSltrr( "Untitled", "FBAppearanceWindow");
            }
        }
        
        if ( rectPtr ) {
            r = *rectPtr;
            if ( r.left == 0 && r.top == 0 ) pMethod = kWindowCenterOnMainScreen;
            if ( gNewWndPositionMethod ) pMethod = gNewWndPositionMethod;		// {bw:20080118}
        } else {
            SetRect( &r, 0, 0, 550, 400 );
            pMethod = kWindowCascadeOnMainScreen;
        }
        
        if ( wc == 0 ) wc = kDocumentWindowClass;           // 20150719 bw
        
        if ( attributes == kFBParamMissing ) {              // 20150719 bw
            if ( wc == kDocumentWindowClass)
                wa = kWindowStandardDocumentAttributes;
            else
                wa = kWindowNoAttributes;
        }
        
        
        Boolean		userSpecifiedStandardHandlerAttr = ( (wa & kWindowStandardHandlerAttribute) != 0 );
        
        wa |= kWindowStandardHandlerAttribute; // all windows are created with kWindowStandardHandlerAttribute
        if ( CreateNewWindow( wc, wa, &r, &w ) != noErr ) goto CantCreateWindow;  // if window wasn't created, bail.
        
        HideGrowBox( w ); // Brian 20150724 - replaces the inline code code now commented out below
        
        // start - hide window grow box is >=10.7 and only if a compositing window  bw 20150513  Brian 20150717
        // OSMajorMinor( &major, &minor );
        //if ( ( major == kMajorRel && minor >= kMinorRel ) || major > kMajorRel ) {
        //if ( ( wa & kWindowCompositingAttribute ) != 0 ) {
        // HIViewRef growBox;
        //HIViewFindByID( HIViewGetRoot( w ), kHIViewWindowGrowBoxID, &growBox );
        //HIViewSetVisible( growBox, false );
        //}
        // }
        // end - hide window grow box is >=10.7 and only if a compositing window
        
        
        InstallFBWndDialogHandler( w );		// for on dialog fn DoDialog
        InstallFBMouseHandler( w );			// for on mouse fn DoMouse
        CreateWindowTrackingRegion( w );	// for dialog cursor events  bw:20080724
        FBWindowSetID( w, id );
        if ( pMethod ) RepositionWindow( w, 0, pMethod );
        
        if ( userSpecifiedStandardHandlerAttr )  // save the fact that the user code specified kWindowStandardHandlerAttribute (bug fix #345)
        {
            UInt32	dummy;
            SetWindowProperty( w, kFBtoCAppSignature, kFBWindowStandardHandlerRequestedProperty, sizeof( UInt32 ), &dummy );
        }
        
    }
    
    if ( titlePtr ) {
        if ( cfTitle ) {
            SetWindowTitleWithCFString( w, titlePtr );
        } else {
            titleStr = CFStringCreateWithPascalString( NULL, titlePtr, kCFStringEncodingMacRoman );
            SetWindowTitleWithCFString( w, titleStr );
            CFRelease( titleStr );
        }
    }
    
    SetPortWindowPort( w );
    
    if ( initText ) {
        TextSize( 9 );
        TextMode( srcCopy );
        MoveTo( 4, 10 );
    }
    
    if ( show ) {
        ShowWindow( w );
        SelectWindow( w );
    } else {
        HideWindow( w );
    }
    
CantCreateWindow:
    if ( cfTitle == false ) {
        if ( popStackOnExit ) gFBStk--; // #692
    }
    return w;
}




#define ATTRIBUTESARRAYMAXSIZE 35

void FBWindow( long inID, CFStringRef inTitle, CGRect inRect, WindowClass inClass, int inAttributes[] )
{
    // ID
    Boolean showWindow = ( inID > 0 );
    inID = abs( inID );
    
#if 1   // cocoaui 20170704 bw
    
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060   // 20170807 bw

#if !__LP64__
    if ( FBWindowIsCocoaWindow( inID ) ) goto CantCreateWindow;
#endif  //!__LP64__
    
#endif // __MAC_OS_X_VERSION_MAX_ALLOWED > 1060   // 20170807 bw

#endif  // cocoaui 20170704 bw

    // initText
    Boolean initText = false;

    // window
    WindowRef w = FBWindowFindByID( inID );
    if ( w ) {  // window already exists
        if ( inTitle ) SetWindowTitleWithCFString( w, inTitle );
        
        if ( !CGRectIsEmpty( inRect ) ) {
            HIWindowSetBounds( w, kWindowContentRgn, kHICoordSpace72DPIGlobal, &inRect );
        }
        
    } else {   // create a new window
               // text
        initText = true;
        
        // posMethod
        WindowPositionMethod posMethod = 0;
        
        // rect
        if ( CGRectIsEmpty( inRect ) ) {
            inRect = CGRectMake( 0, 0, 550, 400 );
            posMethod = kWindowCascadeOnMainScreen;
        } else {
            if ( inRect.origin.x == 0.0 && inRect.origin.y == 0.0 ) posMethod = kWindowCenterOnMainScreen;
            if ( gNewWndPositionMethod ) posMethod = gNewWndPositionMethod;
        }
        
        // class
        if ( inClass == 0 ) inClass = kDocumentWindowClass;
        
        // attributes
        int inIndex = 0;
        int index = 0;
        int attributes[ATTRIBUTESARRAYMAXSIZE];
        Boolean addCollapseBoxAttribute = false;
        Boolean userSpecifiedStandardHandlerAttr = false;
        
        if ( inAttributes ) {
            
            if ( inAttributes[inIndex] == -1 ) {    // param missing - set default attributes (document windows only)
                
                if ( inClass == kDocumentWindowClass ) {
                    int defaultAttributes[] = { kHIWindowBitCloseBox, kHIWindowBitZoomBox, kHIWindowBitCollapseBox, kHIWindowBitResizable, kHIWindowBitLiveResize, 0 };
                    memcpy (attributes, defaultAttributes, sizeof(defaultAttributes) );
                    index = sizeof(defaultAttributes)/sizeof(defaultAttributes[0]) - 1 ;  // sets index without hard-coding Brian 20170626
                    //index = 5;  // 20170626 bw
                }
                
            } else {
                
                while ( inAttributes[inIndex] ) {
                    if ( inAttributes[inIndex] == kHIWindowBitStandardHandler ) userSpecifiedStandardHandlerAttr = true;
                    
                    // floating and utility windows must have kHIWindowBitCollapseBox added after the window has been built
                    if ( ( inAttributes[inIndex] == kHIWindowBitCollapseBox ) && ( inClass == kFloatingWindowClass || inClass == kUtilityWindowClass ) ) {
                        addCollapseBoxAttribute = true;
                    } else {
                        // make sure attribute is available for this window class
                        if ( HIWindowIsAttributeAvailable( inClass, inAttributes[inIndex] ) ) {
                            attributes[index] = inAttributes[inIndex];
                            index++;
                        }
                    }
                    inIndex++;
                }
                
            }
            
        } else {
            
            // inAttributes == NULL (no attributes), so do nothing
            
            /*
            if ( inClass == kDocumentWindowClass ) {
                int defaultAttributes[] = { kHIWindowBitCloseBox, kHIWindowBitZoomBox, kHIWindowBitCollapseBox, kHIWindowBitResizable, kHIWindowBitLiveResize, 0 };
                memcpy (attributes, defaultAttributes, sizeof(defaultAttributes) );
                index = 5;
            }
             */
        }
        
        if ( !userSpecifiedStandardHandlerAttr ) {
            attributes[index] = kHIWindowBitStandardHandler;
            index++;
        }
        attributes[index] = 0;
        
        // create window
        if ( HIWindowCreate( inClass, &attributes[0], NULL, kHICoordSpace72DPIGlobal, &inRect, &w ) != noErr ) goto CantCreateWindow;
        
        // floating and utility windows must have kHIWindowBitCollapseBox added after the window has been built
        if ( addCollapseBoxAttribute ) {
            int collapseAttr[] = { kHIWindowBitCollapseBox, 0 };
            HIWindowChangeAttributes( w, &collapseAttr[0], NULL );
        }
        
        // title
        if ( inTitle ) {
            SetWindowTitleWithCFString( w, inTitle );
        } else {
            SetWindowTitleWithCFString( w, CFSTR("Untitled") );
        }
        
        HideGrowBox( w );
        
        InstallFBWndDialogHandler( w );        // for on dialog fn DoDialog
        InstallFBMouseHandler( w );            // for on mouse fn DoMouse
        CreateWindowTrackingRegion( w );       // for dialog cursor events
        
        if ( userSpecifiedStandardHandlerAttr ) { // save the fact that the user code specified kWindowStandardHandlerAttribute (bug fix #345)
            UInt32 dummy;
            SetWindowProperty( w, kFBtoCAppSignature, kFBWindowStandardHandlerRequestedProperty, sizeof(dummy), &dummy );
        }
        
        FBWindowSetID( w, inID );
        
        if ( posMethod ) RepositionWindow( w, 0, posMethod );
    }
    
    // port
    SetPortWindowPort( w );
    
    // text
    if ( initText ) {
        TextSize( 9 );
        TextMode( srcCopy );
        MoveTo( 4, 10 );
    }
    
    // show/hide
    if ( showWindow ) {
        ShowWindow( w );
        SelectWindow( w );
    } else {
        HideWindow( w );
    }
    
CantCreateWindow:
    return;
}





void FBGetWindow( long wndNum, WindowRef *w )
{
    *w = FBWindowFindByID( wndNum );
}

WindowRef FBGetCurOutputWindow( void )
{
    CGrafPtr    thePort;
    GetPort( &thePort );
    if ( IsValidPort( thePort ) ) return GetWindowFromPort( thePort );
    return NULL;
}



///////////////////////////////////////////////////
//          !!!:   timer stuff                   //
///////////////////////////////////////////////////

void FBInstallTimer( EventTime interval, long (*onTimerFn)( void ) )
{
    /*
     on timer( expr ) fn DoTimer
     timer = expr
     */
    static EventLoopTimerRef        sTheTimer;
    static EventLoopTimerProcPtr    sTheTimerProc;
    
    if ( sTheTimer ) RemoveEventLoopTimer( sTheTimer ); // remove old
    if ( onTimerFn ) sTheTimerProc = (EventLoopTimerProcPtr)onTimerFn; // called via 'on timer(xxx) fn DoTimer'; remember the proc name
    if ( interval < 0.0 )
        interval = TicksToEventTime( -interval ); // supplied as (-ve) ticks
    else if ( interval > 0.0 ); // do nothing
    else // ( interval == 0.0 )
        interval = kEventDurationForever; // don't fire
    if ( sTheTimerProc ) InstallEventLoopTimer( GetMainEventLoop(), interval, interval, sTheTimerProc, NULL, &sTheTimer );
}



///////////////////////////////////////////////////
//          !!!:   mouse stuff                   //
///////////////////////////////////////////////////
static OSStatus FBWindowContentClickHandler( EventHandlerCallRef inCaller, EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) )
{
    OSStatus result = eventNotHandledErr;
    
    GetEventParameter( theEvent, kEventParamWindowMouseLocation, typeQDPoint, NULL, sizeof( Point ), NULL, &gFBLastMousePt );
    GetEventParameter( theEvent, kEventParamClickCount, typeUInt32, NULL, sizeof( UInt32 ), NULL, &gFBMouseClickCount );
    if ( gFBOnMouseVector ) {
        (*gFBOnMouseVector)(); // call user's DoMouse function, if defined
    }
    return result;
}

static OSStatus FBMouseHandler( EventHandlerCallRef inCaller, EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) )
{
    OSStatus result = CallNextEventHandler( inCaller, theEvent );
    if ( result == eventNotHandledErr )
    {
        GetEventParameter( theEvent, kEventParamWindowMouseLocation, typeQDPoint, NULL, sizeof( Point ), NULL, &gFBLastMousePt );
        GetEventParameter( theEvent, kEventParamClickCount, typeUInt32, NULL, sizeof( UInt32 ), NULL, &gFBMouseClickCount );
        if ( gFBOnMouseVector ) (*gFBOnMouseVector)(); // call user's DoMouse function, if defined
    }
    return result;
}




void InstallFBMouseHandler( WindowRef w )
{
    // Updated 24-Dec-2009 to check for compositing attribute of window. If compositing, it installs a window content click handler instead
    // of the mouse handler. Code by bw.
    HIViewRef            c;
    WindowAttributes    wa;
    
    GetWindowAttributes( w, &wa );
    if ( wa & kWindowCompositingAttribute ) {
        static const EventTypeSpec  kMouseEvents1[] = {
            { kEventClassControl, kEventControlClick }
        };
        HIViewFindByID( HIViewGetRoot( w ), kHIViewWindowContentID, &c );
        InstallControlEventHandler( c, &FBWindowContentClickHandler, GetEventTypeCount( kMouseEvents1 ), kMouseEvents1, 0, NULL );
    } else {
        static const EventTypeSpec  kMouseEvents2[] = {
            { kEventClassWindow, kEventWindowClickContentRgn },
        };
        InstallWindowEventHandler( w, &FBMouseHandler, GetEventTypeCount( kMouseEvents2 ), kMouseEvents2, 0, NULL );
    }
}





void CallUserDefinedOnAppEventFunction( long appEventKind, long appEventID ) // cocoaui 20170704 bw
{
//    if ( gFBOnEventVector ) {
//        gFBEventThing.theEvent = appEventKind;
//        (*gFBOnEventVector)();
//    }

    if ( gFBOnAppEventVector ) {
        gFBAppEventInfo.appEventKind = appEventKind;
        gFBAppEventInfo.appEventID = appEventID;
        (*gFBOnAppEventVector)();
    }
}

long FBAppEventFunction( long val )                                         // cocoaui 20170704 bw
{
    if ( val == 0 ) return gFBAppEventInfo.appEventKind;
    return gFBAppEventInfo.appEventID;
}





///////////////////////////////////////////////////
//          !!!:   dialog stuff                  //
///////////////////////////////////////////////////

#if 0// cocoaui 20170704 bw
void CallUserDefinedOnDialogFunction( long fbEventKind, long fbEventID, long fbWndNum )
{
    // call user's DoDialog function, if defined
    if ( gFBOnDialogVector )
    {
        gFBDialogEventInfo.fbEventID   = fbEventID;
        gFBDialogEventInfo.fbEventKind = fbEventKind;
        gFBDialogEventInfo.fbWndNum = fbWndNum; // 20120207
        (*gFBOnDialogVector)();
        if ( gFBDialogEventInfo.fbEventKind == kFBBtnClick ) FBDelay( 8 ); // rate-limit generation of button clicks FBtoC 1.3 build 152 rp:20090702
        if ( gFBQuit ) QuitApplicationEventLoop(); // #421 rp:20090727
    }
}
#endif// cocoaui 20170704 bw

// CE handler
static OSStatus FBDialogEventHandler( EventHandlerCallRef inCaller __attribute__ ((__unused__)), EventRef theEvent, void* inRefcon __attribute__ ((__unused__)) )
{
    OSStatus    result;
    
    result = SetFBDialogEventInfo( theEvent );
    CallUserDefinedOnDialogFunction( gFBDialogEventInfo.fbEventKind, gFBDialogEventInfo.fbEventID, gFBDialogEventInfo.fbWndNum );
    return result;
}

#if 0// cocoaui 20170704 bw
void InstallFBAppDialogHandler( void )
{
    static const EventTypeSpec  kAppEvents[] = {
        { kEventClassCommand, kEventCommandUpdateStatus },
        { kEventClassApplication, kEventAppActivated },
        { kEventClassApplication, kEventAppDeactivated }
    };
    InstallApplicationEventHandler( &FBDialogEventHandler, GetEventTypeCount( kAppEvents ), kAppEvents, 0, NULL );
}
#endif// cocoaui 20170704 bw

void InstallFBWndDialogHandler( WindowRef w )
{
    static const EventTypeSpec  kWndEvents[] = {
        { kEventClassWindow, kEventWindowActivated },
        { kEventClassWindow, kEventWindowDeactivated },
        { kEventClassWindow, kEventWindowDrawContent },
        { kEventClassWindow, kEventWindowBoundsChanged },
        { kEventClassWindow, kEventWindowResizeStarted },
        { kEventClassWindow, kEventWindowResizeCompleted },                        // bw:20080724
        { kEventClassWindow, kEventWindowDragStarted },
        { kEventClassWindow, kEventWindowClose },
        { kEventClassWindow, kEventWindowGetClickActivation },
        { kEventClassWindow, kEventWindowToolbarSwitchMode },
        { kEventClassWindow, kEventWindowCursorChange },                        // bw:20080724
        
        { kEventClassKeyboard, kEventRawKeyDown },
        { kEventClassKeyboard, kEventRawKeyRepeat },
        
        { kEventClassControl, kEventControlHit },
        { kEventClassControl, kEventControlContextualMenuClick },
        
        { kEventClassMouse, kEventMouseEntered },                                // bw:20080724
        { kEventClassMouse, kEventMouseExited }                                    // bw:20080724
    };
    InstallWindowEventHandler( w, &FBDialogEventHandler, GetEventTypeCount( kWndEvents ), kWndEvents, 0, NULL );
}

void FBDialogStatement( long id )
{
    CallUserDefinedOnDialogFunction( kFBUserDialog, id, 0 );
}


#if 0// cocoaui 20170704 bw
/*
 FBDialogFunction
 */
long FBDialogFunction( long val )
{
    if ( val == 0 ) return gFBDialogEventInfo.fbEventKind;
    if ( val < 0 ) return gFBDialogEventInfo.fbWndNum; // 20120207
    return gFBDialogEventInfo.fbEventID;
}
#endif// cocoaui 20170704 bw

/*
 SetFBDialogEventInfo
 
 class                    kind                                |    event parameter                    |    ev                    id                |    notes
 ===========================================================================================================================================================================
 kEventClassCommand        kEventCommandUpdateStatus            |                                    |    _preview            _preMenuClick    |
 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 kEventClassApplication    kEventAppActivated                    |                                    |    _mfEvent            _mfResume        |
 kEventClassApplication    kEventAppDeactivated                |                                    |    _mfEvent            _mfSuspend        |
 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 kEventClassWindow        kEventWindowActivated                |                                    |    _wndActivate        wnd                |
 kEventClassWindow        kEventWindowBoundsChanged            |    kWindowBoundsChangeSizeChanged    |    _wndResized            wnd                |
 kEventClassWindow        kEventWindowBoundsChanged            |                                    |    _preview            _wndMoved        |
 kEventClassWindow        kEventWindowResizeStarted            |                                    |    _preview            _preWndGrow        |
 kEventClassWindow        kEventWindowDragStarted                |                                    |    _preview            _wndDocWillMove    |
 kEventClassWindow        kEventWindowClose                    |                                    |    _wndClose            wnd                |
 kEventClassWindow        kEventWindowZoomed                    |                                    |    _wndZoomIn            wnd                |    should also generate _wndZoomOut
 kEventClassWindow        kEventWindowGetClickActivation        |                                    |    _wndClick            wnd                |
 kEventClassWindow        kEventWindowToolbarSwitchMode        |                                    |    _toolbarClick        wnd                |
 kEventClassWindow        kEventWindowDrawContent                |                                    |    _wndRefresh            wnd                |
 kEventClassWindow        kEventWindowCursorChange            |    kEventParamMouseLocation        |    _cursOverBtn        btn                |
 kEventClassWindow        kEventWindowCursorChange            |    kEventParamMouseLocation        |    _cursOverNothing    N/A                |
 
 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 kEventClassControl        kEventControlContextualMenuClick    |                                    |    _cntxtMenuClick        wnd                |
 kEventClassControl        kEventControlHit                    |                                    |    _btnClick            btn                |
 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 kEventClassKeyboard    kEventRawKeyDown                    |                                    |    _evKey                ascii            |
 kEventClassKeyboard    kEventRawKeyRepeat                    |                                    |    _evKey                ascii            |
 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 kEventClassMouse        kEventMouseEntered                    |                                    |    _cursEvent            wnd                |
 kEventClassMouse        kEventMouseExited                    |                                    |    _cursEvent            wnd                |
 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
 x                                                            |    x                                |    _evDiskInsert        drive            |    not to be implemented
 */
OSStatus SetFBDialogEventInfo( EventRef theEvent )
{
    OSStatus            result = eventNotHandledErr;
    WindowRef            w = NULL;
    ControlID            cID;
    UInt32                changeAttribute;
    ControlRef            c;
    UInt32                features;
    long                ev = 0, id = 0, fbWndNum = 0;   // 20170803 bw
    unsigned char        chr;
    UInt32                dummy;
    Point                where;
    Boolean                setEventInfo = true;    // for live tracking controls in kEventControlHit handler {bw:20080430}
    static ControlRef    sLastCursorControl;        // bw:20080724
    
    gFBDialogEventInfo.fbWndNum = 0; // 20120207
    
    switch ( GetEventClass( theEvent ) )
    {
            //----------------------------------------------------------------------------------------------------------------------------------------
        case kEventClassCommand:
        {
            switch ( GetEventKind( theEvent ) )
            {
                case kEventCommandUpdateStatus:
                    ev = kFBPreview;
                    id = kFBPremenuClick;
                    break;
            }
        }
            break;
            
        case kEventClassApplication:
        {
            switch ( GetEventKind( theEvent ) )
            {
                case kEventAppActivated:
                    ev = kFBMFEvent;
                    id = kFBMFResume;
                    break;
                    
                case kEventAppDeactivated:
                    ev = kFBMFEvent;
                    id = kFBMFSuspend;
                    break;
            }
        }
            break;
            
            //----------------------------------------------------------------------------------------------------------------------------------------
        case kEventClassWindow:
        {
            GetEventParameter( theEvent, kEventParamDirectObject, typeWindowRef, NULL, sizeof( w ), NULL, &w );
//            gFBDialogEventInfo.fbWndNum = FBGetWndNumber( w ); // 20120207
            
            fbWndNum = FBGetWndNumber( w ); // 20170803 bw
            
            switch ( GetEventKind( theEvent ) )
            {
                case kEventWindowGetClickActivation:
                    ev = kFBWndClick;
                    id = FBGetWndNumber( w );
                    break;
                    
                case kEventWindowActivated:
                    ev = kFBWndActivate;
                    id = FBGetWndNumber( w );
                    SelectWindow( w );
                    SetPortWindowPort( w );
                    SetWindowMouseTrackingRegionsEnabled( w, kWindowMouseTrackingTag, true );    // bw:20080724
                    break;
                    
                case kEventWindowDeactivated:
                    ev = kFBWndActivate;
                    id = -FBGetWndNumber( w );
                    SetWindowMouseTrackingRegionsEnabled( w, kWindowMouseTrackingTag, false );    // bw:20080724
                    break;
                    
                case kEventWindowBoundsChanged:
                {
                    GetEventParameter( theEvent, kEventParamAttributes, typeUInt32, NULL, sizeof( UInt32 ), NULL, &changeAttribute );
                    if ( changeAttribute & kWindowBoundsChangeSizeChanged )
                    {
                        ev = kFBWndResized;
                        id = FBGetWndNumber( w );
                        FBResizedWindow( w );
                    }
                    else
                    {
                        ev = kFBPreview;
                        id = kFBWndMoved;
                    }
                }
                    break;
                    
                case kEventWindowResizeStarted:
                    ev = kFBPreview;
                    id = kFBPreWndGrow;
                    break;
                    
                case kEventWindowResizeCompleted:                                // bw:20080724
                    UpdateWindowTrackingRegion( w );
                    break;
                    
                case kEventWindowDragStarted:
                    ev = kFBPreview;
                    id = kFBWndDocWillMove;
                    break;
                    
                case kEventWindowClose:
                    ev = kFBWndClose;
                    id = FBGetWndNumber( w );
                    
                    if ( noErr != GetWindowProperty( w, kFBtoCAppSignature, kFBWindowStandardHandlerRequestedProperty, sizeof( UInt32 ), 0, &dummy ) )
                    {
                        result = noErr; // let the standard handler close the window only if the user specified kWindowStandardHandlerAttribute (bug #345)
                    }
                    break;
                    
                case kEventWindowZoomed:
                    ev = kFBWndZoomIn;
                    //ev = kFBWndZoomOut;// ????
                    id = FBGetWndNumber( w );
                    break;
                    
                case kEventWindowToolbarSwitchMode:
                    ev = kFBToolbarClick;
                    id = FBGetWndNumber( w );
                    break;
                    
                case kEventWindowDrawContent:
                    ev = kFBWndRefresh;
                    id = FBGetWndNumber( w );
                    break;
                    
                case kEventWindowCursorChange:                                    // bw:20080724
                {
                    GetEventParameter( theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof( where ), NULL, &where );
                    GlobalToLocal( &where );
                    c = FindControlUnderMouse( where, w, NULL );
                    if ( c )
                    {
                        if ( sLastCursorControl != c )
                        {
                            ev = kFBCursOverBtn;
                            GetControlID( c, &cID );
                            id = cID.id;
                        }
                    }
                    else
                    {
                        if ( sLastCursorControl ) ev = kFBCursOverNothing;
                    }
                    sLastCursorControl = c;
                }
                    break;
            }
            break;
        }
            
            //----------------------------------------------------------------------------------------------------------------------------------------
        case kEventClassKeyboard:
        {
            switch ( GetEventKind( theEvent ) )
            {
                case kEventRawKeyDown:
                case kEventRawKeyRepeat:
                {
                    UInt32        keyModifiers;
                    HIViewRef    view = NULL;
                    GetKeyboardFocus( GetUserFocusWindow(), &view );
                    
                    fbWndNum = FBGetWndNumber( GetUserFocusWindow() );  // 20170803 bw

                    if ( view == NULL )
                    {
                        GetEventParameter( theEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof( char ), NULL, &chr );
                        id = chr;
                        GetEventParameter( theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof( keyModifiers ), NULL, &keyModifiers );
                        ev = kFBEvKey;
                        if ( keyModifiers & cmdKey ) ev = kFBEvCmdKey;
                    }
                }
                    break;
            }
            break;
        }
            
            //----------------------------------------------------------------------------------------------------------------------------------------
        case kEventClassControl:
        {
            GetEventParameter( theEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof( c ), NULL, &c );
            switch ( GetEventKind( theEvent ) )
            {
                case kEventControlHit:
                    SetPortWindowPort( GetControlOwner( c ) );    // #464 bw:20090715
                    GetControlID( c, &cID );
                    
                    if ( cID.signature == kFBtoCAppSignature )
                    {
                        ControlKind        k;
                        
                        if ( GetControlKind( c, &k ) == noErr )
                        {
                            switch ( k.kind )
                            {
                                case kControlKindLittleArrows:        /* little arrows send FB dialog events through LittleArrowsControlAction */
                                    setEventInfo = false;
                                    break;
                                    
                                case kControlKindSlider:            /* live sliders send FB dialog events through SliderControlAction */
                                    GetControlFeatures( c, &features );
                                    setEventInfo = !(features & kControlSupportsLiveFeedback);
                                    break;
                                    
                                case kControlKindScrollBar:        /* scroll bars send FB dialog events through ScrollbarControlAction (except for non-live slider thumb drags) */
                                    GetControlFeatures( c, &features );
                                    if ( features & kControlSupportsLiveFeedback )
                                    {
                                        setEventInfo = false;
                                    }
                                    else
                                    {
                                        ControlPartCode    part;
                                        GetEventParameter( theEvent, kEventParamControlPart, typeControlPartCode, NULL, sizeof( part ), NULL, &part );
                                        if ( part != kControlIndicatorPart ) setEventInfo = false;
                                    }
                                    break;
                                    
                                default:
                                    break;
                            }
                            
                            if ( setEventInfo )
                            {
                                ev = kFBBtnClick;
                                id = cID.id;
                                fbWndNum = FBGetWndNumber( GetControlOwner( c ) );  // 20170803 bw
                            }
                        }
                    }
                    break;
                    
                case kEventControlContextualMenuClick:
                    ev = kFBCntxtMenuClick;
                    id = FBGetWndNumber( GetControlOwner( c ) );
                    fbWndNum = id;  // 20170803 bw
                    break;
            }
            break;
        }
            
        case kEventClassMouse:                                                    // bw:20080724
        {
            switch ( GetEventKind( theEvent ) )
            {
                case kEventMouseEntered:
                case kEventMouseExited:
                    GetEventParameter( theEvent, kEventParamDirectObject, typeWindowRef, NULL, sizeof( w ), NULL, &w );
                    ev = kFBCursEvent;
                    id = FBGetWndNumber( w );
                    fbWndNum = id;  // 20170803 bw
                    break;
            }
            break;
        }
    }
    
    gFBDialogEventInfo.fbEventKind = ev;
    gFBDialogEventInfo.fbEventID = id;
    gFBDialogEventInfo.fbWndNum = fbWndNum;  // 20170803 bw
    return result;
}


///////////////////////////////////////////////////
//                   button stuff                //
///////////////////////////////////////////////////

#if 1   // kCFAppWndBtn


void WindowSetTitle( SInt32 wndNum, CFStringRef title )
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060
    if ( WindowIsCocoa( wndNum ) ) {// 20170923 bw
        CocoaWindowSetTitle( wndNum, title );
        return;
    }
#endif // __MAC_OS_X_VERSION_MAX_ALLOWED > 1060
    
    WindowRef w = FBWindowFindByID( wndNum );
    if ( w ) {
        SetWindowTitleWithCFString( w, title );
    }
}

CFStringRef WindowCopyTitle( SInt32 wndNum )
{
    CFStringRef title = NULL;
    WindowRef w = FBWindowFindByID( wndNum );
    if ( w ) {
        CopyWindowTitleAsCFString( w, &title );
    }
    return title;
}

void ButtonSetText( SInt32 btnNum, CFStringRef string )
{
    HIViewRef   contentView, view;
    HIViewID viewID = { kFBtoCAppSignature, btnNum };
    WindowRef w = FBGetCurOutputWindow();
    GetRootControl( w, &contentView );
    if ( HIViewFindByID( contentView, viewID, &view ) == noErr ) {
        HIViewSetText( view, string );
        WindowAttributes wa;
        GetWindowAttributes( w, &wa );
        if ( (wa & kWindowCompositingAttribute) == 0 ) DrawOneControl( view );
    }
}

CFStringRef ButtonCopyText( SInt32 btnNum )
{
    CFStringRef string = NULL;
    HIViewRef   contentView, view;
    HIViewID viewID = { kFBtoCAppSignature, btnNum };
    GetRootControl( FBGetCurOutputWindow(), &contentView );
    if ( HIViewFindByID( contentView, viewID, &view ) == noErr ) {
        string = HIViewCopyText( view );
    }
    return string;
}

#endif  // kCFAppWndBtn

void ControlSetText( HIViewRef c, CFStringRef titleStr ) __attribute__ ((noinline));
void ControlSetText( HIViewRef c, CFStringRef titleStr )
/*
 ControlSetText
 OS X 10.3 equivalent of HIViewSetText - Attempts to set kControlEditTextPart. If that fails, sets control title.
 */
{
    if ( SetControlData( c, kControlEditTextPart, kControlEditTextCFStringTag, sizeof(CFStringRef), &titleStr ) )
    {
        SetControlTitleWithCFString( c, titleStr );
    }
    DrawOneControl( c );
}


CFStringRef FBControlCopyText( HIViewRef c ) __attribute__ ((noinline));
CFStringRef FBControlCopyText( HIViewRef c )
{
    /*
     FBControlCopyText
     OS X 10.3 equivalent of HIViewGetText - Attempts to get kControlEditTextPart. If that fails, gets control title.
     */
    CFStringRef        titleStr = NULL;
    
    if ( GetControlData( c, kControlEditTextPart, kControlEditTextCFStringTag, sizeof(CFStringRef), &titleStr, NULL ) )
    {
        CopyControlTitleAsCFString( c, &titleStr );
    }
    return titleStr;
}


HIViewRef FBButtonAmpersandFunction( long btnNum )__attribute__ ((noinline));
HIViewRef FBButtonAmpersandFunction( long btnNum )
{
    /*
     FBButtonAmpersandFunction  button&( btnNum )
     */
    HIViewRef    c = NULL, contentView;
    
    GetRootControl( FBGetCurOutputWindow(), &contentView );
    HIViewID cID = { kFBtoCAppSignature, btnNum };
    HIViewFindByID( contentView, cID, &c );
    return c;
}




void FBButtonClose( long btnNum )
{
    HIViewRef    c, contentView;
    
    HIViewID cID = { kFBtoCAppSignature, btnNum };
    GetRootControl( FBGetCurOutputWindow(), &contentView );
    if ( noErr == HIViewFindByID( contentView, cID, &c ) )
    {
        WindowRef         outputWnd;
        outputWnd = FBGetCurOutputWindow();
        
        if ( c == GetFBWindowScrlH( outputWnd, 1 ) ) RemoveWindowProperty( outputWnd, kFBtoCAppSignature, kFBWindowVScrlProperty );
        
        if ( c == GetFBWindowScrlH( outputWnd, 2 ) ) RemoveWindowProperty( outputWnd, kFBtoCAppSignature, kFBWindowHScrlProperty );
        
        DisposeControl( c );
    }
}


long FBButtonFunction( long btnNum, long selector )
{
    /*
     Bugs:
     - FBGETCTLPAGE selector not yet handled
     - previous value of scroll bar (button with omitted id) not supported
     */
    WindowRef            w = 0;
    HIViewRef            c = 0, contentView = 0;
    Handle                h;
    MenuRef                menuRef;
    UInt16                subControlCount;
#ifdef MAC_OS_X_VERSION_10_5
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    MenuItemIndex    menuVal;
#else
    SInt16        menuVal;
#endif					/*MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5*/
#else
    SInt16        menuVal;
#endif					/*def MAC_OS_X_VERSION_10_5*/
    
    w = FBGetCurOutputWindow();
    
    if ( btnNum == 0 && selector == 0 ) {
        // get previous scroll bar value
        return sFBPrevScrollValue; // 20081114
                                   //return 0;
    }
    
    GetRootControl( w, &contentView );
    HIViewID cID = { kFBtoCAppSignature, btnNum };
    HIViewFindByID( contentView, cID, &c );
    
    HIViewKind  vk;   // 20160226 bw
    HIViewGetKind( c, &vk );  // 20160226 bw
    
    switch ( selector ) {
        case 0:
            if ( IsControlActive( c ) ) return GetControl32BitValue( c ); else return 0;
            
        case kFBGetCtlRawValue:
            return GetControl32BitValue( c );
            
        case kFBGetCtlMinimum:
            return GetControl32BitMinimum( c );
            
        case kFBGetCtlMaximum:
            return GetControl32BitMaximum( c );
            
        case kFBGetCtlPage:
            return 0;    // ********
            
        case kFBGetRootControl:
            GetRootControl( w, &c );
            return (SInt32)c;
            
        case kFBCountSubcontrols:
            CountSubControls( c, &subControlCount );
            return (SInt32)subControlCount;
            
        case kFBGetSupercontrol:
            GetSuperControl( c, &c );
            GetControlID( c, &cID );
            return cID.id;
            
        case kFBGetControlDate:
            gFBControlText$[0] = 0;
            if ( GetControlData( c, kControlEditTextPart, kControlClockLongDateTag, sizeof(LongDateRec), &gFBControlLongDate, NULL ) == noErr )
            {
                LongDateToSeconds( &gFBControlLongDate, &gFBControlSeconds );
                LongDateString( &gFBControlSeconds, shortDate, gFBControlText$, NULL );
            }
            return 0;
            
        case kFBGetControlTime:
            gFBControlText$[0] = 0;
            if ( GetControlData( c, kControlEditTextPart, kControlClockLongDateTag, sizeof(LongDateRec), &gFBControlLongDate, NULL ) == noErr )
            {
                LongDateToSeconds( &gFBControlLongDate, &gFBControlSeconds );
                LongTimeString( &gFBControlSeconds, true, gFBControlText$, NULL );
            }
            return 0;
            
        case kFBGetControlTEHandle:
            GetControlData( c, kControlEditTextPart, kControlEditTextTEHandleTag, sizeof( Handle ), &h, NULL );
            return (SInt32)h;
            
        case kFBGetBevelControlMenuHandle:
            GetBevelButtonMenuHandle( c, &menuRef );
            return (SInt32)menuRef;
            
        case kFBGetBevelControlMenuVal:
            GetBevelButtonMenuValue( c, &menuVal );
            return (SInt32)menuVal;
            
        case kFBGetBevelControlLastMenu:
            GetControlData( c, kControlMenuPart, kControlBevelButtonLastMenuTag, sizeof( SInt16 ), &menuVal, NULL );
            return (SInt32)menuVal;
            
        case kFBGetControlMenuHandle:
            return (SInt32)GetControlPopupMenuHandle( c );
            
        case kFBGetControlMenuID:
            switch ( vk.kind ) {
                case kControlKindBevelButton:
                    GetBevelButtonMenuHandle( c, &menuRef );
                    return (SInt32)GetMenuID( menuRef );
                    break;
                default:    // kControlKindPopupButton, kControlKindPopupGroupBox
                    return (SInt32)GetControlPopupMenuID( c );
                    break;
            }
            break;
            
        default:
            if ( selector < 0 )
            {
                selector = -selector;
                GetIndexedSubControl( c, selector, &c );
                GetControlID( c, &cID );
                return cID.id;
            }
            break;
    }
    return 0;
}


static void LittleArrowsControlAction( ControlRef c, ControlPartCode partCode )
{
    /*
     make little arrows control count the right way
     Update 20080429 bw:
     [1] added CallUserDefinedOnDialogFunction
     [2] don't trigger dialog event when (value < min) or (value > max)
     */
    SInt32        increment = 1;
    Boolean        change = false;
    
    GetControlData( c, kControlEntireControl, kControlLittleArrowsIncrementValueTag, sizeof( increment ), &increment, NULL );
    SInt32 value = GetControl32BitValue( c );
    switch ( partCode )
    {
        case kControlUpButtonPart:
            value += increment;
            change = ( value <= GetControl32BitMaximum( c ) );
            break;
        case kControlDownButtonPart:
            value -= increment;
            change = ( value >= GetControl32BitMinimum( c ) );
            break;
    }
    if ( change )
    {
        HIViewID    viewID;
        GetControlID( c, &viewID );
        SetControl32BitValue( c, value );
        
        WindowRef w = GetControlOwner( c ); // 20170803 bw
        SInt32 wndID = FBGetWndNumber( w ); // 20170803 bw
        CallUserDefinedOnDialogFunction( kFBBtnClick, (long)viewID.id, wndID ); // 20170803 bw
    }
}

static SInt32 sFBLastSliderValue;

static void SliderControlAction( ControlRef c, ControlPartCode __attribute__ ((__unused__)) partCode )
{
    /*
     Installed on live sliders only. We get here after SliderClickHandler() -
     */
    SInt32    value = GetControl32BitValue( c );
    if ( value != sFBLastSliderValue )
    {
        HIViewID    viewID;
        sFBLastSliderValue = value;
        GetControlID( c, &viewID );
        
        WindowRef w = GetControlOwner( c ); // 20170803 bw
        SInt32 wndID = FBGetWndNumber( w ); // 20170803 bw
        CallUserDefinedOnDialogFunction( kFBBtnClick, (long)viewID.id, wndID ); // 20170803 bw
    }
}


static OSStatus SliderClickHandler( EventHandlerCallRef __attribute__ ((__unused__)) inCaller, EventRef __attribute__ ((__unused__)) inEvent, ControlRef c )
{
    /*
     Installed on live sliders only. We get here before SliderControlAction() -
     Sets things up so that SliderControlAction sends FB dialog event iff the value changed
     */
    sFBLastSliderValue = GetControl32BitValue( c );
    return eventNotHandledErr;
}


static SInt32 sFBLastScrollValue;

static void ScrollbarControlAction( HIViewRef c, ControlPartCode part )
{
    /*
     Installed on all scroll bars. We get here after ScrollbarClickHandler() -
     */
    SInt32    delta = 0, nextVal;
    SInt32     val = GetControl32BitValue( c );
    SInt32     min = GetControl32BitMinimum( c );
    SInt32     max = GetControl32BitMaximum( c );
    switch ( part )
    {
        case kControlUpButtonPart:
            if ( val > min ) delta = -1;
            break;
            
        case kControlDownButtonPart:
            if ( val < max ) delta = 1;
            break;
            
        case kControlPageUpPart:
            if ( val > min ) delta = -GetControlViewSize( c );
            break;
            
        case kControlPageDownPart:
            if ( val < max ) delta = GetControlViewSize( c );
            break;
            
        case kControlIndicatorPart:
            delta = sFBLastScrollValue - val;
            val -= delta;
            break;
    }
    
    if ( delta )
    {
        HIViewID    viewID;
        nextVal = val + delta;
        if ( nextVal < min ) nextVal = min;
        if ( nextVal > max ) nextVal = max;
        SetControl32BitValue( c, nextVal );
        sFBPrevScrollValue = sFBLastScrollValue; //20081114
        sFBLastScrollValue = nextVal;
        GetControlID( c, &viewID );
        
        WindowRef w = GetControlOwner( c ); // 20170803 bw
        SInt32 wndID = FBGetWndNumber( w ); // 20170803 bw
        CallUserDefinedOnDialogFunction( kFBBtnClick, (long)viewID.id, wndID ); // 20170803 bw

    }
}


static OSStatus ScrollbarClickHandler( EventHandlerCallRef __attribute__ ((__unused__)) inCaller, EventRef __attribute__ ((__unused__)) inEvent, ControlRef c )
{
    /*
     Installed on all scroll bars. We get here before ScrollbarControlAction() -
     */
    sFBPrevScrollValue = sFBLastScrollValue; //20081114
    sFBLastScrollValue = GetControl32BitValue( c );
    return eventNotHandledErr;
}



void MakeAllControlsIntoFBButtons( HIViewRef c )
{
    ControlID       cID;
    ControlKind     k;
    UInt32          features;
    static const EventTypeSpec    eventSpec[] = { { kEventClassControl, kEventControlClick } };
    
    c = HIViewGetFirstSubview( c );
    while ( c )
    {
        if ( GetControlID( c, &cID ) == noErr )
        {
            if ( cID.id )
            {
                cID.signature = kFBtoCAppSignature;
                SetControlID( c, &cID );
                GetControlKind( c, &k );
                switch ( k.kind )
                {
                    case kControlKindLittleArrows:
                        SetControlAction( c, LittleArrowsControlAction );
                        break;
                        
                    case kControlKindScrollBar:
                        if ( GetControlViewSize( c ) < 1 )    SetControlViewSize( c, 1 );
                        SetControlAction( c, ScrollbarControlAction );
                        InstallControlEventHandler( c, (EventHandlerProcPtr)&ScrollbarClickHandler, 1, eventSpec, c, NULL );
                        break;
                        
                    case kControlKindSlider:
                        GetControlFeatures( c, &features );
                        if ( features & kControlSupportsLiveFeedback )
                        {
                            SetControlAction( c, SliderControlAction );
                            InstallControlEventHandler( c, (EventHandlerProcPtr)&SliderClickHandler, 1, eventSpec, c, NULL );
                        }
                        break;
                    default:
                        break;
                }
            }
            MakeAllControlsIntoFBButtons( c );
        }
        c = HIViewGetNextView( c );
    }
}


void MakeIntoFBWindow( WindowRef w, SInt32 wNum )
{
    FontInfo    info;
    HIViewRef    c;
    
    InstallFBWndDialogHandler( w );
    InstallFBMouseHandler( w );               // bw:20091220
    FBWindowSetID( w, wNum );
    InstallStandardEventHandler( GetWindowEventTarget( w ) );
    SetPortWindowPort( w );
    FBText( applFont, 9, 0, srcCopy );
    GetFontInfo( &info );
    MoveTo( 4, info.ascent + info.descent + info.leading );
    HIViewFindByID( HIViewGetRoot( w ), kHIViewWindowContentID, &c );
    MakeAllControlsIntoFBButtons( c );
}

void MakeIntoFBButton( HIViewRef view, SInt32 btnNum )
{
    HIViewID viewID = { kFBtoCAppSignature, btnNum };
    SetControlID( view, &viewID );
    //    HIViewSetID( view, viewID ); // AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
}


static void SetCBCInfo( SInt16 contentType, SInt32 id, ControlButtonContentInfo *cbcInfo )
{
    cbcInfo->contentType = contentType;
    switch ( contentType )
    {
        case kControlNoContent:
            break;
        case kControlContentIconSuiteRes:
        case kControlContentCIconRes:
        case kControlContentPictRes:
        case kControlContentICONRes:
            cbcInfo->u.resID = id;
            break;
            
        case kControlContentCIconHandle:
            cbcInfo->u.cIconHandle = (CIconHandle)id;
            break;
            
        case kControlContentPictHandle:
            cbcInfo->u.picture = (PicHandle)id;
            break;
            
        case kControlContentIconRef:
            cbcInfo->u.iconRef = (IconRef)id;
            break;
            
        case kControlContentICON:
        case kControlContentIconSuiteHandle:
            cbcInfo->u.ICONHandle = (Handle)id;
            break;
            
        case kControlContentCGImageRef:
            cbcInfo->u.imageRef = (CGImageRef)id;
            break;
    }
}


#if kSmartTabsRadioGroup

HIViewRef RadioGroupCreate( WindowRef w, ControlSize size, SInt32 rows, SInt32 cols, CFArrayRef titles, Rect *rectPtr )
{
    HIViewRef group;
    CreateRadioGroupControl( w, rectPtr, &group );
    
    if ( rows != 0 && cols != 0 ) {
        ControlFontStyleRec   cfs;
        HIViewRef             c;
        Rect                  groupRect, r = {0,0,0,0};
        
        GetControlBounds( group, &groupRect );
        
        WindowRef window = FBGetCurOutputWindow();
        Boolean isCompositing = HIWindowTestAttribute( window, kHIWindowBitCompositing );
        
        cfs.flags = kControlUseFontMask;
        switch ( size ) {
            case kControlSizeSmall:
                cfs.font = kControlFontSmallSystemFont;
                break;
            case kControlSizeMini:
                cfs.font = kControlFontMiniSystemFont;
                break;
            default:
                break;
        }
        
        CreateRadioButtonControl( NULL, &r, NULL, 1, true, &c );
        SetControlData( c, kControlEntireControl, kControlSizeTag, sizeof(size), &size );
        GetBestControlRect( c, &r, NULL );
        DisposeControl( c );
        short btnHeight = r.bottom - r.top;
        
        short w = groupRect.right - groupRect.left;
        short h = groupRect.bottom - groupRect.top;
        
        short cellWidth = w / cols;
        short ySpace = 0;
        if ( rows > 1 ) ySpace = (h - btnHeight * rows) / (rows - 1);
        
        CFIndex index = 0;
        short x = 0;
        if ( !isCompositing ) x = groupRect.left;
        
        for ( long col = 1; col <= cols; col++ ) {
            short y = 0;
            if ( !isCompositing ) y = groupRect.top;
            for ( long row = 1; row <= rows; row++ ) {
                SetRect( &r, x, y, x + cellWidth, y + btnHeight );
                
                CFStringRef title = NULL;
                if ( titles ) {
                    if ( index < CFArrayGetCount( titles ) ) {
                        title = CFStringCreateCopy( kCFAllocatorDefault, CFArrayGetValueAtIndex( titles, index ) );
                    }
                }
                if ( title == NULL ) title = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("Radio %ld"), index + 1 );
                
                CreateRadioButtonControl( NULL, &r, title, 1, true, &c );
                
                CFRelease( title );
                
                SetControlData( c, kControlEntireControl, kControlSizeTag, sizeof(size), &size );
                if ( size != kControlSizeNormal && size != kControlSizeLarge ) SetControlFontStyle( c, &cfs );
                
                HIViewAddSubview( group, c );
                
                y += btnHeight + ySpace;
                index++;
            }
            x += cellWidth;
        }
    }
    return group;
}

HIViewRef RadioGroupViewViewAtIndex( HIViewRef groupView, CFIndex index )
{
    HIViewRef view = NULL;
    HIViewGetIndexedSubview( groupView, index, &view );
    return view;
}

HIViewRef RadioGroupViewAtIndex( SInt32 groupID, CFIndex index )
{ return RadioGroupViewViewAtIndex( FBButtonAmpersandFunction(groupID), index ); }

void RadioGroupViewSetTitleAtIndex( HIViewRef groupView, CFStringRef title, CFIndex index )
{
    HIViewRef view = RadioGroupViewViewAtIndex( groupView, index );
    if ( view ) HIViewSetText( view, title );
}

void RadioGroupSetTitleAtIndex( SInt32 groupID, CFStringRef title, CFIndex index )
{ RadioGroupViewSetTitleAtIndex( FBButtonAmpersandFunction( groupID ), title, index ); }


void TabsViewSetTab( HIViewRef tabsView, SInt32 tabIndex )
{
    HIViewRef pane;
    SInt32 index = HIViewGetMaximum( tabsView );
    while ( index ) {
        HIViewGetIndexedSubview( tabsView, index, &pane );
        HIViewSetVisible( pane, (index == tabIndex) );
        index--;
    }
}

OSStatus TabsViewHitHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void *userData )
{
    HIViewRef tabsView = userData;
    TabsViewSetTab( tabsView, HIViewGetValue(tabsView) );
    return eventNotHandledErr;//noErr;
}

HIViewRef TabsViewCreate( WindowRef w, SInt32 value, SInt32 max, CFArrayRef titleArray, Rect *rectPtr, long type )
{
    HIViewRef tabsView = NULL;
    switch ( type ) {
        case kControlTabSmallNorthProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionNorth, max, 0, &tabsView );
            break;
            
        case kControlTabLargeSouthProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionSouth, max, 0, &tabsView );
            break;
            
        case kControlTabSmallSouthProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionSouth, max, 0, &tabsView );
            break;
            
        case kControlTabLargeEastProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionEast, max, 0, &tabsView );
            break;
            
        case kControlTabSmallEastProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionEast, max, 0, &tabsView );
            break;
            
        case kControlTabLargeWestProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionWest, max, 0, &tabsView );
            break;
            
        case kControlTabSmallWestProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionWest, max, 0, &tabsView );
            break;
        default:
        case kControlTabLargeNorthProc:
            CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionNorth, max, 0, &tabsView );
            break;
    }
    
    if ( titleArray != NULL ) {
        CFIndex count = CFArrayGetCount( titleArray );
        HIViewSetMaximum( tabsView, count );
        CFIndex initIndex = value - 1;  // 20160623 bw
        ControlTabInfoRecV1 infoRec;
        infoRec.version = kControlTabInfoVersionOne;
        infoRec.iconSuiteID = 0;
        Rect r;
        GetTabContentRect( tabsView, &r );
        for ( CFIndex index = 0; index < count; index++ ) {
            infoRec.name = CFArrayGetValueAtIndex( titleArray, index );
            SetControlData( tabsView, index+1, kControlTabInfoTag, sizeof(infoRec), &infoRec );
            HIViewRef pane;
            CreateUserPaneControl( NULL, &r, kControlSupportsEmbedding, &pane );
            HIViewAddSubview( tabsView, pane );
            
            // ===== bw 20170330 =====
            HILayoutInfo layout;
            layout.version = kHILayoutInfoVersionZero;
            HIViewGetLayoutInfo( pane, &layout );
            layout.binding.top.kind = kHILayoutBindTop;
            layout.binding.left.kind = kHILayoutBindLeft;
            layout.binding.bottom.kind = kHILayoutBindBottom;
            layout.binding.right.kind = kHILayoutBindRight;
            HIViewSetLayoutInfo( pane, &layout );
            // ==========
            
            HIViewSetVisible( pane, (index == initIndex) );  // 20160623 bw
        }
        HIViewSetValue( tabsView, value );  // 20160623 bw
    }
    EventTypeSpec ev = {kEventClassControl,kEventControlHit};
    InstallEventHandler( GetControlEventTarget(tabsView), TabsViewHitHandler, 1, &ev, tabsView, NULL );
    return tabsView;
}

void TabsViewAddViewToTab( HIViewRef tabsView, HIViewRef view, SInt32 tabIndex )
{
    HIViewRef pane;
    HIViewGetIndexedSubview( tabsView, tabIndex, &pane );
    HIViewAddSubview( pane, view );
}

void TabsAddViewToTab( SInt32 tabsBtnNum, HIViewRef view, SInt32 tabIndex )
{
    HIViewRef tabsView = FBButtonAmpersandFunction( tabsBtnNum );
    TabsViewAddViewToTab( tabsView, view, tabIndex );
}

void TabsAddButtonToTab( SInt32 tabsBtnNum, SInt32 btnNum, SInt32 tabIndex )
{
    HIViewRef view = FBButtonAmpersandFunction( btnNum );
    TabsAddViewToTab( tabsBtnNum, view, tabIndex );
}

#endif   // kSmartTabsRadioGroup



#if 1   // kCFAppWndBtn

HIViewRef FBAppearanceButton( long id, SInt32 state, SInt32 value, SInt32 min, SInt32 max, void * titlePtr, Rect * rectPtr, long type, Boolean cfTitle )
{
    SInt32                            backupState = state; // :rp:20081128
    ControlFontStyleRec                cfs;
    CFStringRef                        titleStr = NULL;
    
#if kSmartTabsRadioGroup
    CFArrayRef                      titleArray = NULL;
#endif //kSmartTabsRadioGroup
    
    WindowRef                        w;
    HIViewRef                        c = 0, contentView = 0;
    ControlBevelThickness            bevelThickness = 0;
    ControlBevelButtonBehavior        bevelBehaviour = 0;
    ControlBevelButtonMenuBehavior    bevelMenuBehaviour = 0;
    ControlBevelButtonMenuPlacement    bevelMenuPlacement = 0;
    ControlButtonContentInfo        cbcInfo;
    ControlSliderOrientation        sliderOrientation = kControlSliderPointsDownOrRight;
    UInt16                            sliderNumTickMarks = 0;
    SInt16                            contentType = 0;
    Boolean                            noTrack = false, liveTracking = false, isDefaultPushButton, isVisible, useWFont = false;
    ControlKind                        cKind;
    SInt32                            backupValue = value;
    ldesRec**                        ldesH;
    static const EventTypeSpec        eventSpec[] = { { kEventClassControl, kEventControlClick } };
    
    
    if ( titlePtr ) {
        if ( cfTitle == false ) gFBStk--;
    }
    
    isVisible = ( id > 0 );
    id = abs( id );
    w = FBGetCurOutputWindow();
    GetRootControl( w, &contentView );
    HIViewID cID = { kFBtoCAppSignature, id };
    HIViewFindByID( contentView, cID, &c );
    
    
#if kSmartTabsRadioGroup
    if ( titlePtr ) {
        if ( cfTitle ) {
            if ( CFGetTypeID(titlePtr) == CFStringGetTypeID() ) {
                titleStr = CFStringCreateCopy( kCFAllocatorDefault, titlePtr );
            }
            if ( CFGetTypeID(titlePtr) == CFArrayGetTypeID() ) {
                titleArray = CFArrayCreateCopy( kCFAllocatorDefault, titlePtr );
                titleStr = CFStringCreateCopy( kCFAllocatorDefault, CFArrayGetValueAtIndex( titleArray, 0 ) );
            }
        } else {
            titleStr = CFStringCreateWithPascalString( NULL, titlePtr, kCFStringEncodingMacRoman );
        }
    }
#else
    if ( titlePtr ) {
        if ( cfTitle ) {
            titleStr = CFStringCreateCopy( kCFAllocatorDefault, titlePtr );
        } else {
            titleStr = CFStringCreateWithPascalString( NULL, titlePtr, kCFStringEncodingMacRoman );
        }
    }
#endif
    
    
    if ( c )                                                    // control already exists
    {
        if ( state == kFBParamMissing )
        {
            if ( IsControlActive( c ) ) state = kFBButtonStateActiveBtn; else state = kFBButtonStateGrayBtn;
        }
        if ( value == kFBParamMissing ) value = GetControl32BitValue( c );
        if ( min == kFBParamMissing ) min = GetControl32BitMinimum( c );
        if ( max == kFBParamMissing ) max = GetControl32BitMaximum( c );
        
        if ( titleStr ) ControlSetText( c, titleStr );
        if ( rectPtr ) SetControlBounds( c, rectPtr );
        
        SetControl32BitValue( c, value );
        SetControl32BitMinimum( c, min );
        SetControl32BitMaximum( c, max );
    }
    else                                                                        // new control
    {
        // prepare in case graphic control
        cbcInfo.contentType = kControlNoContent;
        cbcInfo.u.resID = value;
        
        if ( state == kFBParamMissing ) state = kFBButtonStateActiveBtn;
        
        
#if kSmartTabsRadioGroup
        switch ( type ) {
            case kControlRadioGroupProc:
                if ( value == kFBParamMissing ) value = kControlSizeNormal;
                break;
            default:
                if ( value == kFBParamMissing ) value = 1;
                break;
        }
#else
        if ( value == kFBParamMissing ) value = 1;
#endif
        
        if ( min == kFBParamMissing ) min = 0;
        if ( max == kFBParamMissing ) max = 1;
        if ( type == 0 ) type = kControlPushButtonProc;
        
        // slider info
        if ( type >= kControlSliderProc && type <= ( kControlSliderProc + 15 ) )
        {
            liveTracking = ( ( type & kControlSliderLiveFeedback ) != 0 );
            if ( type & kControlSliderHasTickMarks ) sliderNumTickMarks = value;
            if ( type & kControlSliderReverseDirection ) sliderOrientation = kControlSliderPointsUpOrLeft;
            if ( type & kControlSliderNonDirectional ) sliderOrientation = kControlSliderDoesNotPoint;
            type = kControlSliderProc;
        }
        
        // _useWFont clashes with non-directional slider
        if ( type != kControlSliderProc )
        {
            useWFont = ( ( type & 0x8 ) != 0 );
            if ( useWFont ) type = ( type & 0xFFFFFFF7 );
        }
        
        // bevel button info
        if ( type >= kControlBevelButtonSmallBevelProc && type <= ( kControlBevelButtonSmallBevelProc + 15 ) )
        {
            bevelThickness = type & ( kControlBevelButtonNormalBevel + kControlBevelButtonLargeBevel );
            bevelBehaviour = min & ( kControlBehaviorToggles + kControlBehaviorSticky );
            bevelMenuBehaviour = min & ( kControlBehaviorMultiValueMenu + kControlBehaviorOffsetContents + kControlBehaviorCommandMenu );
            bevelMenuPlacement = type & kControlBevelButtonMenuOnRight;
            type = kControlBevelButtonSmallBevelProc;
            contentType = min & 0x7f;
        }
        
        switch ( type ) {
            case kControlBevelButtonSmallBevelProc:
            {
                SetCBCInfo( contentType, max, &cbcInfo );
                CreateBevelButtonControl( w, rectPtr, titleStr, bevelThickness, bevelBehaviour, &cbcInfo, value, bevelMenuBehaviour, bevelMenuPlacement, &c );
            }
                break;
                //------------------------------------------------------------------
            case kControlChasingArrowsProc:
                CreateChasingArrowsControl( w, rectPtr, &c );
                break;
                //------------------------------------------------------------------
            case kControlCheckBoxProc:
            case kControlCheckBoxAutoToggleProc:
                CreateCheckBoxControl( w, rectPtr, titleStr, value, ( type == kControlCheckBoxAutoToggleProc ), &c );
                break;
                //------------------------------------------------------------------
            case kControlGroupBoxCheckBoxProc:
            case kControlGroupBoxSecondaryCheckBoxProc:
                CreateCheckGroupBoxControl( w, rectPtr, titleStr, value, ( type == kControlGroupBoxCheckBoxProc ), false, &c );
                break;
                //------------------------------------------------------------------
            case kControlClockTimeProc:
                CreateClockControl( w, rectPtr, kControlClockTypeHourMinute, value, &c );
                break;
            case kControlClockTimeSecondsProc:
                CreateClockControl( w, rectPtr, kControlClockTypeHourMinuteSecond, value, &c );
                break;
            case kControlClockDateProc:
                CreateClockControl( w, rectPtr, kControlClockTypeMonthDayYear, value, &c );
                break;
            case kControlClockMonthYearProc:
                CreateClockControl( w, rectPtr, kControlClockTypeMonthYear, value, &c );
                break;
                //------------------------------------------------------------------
            case kControlTriangleProc:
                CreateDisclosureTriangleControl( w, rectPtr, kControlDisclosureTrianglePointRight, titleStr, value, ( titleStr != NULL && CFStringGetLength( titleStr ) != 0 ), false, &c );
                break;
            case kControlTriangleAutoToggleProc:
                CreateDisclosureTriangleControl( w, rectPtr, kControlDisclosureTrianglePointRight, titleStr, value,  ( titleStr != NULL && CFStringGetLength( titleStr ) != 0 ), true, &c );
                break;
            case kControlTriangleLeftFacingProc:
                CreateDisclosureTriangleControl( w, rectPtr, kControlDisclosureTrianglePointLeft, titleStr, value,  ( titleStr != NULL && CFStringGetLength( titleStr ) != 0 ), false, &c );
                break;
            case kControlTriangleLeftFacingAutoToggleProc:
                CreateDisclosureTriangleControl( w, rectPtr, kControlDisclosureTrianglePointLeft, titleStr, value, ( titleStr != NULL && CFStringGetLength( titleStr ) != 0 ), true, &c );
                break;
                //------------------------------------------------------------------
            case kControlEditTextProc:
            case kControlEditTextPasswordProc:
                CreateEditTextControl( w, rectPtr, titleStr, ( type == kControlEditTextPasswordProc ), true, NULL, &c );
                break;
                //------------------------------------------------------------------
            case kControlEditUnicodeTextProc:
            case kControlEditUnicodeTextPasswordProc:
                CreateEditUnicodeTextControl( w, rectPtr, titleStr, ( type == kControlEditUnicodeTextPasswordProc ), NULL, &c );
                break;
                //------------------------------------------------------------------
            case kControlIconNoTrackProc:
                noTrack = true;
            case kControlIconProc:
                SetCBCInfo( kControlContentCIconRes, value, &cbcInfo );
                CreateIconControl( w, rectPtr, &cbcInfo, noTrack, &c );
                break;
                //------------------------------------------------------------------
            case kControlIconSuiteNoTrackProc:
                noTrack = true;
            case kControlIconSuiteProc:
                SetCBCInfo( kControlContentIconSuiteRes, value, &cbcInfo );
                CreateIconControl( w, rectPtr, &cbcInfo, noTrack, &c );
                break;
                //------------------------------------------------------------------
            case kControlIconRefNoTrackProc:
                noTrack = true;
            case kControlIconRefProc:
                SetCBCInfo( kControlContentIconRef, value, &cbcInfo );
                CreateIconControl( w, rectPtr, &cbcInfo, noTrack, &c );
                break;
                //------------------------------------------------------------------
            case kControlImageWellProc:
                SetCBCInfo( min, value, &cbcInfo );
                CreateImageWellControl( w, rectPtr, &cbcInfo, &c );
                break;
                //------------------------------------------------------------------
            case kControlLittleArrowsProc:
                CreateLittleArrowsControl( w, rectPtr, value, min, max, 1, &c );
                SetControlAction( c, LittleArrowsControlAction );
                break;
                //------------------------------------------------------------------
            case kControlListBoxProc:
                ldesH = (ldesRec**)GetResource( kControlListDescResType, value );
                if ( ldesH )
                {
                    CreateListBoxControl( w, rectPtr, FALSE, (*ldesH)->numberOfRows, (*ldesH)->numberOfColumns, (*ldesH)->hasHorizScroll, (*ldesH)->hasVertScroll, (*ldesH)->cellHeight, (*ldesH)->cellWidth, (*ldesH)->hasSizeBox, NULL, &c );
                }
                break;
                //------------------------------------------------------------------
            case kControlPictureNoTrackProc:
                noTrack = true;
            case kControlPictureProc:
                SetCBCInfo( kControlContentPictRes, value, &cbcInfo );
                CreatePictureControl( w, rectPtr, &cbcInfo, noTrack, &c );
                break;
                //------------------------------------------------------------------
            case kControlPlacardProc:
                CreatePlacardControl( w, rectPtr, &c );
                break;
                //------------------------------------------------------------------
            case kControlPopupArrowEastProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationEast, kControlPopupArrowSizeNormal, &c );
                break;
            case kControlPopupArrowWestProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationWest, kControlPopupArrowSizeNormal, &c );
                break;
            case kControlPopupArrowNorthProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationNorth, kControlPopupArrowSizeNormal, &c );
                break;
            case kControlPopupArrowSouthProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationSouth, kControlPopupArrowSizeNormal, &c );
                break;
            case kControlPopupArrowSmallEastProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationEast, kControlPopupArrowSizeSmall, &c );
                break;
            case kControlPopupArrowSmallWestProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationWest, kControlPopupArrowSizeSmall, &c );
                break;
            case kControlPopupArrowSmallNorthProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationNorth, kControlPopupArrowSizeSmall, &c );
                break;
            case kControlPopupArrowSmallSouthProc:
                CreatePopupArrowControl( w, rectPtr, kControlPopupArrowOrientationSouth, kControlPopupArrowSizeSmall, &c );
                break;
                //------------------------------------------------------------------
            case kControlPopupButtonProc:
                CreatePopupButtonControl( w, rectPtr, titleStr, min, ( max == -1 ), max, 0, 0, &c ); // 'min' is menuID
                break;
                //------------------------------------------------------------------
            case kControlGroupBoxPopupButtonProc:
            case kControlGroupBoxSecondaryPopupButtonProc:
                CreatePopupGroupBoxControl( w, rectPtr, titleStr, ( type == kControlGroupBoxPopupButtonProc ), min, ( max == -1 ), max, 0, 0, &c );
                break;
                //------------------------------------------------------------------
            case kControlProgressBarProc:
                CreateProgressBarControl( w, rectPtr, value, min, max, false, &c );
                break;
                //------------------------------------------------------------------
            case kControlPushButtonProc:
                CreatePushButtonControl( w, rectPtr, titleStr, &c );
                if ( state == kFBButtonStateMarkedBtn )
                {
                    isDefaultPushButton = true;
                    SetControlData( c, kControlEntireControl, kControlPushButtonDefaultTag, sizeof( Boolean ), &isDefaultPushButton );
                }
                break;
                //------------------------------------------------------------------
            case kControlPushButLeftIconProc:
                SetCBCInfo( kControlContentCIconRes, max, &cbcInfo );
                CreatePushButtonWithIconControl( w, rectPtr, titleStr, &cbcInfo, kControlPushButtonIconOnLeft, &c );
                break;
                //------------------------------------------------------------------
            case kControlPushButRightIconProc:
                SetCBCInfo( kControlContentCIconRes, max, &cbcInfo );
                CreatePushButtonWithIconControl( w, rectPtr, titleStr, &cbcInfo, kControlPushButtonIconOnRight, &c );
                break;
                //------------------------------------------------------------------
            case kControlRadioButtonProc:
            case kControlRadioButtonAutoToggleProc:
                CreateRadioButtonControl( w, rectPtr, titleStr, value, ( type == kControlRadioButtonAutoToggleProc ), &c );
                break;
                //------------------------------------------------------------------
            case kControlRadioGroupProc:
                
#if kSmartTabsRadioGroup
                
                if ( cfTitle ) {
                    if ( titlePtr ) {
                        if ( titleStr != NULL && titleArray == NULL ) {
                            titleArray = CFStringCreateArrayBySeparatingStrings( kCFAllocatorDefault, titleStr, CFSTR(";") );
                        }
                    }
                }
                c = RadioGroupCreate( w, value, min, max, titleArray, rectPtr );
                
#else
                CreateRadioGroupControl( w, rectPtr, &c );
#endif
                
                break;
                //------------------------------------------------------------------
            case kControlRelevanceBarProc:
                CreateRelevanceBarControl( w, rectPtr, value, min, max, &c );
                break;
                //------------------------------------------------------------------
            case kControlRoundButtonProc:                                        // bw:20080729
                CreateRoundButtonControl( w, rectPtr, kControlRoundButtonNormalSize, NULL, &c );
                break;
                //------------------------------------------------------------------
            case kControlScrollBarLiveProc:
                liveTracking = true;
            case kControlScrollBarProc:
                CreateScrollBarControl( w, rectPtr, value, min, max, 1, liveTracking, 0, &c );
                SetControlAction( c, ScrollbarControlAction );
                InstallControlEventHandler( c, (EventHandlerProcPtr)&ScrollbarClickHandler, 1, eventSpec, c, NULL );
                break;
                //------------------------------------------------------------------
                //case kControlScrollTextBoxProc:
                //case kControlScrollTextBoxAutoScrollProc:
                //break;
                //------------------------------------------------------------------
            case kControlSeparatorLineProc:
                CreateSeparatorControl( w, rectPtr, &c );
                break;
                //------------------------------------------------------------------
            case kControlSliderProc:
                CreateSliderControl( w, rectPtr, value, min, max, sliderOrientation, sliderNumTickMarks, liveTracking, 0, &c );
                if ( liveTracking )
                {
                    SetControlAction( c, SliderControlAction );
                    InstallControlEventHandler( c, (EventHandlerProcPtr)&SliderClickHandler, 1, eventSpec, c, NULL );
                }
                break;
                //------------------------------------------------------------------
            case kControlStaticTextProc:
                CreateStaticTextControl( w, rectPtr, titleStr, NULL, &c );
                break;
                //------------------------------------------------------------------
                
#if kSmartTabsRadioGroup
                
            case kControlTabLargeNorthProc:
            case kControlTabSmallNorthProc:
            case kControlTabLargeSouthProc:
            case kControlTabSmallSouthProc:
            case kControlTabLargeEastProc:
            case kControlTabSmallEastProc:
            case kControlTabLargeWestProc:
            case kControlTabSmallWestProc:
                if ( titlePtr ) {
                    if ( titleStr != NULL && titleArray == NULL ) {
                        titleArray = CFStringCreateArrayBySeparatingStrings( kCFAllocatorDefault, titleStr, CFSTR(";") );
                    }
                }
                c = TabsViewCreate( w, value, max, titleArray, rectPtr, type );
                break;
                
#else
                
            case kControlTabLargeNorthProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionNorth, max, 0, &c );
                break;
                
            case kControlTabSmallNorthProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionNorth, max, 0, &c );
                break;
                
            case kControlTabLargeSouthProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionSouth, max, 0, &c );
                break;
                
            case kControlTabSmallSouthProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionSouth, max, 0, &c );
                break;
                
            case kControlTabLargeEastProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionEast, max, 0, &c );
                break;
                
            case kControlTabSmallEastProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionEast, max, 0, &c );
                break;
                
            case kControlTabLargeWestProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeLarge, kControlTabDirectionWest, max, 0, &c );
                break;
                
            case kControlTabSmallWestProc:
                CreateTabsControl( w, rectPtr, kControlTabSizeSmall, kControlTabDirectionWest, max, 0, &c );
                break;
                //------------------------------------------------------------------
                
#endif  // kSmartTabsRadioGroup
                
            case kControlGroupBoxTextTitleProc:
            case kControlGroupBoxSecondaryTextTitleProc:
                CreateGroupBoxControl( w, rectPtr, titleStr, ( type == kControlGroupBoxTextTitleProc ), &c );
                break;
                //------------------------------------------------------------------
            case kControlUserPaneProc:
                CreateUserPaneControl( w, rectPtr, value, &c );
                break;
                //------------------------------------------------------------------
            case kControlWindowHeaderProc:
            case kControlWindowListViewHeaderProc:
                CreateWindowHeaderControl( w, rectPtr, ( type == kControlWindowListViewHeaderProc ), &c );
                break;
                //------------------------------------------------------------------
            default:
                break;
        }
        
        SetControlID( c, &cID );                                                // was HIViewSetID( c, &cID );    // OS X 10.4
        
        if ( useWFont )
        {
            cfs.flags = kControlUseFontMask | kControlUseSizeMask | kControlUseFaceMask;
            cfs.font = gTextFont;
            cfs.size = gTextSize;
            cfs.style = gTextFace;
            SetControlFontStyle( c, &cfs );
        }
    }
    
    if ( backupState != kFBParamMissing )
    {
        switch ( state )
        {
            case kFBButtonStateGrayBtn:
                DeactivateControl( c );
                break;
                
            case kFBButtonStateActiveBtn:
                if ( backupValue == kFBParamMissing )
                {
                    GetControlKind( c, &cKind );
                    if ( cKind.kind == kControlKindRadioButton || cKind.kind == kControlKindCheckBox ) SetControl32BitValue( c, 0 );
                }
                ActivateControl( c );
                HiliteControl( c, 0 );        // bw:20080527 fix #359
                break;
                
            case kFBButtonStateMarkedBtn:
                GetControlKind( c, &cKind );
                switch ( cKind.kind )
            {
                case kControlKindRadioButton:
                case kControlKindCheckBox:
                    if ( backupValue == kFBParamMissing ) SetControl32BitValue( c, 1 );
                    break;
                    
                default:
                    HiliteControl( c, kControlButtonPart );
                    break;
            }
                ActivateControl( c );
                break;
        }
        
    }
    
    SetControlVisibility( c, isVisible, true );
    if ( titleStr ) CFRelease( titleStr );
    
#if kSmartTabsRadioGroup
    if ( titleArray ) CFRelease( titleArray );
#endif
    
    return c;
}


/*
 FBButton
 
 Bugs:
 - _CDEFBaseID not handled
 */
HIViewRef FBButton( long id, long state, void *titlePtr, Rect *rectPtr, long type, Boolean cfTitle )
{
    switch ( type ) {
        case kFBPush:
            type = kControlPushButtonProc;
            break;
        case kFBCheckbox:
            type = kControlCheckBoxProc;
            break;
        case kFBRadio:
            type = kControlRadioButtonProc;
            break;
        case kFBShadow:
            type = kControlPushButtonProc;
            if ( state == kFBButtonStateActiveBtn ) state = kFBButtonStateMarkedBtn;
            break;
        default:
            break;
    }
    return FBAppearanceButton( id, state, kFBParamMissing, kFBParamMissing, kFBParamMissing, titlePtr, rectPtr, type, cfTitle );
}

#else


#endif // kCFAppWndBtn



FBBoolean FBCanGrow( WindowRef w)
{
    Boolean         canGrow = false;
    WindowAttributes  wa = 0;
    
    if ( GetWindowAttributes( w, &wa ) == noErr )
    {
        if ( wa & kWindowResizableAttribute ) canGrow = true;
    }
    return canGrow;
}

void GetScrlRectFB( long  scrlType, Rect* scrlRect, Rect* portRect, FBBoolean  canGrow, WindowClass  wc )
{
    Point             pt;
    long              size;
    short             keep;
    
    pt.h = 0;
    pt.v = 0;
    if ( wc == kFloatingWindowClass )
    {
        GetThemeMetric( 1, &size );
    }
    else
    {
        GetThemeMetric( 0, &size );
    }
    
    if ( scrlType == 2 )
    {
        keep = scrlRect->top;
        BlockMoveData( portRect, scrlRect, 8 );
        scrlRect->top = keep;
        scrlRect->left = scrlRect->right - size;
        if ( canGrow ) scrlRect->bottom = scrlRect->bottom - size;
        scrlRect->bottom += 1;
        scrlRect->right += 1;
    }
    else  if ( scrlType == 3 )
    {
        keep = scrlRect->left;
        BlockMoveData( portRect, scrlRect, 8 );
        scrlRect->left = keep;
        scrlRect->top = scrlRect->bottom - size;
        if ( canGrow ) scrlRect->right = scrlRect->right - size;
        scrlRect->right += 1;
        scrlRect->bottom += 1;
    }
}

HIViewRef GetFBWindowScrlH( WindowRef w, long scrollType )
{
    HIViewRef c = NULL;
    
    if ( scrollType == kFBScrollVert )
    {
        GetWindowProperty( w, kFBtoCAppSignature, kFBWindowVScrlProperty, sizeof( c ), 0, &c );
    }
    else // _scrollHorz
    {
        GetWindowProperty( w, kFBtoCAppSignature, kFBWindowHScrlProperty, sizeof( c ), 0, &c );
    }
    return c;
}


void FBResizedWindow( WindowRef  w )
{
    ControlRef        c;
    Rect              r, oldR;
    short             wdth, height;
    
    GetWindowPortBounds( w, &r );
    c = GetFBWindowScrlH( w, kFBScrollVert );
    if ( c )
    {
        GetControlBounds( c, &oldR );
        wdth = oldR.right - oldR.left;
        MoveControl( c, r.right - wdth + 1, oldR.top );
        SizeControl( c, wdth, r.bottom - oldR.top - wdth + 2 );
    }
    
    c = GetFBWindowScrlH( w, kFBScrollHorz );
    if ( c )
    {
        GetControlBounds( c, &oldR );
        height = oldR.bottom - oldR.top;
        MoveControl( c, oldR.left, r.bottom - height + 1 );
        SizeControl( c, r.right - oldR.left - height + 2, height );
    }
}


#if 1   // kCFAppWndBtn

void FBScrollButton( long id, long current, long min, long max, long page, Rect *rectPtr, long type )
{
    HIViewRef c;
    Rect              r;
    Rect              portRect;
    WindowRef         w;
    WindowClass          wc = 0;
    
    w = FBGetCurOutputWindow();
    GetWindowPortBounds( w, &portRect );
#if 0
    r = portRect;
#else
    if ( rectPtr )
    {
        BlockMoveData( rectPtr, &r, sizeof( Rect ) );
    }
    else
    {
        r = portRect;
    }
#endif
    GetWindowClass( w, &wc );
    
    if ( type == kFBScrollVert )
    {
        type = 2;
        GetScrlRectFB( type, &r, &portRect, FBCanGrow( w ), wc );
        rectPtr = &r;
        c = FBAppearanceButton( abs( id ), kFBParamMissing, current, min, max, 0 /*no title*/, rectPtr, kControlScrollBarLiveProc, false );
        SetWindowProperty( w, kFBtoCAppSignature, kFBWindowVScrlProperty, sizeof( c ), &c );
    }
    else  if ( type == kFBScrollHorz )
    {
        type = 3;
        GetScrlRectFB( type, &r, &portRect, FBCanGrow( w ), wc );
        rectPtr = &r;
        c = FBAppearanceButton( abs( id ), kFBParamMissing, current, min, max, 0 /*no title*/, rectPtr, kControlScrollBarLiveProc, false );
        SetWindowProperty( w, kFBtoCAppSignature, kFBWindowHScrlProperty, sizeof( c ), &c );
    }
    else                // _scrollOther or kFBParamMissing
    {
        c = FBAppearanceButton( abs( id ), kFBParamMissing, current, min, max, 0 /*no title*/, rectPtr, kControlScrollBarLiveProc, false );
    }
    
    if ( c )
    {
        if ( page != kFBParamMissing ) SetControlViewSize( c, page );
    }
}

#else



#endif  // kCFAppWndBtn



#endif  /* !__LP64__ */


///////////////////////////////////////////////////
//          print to window/file                 //
///////////////////////////////////////////////////

long            gFBWidth = -1; // default -1 (_textWrap)
long            gFBWidthLprint = -1; // default -1 (_textWrap)
long            gFBRoute;
#define			kFBBufferCount	6
#define			kToBuffer		1000


void PrintCR()
{
#if !__LP64__
    CGrafPtr          port;
    FontInfo          fi;
    RgnHandle         tempRgn;
    Rect              r;
    Point             tempPoint;
    short             lineHeight;
    short             portBottom;
    short             portRight;
#endif  /* !__LP64__ */
    
    if ( gFBRoute == 0 || gFBRoute == 128 ) // _toScreen or _toPrinter
    {
#if !__LP64__
        FBCheckForPageFeed( -1 );
        if ( gFBRoute == 128 ) gFBPagefn++; // _toPrinter
        FBMakeSureItsAWindow();
        GetFontInfo( &fi );
        lineHeight = fi.ascent + fi.descent + fi.leading;
        GetPen( &tempPoint );
        tempPoint.v += lineHeight;
        GetPort( &port );
        GetPortBounds( port, &r );
        portBottom = r.bottom;
        portRight = r.right;
        
        // 20080127
        WindowRef         outputWnd;
        ControlRef        c;
        Rect              scrollRect;
        outputWnd = FBGetCurOutputWindow();
        c = GetFBWindowScrlH( outputWnd, kFBScrollVert );
        if ( c )
        {
            GetControlBounds( c, &scrollRect );
            portRight -= (scrollRect.right - scrollRect.left - 1);
        }
        c = GetFBWindowScrlH( outputWnd, kFBScrollHorz );
        if ( c )
        {
            GetControlBounds( c, &scrollRect );
            portBottom -= (scrollRect.bottom - scrollRect.top - 1);
        }
        // 20080127
        
        
        if ( tempPoint.v + fi.descent >= portBottom )
        {
            tempPoint.v -= lineHeight;
            SetRect( &r, 0, 0, portRight, portBottom );
            tempRgn = NewRgn();
            ScrollRect( &r, 0, -lineHeight, tempRgn );
            DisposeRgn( tempRgn );
        }
        MoveTo( 4, tempPoint.v );
#endif  /* !__LP64__ */
    }
    else if ( gFBRoute >= kToBuffer )
    {
        FBAddCharacterToBuffer( 13 );
    }
    else // gFBRoute 1 to 127 and 129 to 255 represent files
    {
        FBFilePrintCR();
    }
}


#if !__LP64__

long FBPortWidth( void )
{
    CGrafPtr    port;
    Rect        r;
    WindowRef    outputWnd;
    ControlRef    c;
    Rect        scrollRect;
    
    GetPort( &port );
    GetPortBounds( port, &r );
    outputWnd = FBGetCurOutputWindow();
    c = GetFBWindowScrlH( outputWnd, kFBScrollVert );
    if ( c )
    {
        GetControlBounds( c, &scrollRect );
        r.right -= (scrollRect.right - scrollRect.left - 1);
    }
    return r.right - r.left;
}


void FBDrawString( Str255  string$_p, short  maxWidth )
{
    Str255 string$;  PSstrcpy( string$, string$_p );
    gFBStk--;
    Point             tempPoint;
    short             count;
    short             theWidth;
    
    if ( maxWidth >= 0 )
    {
        if ( maxWidth == 0 )    // width [lprint] 0  undocumented !!!:rp:20080109
        {
            theWidth = FBPortWidth() - 4;
        }
        else                    // width [lprint] numChars
        {
            theWidth = maxWidth*CharWidth( '8' );
        }
        GetPen( &tempPoint );
        if ( StringWidth( string$ ) + tempPoint.h > theWidth )
        {
            count = 1;
            do
            {
                if ( CharWidth( string$[count] ) + tempPoint.h > theWidth )
                {
                    PrintCR();
                }
                DrawChar( string$[count] );
                GetPen( &tempPoint );
                count += 1;
            }
            while ( count <= string$[0] );
        }
        else
        {
            DrawString( string$ );
        }
    }
    else
    {
        if ( maxWidth == -1 )    // width [lprint] _textWrap
        {
            GetPen( &tempPoint );
            theWidth = FBPortWidth() - tempPoint.h;
            count = string$[0];
            while ( count > 0  && TextWidth( string$, 1, count ) >= theWidth )
            {
                count--;
            }
            DrawText( string$, 1, count );
            if ( count < string$[0] )
            {
                PrintCR();
                DrawText( string$, count + 1, string$[0] - count );
            }
        }
        else                // width [lprint] _noTextWrap
        {
            DrawString( string$ );
        }
    }
}

/*
 call PrintCR() for <cr> and FBDrawString() for all other characters
 */
void FBLimitDrawString( Str255  string_p$, short  maxWidth )
{
    Str255 s;  PSstrcpy( s, string_p$ );
    Str255            t;
    long              x;
    
    if ( s[0] == 1 && s[1] == 13 ) // <cr> only
    {
        PrintCR();
    }
    else // all other strings
    {
        do // if s contains a <cr>, process the stuff before the <cr>, then everything that follows
        {
            x = PSinstr( 1, PSstrcpy( STACK_PUSH(), s ), PSstrcpy( STACK_PUSH(), PSstring( 1, 13 ) ) );
            if ( x )
            {
                PSstrcpy( t, s );
                PSstrcpy( s, PSleft( PSstrcpy( STACK_PUSH(), t ), x - 1 ) ); // before the <cr>
                FBDrawString( PSstrcpy( STACK_PUSH(), s ), maxWidth );
                PSstrcpy( s, PSmid( PSstrcpy( STACK_PUSH(), t ), x + 1, 255 ) ); // after the <cr>
                if ( s[0] ) { PrintCR(); }
            }
        }
        while ( x );
        FBDrawString( PSstrcpy( STACK_PUSH(), s ), maxWidth );
    }
}

#endif  /* !__LP64__ */


void FBPrintString( Str255  s ){
    if ( gFBRoute == 0 )    {    // _toScreen
#if !__LP64__
        FBMakeSureItsAWindow();
        FBLimitDrawString( s, gFBWidth );
        FBFlushWindowBuffer( -3 );
#endif  /* !__LP64__ */
    } else if ( gFBRoute == 128    )  { // _toPrinter
        
#if !__LP64__
        FBCheckForPageFeed( false );
        FBLimitDrawString( s, gFBWidthLprint );
#endif  /* !__LP64__ */
    } else if ( gFBRoute >= kToBuffer ) { // _toBuffer
        FBAddStringToBuffer( PSstrcpy( STACK_PUSH(), s ) );
    }else {                        // fileID
        FBFilePrintString( PSstrcpy( STACK_PUSH(), s ) );
    }
    --gFBStk;
}




const Boolean         gNumberLeadingSpace = true;

void FBAddLeadingSpace( char * sPtr )
{
    char *             sPtrM;
    if ( gNumberLeadingSpace )
    {
        if ( *(sPtr + 1) != '-'  )
        {
            sPtrM = sPtr + *sPtr;
            *sPtr = *sPtr + 1;
            while ( sPtrM > sPtr )
            {
                *(sPtrM + 1) = *sPtrM;
                sPtrM--;
            }
            *(sPtr + 1) = ' ';
        }
    }
}


void PrintLong( const SInt64  theNumber )
{
    Str255  string;
    
    string[0] = sprintf( (char*)&string + 1, "%lld", theNumber);
    FBAddLeadingSpace( (char*)&string );
    FBPrintString( PSstrcpy( STACK_PUSH(), string ) );
}


StringPtr FBFloat2String( const double  theNum, long  style, long  digits )
{
    // adapted from FBFloat2String in Subs Compiler.incl
    // style == 1, digits == 10 on entry
    gFBStk++;
    short             fpexp;
    UInt8             nLen = 0;
    Boolean           keepZeros = false;
    Str255            theStr = {};
    decform           myDecForm;
    decimal           myDecimalRec;
    
    if ( digits < 0 ) digits = 0;
    
#if TARGET_RT_LITTLE_ENDIAN
    fpexp =  (*(signed char*)((void*)&theNum + 7) << 8) + *(unsigned char*)((void*)&theNum + 6);
#else
    fpexp = *(short*)&theNum;
#endif
    fpexp = (fpexp & 0x7FF0) >> 4;
    if ( fpexp ) fpexp -= 0x400;
    
    if ( style < 0 ) // 'using'  Note: we never call FBFloat2String for this; instead we call PSusing() -
    {
        style = style + 2; // -1 normal --> FIXEDDECIMAL, -2 --> FLOATDECIMAL
        keepZeros = true;
    }
    else
    {
        if ( fpexp < -8 ) // < .01
        {
            style = 0;
            digits--;
        }
        if ( fpexp >= 31 ) // > 9999999999
        {
            style = 0;
            digits--;
        }
        if ( style && (fpexp > 0) )
        {
            if ( digits < 1 ) digits = 1; //need at least 1 past dp
        }
        if ( style == FIXEDDECIMAL )
        {
            digits -= (fpexp*12)/39;
            if ( digits < 1 ) digits = 1;
        }
    }
    
    //fprintf( stderr, "FBFloat2String1 %g fpexp %d style %d digits %d  keepzeros %d\n", theNum,(int)fpexp, (int)style, (int)digits, (int)keepZeros );
    myDecForm.style = style;  // FLOATDECIMAL 0   FIXEDDECIMAL  1
    myDecForm.digits = digits;
    num2dec( &myDecForm, theNum, &myDecimalRec );
    if ( keepZeros == false && style == FLOATDECIMAL )
    {
        nLen = myDecimalRec.sig.length;
        while ( myDecimalRec.sig.text[nLen - 1] == '0' )
        {
            nLen--;
            myDecForm.digits--;
            myDecimalRec.exp++;
        }
        myDecimalRec.sig.length = nLen;
        myDecimalRec.sig.text[nLen] = 0;
    }
    
    dec2str( &myDecForm, &myDecimalRec, (void*)&theStr + 1 );
    
    UInt8             plen = 1;
    while ( theStr[plen] ) { plen++; }
    *(char*)(&theStr) = plen - 1;
    // theStr now a proper pascal string
    nLen = theStr[0];
    if ( keepZeros == false && style == FIXEDDECIMAL )
    {
        while ( *(char*)((void*)&theStr + nLen) == '0' ) { nLen--; } //remove trailing '0's
        if ( *(char*)((void*)&theStr + nLen) == '.' ) nLen--; // remove trailing '.'
        *(char*)(&theStr) = nLen;
        FBAddLeadingSpace( (char*)&theStr );
    }
    
    gFBStk--;
    return PSstrcpy( gReturnedString, theStr );
}


void PrintFloat( const double  d )
{
    FBPrintString( PSstrcpy( STACK_PUSH(), FBFloat2String( d, FIXEDDECIMAL, gFBFloatMaxDigits ) ) );
}



void PrintTab( long tabs )
{
    if ( gFBRoute == 0 || gFBRoute == 128 ) // _toScreen or _toPrinter
    {
#if !__LP64__
        short             tabwidth;
        Point             tempPoint;
        
        //        if ( tabs < 1 ) tabs = 1;
        tabwidth = CharWidth( '0' ) * gPSDefTabStops;
        GetPen( &tempPoint );
        MoveTo( (tempPoint.h / tabwidth + 1) * tabwidth, tempPoint.v );
#endif  /* !__LP64__ */
    }
    else if ( gFBRoute >= kToBuffer )
    {
        FBAddCharacterToBuffer( 44 );  // _","
    }
    else // gFBRoute 1 to 127 and 129 to 255 represent files
    {
        FBFilePrintTab();
    }
}



void PrintSpc( long spc )
{
    while ( spc > 0 )
    {
        FBPrintString( PSstrcpy( STACK_PUSH(), "\p " ) );
        spc--;
    }
}



void PrintContainerWnd( void  **ctnrVarPtr ) {
    SInt64            ctnrLength, p;
    UInt8             c;
    Str255            tempPStr;
    void             *ctnrPointer;
    
    ctnrPointer = *ctnrVarPtr;
    if ( ctnrPointer ) {
        ctnrLength = CtnrRequestedSize(ctnrPointer);
        p = 0;
        tempPStr[0] = 0;
        while ( p < ctnrLength ) {
            if ( tempPStr[0] == 255 ) {
                FBPrintString( PSstrcpy( STACK_PUSH(), tempPStr ) );
                tempPStr[0] = 0;
            }
            c = *(UInt8*)(ctnrPointer + p);
            p++;
            if ( c == 13 ){
                FBPrintString( PSstrcpy( STACK_PUSH(), tempPStr ) );
                PrintCR();
                tempPStr[0] = 0;
            } else {
                tempPStr[++tempPStr[0]] = c;
            }
        }
        FBPrintString( PSstrcpy( STACK_PUSH(), tempPStr ) );
    }
}




///////////////////////////////////////////////////
//        support for route _toBuffer            //
///////////////////////////////////////////////////

#if NO_SUFFIX_ON_ARRAY_NAMES
Handle        gFBBuffer[kFBBufferCount];

char* FBCheckBuffer( long  bytes2add )
{
    Handle            hnld;
    long              oldSize;
    hnld = gFBBuffer[gFBRoute -  kToBuffer];
    if ( hnld == 0 )
    {
        hnld = NewHandle( bytes2add );
        gFBBuffer[gFBRoute -  kToBuffer] = hnld;
        oldSize = 0;
    }
    else
    {
        oldSize = GetHandleSize( hnld );
        SetHandleSize( hnld, oldSize + bytes2add );
    }
    return *hnld + oldSize;
}

#else

Handle        gFBBuffer_A[kFBBufferCount];

char* FBCheckBuffer( long  bytes2add )
{
    Handle            hnld;
    long              oldSize;
    hnld = gFBBuffer_A[gFBRoute -  kToBuffer];
    if ( hnld == 0 )
    {
        hnld = NewHandle( bytes2add );
        gFBBuffer_A[gFBRoute -  kToBuffer] = hnld;
        oldSize = 0;
    }
    else
    {
        oldSize = GetHandleSize( hnld );
        SetHandleSize( hnld, oldSize + bytes2add );
    }
    return *hnld + oldSize;
}
#endif /* NO_SUFFIX_ON_ARRAY_NAMES */



void FBAddCharacterToBuffer( short  c )
{
    char*              tempPtr;
    tempPtr = FBCheckBuffer( 1 );
    *tempPtr = c;
}


void FBAddStringToBuffer( Str255  string$_p )
{
    Str255 string$;  PSstrcpy( string$, string$_p );
    --gFBStk;
    char*              tempPtr;
    tempPtr = FBCheckBuffer( string$[0] );
    BlockMoveData( &string$[1], tempPtr, string$[0] );
}



#if !__LP64__


///////////////////////////////////////////////////
//       used if _debugPrintStringStackLevel     //
///////////////////////////////////////////////////
void DebugPrintStackLevelWnd()
{
    DrawString( "\pgFBStk = " );
    PrintLong( gFBStk );
    PrintCR();
}



///////////////////////////////////////////////////
//      FB graphics (QuickDraw) runtime          //
///////////////////////////////////////////////////

short             gFBRatioX = 128;
short             gFBRatioY = 128;
short             gGrafLastX, gGrafLastY;

long Csrlin()
{
    long              curLine;
    Point             currPoint;
    FontInfo          info;
    
    GetFontInfo( &info );
    GetPen( &currPoint );
    curLine = currPoint.v / (info.ascent + info.descent + info.leading);
    return curLine;
}

#endif  /* !__LP64__ */


long Pos( long device )
{
    long        vPos = 0;
    
    if ( device == 0 || device == 1 ) //_printDev or _anyDev
    {
#if !__LP64__
        Point        currPoint;
        GetPen( &currPoint );
        vPos = currPoint.h / CharWidth( '0' );
#endif  /* !__LP64__ */
    }
    else if ( device >= kToBuffer )
    {
        Handle      buffer;
#if NO_SUFFIX_ON_ARRAY_NAMES
        buffer = gFBBuffer[device - kToBuffer];
#else
        buffer = gFBBuffer_A[device - kToBuffer];
#endif
        vPos = GetHandleSize( buffer );
    }
    else
    {
        vPos = FBFileGetPos();
    }
    //20080131
    return vPos;
}

#if !__LP64__

void FBLongColor( long  blue, long  green, long  red, long  backGround )
{
    RGBColor          theRGB;
    theRGB.red   = red;
    theRGB.green = green;
    theRGB.blue  = blue;
    if ( backGround )
    {
        RGBForeColor( &theRGB );
    }
    else
    {
        RGBBackColor( &theRGB );
    }
}


void FBColor( long  theColor )
{
    long         color;
    color =  33;
    theColor = theColor & ( 7 );
    if ( theColor ==  6 )
    {
        color =  205;
    }
    else if ( theColor ==  4 )
    {
        color =  409;
    }
    else if ( theColor ==  3 )
    {
        color =  273;
    }
    else if ( theColor ==  2 )
    {
        color =  341;
    }
    else if ( theColor ==  1 )
    {
        color =  69;
    }
    else if ( theColor ==  0 )
    {
        color =  30;
    }
    else if ( theColor ==  5 )
    {
        color =  137;
    }
    ForeColor( color );
}


void FBPen( long  PenX, long  PenY, long  PenVis, long  ModeNum, long  PatNum )
{
    Pattern           pat;
    CGrafPtr          Port;
    GetPort( &Port );
    if ( PenX >= 0 )
    {
        if ( PenY < 0 )
        {
            PenY = PenX;
        }
        PenSize( PenX, PenY );
    }
    if ( PenVis == 0 )
    {
        if ( GetPortPenVisibility( Port ) >= 0 )
        {
            HidePen();
        }
    }
    else if ( PenVis == 1 )
    {
        if ( GetPortPenVisibility( Port ) < 0 )
        {
            ShowPen();
        }
    }
    if ( ModeNum >= 0 )
    {
        PenMode( ModeNum );
    }
    if ( PatNum >= 0 )
    {
        GetIndPattern( &pat, 0, PatNum + 1 );
        PenPat( &pat );
    }
}


void FBSaveMaxPixel( long  x, long  y )
{
    gGrafLastX = x;
    gGrafLastY = y;
}


void FBPlot( long  x, long  y )
{
    FBSaveMaxPixel( x, y );
    MoveTo( x, y );
    LineTo( x, y );
}


void FBPlotTo( long  x, long  y )
{
    MoveTo( gGrafLastX, gGrafLastY );
    FBSaveMaxPixel( x, y );
    LineTo( x, y );
}


void FBBox( long  l, long  t, long  r, long  b, long  fill )
{
    Rect             rr;
    if ( t > b )
    {
        FBSwap( &t, &b, 4 );
    }
    if ( l > r )
    {
        FBSwap( &l, &r, 4 );
    }
    SetRect( &rr, l, t, r, b );
    FBSaveMaxPixel( r, b );
    if ( fill )
    {
        PaintRect( &rr );
    }
    else
    {
        FrameRect( &rr );
    }
}


void FBCircle( long  h, long  v, long  Radius, long  Start, long  EndAngle, long  Flags )
{
    Rect              t;
    long              StrtDeg;
    long              EndDeg;
    double            temp;
    Radius = abs( Radius );
    SetRect( &t, h - Radius*gFBRatioX/128, v - Radius*gFBRatioY/128, h + Radius*gFBRatioX/128, v + Radius*gFBRatioY/128 );
    FBSaveMaxPixel( t.right, t.bottom );
    if ( Flags & ( 0xFFFF )  )
    {
        Start = 64 - Start;
        StrtDeg = (Start*360 + 128)/256;
        EndAngle = Start - EndAngle;
        EndDeg = (EndAngle*360 + 128)/256;
        if ( Flags & ( 0xFFFF0000 )  )
        {
            if ( Flags & ( 2 )  )
            {
                FrameArc( &t, StrtDeg, EndDeg - StrtDeg );
            }
            else
            {
                PaintArc( &t, StrtDeg, EndDeg - StrtDeg );
            }
        }
        else
        {
            if ( Flags & ( 2 )  )
            {
                FrameArc( &t, StrtDeg, EndDeg - StrtDeg );
            }
            else
            {
                temp = 180.0/3.14159265359;
                MoveTo( h, v );
                LineTo( h + sin( StrtDeg/temp )*(Radius*gFBRatioX/128 - 1), v - cos( StrtDeg/temp )*(Radius*gFBRatioY/128 - 1) );
                FrameArc( &t, StrtDeg, EndDeg - StrtDeg );
                MoveTo( h + sin( EndDeg/temp )*(Radius*gFBRatioX/128 - 1), v - cos( EndDeg/temp )*(Radius*gFBRatioY/128 - 1) );
                LineTo( h, v );
            }
        }
    }
    else
    {
        if ( Flags & ( 0xFFFF0000 )  )
        {
            PaintOval( &t );
        }
        else
        {
            FrameOval( &t );
        }
    }
    MoveTo( h, v );
    gGrafLastX = h;
    gGrafLastY = v;
}


void PrintMovePixel( long  x, long  y )
{
    FBSaveMaxPixel( x, y );
    MoveTo( x, y );
}


void FBRatio( long  x, long  y )
{
    gFBRatioX = x + 128;
    gFBRatioY = y + 128;
}


void PrintMoveChar( long  x, long  y )
{
    FontInfo          fontInfo;
    FBSaveMaxPixel( x + 20, y + 4 );
    GetFontInfo( &fontInfo );
    MoveTo( x*CharWidth( '0' ), (y + 1)*(fontInfo.ascent + fontInfo.descent + fontInfo.leading) );
}


void FBFill( long  x, long  y )
{
    CGrafPtr          bigPort;
    BitMap            pm;
    Point             pp;
    
    pp.h = x;
    pp.v = y;
    GetPort( &bigPort );
    GetPortBounds( bigPort, &pm.bounds );
    if ( PtInRect( pp, &pm.bounds ) )
    {
        pm.rowBytes = ((pm.bounds.right - pm.bounds.left + 31) >> 5) << 2;
        pm.baseAddr = NewPtrClear( pm.rowBytes*(pm.bounds.bottom - pm.bounds.top + 1) );
        if ( pm.baseAddr )
        {
            SeedCFill( GetPortBitMapForCopyBits( bigPort ), &pm, &pm.bounds, &pm.bounds, pp.h, pp.v, 0, 0 );
            CopyBits( &pm, GetPortBitMapForCopyBits( bigPort ), &pm.bounds, &pm.bounds,  1, 0 );
            DisposePtr( pm.baseAddr );
        }
    }
}




void Cls( )
{
    CGrafPtr          port;
    Rect              r;
    WindowRef          w;
    FontInfo          fi;
    
    FBMakeSureItsAWindow();
    GetPort( &port );
    GetPortBounds( port, &r );
    EraseRect( &r );
    w = GetWindowFromPort( port );
    DrawControls( w );
    FBFlushWindowBuffer( 0 );
    GetFontInfo( &fi );
    MoveTo( 4, fi.ascent + fi.descent + fi.leading );
}



long ClsLine()
{
    CGrafPtr          port;
    FontInfo          fi;
    Rect              r;
    WindowRef         w;
    Point             currPoint;
    long              wNum;
    RgnHandle         clipRgn;
    
    FBMakeSureItsAWindow();    // MN
    GetFontInfo( &fi );
    GetPen( &currPoint );
    GetPort( &port );
    GetPortBounds( port, &r );
    SetRect( &r, currPoint.h, currPoint.v - fi.ascent, r.right, currPoint.v + fi.descent );
    EraseRect( &r );
    FBFlushWindowBuffer( 0 );
    
    w = GetWindowFromPort( port );
    wNum = FBGetWndNumber( w );
    if ( wNum )
    {
        clipRgn = NewRgn();
        GetClip( clipRgn );
        // DrawAllControlsFB( w );
        SetClip( clipRgn );
        DisposeRgn( clipRgn );
    }
    MoveTo( currPoint.h, currPoint.v );
    return 0;
}

long ClsPage()
{
    CGrafPtr          port;
    FontInfo          fi;
    Rect              r;
    WindowRef         w;
    Point             currPoint;
    long              wNum;
    RgnHandle         clipRgn;
    
    FBMakeSureItsAWindow();    // MN
    GetFontInfo( &fi );
    GetPen( &currPoint );
    GetPort( &port );
    GetPortBounds( port, &r );
    SetRect( &r, currPoint.h, currPoint.v - fi.ascent, r.right, currPoint.v + fi.descent );
    EraseRect( &r );
    GetPortBounds( port, &r );
    SetRect( &r, 0, currPoint.v + fi.descent, r.right, r.bottom );
    EraseRect( &r );
    FBFlushWindowBuffer( 0 );
    
    w = GetWindowFromPort( port );
    wNum = FBGetWndNumber( w );
    if ( wNum )
    {
        clipRgn = NewRgn();
        GetClip( clipRgn );
        // DrawAllControlsFB( w );
        SetClip( clipRgn );
        DisposeRgn( clipRgn );
    }
    MoveTo( currPoint.h, currPoint.v );
    return 0;
}




///////////////////////////////////////////////////
//                FlushWindowBuffer              //
///////////////////////////////////////////////////

void FBFlushWindowBuffer( long  wNum )
{
    /*
     To avoid performance problems from coalesced updates  http://developer.apple.com/library/mac/#technotes/tn2005/tn2133.html
     this does nothing if we are called by FBPrintString() and the call is too soon (< 2 ticks) after the previous call.
     wNum == 0 means front window.
     */
    static Boolean        sFBNoAutoFlush = false;
    static UInt32        sLastFlushTicks = 0;
    WindowRef            w;
    CGrafPtr            port;
    
    if ( wNum ==  -1 ) // _FBAutoFlushOff
    {
        sFBNoAutoFlush = true;
        return;
    }
    if ( wNum == -2 ) // _FBAutoFlushOn
    {
        sFBNoAutoFlush = false;
        return;
    }
    
    if ( wNum != -3 ) sLastFlushTicks = 0; // #575  force update unless we are called by FBPrintString()
    
    if ( sFBNoAutoFlush == false  &&  wNum == -3 ) // wNum == -3 only when called by FBPrintString()
    {
        wNum = 0;
    }
    
    
    if ( sFBNoAutoFlush == false  ||  wNum >= 0  )
    {
        if ( wNum )
        {
            w = FBWindowFindByID( wNum );
            if ( w == 0 ) return;
            port = GetWindowPort( w );
        }
        else
        {
            GetPort( &port );
        }
        if ( QDIsPortBuffered( port ) && (TickCount() - sLastFlushTicks) > 1 )
        {
            QDFlushPortBuffer( port, 0 );
            sLastFlushTicks = TickCount();
        }
    }
}


///////////////////////////////////////////////////
//                    Printing                   //
///////////////////////////////////////////////////

short             gFBPagefn = 1;
short             gFBStdFont;
short             gFBStdSize;
PMPrintSession    gFBPrintSession;
PMPrintSettings   gFBPrintSettings;
PMPageFormat      gFBPageFormat;
long              gFBFirstPrintedPage;
long              gFBLastPrintedPage;
Handle            gFBPRHandle;
CGrafPtr          gFBPRPort;
Rect              gFBPrintRect;
FBBoolean         gFBPRCancel;

void FBEndSession()
{
    if ( gFBPrintSession )
    {
        PMRelease( gFBPrintSession );
        gFBPrintSession =  NULL;
    }
}


OSStatus FBBeginSession()
{
    OSStatus          rslt;
    
    if ( gFBPrintSession == NULL )
    {
        rslt = PMCreateSession( &gFBPrintSession );
    }
    else
    {
        rslt = noErr;
    }
    
    if ( rslt ==  noErr )
    {
        if ( gFBPageFormat == NULL )
        {
            rslt = PMCreatePageFormat( &gFBPageFormat );
            if ( rslt ==  noErr )
            {
                rslt = PMSessionDefaultPageFormat( gFBPrintSession, gFBPageFormat );
            }
        }
        if ( gFBPrintSettings == NULL )
        {
            rslt = PMCreatePrintSettings( &gFBPrintSettings );
            if ( rslt ==  noErr )
            {
                rslt = PMSessionDefaultPrintSettings( gFBPrintSession, gFBPrintSettings );
            }
        }
    }
    return rslt;
}


void FBFixUpPrhandle()
{
    if ( gFBPageFormat )
    {
        if ( gFBPRHandle )
        {
            OSStatus          rslt;
            Handle            h;
            rslt = PMSessionMakeOldPrintRecord( gFBPrintSession, gFBPrintSettings, gFBPageFormat, &h );
            if ( rslt )
            {
                printf( "%1s", PScstr( PSstrcpy( TO_STACK, PSltrr( "FB FBFixUpPrhandle PMSessionMakeOldPrintRecord error", "FBFixUpPrhandle" ) ) ) );
                ExitToShell();
            }
            else
            {
                DisposeHandle( gFBPRHandle );
                // fix endian bug in PMSessionMakeOldPrintRecord()
                *(short*)((*h) + 8) = CFSwapInt16BigToHost( *(short*)((*h) + 8) );
                *(short*)((*h) + 10) = CFSwapInt16BigToHost( *(short*)((*h) + 10) );
                *(short*)((*h) + 12) = CFSwapInt16BigToHost( *(short*)((*h) + 12) );
                *(short*)((*h) + 14) = CFSwapInt16BigToHost( *(short*)((*h) + 14) );
                gFBPRHandle = h;
            }
        }
    }
}

void CloseLPrint();

FBBoolean DoPageSetup()
{
    OSStatus        rslt;
    Boolean         accepted;
    
    CloseLPrint();
    rslt = FBBeginSession();
    if ( gFBPrintSession != 0  && gFBPageFormat != 0  )
    {
        PMSessionPageSetupDialog( gFBPrintSession, gFBPageFormat, &accepted );
        rslt = -(accepted == 0 );
        FBFixUpPrhandle();
    }
    return rslt;
}


FBBoolean FBPrintDialog()
{
    OSStatus    rslt;
    Boolean        accepted, valResult;
    
    FBBeginSession();
    PMSetPageRange( gFBPrintSettings, gFBFirstPrintedPage, gFBLastPrintedPage );
    PMSessionValidatePageFormat( gFBPrintSession, gFBPageFormat, &valResult );
    PMSessionValidatePrintSettings( gFBPrintSession, gFBPrintSettings, &valResult );
    PMSessionPrintDialog( gFBPrintSession, gFBPrintSettings, gFBPageFormat, &accepted );
    rslt = -(accepted == false );
    FBFixUpPrhandle();
    return rslt;
}


void FBGetPRHandle()
{
    gFBPRCancel = false;
    SetRect( &gFBPrintRect, 0, 0, 640, 768 );
    if ( gFBPRHandle == 0 )
    {
        gFBPRHandle = NewHandleClear( 120 );
        BlockMoveData( &gFBPrintRect, *gFBPRHandle +  2 +  6, 8 );
        BlockMoveData( &gFBPrintRect, *gFBPRHandle +  16, 8 );
    }
    FBBeginSession();
    FBFixUpPrhandle();
}


Handle FBGetPRHandleRuntime()
{
    if ( gFBPRHandle == 0 )
    {
        FBGetPRHandle();
    }
    return gFBPRHandle;
}


void FBPRInit()
{
    SetRect( &gFBPrintRect, 0, 0, 640, 768 );
    gFBPRCancel = false;
    gFBPrintSettings = NULL;
    gFBPageFormat = NULL;
    gFBFirstPrintedPage = 1;
    gFBLastPrintedPage = kPMPrintAllPages; // was 9999 :rp:20090901
}


void FBPageSetUp()
{
    gFBPRCancel = -( DoPageSetup() != false );
}

// called for 'def lprint'
void FBPrintDialogRuntime()
{
    gFBPRCancel = FBPrintDialog();
}


void ClearLPrint()
{
    //    short             err;
    if ( gFBPrintSession )
    {
        if ( gFBPRPort )
        {
            PMSessionEndPage( gFBPrintSession );
            gFBPRPort = NULL;
        }
    }
    gFBPagefn = 1;
}


void CloseLPrint()
{
    if ( gFBPrintSession )
    {
        ClearLPrint();
        PMSessionEndDocument( gFBPrintSession );
    }
    FBEndSession();
    if ( gFBPrintSettings )
    {
        PMRelease( gFBPrintSettings );
        gFBPrintSettings = NULL;
    }
    if ( gFBPageFormat )
    {
        PMRelease( gFBPageFormat );
        gFBPageFormat = NULL;
    }
    gFBPRPort = NULL;
    gFBPagefn = 1;
}


void FBSendToPrinter()
{
    FontInfo          fi;
    short             lineHeight;
    if ( gFBPRPort )
    {
        SetPort( gFBPRPort );
    }
    else
    {
        FBBeginSession();
        if ( gFBPrintSession )
        {
            PMSessionBeginDocument( gFBPrintSession, gFBPrintSettings, gFBPageFormat );
            PMSessionBeginPage( gFBPrintSession, gFBPageFormat,  0 );
            PMSessionGetGraphicsContext( gFBPrintSession, kPMGraphicsContextQuickdraw, (void**)&gFBPRPort );
            SetPort( gFBPRPort );
            TextFont( gFBStdFont );
            TextSize( gFBStdSize );
            GetFontInfo( &fi );
            lineHeight = fi.ascent + fi.descent + fi.leading;
            MoveTo( 4, lineHeight );
        }
    }
}

long DoPrintPicFB( PicHandle  picH, Rect* r )
{
    OSStatus          rslt;
    //    OSStatus          ignore;
    OSStatus          tempErr;
    CGrafPtr          origPort;
    CGrafPtr          printingPort;
    Rect              pageRect;
    PMRect            tempPageRect;
    
    rslt = FBPrintDialog();
    if ( rslt ==  0 )
    {
        rslt = PMSessionValidatePrintSettings( gFBPrintSession, gFBPrintSettings,  0 );
        if ( rslt ==  0 )
        {
            PMSetFirstPage( gFBPrintSettings, 1,  0 );
        }
        if ( rslt ==  0 )
        {
            PMSetLastPage( gFBPrintSettings, 1,  0 );
        }
        if ( rslt ==  0 )
        {
            rslt = PMSessionBeginDocument( gFBPrintSession, gFBPrintSettings, gFBPageFormat );
            if ( rslt ==  0 )
            {
                rslt = PMGetAdjustedPageRect( gFBPageFormat, &tempPageRect );
                if ( rslt ==  0 )
                {
                    pageRect.top = tempPageRect.top;
                    pageRect.left = tempPageRect.left;
                    pageRect.bottom = tempPageRect.bottom;
                    pageRect.right = tempPageRect.right;
                    if ( (-(PMSessionError( gFBPrintSession ) ==  0 )) )
                    {
                        rslt = PMSessionBeginPage( gFBPrintSession, gFBPageFormat,  0 );
                        if ( rslt ==  0 )
                        {
                            GetPort( &origPort );
                            rslt = PMSessionGetGraphicsContext( gFBPrintSession, kPMGraphicsContextQuickdraw, (void**)&printingPort );
                            if ( rslt ==  0 )
                            {
                                SetPort( printingPort );
                                ClipRect( &pageRect );
                                DrawPicture( picH, r );
                                SetPort( origPort );
                            }
                            tempErr = PMSessionEndPage( gFBPrintSession );
                            if ( rslt ==  0 )
                            {
                                rslt = tempErr;
                            }
                        }
                    }
                }
                tempErr = PMSessionEndDocument( gFBPrintSession );
                if ( rslt ==  0 )
                {
                    rslt = tempErr;
                }
            }
        }
    }
    CloseLPrint();
    return rslt;
}

void PageLPrint()
{
    PicHandle         picH;
    CGrafPtr          port;
    Rect              portRect;
    RgnHandle         oldClip;
    WindowRef         w;
    //    OSStatus          ignore;
    GetPort( &port );
    w = GetWindowFromPort( port );
    if ( w )
    {
        GetWindowPortBounds( w, &portRect );
        oldClip = NewRgn();
        GetClip( oldClip );
        ClipRect( &portRect );
        picH = OpenPicture( &portRect );
        CopyBits( GetPortBitMapForCopyBits( port ), GetPortBitMapForCopyBits( port ), &portRect, &portRect, 0, 0 );
        ClosePicture();
        SetClip( oldClip );
        DisposeRgn( oldClip );
        if ( picH )
        {
            DoPrintPicFB( picH, &portRect );
            SetPort( port );
            DrawPicture( picH, &portRect );
            ValidWindowRect( w, &portRect );
            KillPicture( picH );
        }
    }
    InitCursor();
}


long FBPrintPict( PicHandle  ThePict )
{
    Rect              PictRect;
    
    FBPrintDialogRuntime();
    if ( gFBPRCancel ==  0 )
    {
        gFBRoute =  128;
        BlockMoveData( (void*)*(long*)(ThePict), &PictRect, 8 );
        OffsetRect( &PictRect, -PictRect.left, -PictRect.top );
        DrawPicture( ThePict, &PictRect );
        gFBRoute =  0;
        CloseLPrint();
    }
    return 0;
}


FBBoolean PrError()
{
    return  -( gFBPrintSession == 0 );
}


FBBoolean CheckForPRCancel()
{
    gFBPRCancel = ( PMSessionError( gFBPrintSession ) == kPMCancel );
    if ( gFBPRCancel )
    {
        CloseLPrint();
    }
    return gFBPRCancel;
}

#endif  /* !__LP64__ */

void FBRoute( long where )
{
#if !__LP64__
    if ( where == 128 ) // _toPrinter
    {
        FBSendToPrinter();
    }
    else
    {
        CheckForPRCancel();
    }
#endif  /* !__LP64__ */
    gFBRoute = where;
}


#if !__LP64__


void FBCheckForPageFeed( long  isCr )
{
    Rect              r;
    Point             penPos;
    FontInfo          fi;
    long              lHt;
    short             fnt;
    short             size;
    short             mde;
    signed char       face;
    
    if ( gFBRoute == 128 )
    {
        GetFontInfo( &fi );
        GetPen( &penPos );
        GetPortBounds( gFBPRPort, &r );
        lHt = fi.descent + fi.leading;
        if ( isCr ) lHt += fi.ascent;
        
        if ( penPos.v + lHt >= r.bottom )
        {
            fnt  = GetPortTextFont( gFBPRPort );
            size = GetPortTextSize( gFBPRPort );
            face = GetPortTextFace( gFBPRPort );
            mde  = GetPortTextMode( gFBPRPort );
            
            FBRoute( 0 );
            ClearLPrint();
            FBRoute( 128 );
            
            TextFont( fnt );
            TextSize( size );
            TextFace( face );
            TextMode( mde );
            
            MoveTo( 4, fi.ascent + fi.descent + fi.leading );
        }
    }
}



void FBText( long  font, long  size, long  style, long  xMode )
{
    CGrafPtr   port;
    
    GetPort( &port );
    if ( gFBBadPort != port )
    {
        if ( font != -1 )
        {
            TextFont( font );
            gTextFont = font;
        }
        if ( size > -1 )
        {
            TextSize( size );
            gTextSize = size;
        }
        if ( style > -1 )
        {
            TextFace( style );
            gTextFace = style;
        }
        if ( xMode > -1 )
        {
            TextMode( xMode );
            gTextMode = xMode;
        }
    }
}

#endif  /* !__LP64__ */



///////////////////////////////////////////////////
//          shutdown, stop, errors               //
///////////////////////////////////////////////////

#if !kNSFBFullStop

void FBfullStop( StringPtr  major_p, StringPtr  explanation, FBBoolean  fatal )
{
    /*
     final common path for most stop alerts
     if ( fatal ) exits to shell without returning
     */
    Str255 major;  PSstrcpy( major, major_p );
    
#if !__LP64__
    AlertStdAlertParamRec alertParam;
    Str15             defaultBtnTitle;
    Str15             cancelBtnTitle;
    short             itemHit;
    if ( fatal )
    {
        alertParam.cancelText = NULL;
        PSstrcpyn( defaultBtnTitle, PSltrr( "Quit", "FBfullStop" ), 16 );
    }
    else
    {
        PSstrcpyn( defaultBtnTitle, PSltrr( "Stop", "FBfullStop" ), 16 );
        PSstrcpyn( cancelBtnTitle, PSltrr( "Continue", "FBfullStop" ), 16 );
        alertParam.cancelText = cancelBtnTitle;
    }
    alertParam.movable = true;
    alertParam.helpButton = false;
    alertParam.filterProc = NULL;
    alertParam.defaultText = defaultBtnTitle;
    alertParam.otherText = "\pCrash";
    alertParam.defaultButton = kAlertStdAlertOKButton;
    alertParam.cancelButton = kAlertStdAlertCancelButton;
    alertParam.position     = kWindowDefaultPosition;
    InitCursor();
    ShowCursor();
    StandardAlert( kAlertCautionAlert, major, explanation, &alertParam, &itemHit );
    if ( itemHit == kAlertStdAlertOtherButton ) abort(); // rp 20111224
    if ( itemHit == kAlertStdAlertOKButton ) ExitToShell();
#else
    FBPrintString( PSstrcpy( STACK_PUSH(), "\pStopped" ) );
    FBPrintString( PSstrcpy( STACK_PUSH(), major ) );
    FBPrintString( PSstrcpy( STACK_PUSH(), explanation ) );
    exit( 1 );
#endif  /* !__LP64__ */
}

#endif


#define RUNTIME_ERR_MSG_IS_ALERT 0

#if kNSFBFullStop

void RuntimeErrMsg( StringPtr  msg )
{
    /*
     Called by EmbedButton etc in Subs Appearance Btns.incl
     and by FBEditDollarStatement()
     Supposed to be shown for a run but skipped in a built app, but FBtoC has no such concept
     */
#if RUNTIME_ERR_MSG_IS_ALERT
    
    Str255 msgStr;  PSstrcpy( msgStr, msg );
    gFBStk--;
    CFStringRef string = CFStringCreateWithPascalString( kCFAllocatorDefault, msgStr, kCFStringEncodingMacRoman );
    FBFullStop( string, CFSTR(""), false ); // in the user's face
    CFRelease( string );
#else
    char string[256];
    BlockMoveData( &msg[1], string, msg[0] );
    string[msg[0]] = 0; // null-terminate
    fprintf( stderr, "%s\n", string ); // discreetly hidden from Joe Public
#endif
}

static CFStringRef FBCreateLineNumberMsg( const int lineNumber )
{
    /*
     "At line xxxx."
     */
    CFStringRef atLineString = (CFStringRef)NSLocalizedStringFromTable(@"At line",@"Runtime",@"FBCreateLineNumberMsg");
    CFStringRef msg = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@ %d"), atLineString, lineNumber );
    return msg;
}

#else

void RuntimeErrMsg( StringPtr  msg )
{
    /*
     Called by EmbedButton etc in Subs Appearance Btns.incl
     and by FBEditDollarStatement()
     Supposed to be shown for a run but skipped in a built app, but FBtoC has no such concept
     */
#if RUNTIME_ERR_MSG_IS_ALERT
    FBfullStop( msg, "\p", false ); // in the user's face
#else
    char string[256];
    BlockMoveData( &msg[1], string, msg[0] );
    string[msg[0]] = 0; // null-terminate
    fprintf( stderr, "%s\n", string ); // discreetly hidden from Joe Public
#endif
}

static StringPtr FBLineNumberMsg( const int lineNumber )
{
    /*
     "At line xxxx."
     */
    Str255 temp;
    
    PSstrcpy( temp, PSltrr( "At line", "FBLineNumberMsg" ) );
    PSstrcat( temp, PSstr( lineNumber ) );
    return PSstrcpy( gReturnedString, temp );
}

#endif  // kNSFBFullStop




#if kNSFBFullStop
void FBStopMsg( ConstStr255Param msg_p, const int  lineNumber ) {
    /*
     FB 'stop'
     */
    Str255 msg;  PSstrcpy( msg, msg_p );
    gFBStk--;
    
    CFStringRef msgString     = CFStringCreateWithPascalString( kCFAllocatorDefault, msg, kCFStringEncodingMacRoman );
    CFStringRef lineNumString = FBCreateLineNumberMsg(lineNumber);
    FBFullStop( msgString, lineNumString, false );
    
    CFRelease( msgString );
    CFRelease( lineNumString );
    
}

void FBShutdown( void *msgPtr, Boolean cfType )
{
    /*
     FB 'shutdown'
     */
    if ( cfType ) {
        FBFullStop( msgPtr, (CFStringRef)NSLocalizedStringFromTable(@"The application must quit",@"Runtime",@"FBShutdown"), true );
    } else {
        Str255 msg;  PSstrcpy( msg, msgPtr );
        gFBStk--;
        CFStringRef msgString = CFStringCreateWithPascalString( kCFAllocatorDefault, msg, kCFStringEncodingMacRoman );
        FBFullStop( msgString, (CFStringRef)NSLocalizedStringFromTable(@"The application must quit",@"Runtime",@"FBShutdown"), true );
        CFRelease( msgString );
    }
}

void FBReportRangeError( const int  value, const int  lineNumber, const int  limit, CFStringRef fileName )
{
    /*
     array bounds error
     */
    CFStringRef atLineMsg = FBCreateLineNumberMsg( lineNumber );
    CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable(@" of ",@"Runtime",@"FBReportRangeError");
    CFStringRef s2 = (CFStringRef)NSLocalizedStringFromTable(@"Index ",@"Runtime",@"FBReportRangeError");
    CFStringRef s3 = (CFStringRef)NSLocalizedStringFromTable(@" is outside the DIM range 0 to",@"Runtime",@"FBReportRangeError");
    CFStringRef string = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%@%@\n%@%d%@ %d"),atLineMsg,s1,fileName,s2,value,s3,limit);
    CFRelease(atLineMsg);
    
    // get C string for fprintf
    char *buffer;
    CFIndex length = CFStringGetMaximumSizeForEncoding( CFStringGetLength( string ), kCFStringEncodingMacRoman ) + 1;
    buffer = malloc( length );
    CFStringGetCString( string, buffer, length, kCFStringEncodingMacRoman );
    fprintf( stderr, "Array bounds error:\n" );
    fprintf( stderr, "%s. [Type ctrl-C if process hangs]\n", buffer );//(UInt8 *)
    free( buffer );
    
    FBFullStop( (CFStringRef)NSLocalizedStringFromTable( @"Array bounds error",@"Runtime",@"FBReportRangeError"), string, true );
    CFRelease( string );
}

#else

void FBStopMsg( ConstStr255Param  msg_p, const int  lineNumber )
{
    /*
     FB 'stop'
     */
    Str255 msg;  PSstrcpy( msg, msg_p );
    gFBStk--;
    FBfullStop( msg, FBLineNumberMsg( lineNumber ), false );
}

void FBShutdown( ConstStr255Param  msg_p )
{
    /*
     FB 'shutdown'
     */
    Str255 msg;  PSstrcpy( msg, msg_p );
    gFBStk--;
    FBfullStop( msg, PSltrr( "The application must quit", "FBShutdown" ), true );
}

void FBReportRangeError( const int  value, const int  lineNumber, const int  limit, ConstStr255Param fileName )
{
    /*
     array bounds error
     */
    Str255      msg;
    
    PSstrcpy( msg, FBLineNumberMsg( lineNumber ) );
    PSstrcat( msg, PSltrr( " of ", "FBReportRangeError" ) );
    PSstrcat( msg, fileName );
    PSstrcat( msg, PSltrr( "\nIndex ", "FBReportRangeError" ) );
    PSstrcat( msg, PSstr( value ) );
    PSstrcat( msg, PSltrr( " is outside the DIM range 0 to", "FBReportRangeError" ) );
    PSstrcat( msg, PSstr( limit ) );
    
    // log to stderr
    char string[256];
    BlockMoveData( &msg[1], string, msg[0] );
    string[msg[0]] = 0; // null-terminate
    fprintf( stderr, "Array bounds error:\n" );
    fprintf( stderr, "%s. [Type ctrl-C if process hangs]\n", string );
    
    FBfullStop( PSltrr( "Array bounds error", "FBReportRangeError"), msg, true );
}

#endif


///////////////////////////////////////////////////
//                    picture                    //
///////////////////////////////////////////////////

#if !__LP64__
PicHandle      gFBTempPictureH;

void FBPicture( Rect* rectPtr, PicHandle pictH )
{
    Rect              tempRect;
    
    if ( pictH == NULL ) pictH = gFBTempPictureH;
    if ( pictH )
    {
        if ( rectPtr )
        {
            if ( rectPtr->bottom == -32768 )
            {
#if TARGET_RT_LITTLE_ENDIAN
                QDGetPictureBounds( pictH, &tempRect ); // 10.3
#else
                tempRect = (**pictH).picFrame; // wrong values on Intel
#endif
                OffsetRect( &tempRect, rectPtr->left - tempRect.left, rectPtr->top - tempRect.top );
            }
            else
            {
                BlockMoveData( rectPtr, &tempRect, 8 );
            }
        }
        else
        {
#if TARGET_RT_LITTLE_ENDIAN
            QDGetPictureBounds( pictH, &tempRect );
#else
            tempRect = (**pictH).picFrame;
#endif
        }
        DrawPicture( pictH, &tempRect );
    }
}

void FBPictureOn( Rect* rectPtr )
{
    WindowRef         w;
    Rect              r;
    
    if ( gFBTempPictureH == 0 )
    {
        if ( rectPtr )
        {
            gFBTempPictureH = OpenPicture( rectPtr );
        }
        else
        {
            w = FBGetCurOutputWindow();
            if ( w )
            {
                GetWindowPortBounds( w, &r );
                gFBTempPictureH = OpenPicture( &r );
            }
        }
    }
}

void FBPictureOff( PicHandle * picHPtr )
{
    if ( gFBTempPictureH )
    {
        ClosePicture();
        if ( picHPtr )
        {
            *picHPtr = gFBTempPictureH;
            gFBTempPictureH = NULL;
        }
    }
    else
    {
        *picHPtr =  NULL;
    }
}


short             gTextFont = applFont;
short             gTextSize = 9;
short             gTextFace = normal;
short             gTextMode = srcCopy;



///////////////////////////////////////////////////
//          threadbegin, threadstatus            //
///////////////////////////////////////////////////


Boolean           gThreadsUsed;
short             gFBInThread;
short             gFBAbortThreads;
long              gFBThreadList[32];
long              gFBThreadFlags[32];
long              gFBThreadProc[32];

long FBThreadStatus( unsigned long  ticks )
{
    //    short             err;
    ticks += TickCount();
    do
    {
        YieldToAnyThread();
    }
    while ( !( -(TickCount() >= ticks ) || gFBAbortThreads ) );
    return gFBAbortThreads;
}


void FBWaitForAllThreads()
{
    //    short             abort;
    gFBAbortThreads = -1;
    while ( gFBInThread > 0 )
    {
        FBThreadStatus( 0 );
    }
}



void FBMasterThreader( long  flags )
{
    long              FBThreadVector;
    FBThreadVector = gFBThreadProc[flags];
    gFBInThread++;
    (*(long(*)(short)) FBThreadVector)( gFBThreadFlags[flags] );
    gFBInThread--;
    gFBThreadList[flags] = 0;
}


long FBThreadBegin( long  LocalFNProc, long  flags, long  stackmin )
{
    ThreadEntryProcPtr  threadProc;
    ThreadID            threadID;
    short               err;
    long                i;
    if ( stackmin == 0 )
    {
        stackmin = 131072;
    }
    i = 0;
    while ( (i <  31 ) && ( gFBThreadList[i] != 0 )  )
    {
        i++;
    }
    threadProc = (ThreadEntryProcPtr)((void*)&FBMasterThreader );
    threadProc = NewThreadEntryUPP( threadProc );
    err = NewThread( 1, threadProc, (long*)i, stackmin,  12, 0, &threadID );
    if ( err )
    {
    }
    else
    {
        if ( ( i <=  31 ) & (gFBThreadList[i] == 0 )  )
        {
            gFBThreadProc[i] = LocalFNProc;
            gFBThreadList[i] = threadID;
            gFBThreadFlags[i] = flags;
        }
    }
    YieldToAnyThread();
    gThreadsUsed = true;
    return i;
}

///////////////////////////////////////////////////
//                    cursor                     //
///////////////////////////////////////////////////


void FBCursor( long  crsrID )
{
    CCrsrHandle       crsrHndl;
    CursHandle        crsHndl;
    
    if ( gFBAplActive < 0 ) return;
    if ( crsrID < 0 )
    {
        crsrID = -crsrID;
        gFBLastCrsrWnd = 0;
    }
    if ( crsrID != gFBLastCursor )
    {
        gFBLastCursor = crsrID;
        crsrHndl =  GetCCursor( crsrID );
        if ( crsrHndl )
        {
            SetCCursor( crsrHndl );
            DisposeCCursor( crsrHndl );
        }
        else
        {
            crsHndl = GetCursor( crsrID );
            if ( crsHndl )
            {
                SetCursor( *crsHndl );
            }
            else
            {
                InitCursor();
            }
        }
    }
}


void FBCursors( long  crsrID, long  crsrType )
{
    if ( gFBAplActive < 0 ) return;
    if ( crsrID < 0 )
    {
        crsrID = -crsrID;
        gFBLastCrsrWnd = 0;
    }
    if ( crsrType ==  32768 || crsrType == 65536 ) // _themeCursorStatic or _themeCursorAnimate (treated as _themeCursorStatic)
    {
        gFBLastCursor = crsrID;
        gFBLastCursType = 32768;
        SetThemeCursor( crsrID );
    }
    else
    {
        FBCursor( crsrID );
    }
}


///////////////////////////////////////////////////
//                    inkey$                     //
///////////////////////////////////////////////////

StringPtr FBInkey()
{
    Str1      outMsg;
    UInt32    oldEventTicks;
    
    outMsg[0] = 0;
    oldEventTicks = gFBEventThing.eventTicks;
    gFBEventThing.eventTicks = 2;
    FBHandleEvents();
    gFBEventThing.eventTicks = oldEventTicks;
    if ( gFBEventThing.theEvent.what == keyDown || gFBEventThing.theEvent.what == autoKey )
    {
        outMsg[0] = 1;
        outMsg[1] = gFBEventThing.theEvent.message;
        gFBEventThing.theEvent.what = nullEvent;
    }
    return PSstrcpy( gReturnedString, outMsg );
}

///////////////////////////////////////////////////
//                line input                     //
///////////////////////////////////////////////////

void FBLineInput( Str255  prompt_p, void*  vPtr )
{
    Str255 prompt;  PSstrcpy( prompt, prompt_p );
    gFBStk -= 1;
    short             lineLen, cWidth;
    short             theChar;
    Point             start, curr;
    Rect              blinkRect;
    Boolean            blinkState;
    unsigned long     nxtBlinkTicks;
    FontInfo          fi;
    Str255            tempStr, lineStr;
    
    lineLen = 0;
    if ( prompt[0] )    FBPrintString( PSstrcpy( STACK_PUSH(), prompt ) );
    GetPen( &start );
    GetFontInfo( &fi );
    nxtBlinkTicks = TickCount();
    blinkState = 0;
    do
    {
        GetPen( &curr );
        if ( TickCount() > nxtBlinkTicks )
        {
            nxtBlinkTicks = TickCount() + 15;
            blinkState = ~blinkState;
            SetRect( &blinkRect, curr.h, curr.v, curr.h + fi.widMax, curr.v + 2 );
            InvertRect( &blinkRect );
        }
        PSstrcpy( tempStr, FBInkey() );
        theChar = PSasc( PSstrcpy( STACK_PUSH(), tempStr ) );
        
        if ( theChar == 3 || theChar == 13 )
        {
            theChar = 13;
        }
        else if ( theChar == 127 || theChar == 8 )
        {
            if ( lineLen )
            {
                if ( blinkState )
                {
                    blinkState = 0;
                    InvertRect( &blinkRect );
                }
                cWidth = CharWidth( *(unsigned char*)((void*)&lineStr + lineLen) );
                SetRect( &blinkRect, curr.h - cWidth, curr.v - fi.ascent, curr.h, curr.v + fi.descent );
                EraseRect( &blinkRect );
                MoveTo( curr.h - cWidth, curr.v );
                lineLen--;
            }
            
        }
        else if ( theChar < 32 )
        {
            if ( theChar ) SysBeep( 1 );
        }
        else
        {
            if ( lineLen < 255 )
            {
                if ( blinkState )
                {
                    blinkState = 0;
                    InvertRect( &blinkRect );
                }
                FBPrintString( PSstrcpy( STACK_PUSH(), tempStr ) );
                lineLen++;
                *(char*)( (void*)&lineStr + lineLen ) = theChar;
            }
            else
            {
                SysBeep( 1 );
            }
        }
    }
    while ( ( theChar != 13 ) );
    
    if ( blinkState )
    {
        //        blinkState = 0;
        InvertRect( &blinkRect );
    }
    *(char*)( &lineStr ) = lineLen;
    PSstrcpy( ((StringPtr)vPtr), lineStr );
    PrintCR();
}

void FBParseInput( short  VarType, void * vPtr );

void FBToCInput( Str255  prompt, void *target, short varType )
{
    gFBLinePos = 1;
    FBLineInput( prompt, (StringPtr)&gFBLineBuf );
    FBParseInput( varType, target );
}


///////////////////////////////////////////////////
//                    mouse                      //
///////////////////////////////////////////////////


/*
 bugs:
 - _releaseHorz, _releaseVert not implemented
 */
long Mouse( long  mArg )
{
    Point             tempPoint;
    long              mResult;
    WindowRef         w;
    
    GetMouse( &tempPoint );
    switch ( mArg )
    {
        case 0:
            mResult = gFBMouseClickCount;
            if ( mResult > 3 ) mResult = 3;
            if ( StillDown() ) mResult = -mResult;
            break;
        case 1:        // _horz
            mResult = tempPoint.h;
            break;
        case 2:        // _vert
            mResult = tempPoint.v;
            break;
        case 3:        // _lastMHorz
            mResult = gFBLastMousePt.h;
            break;
        case 4:        // _lastMVert
            mResult = gFBLastMousePt.v;
            break;
        case 5:
        case 6:
            mResult = 0; // _releaseHorz, _releaseVert
            break;
        case 7:        // _mouseWindow
            FindWindow( gFBEventThing.theEvent.where, &w );
            mResult = FBGetWndNumber( w );
            break;
        default:
            mResult = 0;
            break;
    }
    gFBLastMousePt = tempPoint;
    return mResult;
}


/*
 MaxWindow width, height
 
 Bugs:
 - only works on current output window BW
 */
void FBMaxWindow( long width, long height )
{
    HISize        maxLimits = {width, height};
    
    SetWindowResizeLimits( FBGetCurOutputWindow(), NULL, &maxLimits );
}


/*
 MinWindow width, height
 
 Bugs:
 - only works on current output window BW
 */
void FBMinWindow( long width, long height )
{
    HISize        minLimits = {width, height};
    
    SetWindowResizeLimits( FBGetCurOutputWindow(), &minLimits, NULL );
}

#if 0
/*
 void PrintPoint( Point*  thePoint )
 {
 Str63             t;
 FBPrintString( PSstrcpy( STACK_PUSH(), "\p(" ) ); NumToString( thePoint->h, (void*)&t );
 FBPrintString( PSstrcpy( STACK_PUSH(), t ) );  FBPrintString( PSstrcpy( STACK_PUSH(), "\px," ) );  NumToString( thePoint->v, (void*)&t );
 FBPrintString( PSstrcpy( STACK_PUSH(), t ) );  FBPrintString( PSstrcpy( STACK_PUSH(), "\py)" ) );
 }
 
 
 void PrintRect( Rect* theAddr )
 {
 Str15             n;
 FBPrintString( PSstrcpy( STACK_PUSH(), "\p(" ) );
 NumToString( theAddr->top, (void*)&n );
 FBPrintString( (PSstrcpy( STACK_PUSH(), n ), PSstrcat( TO_STACK, "\p," )) );
 NumToString( theAddr->left, (void*)&n );
 FBPrintString( (PSstrcpy( STACK_PUSH(), n ), PSstrcat( TO_STACK, "\p," )) );
 NumToString( theAddr->bottom, (void*)&n );
 FBPrintString( (PSstrcpy( STACK_PUSH(), n ), PSstrcat( TO_STACK, "\p," )) );
 NumToString( theAddr->right, (void*)&n );
 FBPrintString( (PSstrcpy( STACK_PUSH(), n ), PSstrcat( TO_STACK, "\p)" )) );
 }
 */
#endif

/*
 SetZoom [#]wndNum[, @rect | (h1,v1)-(h2,v2)]
 
 Bugs:
 - not yet implemented BW
 */
//void FBSetZoom( long id, Rect *r )
//{
//    if ( r )
//    {
//
//    }
//    else
//    {
//
//    }
//}

#endif  /* !__LP64__ */


///////////////////////////////////////////////////
//                  FinderInfo                   //
///////////////////////////////////////////////////


#if !DECARBONATE

CFMutableArrayRef sFinderInfoArray;


static CFMutableArrayRef FinderInfoGetArray( void )
{
    return sFinderInfoArray;
}


static void FinderInfoSetArray( CFMutableArrayRef array )
{
    sFinderInfoArray = array;
}


static void FinderInfoReleaseArray( void )
{
    CFMutableArrayRef    array;
    
    array = FinderInfoGetArray();
    if ( array ) CFRelease( array );
    FinderInfoSetArray( 0 );
}


void ClearFinderInfo( void )
{
    FinderInfoReleaseArray();
}


static long FinderInfoFSRef( short *index, FSRef *ref, OSType *type )
{
    CFMutableArrayRef        array;
    CFDataRef                data;
    CFNumberRef                number;
    FSRef                    fsRef;
    FSCatalogInfo            catalogInfo;
    FInfo                    info;
    short                    count, i, action = kFBFinderInfoErr;
    
    array = FinderInfoGetArray();
    if ( array )
    {
        count = CFArrayGetCount( array ) - 1;
        
        if ( *index == 0 )
        {
            *index = count;
        }
        else
        {
            if ( *index <= count )
            {
                i = abs( *index );
                data = CFArrayGetValueAtIndex( array, i );
                if ( data )
                {
                    if ( CFDataGetLength( data ) == sizeof( FSRef ) )
                    {
                        CFDataGetBytes( data, CFRangeMake( 0, sizeof( FSRef ) ), (UInt8*)&fsRef );
                        BlockMoveData( &fsRef, ref, sizeof( FSRef ) );
                        FSGetCatalogInfo( ref, kFSCatInfoFinderInfo, &catalogInfo, NULL, NULL, NULL );
                        BlockMoveData( &catalogInfo.finderInfo, &info, sizeof( FInfo ) );
                        *type = info.fdType;
                    }
                }
            }
        }
        
        number = CFArrayGetValueAtIndex( array, 0 );
        if ( number ) CFNumberGetValue( number, kCFNumberSInt32Type, &action );
    }
    return action;
}


#if !__LP64__
static long FinderInfoFSSpec( short *index, FSSpec *spec, OSType *type )
{
    FSRef    ref;
    
    short action = FinderInfoFSRef( index, &ref, type );
    FSGetCatalogInfo( &ref, 0, NULL, NULL, spec, NULL );
    return action;
}
#endif  /* !__LP64__ */

static long FinderInfoCFURL( short *index, CFURLRef *url, OSType *type )
{
    FSRef    ref;
    
    short action = FinderInfoFSRef( index, &ref, type );
    *url = CFURLCreateFromFSRef( kCFAllocatorDefault, &ref );
    return action;
}


long FBFinderInfo( short *index, void *specRefUrl, OSType *type, short varType )
{
    long action = kFBFinderInfoErr;
    
    switch ( varType )
    {
#if !__LP64__
        case kFBFinderInfoVarTypeFSSpec:
            action = FinderInfoFSSpec( index, specRefUrl, type );
            break;
#endif  /* !__LP64__ */
        case kFBFinderInfoVarTypeFSRef:
            action = FinderInfoFSRef( index, specRefUrl, type );
            break;
            
        case kFBFinderInfoVarTypeCFURL:
            action = FinderInfoCFURL( index, specRefUrl, type );
            break;
    }
    return action;
}



static void FinderInfoOpenPrint( AppleEvent *aevt, AppleEvent *reply, long refCon )
{
    AEDesc                docList;
    FSRef                ref;
    CFMutableArrayRef    array;
    CFDataRef            data;
    CFNumberRef            number;
    long                count;
    SInt32                index, action = refCon;
    
    //    reply = reply;
    AEGetParamDesc( aevt, keyDirectObject, typeAEList, &docList );
    AECountItems( &docList, &count );
    if ( count )
    {
        FinderInfoReleaseArray();
        array = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks );
        if ( array )
        {
            number = CFNumberCreate( kCFAllocatorDefault, kCFNumberSInt32Type, &action );
            if ( number )
            {
                CFArrayAppendValue( array, number );
                CFRelease( number );
            }
            
            for ( index = 1; index <= count; index++ )
            {
                AEGetNthPtr( &docList, index, typeFSRef, NULL, NULL, &ref, sizeof( FSRef ), NULL );
                data = CFDataCreate( kCFAllocatorDefault, (UInt8*)&ref, sizeof( FSRef ) );
                if ( data )
                {
                    CFArrayAppendValue( array, data );
                    CFRelease( data );
                }
            }
            FinderInfoSetArray( array );
        }
    }
    (*gFBOnFinderInfoVector)();
}


// called only if 'on FinderInfo fn DoFinderInfo' is in the user's source
void InstallFBFinderInfoHandler( void )
{
#if __LP64__
    AEInstallEventHandler( kCoreEventClass, kAEOpenDocuments, (AEEventHandlerUPP)FinderInfoOpenPrint, (SRefCon)kFBFinderInfoOpen, false );
    AEInstallEventHandler( kCoreEventClass, kAEPrintDocuments, (AEEventHandlerUPP)FinderInfoOpenPrint, (SRefCon)kFBFinderInfoPrint, false );
#else
    AEInstallEventHandler( kCoreEventClass, kAEOpenDocuments, (AEEventHandlerUPP)FinderInfoOpenPrint, kFBFinderInfoOpen, false );
    AEInstallEventHandler( kCoreEventClass, kAEPrintDocuments, (AEEventHandlerUPP)FinderInfoOpenPrint, kFBFinderInfoPrint, false );
#endif
}


#endif // !DECARBONATE

