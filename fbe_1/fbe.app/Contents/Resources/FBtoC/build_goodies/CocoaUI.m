/*
 CocoaUI.m
 
 Bernie Wylde 20170712
 
 updated: 20171026
 */

#include "CocoaUI.h"

// application
void CocoaAppRun()
{
    static BOOL sCocoaAppRun = NO;
    if ( !sCocoaAppRun ) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        sCocoaAppRun = YES;
        [NSApp run];
        [pool drain];
    }
}

BOOL IsCocoaApplication()
{
#if __LP64__
    return YES;
#endif
    if ( [NSApp delegate] ) return YES;
    return NO;
}


#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060

#pragma mark - --[cocoa]--


#pragma mark - alerts

void CocoaShowAlert( NSAlertStyle style, NSString *msg, NSString *info )
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setAlertStyle:style];
    [alert setMessageText:msg];
    [alert setInformativeText:info];
    [alert runModal];
    [alert release];
}


#pragma mark - application

CocoaApplicationDelegate *sCocoaApplicationDelegate;

@implementation CocoaApplicationDelegate

@synthesize windows;
@synthesize boolVal;
@synthesize longVal;
@synthesize stringVal;
@synthesize arrayVal;
@synthesize dictVal;
@synthesize urlVal;
@synthesize menuObj;

- (id)init {
    if ( self = [super init] ) {
        windows = [[NSMutableArray alloc] initWithCapacity:0];
    }
    return self;
}


- (void)timerFired:(NSTimer *)timer {
    CocoaTimerCallbackType callback = [timer.userInfo pointerValue];
    if ( (*callback)(timer) ) {
        [timer invalidate];
    }
}



#pragma mark - application delegate methods
- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    if ( ![NSApp mainMenu] ) BuildMinimalMenu();    // make sure we have at least a quit item
    CallUserDefinedOnAppEventFunction( appWillFinishLaunching, 0 );
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidFinishLaunching, 0 );
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)app {
    longVal = NSTerminateNow;
    CallUserDefinedOnAppEventFunction( appWillFinishLaunching, 0 );
    return longVal;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
    boolVal = NO;
    CallUserDefinedOnAppEventFunction( appShouldTerminateAfterLastWindowClosed, 0 );
    return boolVal;
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appWillTerminate, 0 );
}

- (void)applicationWillBecomeActive:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appWillBecomeActive, 0 );
}

- (void)applicationDidBecomeActive:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidBecomeActive, 0 );
}

- (void)applicationWillResignActive:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appWillResignActive, 0 );
}

- (void)applicationDidResignActive:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidResignActive, 0 );
}

- (void)applicationWillHide:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appWillHide, 0 );
}

- (void)applicationDidHide:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidHide, 0 );
}

- (void)applicationWillUnhide:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appWillUnhide, 0 );
}

- (void)applicationDidUnhide:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidUnhide, 0 );
}

- (void)applicationWillUpdate:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appWillUpdate, 0 );
}

- (void)applicationDidUpdate:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidUpdate, 0 );
}

- (BOOL)applicationShouldHandleReopen:(NSApplication *)app hasVisibleWindows:(BOOL)flag {
    boolVal = NO;
    CallUserDefinedOnAppEventFunction( appShouldHandleReopen, flag );
    return boolVal;
}

- (NSMenu *)applicationDockMenu:(NSApplication *)app {
    menuObj = nil;
    CallUserDefinedOnAppEventFunction( appDockMenu, 0 );
    return menuObj;
}

- (NSError *)application:(NSApplication *)app willPresentError:(NSError *)error {
    // .....
    return error;
}

- (void)applicationDidChangeScreenParameters:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidChangeScreenParameters, 0 );
}

/*
 this method doesn't get called if application:openFiles: exists
 */
//- (BOOL)application:(NSApplication *)app openFile:(NSString *)filename {
//    stringVal = filename;
//    boolVal = NO;
//    CallUserDefinedOnAppEventFunction( appOpenFile, 0 );
//    return boolVal;
//}

- (BOOL)application:(id)sender openFileWithoutUI:(NSString *)filename {
    stringVal = filename;
    boolVal = NO;
    CallUserDefinedOnAppEventFunction( appOpenFileWithoutUI, 0 );
    return boolVal;
}

- (BOOL)application:(NSApplication *)app openTempFile:(NSString *)filename {
    stringVal = filename;
    boolVal = NO;
    CallUserDefinedOnAppEventFunction( appOpenTempFile, 0 );
    return boolVal;
}

/*
 this method stops application:openFile: from being called so both methods can't exist
 */
- (void)application:(NSApplication *)app openFiles:(NSArray *)filenames {
    stringVal = [[filenames objectAtIndex:0] lastPathComponent];
    NSMutableArray *tempArray = [NSMutableArray arrayWithCapacity:0];
    for ( NSString *path in filenames ) {
        [tempArray addObject:[NSURL fileURLWithPath:path]];
    }
    urlVal = [tempArray objectAtIndex:0];
    arrayVal = (NSArray *)tempArray;
    CallUserDefinedOnAppEventFunction( appOpenURLs, 0 );
}

- (BOOL)applicationOpenUntitledFile:(NSApplication *)app {
    boolVal = NO;
    CallUserDefinedOnAppEventFunction( appOpenUntitledFile, 0 );
    return boolVal;
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)app {
    boolVal = NO;
    CallUserDefinedOnAppEventFunction( appShouldOpenUntitledFile, 0 );
    return boolVal;
}

- (BOOL)application:(NSApplication *)app printFile:(NSString *)filename {
    stringVal = filename;
    boolVal = YES;
    CallUserDefinedOnAppEventFunction( appPrintFile, 0 );
    return boolVal;
}

- (NSApplicationPrintReply)application:(NSApplication *)app printFiles:(NSArray *)filenames withSettings:(NSDictionary *)printSettings showPrintPanels:(BOOL)showPrintPanels {
    longVal = NSPrintingSuccess;
    arrayVal = filenames;
    dictVal = printSettings;
    boolVal = showPrintPanels;
    CallUserDefinedOnAppEventFunction( appPrintFiles, 0 );
    return longVal;
}

- (void)application:(NSApplication *)app didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    // dataVal = deviceToken;
    CallUserDefinedOnAppEventFunction( appDidRegisterForRemoteNotificationsWithDeviceToken, 0 );
}

- (void)application:(NSApplication *)app didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    CallUserDefinedOnAppEventFunction( appDidFailToRegisterForRemoteNotifications, 0 );
}

- (void)application:(NSApplication *)app didReceiveRemoteNotification:(NSDictionary *)userInfo {
    dictVal = userInfo;
    CallUserDefinedOnAppEventFunction( appDidReceiveRemoteNotification, 0 );
}

- (void)application:(NSApplication *)app didDecodeRestorableState:(NSCoder *)coder {
    // coder
    CallUserDefinedOnAppEventFunction( appDidDecodeRestorableState, 0 );
}

- (void)application:(NSApplication *)app willEncodeRestorableState:(NSCoder *)coder {
    // coder
    CallUserDefinedOnAppEventFunction( appWillEncodeRestorableState, 0 );
}

- (BOOL)application:(NSApplication *)app willContinueUserActivityWithType:(NSString *)userActivityType {
    boolVal = NO;
    stringVal = userActivityType;
    CallUserDefinedOnAppEventFunction( appWillContinueUserActivityWithType, 0 );
    return boolVal;
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
- (BOOL)application:(NSApplication *)app continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray *restorableObjects))restorationHandler {
    boolVal = NO;
    //arrayVal = restorableObjects;
    CallUserDefinedOnAppEventFunction( appContinueUserActivity, 0 );
    return boolVal;
}
#endif

- (void)application:(NSApplication *)app didFailToContinueUserActivityWithType:(NSString *)userActivityType error:(NSError *)error {
    stringVal = userActivityType;
    // error
    CallUserDefinedOnAppEventFunction( appDidFailToContinueUserActivityWithType, 0 );
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
- (void)application:(NSApplication *)app didUpdateUserActivity:(NSUserActivity *)userActivity {
    CallUserDefinedOnAppEventFunction( appDidUpdateUserActivity, 0 );
}
#endif

- (void)applicationDidChangeOcclusionState:(NSNotification *)notification {
    CallUserDefinedOnAppEventFunction( appDidChangeOcclusionState, 0 );
}

/*
 if this method were to be implemented, it would prevent application:openFiles from being called
 */
//#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101200
//- (void)application:(NSApplication *)application openURLs:(NSArray<NSURL *> *)urls {
//    urlVal = [urls objectAtIndex:0];
//    arrayVal = urls;
//    CallUserDefinedOnAppEventFunction( appOpenURLs, 0 );
//}
//#endif

@end


CocoaApplicationDelegate *CocoaApplicationDelegateGet()
{
    CocoaApplicationDelegate *appDelegate = (CocoaApplicationDelegate *)[NSApp delegate];
    if ( !appDelegate ) {
        appDelegate = [[CocoaApplicationDelegate alloc] init];
        sCocoaApplicationDelegate = appDelegate;
    }
    return appDelegate;
}


// event
void AppEventSetBool( Boolean value )
{ [CocoaApplicationDelegateGet() setBoolVal:value]; }

void AppEventSetLong( long value )
{ [CocoaApplicationDelegateGet() setLongVal:value]; }

CFStringRef AppEventString()
{ return (CFStringRef)[CocoaApplicationDelegateGet() stringVal]; }

CFArrayRef AppEventArray()
{ return (CFArrayRef)[CocoaApplicationDelegateGet() arrayVal]; }

CFDictionaryRef AppEventDictionary()
{ return (CFDictionaryRef)[CocoaApplicationDelegateGet() dictVal]; }

CFURLRef AppEventURL()
{ return (CFURLRef)[CocoaApplicationDelegateGet() urlVal]; }

void AppEventSetMenu( NSMenu *obj )
{ [CocoaApplicationDelegateGet() setMenuObj:obj]; }




SInt32 AppKeyWindow()
{
    NSWindow *w = [NSApp keyWindow];
    if ( w ) return [w tag];
    return 0;
}

SInt32 AppMainWindow()
{
    NSWindow *w = [NSApp mainWindow];
    if ( w ) return [w tag];
    return 0;
}

Boolean AppIsHidden()
{ return [NSApp isHidden]; }

void AppHide()
{ [NSApp hide:nil]; }

void AppUnhide()
{ [NSApp unhide:nil]; }

void AppUnhideWithoutActivation()
{ [NSApp unhideWithoutActivation]; }

Boolean AppIsActive()
{ return [NSApp isActive]; }

void AppActivateIgnoringOtherApps( Boolean flag )
{ [NSApp activateIgnoringOtherApps:flag]; }

void AppHideOtherApplications()
{ [NSApp hideOtherApplications:nil]; }

void AppUnhideAllApplications()
{ [NSApp unhideAllApplications:nil]; }

void AppShowHelp()
{ [NSApp showHelp:nil]; }


void AppSetTimer( NSTimeInterval interval, CocoaTimerCallbackType callback, BOOL repeats )
{
    CocoaApplicationDelegate *appDelegate = (CocoaApplicationDelegate *)[NSApp delegate];
    if ( appDelegate ) {
        [NSTimer scheduledTimerWithTimeInterval:interval target:appDelegate selector:@selector(timerFired:) userInfo:[NSValue valueWithPointer:callback] repeats:repeats];
    }
}


// the following two functions are identical. maybe one of them should be removed at some point. bw 20170822
void CocoaAppInit()
{
    NSApplication *app = [NSApplication sharedApplication];
    if ( ![app delegate] ) {
        CocoaApplicationDelegate *appDelegate = [[[CocoaApplicationDelegate alloc] init] autorelease];
        [app setDelegate:appDelegate];
    }
}

void CocoaInit()
{
    NSApplication *app = [NSApplication sharedApplication];
    if ( ![app delegate] ) {
        CocoaApplicationDelegate *appDelegate = [[[CocoaApplicationDelegate alloc] init] autorelease];
        [app setDelegate:appDelegate];
    }
}

SInt32 AppOutputWindow()
{ return CocoaOutputWindowTag(); }



#if __LP64__

// application
void CallUserDefinedOnAppEventFunction( long appEventKind, long appEventID ) // cocoaui 20170704 bw
{
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

#endif  // __LP64__




#pragma mark - menu
///////////////////////////////////////////////////
//                  cocoa menu                   //
///////////////////////////////////////////////////

void BuildMinimalMenu()
{
    NSString *appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
    
    NSMenu *mainMenu = [NSApp mainMenu];
    if ( !mainMenu ) mainMenu = [[[NSMenu alloc] initWithTitle:@"MainMenu"] autorelease];
    
    NSMenuItem *item = [mainMenu addItemWithTitle:appName action:nil keyEquivalent:@""];
    NSMenu *mApp = [[[NSMenu alloc] initWithTitle:appName] autorelease];
    [mainMenu setSubmenu:mApp forItem:item];
    
    [mApp addItemWithTitle:[NSString stringWithFormat:@"About %@", appName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    [mApp addItem:[NSMenuItem separatorItem]];
    [mApp addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName] action:@selector(terminate:) keyEquivalent:@"q"];
    [NSApp setMainMenu:mainMenu];
}

@implementation CocoaMenuItemTarget

@synthesize validateCallback;
@synthesize boolVal;

- (void)menuItemAction:(id)sender {
    if ( gFBOnMenuVector ) {
        NSMenu *menu = [sender menu];
        NSInteger menuIndex = [menu tag];
        if ( menuIndex <= 0 ) {
            NSMenu *supermenu = [menu supermenu];
            menuIndex = [supermenu indexOfItemWithSubmenu:menu];
        }
        NSInteger itemIndex = [menu indexOfItem:sender];
        gFBMenuEventInfo.menu = menuIndex;
        gFBMenuEventInfo.item = itemIndex;
        (*gFBOnMenuVector)(); // call user's DoMenu function
    }
}

//- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item {
//    return YES;
//}
//
//- (BOOL)validateMenuItem:(NSMenuItem *)menuItem {
//    boolVal = YES;
//    if ( gFBOnMenuVector ) {
//        NSMenu *menu = [menuItem menu];
//        if ( [menu autoenablesItems] ) {
//            NSInteger menuIndex = [menu tag];
//            if ( menuIndex <= 0 ) {
//                NSMenu *supermenu = [menu supermenu];
//                menuIndex = [supermenu indexOfItemWithSubmenu:menu];
//            }
//            NSInteger itemIndex = [menu indexOfItem:menuItem];
//            gFBMenuEventInfo.menu = -menuIndex;
//            gFBMenuEventInfo.item = itemIndex;
//            (*gFBOnMenuVector)(); // call user's DoMenu function
//        }
//    }
//    return boolVal;
//}
//

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem {
    BOOL flag = [menuItem isEnabled];
    if ( validateCallback ) {
        NSMenu *menu = [menuItem menu];
        NSInteger menuIndex = [menu tag];
        if ( menuIndex <= 0 ) {
            NSMenu *supermenu = [menu supermenu];
            menuIndex = [supermenu indexOfItemWithSubmenu:menu];
        }
        NSInteger itemIndex = [menu indexOfItem:menuItem];
        return (*validateCallback)(menuIndex,itemIndex);
    }
    return flag;
}

@end

// private
CocoaMenuItemTarget *CocoaMenuItemTargetGet()
{
    static CocoaMenuItemTarget *sCocoaMenuItemTarget = nil;
    if ( !sCocoaMenuItemTarget ) sCocoaMenuItemTarget = [[CocoaMenuItemTarget alloc] init];
    return sCocoaMenuItemTarget;
}

void MenuSetValidateItemsCallback( CocoaValidateMenuItemCallbackType callback )
{
    CocoaMenuItemTarget *target = CocoaMenuItemTargetGet();
    if ( target ) {
        target.validateCallback = callback;
    }
}



void MenuEventSetBool( Boolean flag )
{ [CocoaMenuItemTargetGet() setBoolVal:flag]; }


static NSMutableArray *sCocoaOtherMenus;

void CocoaOtherMenuAdd( NSMenu *menu )
{
    if ( sCocoaOtherMenus == nil ) sCocoaOtherMenus = [[NSMutableArray alloc] initWithCapacity:0];
    [sCocoaOtherMenus addObject:menu];
}

NSMenu *CocoaOtherMenuAtIndex( SInt32 index )
{
    if ( sCocoaOtherMenus ) {
        for ( NSMenu *menu in sCocoaOtherMenus ) {
            if ( [menu tag] == index ) return menu;
        }
        
    }
    return nil;
}

@implementation NSMenu (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

// private
void CocoaMenuPadItemsToIndex( NSMenu *menu, SInt32 index )
{
    NSInteger itemCount = [menu numberOfItems];
    if ( index > 1 ) {
        while ( itemCount < index ) {
            NSMenuItem *newItem = [menu addItemWithTitle:@"" action:nil keyEquivalent:@""];
            NSMenu *newMenu = [[NSMenu alloc] initWithTitle:@""];
            [menu setSubmenu:newMenu forItem:newItem];
            itemCount++;
        }
    }
}

NSMenu *MenuAtIndex( SInt32 index )
{
    if ( index > 100 ) {
        return CocoaOtherMenuAtIndex( index );
    } else {
        NSMenu *mainMenu = [NSApp mainMenu];
        if ( mainMenu ) {
            SInt32 count = [mainMenu numberOfItems];
            if ( index < count ) {
                NSMenuItem *item = [mainMenu itemAtIndex:index];
                return [item submenu];
            }
        }
    }
    return nil;
}

NSMenuItem *MenuItemAtIndex( NSMenu *menu, SInt32 index )
{
    SInt32 count = [menu numberOfItems];
    if ( index < count ) return [menu itemAtIndex:index];
    return nil;
}




// public
CFStringRef MenuTitle( SInt32 index )
{
    NSMenu *mainMenu = [NSApp mainMenu];
    if ( mainMenu ) {
        NSMenuItem *item = MenuItemAtIndex( mainMenu, index );
        if ( item ) return (CFStringRef)[item title];
    }
    return NULL;
}

CFStringRef MenuItemTitle( SInt32 menuIndex, SInt32 itemIndex )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        NSMenuItem *item = MenuItemAtIndex( menu, itemIndex );
        if ( item ) return (CFStringRef)[item title];
    }
    return NULL;
}

void MenuSetSubmenu( SInt32 parMenuIndex, SInt32 parItemIndex, SInt32 submenuIndex )
{
        NSMenu *parMenu = MenuAtIndex( parMenuIndex );
        if ( parMenu ) {
            NSMenuItem *parItem = [parMenu itemAtIndex:parItemIndex];
            if ( parItem ) {
                NSMenu *submenu = MenuAtIndex( submenuIndex );
                if ( submenu ) {
                    [parMenu setSubmenu:submenu forItem:parItem];
                }
            }
        }
}

void MenuSetAutoenablesItems( SInt32 menuIndex, Boolean flag )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        [menu setAutoenablesItems:flag];
    }
}


void MenuBarSetVisible( Boolean flag )
{ [NSMenu setMenuBarVisible:flag]; }

CGFloat MenuBarHeight()
{ return [[NSApp mainMenu] menuBarHeight]; }

void MenuRemoveItem( SInt32 menuIndex, SInt32 itemIndex )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        [menu removeItemAtIndex:itemIndex];
    }
}

void MenuRemoveAllItems( SInt32 menuIndex )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        [menu removeAllItems];
    }
}

SInt32 MenuItemWithTitle( SInt32 menuIndex, CFStringRef title )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        return [menu indexOfItemWithTitle:(NSString *)title];
    }
    return 0;
}

SInt32 MenuNumberOfItems( SInt32 menuIndex )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) return [menu numberOfItems];
    return 0;
}

Boolean MenuPopUp( SInt32 menuIndex, SInt32 itemIndex, CGPoint location, SInt32 viewTag )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        NSMenuItem *item = [menu itemAtIndex:itemIndex];
        if ( item ) {
            NSView *view = ViewWithTag( viewTag );
            if ( view ) {
                return [menu popUpMenuPositioningItem:item atLocation:NSPointFromCGPoint(location) inView:view];
            }
        }
    }
    return NO;
}

void MenuSetShowsStateColumn( SInt32 menuIndex, Boolean flag )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        [menu setShowsStateColumn:flag];
    }
}

void MenuSetFont( SInt32 menuIndex, CTFontRef font )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        [menu setFont:(NSFont *)font];
    }
}

void MenuItemSetState( SInt32 menuIndex, SInt32 itemIndex, NSCellStateValue state )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        NSMenuItem *item = MenuItemAtIndex( menu, itemIndex );
        if ( item ) [item setState:state];
    }
}

// custom
void MenuSetTag( SInt32 menuIndex, SInt32 itemIndex, SInt32 tag )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        NSMenuItem *item = nil;
        if ( itemIndex < 0 ) {  // set menu tag
            item = [[menu supermenu] itemAtIndex:menuIndex];
        } else {                // set item tag
            item = MenuItemAtIndex( menu, itemIndex );
        }
        if ( item ) [item setTag:tag];
    }
}

void MenuItemSetAction( SInt32 menuIndex, SInt32 itemIndex, CFStringRef actionName )
{
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        NSMenuItem *item = MenuItemAtIndex( menu, itemIndex );
        if ( item ) {
            [item setTarget:nil];
            [item setAction:NSSelectorFromString((NSString *)actionName)];
        }
    }
}

void MenuSetFontWithName( SInt32 menuIndex, CFStringRef inName, CGFloat size )
{
    BOOL usePrevFont = NO;
    NSString *name = (NSString *)inName;
    NSMenu *menu = MenuAtIndex( menuIndex );
    if ( menu ) {
        if ( name ) {
            usePrevFont = ( [name length] == 0 );
        } else {
            usePrevFont = YES;
        }
        if ( usePrevFont ) {
            name = [[menu font] fontName];
        }
        if ( (int)size == 0 ) size = [[menu font] pointSize];
        [menu setFont:[NSFont fontWithName:(NSString *)name size:size]];
    }
}


void CocoaMenu( SInt32 inMenuIndex, SInt32 inItemIndex, SInt32 inEnabled, CFStringRef inTitle, CFStringRef inKeyEquivalent, SInt32 inKeyEquivalentModifier )
{
    if ( !IsCocoaApplication() ) {
        CocoaShowAlert( NSCriticalAlertStyle, @"Runtime error.", @"Cocoa Menu can only be used with Cocoa runtime." );
        return;
    }
    
    // title
    NSString *title = (NSString *)inTitle;
    
    // keyEquivalent
    NSString *keyEquivalent = (NSString *)inKeyEquivalent;
    if ( !keyEquivalent ) keyEquivalent = @"";
    
    // make sure we have at least a minimal menu
    if ( ![NSApp mainMenu] ) {
        BuildMinimalMenu();
    }
    
    // get the main menu
    NSMenu *mainMenu = [NSApp mainMenu];
    if ( mainMenu ) {
        
        if ( inMenuIndex == 0 ) { // application menu
            NSMenu *appMenu = [[mainMenu itemAtIndex:0] submenu];
            
            NSMenuItem *item = [appMenu itemAtIndex:inItemIndex];
            
            BOOL userItemExists = NO;
            if ( item ) {
                if ( [item tag] ) {
                    userItemExists = YES;
                }
            }
            
            if ( userItemExists ) {   // items exists
                
                // enabled
                if ( inEnabled != kFBParamMissing ) [item setEnabled:inEnabled];
                
                // title
                if ( title ) [item setTitle:title];
                
                // keyEquivalent
                if ( [keyEquivalent length] ) [item setKeyEquivalent:keyEquivalent];
                
                // keyEquivalentModifierMask
                if ( inKeyEquivalentModifier != kFBParamMissing ) [item setKeyEquivalentModifierMask:inKeyEquivalentModifier];
                
                // target
                if ( ![item target] ) [item setTarget:CocoaMenuItemTargetGet()];
                
                // action
                if ( ![item action] ) [item setAction:@selector(menuItemAction:)];
                
            } else {        // create item
                
                NSInteger index = 1;
                while ( [[appMenu itemAtIndex:index] tag] != 0 ) {
                    index++;
                }
                
                if ( index ) {
                    BOOL separator = ( title == NULL || [title isEqualToString:@"-"] );
                    
                    NSMenuItem *newAppItem;
                    
                    if ( separator ) {
                        newAppItem = [NSMenuItem separatorItem];
                    } else {
                        newAppItem = [[NSMenuItem alloc] initWithTitle:title action:@selector(menuItemAction:) keyEquivalent:keyEquivalent];
                        
                        // key equivalent modifier
                        if ( inKeyEquivalentModifier != kFBParamMissing ) [newAppItem setKeyEquivalentModifierMask:inKeyEquivalentModifier];
                        [newAppItem setTarget:CocoaMenuItemTargetGet()];
                    }
                    [newAppItem setTag:-123];
                    [appMenu insertItem:newAppItem atIndex:index];
                }
            }
            
        } else {        // menu other than the application menu
            
            BOOL otherMenuFlag = ( inMenuIndex > 100 ); // non-menubar menu
            
            NSInteger mainMenuItemCount = [mainMenu numberOfItems];
            NSInteger itemCount = 0;
            NSMenu *menu = nil;
            NSMenuItem *menuItem = nil;

            if ( otherMenuFlag ) {           // other menu
                menu = CocoaOtherMenuAtIndex( inMenuIndex );
            } else {                         // menu bar menu
                
                itemCount = mainMenuItemCount;
                if ( inMenuIndex < itemCount ) {
                    menuItem = [mainMenu itemAtIndex:inMenuIndex];
                    menu = [menuItem submenu];
                }
            }
            
            if ( menu ) {
                if ( inItemIndex < 0 ) {    // we're changing the menu itself
                    
                    if ( menuItem ) {   // main menu item
                        // enabled
                        if ( inEnabled != kFBParamMissing ) [menuItem setEnabled:inEnabled];
                    }
                    
                    // title
                    if ( title ) [menu setTitle:title];
                    
                } else {    // add/change item
                    
                    NSMenuItem *item = nil;
                    
                    NSInteger itemCount = [menu numberOfItems];
                    
                    if ( inItemIndex < itemCount ) {   // item exists
                        
                        item = [menu itemAtIndex:inItemIndex];
                        
                        // enabled
                        if ( inEnabled != kFBParamMissing ) [item setEnabled:inEnabled];
                        
                        // title
                        if ( title ) [item setTitle:title];
                        
                        // keyEquivalent
                        if ( [keyEquivalent length] ) [item setKeyEquivalent:keyEquivalent];
                        
                        // keyEquivalentModifierMask
                        if ( inKeyEquivalentModifier != kFBParamMissing ) [item setKeyEquivalentModifierMask:inKeyEquivalentModifier];
                        
                        // target
                        if ( ![item target] ) [item setTarget:CocoaMenuItemTargetGet()];
                        
                        // action
                        if ( ![item action] ) [item setAction:@selector(menuItemAction:)];
                        
                    } else {        // create item
                        
                        while ( inItemIndex > itemCount ) {
                            [menu addItem:[NSMenuItem separatorItem]];
                            itemCount = [menu numberOfItems];
                        }
                        
                        BOOL separator = ( title == NULL || [title isEqualToString:@"-"] );
                        
                        if ( separator ) {
                            item = [NSMenuItem separatorItem];
                        } else {
                            item = [[NSMenuItem alloc] initWithTitle:title action:@selector(menuItemAction:) keyEquivalent:keyEquivalent];
                            
                            // key equivalent modifier
                            if ( inKeyEquivalentModifier != kFBParamMissing ) [item setKeyEquivalentModifierMask:inKeyEquivalentModifier];
                            
                            // enabled
                            if ( inEnabled != kFBParamMissing ) [item setEnabled:inEnabled];
                            
                            [item setTarget:CocoaMenuItemTargetGet()];
                        }
                        [menu insertItem:item atIndex:inItemIndex];
                    }
                    
                }
                
            } else {
                
                if ( inItemIndex >= 0 ) return;    // we're creating a new menu here, so item index must be negative
                
                if ( !title ) title = @"";
                
                NSMenu *menu = [[NSMenu alloc] initWithTitle:title];
                
                if ( otherMenuFlag ) {
                    [menu setTag:inMenuIndex];
                    CocoaOtherMenuAdd( menu );
                } else {
                    
                    CocoaMenuPadItemsToIndex( mainMenu, inMenuIndex );// fill unused items

                    //NSMenuItem *menuItem = [mainMenu addItemWithTitle:title action:nil keyEquivalent:@""];

                    NSMenuItem *menuItem = [mainMenu insertItemWithTitle:title action:nil keyEquivalent:@"" atIndex:inMenuIndex];
                    
                    [mainMenu setSubmenu:menu forItem:menuItem];
                }
                
                [menu setAutoenablesItems:NO];
            }
            
        }
    }
}

#pragma mark - editmenu
///////////////////////////////////////////////////
//                  cocoa editmenu               //
///////////////////////////////////////////////////

void CocoaEditMenu( SInt32 inMenuIndex )
{
    if ( !IsCocoaApplication() ) {
        CocoaShowAlert( NSCriticalAlertStyle, @"Runtime error.", @"EditMenu can only be used with Cocoa runtime." );
        return;
    }
    
    // make sure we have at least a minimal menu
    if ( ![NSApp mainMenu] ) BuildMinimalMenu();
    
    // get the main menu
    NSMenu *mainMenu = [NSApp mainMenu];
    if ( mainMenu ) {
        
        CocoaMenuPadItemsToIndex( mainMenu, inMenuIndex );// fill unused items
        
        NSMenuItem *editMenuItem = [mainMenu addItemWithTitle:@"Edit" action:nil keyEquivalent:@""];

        NSMenu *mEdit = [[NSMenu alloc] initWithTitle:@"Edit"];
        [mainMenu setSubmenu:mEdit forItem:editMenuItem];
        
        [mEdit addItemWithTitle:@"Undo" action:@selector(undo:) keyEquivalent:@"z"];
        [mEdit addItemWithTitle:@"Redo" action:@selector(redo:) keyEquivalent:@"Z"];
        [mEdit addItem:[NSMenuItem separatorItem]];
        [mEdit addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
        [mEdit addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
        [mEdit addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];
        [mEdit addItemWithTitle:@"Delete" action:@selector(delete:) keyEquivalent:@""];
        [mEdit addItemWithTitle:@"Select All" action:@selector(selectAll:) keyEquivalent:@"a"];
    }
}

#pragma mark - windowmenu
///////////////////////////////////////////////////
//               cocoa windowmenu               //
///////////////////////////////////////////////////

void CocoaWindowMenu( SInt32 inMenuIndex )
{
    if ( !IsCocoaApplication() ) {
        CocoaShowAlert( NSCriticalAlertStyle, @"Runtime error.", @"WindowsMenu can only be used with Cocoa runtime." );
        return;
    }
    
    // make sure we have at least a minimal menu
    if ( ![NSApp mainMenu] ) BuildMinimalMenu();
    
    NSMenu *mainMenu = [NSApp mainMenu];
    if ( mainMenu ) {
        // fill unused items
        CocoaMenuPadItemsToIndex( mainMenu, inMenuIndex );

        NSMenuItem *windowMenuItem = [mainMenu addItemWithTitle:@"Window" action:nil keyEquivalent:@""];
        NSMenu *windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
        [mainMenu setSubmenu:windowMenu forItem:windowMenuItem];
        [windowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
        [NSApp setWindowsMenu:windowMenu];
    }
}


#pragma mark - cocoa event    // 20180829 bw
///////////////////////////////////////////////////
//                  cocoa event                 //
///////////////////////////////////////////////////

@implementation CocoaEvent
@synthesize nsEvent;
@end

// private
CocoaEvent *CocoaEventObj()
{
    static CocoaEvent *sCocoaEvent = nil;
    if ( !sCocoaEvent ) sCocoaEvent = [[CocoaEvent alloc] init];
    return sCocoaEvent;
}

void CocoaEventSetEvent( NSEvent *event )
{ [CocoaEventObj() setNsEvent:event]; }


// public
CGPoint EventLocationInWindow()
{ return NSPointToCGPoint([[CocoaEventObj() nsEvent] locationInWindow]); }

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
NSEventModifierFlags EventModifierFlags()
{ return [[CocoaEventObj() nsEvent] modifierFlags]; }
#else
UInt32 EventModifierFlags()
{ return [[CocoaEventObj() nsEvent] modifierFlags]; }
#endif

CFTimeInterval EventTimestamp()
{ return [[CocoaEventObj() nsEvent] timestamp]; }

NSEventType eventType()// lowercase 'e' to avoid conflict with Carbon's 'EventType'
{ return [[CocoaEventObj() nsEvent] type]; }

void *EventWindow()
{ return [[CocoaEventObj() nsEvent] window]; }

CFTimeInterval EventKeyRepeatDelay()
{ return [NSEvent keyRepeatDelay]; }

CFTimeInterval EventKeyRepeatInterval()
{ return [NSEvent keyRepeatInterval]; }

CFStringRef EventCharacters()
{ return (CFStringRef)[[CocoaEventObj() nsEvent] characters]; }

CFStringRef EventCharactersIgnoringModifiers()
{ return (CFStringRef)[[CocoaEventObj() nsEvent] charactersIgnoringModifiers]; }

Boolean EventIsARepeat()
{ return [[CocoaEventObj() nsEvent] isARepeat]; }

unsigned short EventKeyCode()
{ return [[CocoaEventObj() nsEvent] keyCode]; }

UInt32 EventPressedMouseButtons()
{ return [NSEvent pressedMouseButtons]; }

CFTimeInterval EventDoubleClickInterval()
{ return [NSEvent doubleClickInterval]; }

CGPoint EventMouseLocation()
{ return NSPointToCGPoint([NSEvent mouseLocation]); }

SInt32 EventClickCount()
{ return [[CocoaEventObj() nsEvent] clickCount]; }


// custom
CGPoint EventLocationInView( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        return NSPointToCGPoint([view convertPoint:[[CocoaEventObj() nsEvent] locationInWindow] fromView:nil]);
    }
    return CGPointZero;
}



#pragma mark - dialog event     // 20180825 bw
///////////////////////////////////////////////////
//                  dialog event                 //
///////////////////////////////////////////////////

@implementation CocoaDialogEvent

@synthesize boolVal;
@synthesize longVal;
@synthesize floatVal;
@synthesize doubleVal;
@synthesize sizeVal;
@synthesize rectVal;
@synthesize viewTag;
//@synthesize cgContext;
//@synthesize nsEvent;

@end

CocoaDialogEvent *CocoaDialogEventObj()
{
    static CocoaDialogEvent *sCocoaDialogEvent = nil;
    if ( !sCocoaDialogEvent ) sCocoaDialogEvent = [[CocoaDialogEvent alloc] init];
    return sCocoaDialogEvent;
}

// NS
NSSize DialogEventNSSize()
{ return [CocoaDialogEventObj() sizeVal]; }

void DialogEventSetNSSize( NSSize value )
{ [CocoaDialogEventObj() setSizeVal:value]; }

NSRect DialogEventNSRect()
{ return [CocoaDialogEventObj() rectVal]; }

void DialogEventSetNSRect( NSRect value )
{ [CocoaDialogEventObj() setRectVal:value]; }


// non-NS
BOOL DialogEventBool()
{ return [CocoaDialogEventObj() boolVal]; }

void DialogEventSetBool( BOOL value )
{ [CocoaDialogEventObj() setBoolVal:value]; }

long DialogEventLong()
{ return [CocoaDialogEventObj() longVal]; }

void DialogEventSetLong( long value )
{ [CocoaDialogEventObj() setLongVal:value]; }

float DialogEventFloat()
{ return [CocoaDialogEventObj() floatVal]; }

void DialogEventSetFloat( float value )
{ [CocoaDialogEventObj() setFloatVal:value]; }

double DialogEventDouble()
{ return [CocoaDialogEventObj() doubleVal]; }

void DialogEventSetDouble( double value )
{ [CocoaDialogEventObj() setDoubleVal:value]; }

CGSize DialogEventSize()
{ return NSSizeToCGSize([CocoaDialogEventObj() sizeVal]); }

void DialogEventSetSize( CGSize value )
{ [CocoaDialogEventObj() setSizeVal:NSSizeFromCGSize(value)]; }

CGRect DialogEventRect()
{ return NSRectToCGRect([CocoaDialogEventObj() rectVal]); }

void DialogEventSetRect( CGRect value )
{ [CocoaDialogEventObj() setRectVal:NSRectFromCGRect(value)]; }

SInt32 DialogEventViewTag()
{ return [CocoaDialogEventObj() viewTag]; }

void DialogEventSetViewTag( SInt32 value )
{ [CocoaDialogEventObj() setViewTag:value];}


#pragma mark - alert
///////////////////////////////////////////////////
//                  cocoa alert                  //
///////////////////////////////////////////////////

static NSMutableArray *sCocoaAlerts;

@implementation CocoaAlertDelegate

@synthesize boolVal;

#pragma mark - alert delegate methods
- (BOOL)alertShowHelp:(NSAlert *)alert {
    DialogEventSetBool( NO );
    CallUserDefinedOnDialogFunction( alertShowHelp, [alert tag], [alert tag] );
    return DialogEventBool();
}

- (void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo {
    CallUserDefinedOnDialogFunction( alertDidEnd, returnCode, [alert tag] );
}

@end

CocoaAlertDelegate *CocoaAlertDelegateGet()
{
    static CocoaAlertDelegate *sCocoaAlertDelegate = nil;
    if ( !sCocoaAlertDelegate ) sCocoaAlertDelegate = [[CocoaAlertDelegate alloc] init];
    return sCocoaAlertDelegate;
}


NSAlert *CocoaAlertWithTag( SInt32 tag )
{
    if ( sCocoaAlerts ) {
        for ( NSAlert *alert in sCocoaAlerts ) {
            if ( [alert tag] == tag ) return alert;
        }
    }
    return nil;
}

void CocoaAlertAdd( NSAlert *alert )
{
    if ( !sCocoaAlerts ) sCocoaAlerts = [[NSMutableArray alloc] initWithCapacity:0];
    [sCocoaAlerts addObject:alert];
}

void AlertRemove( SInt32 tag )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [sCocoaAlerts removeObject:alert];
    }
}


@implementation NSAlert (Additions)
@dynamic tagNumber;
@dynamic sheetNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (BOOL)sheet {
    return [objc_getAssociatedObject(self, @selector(sheetNumber)) boolValue];
}

- (void)setSheet:(BOOL)sheet {
    objc_setAssociatedObject(self, @selector(sheetNumber), [NSNumber numberWithBool:sheet], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

void AlertSetAccessoryView( SInt32 alertTag, SInt32 viewTag )
{
    NSAlert *alert = CocoaAlertWithTag( alertTag );
    if ( alert ) {
        NSView *view = CocoaViewWithTag( viewTag );
        if ( view ) {
            [alert setAccessoryView:view];
        }
    }
}

void AlertSetShowsHelp( SInt32 tag, Boolean flag )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [alert setShowsHelp:flag];
    }
}

CFStringRef AlertHelpAnchor( SInt32 tag )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        return (CFStringRef)[alert helpAnchor];
    }
    return nil;
}

void AlertSetHelpAnchor( SInt32 tag, CFStringRef anchorName )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [alert setHelpAnchor:(NSString *)anchorName];
    }
}

void AlertSetShowsSuppressionButton( SInt32 tag, Boolean flag )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [alert setShowsSuppressionButton:flag];
    }
}

void AlertSuppressionButtonSetState( SInt32 tag, NSCellStateValue value )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [[alert suppressionButton] setState:value];
    }
}

NSCellStateValue AlertSuppressionButtonState( SInt32 tag )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        return [[alert suppressionButton] state];
    }
    return NSOffState;
}

void AlertSuppressionButtonSetTitle( SInt32 tag, CFStringRef title )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [[alert suppressionButton] setTitle:(NSString *)title];
    }
}

void AlertSetIcon( SInt32 tag, CFStringRef imageName )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        [alert setIcon:[NSImage imageNamed:(NSString *)imageName]];
    }
}

void AlertSetIconCGImage( SInt32 tag, CGImageRef cgImage )
{
    NSAlert *alert = CocoaAlertWithTag( tag );
    if ( alert ) {
        NSImage *image = [[NSImage alloc] initWithCGImage:cgImage size:NSZeroSize];
        if ( image ) {
            [alert setIcon:image];
            [image release];
        }
    }
}



NSInteger CocoaAlert( SInt32 inTag, SInt32 inStyle, CFStringRef inMsg, CFStringRef inInfo, CFTypeRef inButtonTitles, SInt32 inSheetFlag )
{
    NSInteger response = 0;
    
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *msg = (NSString *)inMsg;
    NSString *info = (NSString *)inInfo;
    
    NSAlert *alert = CocoaAlertWithTag( inTag );
    
    if ( alert ) {    // exists
        
        // style
        if ( inStyle != kFBParamMissing ) {
            [alert setAlertStyle:(NSAlertStyle)inStyle];
        }

        if ( inSheetFlag != kFBParamMissing ) [alert setSheet:inSheetFlag];
        
    } else {        // create
        
        // init
        alert = [[NSAlert alloc] init];
        
        // style
        if ( inStyle == kFBParamMissing ) {
            [alert setAlertStyle:NSWarningAlertStyle];
        } else {
            [alert setAlertStyle:(NSAlertStyle)inStyle];
        }
        
        // button titles
        if ( inButtonTitles ) {
            NSArray *titles = nil;
            
            if ( CFGetTypeID(inButtonTitles) == CFStringGetTypeID() ) {
                titles = [(NSString *)inButtonTitles componentsSeparatedByString:@";"];
                
            } else if ( CFGetTypeID(inButtonTitles) == CFArrayGetTypeID() ) {
                titles = (NSArray *)inButtonTitles;
            }
            
            if ( titles ) {
                for ( NSString *title in titles ) {
                    [alert addButtonWithTitle:title];
                }
            }
        }
        
        // sheet
        if ( inSheetFlag != kFBParamMissing ) {
            [alert setSheet:inSheetFlag];
        }
        
        // delegate
        [alert setDelegate:CocoaAlertDelegateGet()];

        // tag
        [alert setTag:inTag];
        
        // add to sheets array
        CocoaAlertAdd( alert );
    }
    
    // msg
    if ( msg != nil ) [alert setMessageText:msg];
    
    // info
    if ( info != nil ) [alert setInformativeText:info];
    
    if ( show ) {
        // get front window
        SInt32 wndTag = 0;
        NSWindow *window = CocoaTargetWindow( &wndTag );

        if ( [alert sheet] && window ) {  // sheet
            
            // get delegate
            CocoaAlertDelegate *delegate = CocoaAlertDelegateGet();
            
            // beginSheet
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1080
            [alert beginSheetModalForWindow:window completionHandler:^(NSModalResponse returnCode) {
                [delegate alertDidEnd:alert returnCode:returnCode contextInfo:nil];
            }];
#else
            [alert beginSheetModalForWindow:window modalDelegate:delegate didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:nil];
#endif
        } else {
            
            response = [alert runModal];
            CallUserDefinedOnDialogFunction( alertDidEnd, response, [alert tag] );

        }
        
    } else {    // hide
        
        // .....
        
    }
    
    return response;
}


#pragma mark - control
///////////////////////////////////////////////////
//                  cocoa control                //
///////////////////////////////////////////////////

@implementation NSControl (Additions)
@dynamic callbackValue;

- (CocoaControlCallbackType)cocoaControlCallback {
    return [objc_getAssociatedObject(self, @selector(callbackValue)) pointerValue];
}

- (void)setCocoaControlCallback:(CocoaControlCallbackType)callback {
    objc_setAssociatedObject(self, @selector(callbackValue), [NSValue valueWithPointer:callback], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end


@implementation CocoaControlTarget

@synthesize allControlsCallback;

- (void)controlAction:(id)sender {
    CocoaOutputWindowSetTag( [sender wndTag] );
    
    if ( [(NSControl *)sender cocoaControlCallback] ) {
        CocoaControlCallbackType callback = [(NSControl *)sender cocoaControlCallback];
        (*callback)([sender tag],[sender wndTag],1);
    } else if ( allControlsCallback ) {
        (*allControlsCallback)([sender tag],[sender wndTag],1);
    } else {
        CallUserDefinedOnDialogFunction( 1, [sender tag], [sender wndTag] );// 1 == kFBBtnClick
    }
    
}
@end

// private
CocoaControlTarget *CocoaControlTargetObj()
{
    static CocoaControlTarget *sCocoaControlTarget = nil;
    if ( !sCocoaControlTarget ) sCocoaControlTarget = [[CocoaControlTarget alloc] init];
    return sCocoaControlTarget;
}

void CocoaControlCallUserDefinedOnDialogOrCallback( SInt32 ev, id sender  )
{
    CocoaOutputWindowSetTag( [sender wndTag] );

    CocoaControlTarget *target = CocoaControlTargetObj();
    CocoaControlCallbackType allControlsCallback = target.allControlsCallback;
    
    if ( [(NSControl *)sender cocoaControlCallback] ) {
        CocoaControlCallbackType callback = [(NSControl *)sender cocoaControlCallback];
        (*callback)(ev,[sender tag],[sender wndTag]);
    } else if ( allControlsCallback ) {
        (*allControlsCallback)(ev,[sender tag],[sender wndTag]);
    } else {
        CallUserDefinedOnDialogFunction( ev, [sender tag], [sender wndTag] );
    }
}



// private
// fix font to match I.B.
void CocoaControlFixFont( id c )
{ [c setFont:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:[[c cell] controlSize]]]]; }


// public
double ControlDoubleValue( SInt32 tag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) return [c doubleValue];
    return 0.0;
}

void ControlSetDoubleValue( SInt32 tag, double value )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setDoubleValue:value];
}

SInt32 ControlIntegerValue( SInt32 tag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) return [c integerValue];
    return 0;
}

void ControlSetIntegerValue( SInt32 tag, SInt32 value )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setIntegerValue:value];
}

CFStringRef ControlStringValue( SInt32 tag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) return (CFStringRef)[c stringValue];
    return NULL;
}

void ControlSetStringValue( SInt32 tag, CFStringRef string )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setStringValue:(NSString *)string];
}

void ControlSetSize( SInt32 tag, NSControlSize size )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) {
        [[c cell] setControlSize:size];
        [c setFont:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:size]]];
    }
}

void ControlSetAlignment( SInt32 tag, NSTextAlignment alignment )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setAlignment:alignment];
}


void ControlSizeToFit( SInt32 tag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c sizeToFit];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void ControlSetLineBreakMode( SInt32 tag, NSLineBreakMode mode )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setLineBreakMode:mode];
}
#endif

void ControlSetFont( SInt32 tag, CTFontRef font )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setFont:(NSFont *)font];
}

void ControlSetAttributedStringValue( SInt32 tag, CFAttributedStringRef string )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setAttributedStringValue:(NSAttributedString *)string];
}

void ControlTakeDoubleValueFrom( SInt32 tag, SInt32 fromViewTag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) {
        id fromView = (id)CocoaViewWithTag( fromViewTag );
        if ( fromView ) {
            [c takeDoubleValueFrom:fromView];
        }
    }
}

void ControlTakeIntegerValueFrom( SInt32 tag, SInt32 fromViewTag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) {
        id fromView = (id)CocoaViewWithTag( fromViewTag );
        if ( fromView ) {
            [c takeIntegerValueFrom:fromView];
        }
    }
}

void ControlTakeStringValueFrom( SInt32 tag, SInt32 fromViewTag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) {
        id fromView = (id)CocoaViewWithTag( fromViewTag );
        if ( fromView ) [c takeStringValueFrom:fromView];
    }
}

void ControlSetEnabled( SInt32 tag, Boolean flag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setEnabled:flag];
}

Boolean ControlIsEnabled( SInt32 tag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) return [c isEnabled];
    return false;
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
CGSize ControlSizeThatFits( SInt32 tag, CGSize size )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) return NSSizeToCGSize([c sizeThatFits:NSSizeFromCGSize(size)]);
    return CGSizeZero;
}
#endif

void ControlSetContinuous( SInt32 tag, Boolean flag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c setContinuous:flag];
}

void ControlPerformClick( SInt32 tag )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) [c performClick:nil];
}


// custom
void ControlSetFontWithName( SInt32 tag, CFStringRef inName, CGFloat size )
{
    BOOL usePrevFont = NO;
    NSString *name = (NSString *)inName;
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) {
        if ( name ) {
            usePrevFont = ( [name length] == 0 );
        } else {
            usePrevFont = YES;
        }
        if ( usePrevFont ) name = [[c font] fontName];
        if ( (int)size == 0 ) size = [[c font] pointSize];
        [c setFont:[NSFont fontWithName:(NSString *)name size:size]];
    }
}

void ControlSetAction( SInt32 tag, CFStringRef actionName )
{
    NSControl *c = (NSControl *)CocoaViewWithTag( tag );
    if ( c ) {
        [c setTarget:nil];
        [c setAction:NSSelectorFromString((NSString *)actionName)];
    }
}

void ControlSetActionCallback( SInt32 tag, void *callback )
{
    if ( tag == 0 ) {   // all controls
        CocoaControlTarget *target = CocoaControlTargetObj();
        [target setAllControlsCallback:callback];
    } else {            // individual control
        NSControl *c = (NSControl *)CocoaViewWithTag( tag );
        if ( c ) {
            [c setCocoaControlCallback:callback];
        }
    }
}




#pragma mark - box
///////////////////////////////////////////////////
//                  cocoa box                    //
///////////////////////////////////////////////////

@implementation NSBox (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

// private
NSBox *BoxWithTag( SInt32 tag )
{
    NSBox *view = (NSBox *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSBox class] ) return view;
    }
    return nil;
}

void BoxSetBorderType( SInt32 tag, SInt32 type )
{
    NSBox *view = BoxWithTag( tag );
    if ( view ) [(NSBox *)view setBorderType:type];
}

void BoxSetTitlePosition( SInt32 tag, SInt32 position )
{
    NSBox *view = BoxWithTag( tag );
    if ( view ) [(NSBox *)view setTitlePosition:position];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void BoxSetBorderColor( SInt32 tag, CGColorRef col )
{
    NSBox *view = BoxWithTag( tag );
    if ( view ) {
        if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
            [(NSBox *)view setBorderColor:[NSColor colorWithCGColor:col]];  // macOS 10.8
        }
    }
}

void BoxSetFillColor( SInt32 tag, CGColorRef col )
{
    NSBox *view = BoxWithTag( tag );
    if ( view ) {
        if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
            [(NSBox *)view setFillColor:[NSColor colorWithCGColor:col]];  // macOS 10.8
        }
    }
}

#endif

void BoxSetBorderWidth( SInt32 tag, CGFloat width )
{
    NSBox *view = BoxWithTag( tag );
    if ( view ) [(NSBox *)view setBorderWidth:width];
}

void BoxSetCornerRadius( SInt32 tag, CGFloat radius )
{
    NSBox *view = BoxWithTag( tag );
    if ( view ) [(NSBox *)view setCornerRadius:radius];
}

void BoxSetTransparent( SInt32 tag, Boolean flag )
{
    NSBox *box = BoxWithTag( tag );
    if ( box ) [box setTransparent:flag];
}

void BoxSetTitleFont( SInt32 tag, CTFontRef font )
{
    NSBox *box = BoxWithTag( tag );
    if ( box ) [box setTitleFont:(NSFont *)font];
}

void BoxSizeToFit( SInt32 tag )
{
    NSBox *box = BoxWithTag( tag );
    if ( box ) [box sizeToFit];
}

// custom
void BoxSetBorderRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSBox *box = BoxWithTag( tag );
    if ( box ) {
        [box setBorderColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]];
    }
}

void BoxSetFillRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSBox *box = BoxWithTag( tag );
    if ( box ) {
        [box setFillColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]];
    }
}

void BoxSetTitleFontWithName( SInt32 tag, CFStringRef inName, CGFloat size )
{
    BOOL usePrevFont = NO;
    NSString *name = (NSString *)inName;
    NSBox *box = BoxWithTag( tag );
    if ( box ) {
        if ( name ) {
            usePrevFont = ( [name length] == 0 );
        } else {
            usePrevFont = YES;
        }
        if ( usePrevFont ) {
            name = [[box titleFont] fontName];
        }
        if ( (int)size == 0 ) size = [[box titleFont] pointSize];
        [box setTitleFont:[NSFont fontWithName:(NSString *)name size:size]];
    }
}

void CocoaBox( SInt32 inTag, CFStringRef inTitle, CGRect inRect, SInt32 inType, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *title = (NSString *)inTitle;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        
        NSBox *box = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( box ) {    // already exists
            
            // make sure it's same view type
            if ( [box class] != [NSBox class] ) return;
            
            // title
            if ( title ) [box setTitle:title];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [box setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 17, 16, 177, 114 );
            }
            
            // init
            box = [[NSBox alloc] initWithFrame:rect];
            
            // default autoresizing
            [box setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [box setTag: inTag];
            
            // title
            if ( title ) [box setTitle:title];
            
            // type
            if ( inType != kFBParamMissing ) [box setBoxType:inType];
            
            // wndTag
            [box setWndTag:inWndTag];
            
            // add to window
            [contentView addSubview:box];
            [box release];
        }
        
        [box setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - button
/////////////////////////////////////////////////////
////                  cocoa button                 //
/////////////////////////////////////////////////////

// private
NSButton *ButtonWithTag( SInt32 tag )
{
    NSButton *btn = (NSButton *)CocoaViewWithTag( tag );
    if ( btn ) {
        if ( [btn class] == [NSButton class] ) return btn;
    }
    return nil;
}

// user
SInt32 ButtonState( SInt32 tag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) return [btn state];
    return 0;
}

Boolean ButtonIsEnabled( SInt32 tag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) return [btn isEnabled];
    return false;
}

CFStringRef ButtonTitle( SInt32 tag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) return (CFStringRef)[btn title];
    return NULL;
}

void ButtonSetSound( SInt32 tag, CFStringRef sndName )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setSound:[NSSound soundNamed:(NSString *)sndName]];
}

void ButtonSetKeyEquivalent( SInt32 tag, CFStringRef key )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setKeyEquivalent:(NSString *)key];
}

void ButtonSetKeyEquivalentModifierMask( SInt32 tag, SInt32 mask )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setKeyEquivalentModifierMask:mask];
}

void ButtonSetCGImage( SInt32 tag, CGImageRef cgImage )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) {
        NSImage *image = [[NSImage alloc] initWithCGImage:cgImage size:NSZeroSize];
        if ( image ) {
            [btn setImage:image];
            [image release];
        }
    }
}

void ButtonSetImage( SInt32 tag, CFStringRef imageName )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) {
        [btn setImage:[NSImage imageNamed:(NSString *)imageName]];
    }
}

void ButtonSetImagePosition( SInt32 tag, NSCellImagePosition position )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setImagePosition:position];
}

void ButtonSetImageScaling( SInt32 tag, NSImageScaling scaling )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [[btn cell] setImageScaling:scaling];
}

void ButtonSetBordered( SInt32 tag, Boolean flag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setBordered:flag];
}

void ButtonSetShowsBorderOnlyWhileMouseInside( SInt32 tag, Boolean flag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setShowsBorderOnlyWhileMouseInside:flag];
}

void ButtonPerformClick( SInt32 tag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn performClick:nil];
}

void ButtonSetAttributedTitle( SInt32 tag, CFAttributedStringRef title )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setAttributedTitle:(NSAttributedString *)title];
}

void ButtonSetAlternateTitle( SInt32 tag, CFStringRef title )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setAlternateTitle:(NSString *)title];
}

void ButtonSetPeriodicDelay( SInt32 tag, float delay, float interval )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setPeriodicDelay:delay interval:interval];
}

void ButtonSetAttributedAlternateTitle( SInt32 tag, CFAttributedStringRef title )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setAttributedAlternateTitle:(NSAttributedString *)title];
}

void ButtonSetAlternateImage( SInt32 tag, CFStringRef imageName )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) {
        [btn setAlternateImage:[NSImage imageNamed:(NSString *)imageName]];
    }
}

void ButtonSetTransparent( SInt32 tag, Boolean flag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setTransparent:flag];
}

void ButtonSetAllowsMixedState( SInt32 tag, Boolean flag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setAllowsMixedState:flag];
}

void ButtonSetNextState( SInt32 tag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn setNextState];
}

void ButtonHighlight( SInt32 tag, Boolean flag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn highlight:flag];
}

void ButtonSetHighlightsBy( SInt32 tag, UInt32 mask )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [[btn cell] setHighlightsBy:mask];
}


// custom
void ButtonClose( SInt32 tag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [btn removeFromSuperview];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void ButtonSetTitleColor( SInt32 tag, CGColorRef col )
{
    if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
        NSButton *btn = ButtonWithTag( tag );
        if ( btn ) {
            NSColor *color = [NSColor colorWithCGColor:col];  // macOS 10.8
            NSMutableAttributedString *as = [[btn attributedTitle] mutableCopy];
            [as addAttribute:NSForegroundColorAttributeName value:color range:NSMakeRange(0,[as length])];
            [btn setAttributedTitle:as];
            [as release];
        }
    }
}

void ButtonSetBackgroundColor( SInt32 tag, CGColorRef col )
{
    if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
        NSButton *btn = ButtonWithTag( tag );
        if ( btn ) {
            [[btn cell] setBackgroundColor:[NSColor colorWithCGColor:col]];
        }
    }
}
#endif

void ButtonSetImageDimsWhenDisabled( SInt32 tag, Boolean flag )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) [[btn cell] setImageDimsWhenDisabled:flag];
}

void ButtonSetTitleRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) {
        NSColor *color = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
        NSMutableAttributedString *as = [[btn attributedTitle] mutableCopy];
        [as addAttribute:NSForegroundColorAttributeName value:color range:NSMakeRange(0,[as length])];
        [btn setAttributedTitle:as];
        [as release];
    }

}

void ButtonSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSButton *btn = ButtonWithTag( tag );
    if ( btn ) {
        NSColor *color = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
        [[btn cell] setBackgroundColor:color];
    }
}


void CocoaButton( SInt32 inTag, SInt32 inEnabled, SInt32 inState, CFStringRef inTitle, CGRect inRect, SInt32 inType, SInt32 inStyle, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *title = (NSString *)inTitle;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        
        NSButton *btn = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( btn ) {    // already exists
            
            // make sure it's same view type
            if ( [btn class] != [NSButton class] ) return;
            
            // state
            if ( inState != kFBParamMissing ) {
                if ( inState == NSMixedState ) [btn setAllowsMixedState:YES];
                [btn setState:inState];
            }
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [btn setEnabled:inEnabled];
            
            // title
            if ( title ) {
                [btn setTitle:title];
            }
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [btn setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 14, 13, 81, 32 );
            }
            
            // init
            btn = [[NSButton alloc] initWithFrame:rect];
            
            // default autoresizing
            [btn setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [btn setTag: inTag];
            
            // state
            if ( inState != kFBParamMissing ) {
                if ( inState == NSMixedState ) [btn setAllowsMixedState:YES];
                [btn setState:inState];
            }
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [btn setEnabled:inEnabled];
            
            // type
            if ( inType == kFBParamMissing ) inType = 0;
            [btn setButtonType:inType];
            
            // style
            if ( inStyle == kFBParamMissing ) inStyle = NSRoundedBezelStyle;
            [btn setBezelStyle:inStyle];
            
            switch ( inStyle ) {
                case NSRecessedBezelStyle:
                    [btn setShowsBorderOnlyWhileMouseInside:YES];
                    break;
                default:
                    break;
            }
            
            // title
            if ( title ) {
                [btn setTitle:title];
                if ( inStyle == NSRoundedBezelStyle ) {
                    if ( inType == NSMomentaryLightButton || inType == NSMomentaryPushInButton ) {
                        if ( [title isEqualToString:@"OK"] ) [btn setKeyEquivalent:@"\r"];
                        if ( [title isEqualToString:@"Cancel"] ) [btn setKeyEquivalent:@"\e"];
                    }
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101200//101003
                    if ( inType == NSButtonTypeAccelerator || inType == NSButtonTypeMultiLevelAccelerator ) {
                        if ( [title isEqualToString:@"OK"] ) [btn setKeyEquivalent:@"\r"];;
                        if ( [title isEqualToString:@"Cancel"] ) [btn setKeyEquivalent:@"\e"];
                    }
#endif
                }
            }

            // wndTag
            [btn setWndTag:inWndTag];
            
            // fix font
            switch ( inStyle ) {
                case NSRoundRectBezelStyle:
                    [btn setFont:[NSFont systemFontOfSize:12]];
                    break;
                case NSRecessedBezelStyle:
                    [[btn cell] setControlSize:NSSmallControlSize];
                    [btn setFont:[NSFont boldSystemFontOfSize:12]];
                    break;
                    
                case NSInlineBezelStyle:
                    [btn setFont:[NSFont boldSystemFontOfSize:11]];
                    break;
                    
                default:
                    CocoaControlFixFont( btn );
                    break;
            }
            
            // target/action
            [btn setTarget:CocoaControlTargetObj()];
            [btn setAction:@selector(controlAction:)];

            // add to window/popover
            [contentView addSubview:btn];
            [btn release];
        }
        
        [btn setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - colorpanel
///////////////////////////////////////////////////
//                  cocoa panel                  //
///////////////////////////////////////////////////

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070

void ColorPanelSetColor( CGColorRef col )
{ [[NSColorPanel sharedColorPanel] setColor:[NSColor colorWithCGColor:col]]; }

CGColorRef ColorPanelColor()
{ return [[[NSColorPanel sharedColorPanel] color] CGColor]; }

#endif

void ColorPanelSetMode( NSColorPanelMode mode )
{ [[NSColorPanel sharedColorPanel] setMode:mode]; }

void ColorPanelSetShowsAlpha( Boolean flag )
{ [[NSColorPanel sharedColorPanel] setShowsAlpha:flag]; }

CGFloat ColorPanelAlpha()
{ return [[NSColorPanel sharedColorPanel] alpha]; }

// custom
void ColorPanelShow()
{ [[NSColorPanel sharedColorPanel] makeKeyAndOrderFront:nil]; }

void ColorPanelClose()
{
    if ( [[NSColorPanel sharedColorPanel] isVisible] ) {
        [[NSColorPanel sharedColorPanel] orderOut:nil];
    }
}

void ColorPanelSetRGB( CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{ [[NSColorPanel sharedColorPanel] setColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]]; }

#pragma mark - color well
///////////////////////////////////////////////////
//                  cocoa color well             //
///////////////////////////////////////////////////

// private
NSColorWell *ColorWellWithTag( SInt32 tag )
{
    NSColorWell *well = (NSColorWell *)CocoaViewWithTag( tag );
    if ( well ) {
        if ( [well class] == [NSColorWell class] ) return well;
    }
    return nil;
}

// public
CGColorRef ColorWellColor( SInt32 tag )
{
    NSColorWell *well = ColorWellWithTag( tag );
    if ( well ) {
        NSColor *color = [well color];
        NSInteger numberOfComponents = [color numberOfComponents];
        CGFloat components[numberOfComponents];
        CGColorSpaceRef colorSpace = [[color colorSpace] CGColorSpace];
        [color getComponents:(CGFloat *)&components];
        return (CGColorRef)[(id)CGColorCreate(colorSpace, components) autorelease];
    }
    return NULL;
}

void ColorWellTakeColorFrom( SInt32 tag, SInt32 senderTag )
{
    NSColorWell *well = ColorWellWithTag( tag );
    if ( well ) {
        id sender = CocoaViewWithTag( senderTag );
        if ( sender ) {
            NSColor *color = [sender color];
            [well setColor:color];
        }
    }
}

void ColorWellActivate( SInt32 tag, Boolean exclusive )
{
    NSColorWell *well = ColorWellWithTag( tag );
    if ( well ) [well activate:exclusive];
}

void ColorWellDeactivate( SInt32 tag )
{
    NSColorWell *well = ColorWellWithTag( tag );
    if ( well ) [well deactivate];
}

void ColorWellDrawWellInside( SInt32 tag, CGRect insideRect )
{
    NSColorWell *well = ColorWellWithTag( tag );
    if ( well ) [well drawWellInside:NSRectFromCGRect(insideRect)];
}

void ColorWellSetRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSColorWell *well = ColorWellWithTag( tag );
    if ( well ) {
        [well setColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]];
    }
}


void CocoaColorWell( SInt32 inTag, SInt32 inEnabled, CGColorRef inColor, CGRect inRect, SInt32 inBordered, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        
        NSColorWell *well = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( well ) {   // exists
            
            // make sure it's same view type
            if ( [well class] != [NSColorWell class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [well setEnabled:inEnabled];
            
            // color
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
            if ( inColor != NULL ) {
                if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
                    [well setColor:[NSColor colorWithCGColor:inColor]];  // macOS 10.8
                }
            }
#endif
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [well setFrame:rect];
            
        } else {    // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 20, 20, 44, 23 );
            }
            
            // init
            well = [[NSColorWell alloc] initWithFrame:rect];
            
            // default autoresizing
            [well setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [well setTag: inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [well setEnabled:inEnabled];
            
            // color
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
            if ( inColor != NULL ) {
                if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
                    [well setColor:[NSColor colorWithCGColor:inColor]];  // macOS 10.8
                }
            }
#endif
            
            // bordered
            if ( inBordered != kFBParamMissing ) [well setBordered:inBordered];
            
            // wndTag
            [well setWndTag:inWndTag];
            
            // target/action
            [well setTarget:CocoaControlTargetObj()];
            [well setAction:@selector(controlAction:)];

            // add to window
            [contentView addSubview:well];
            [well release];
        }
        
        [well setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - combobox
///////////////////////////////////////////////////
//                  cocoa combobox               //
///////////////////////////////////////////////////

@implementation CocoaComboBoxDelegate

@synthesize comboBoxData;

#pragma mark - combobox delegate methods
- (void)controlTextDidBeginEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidBeginEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidBeginEditing, notification.object );
}

- (void)controlTextDidChange:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidChange, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidChange, notification.object );
}

- (void)controlTextDidEndEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidEndEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidEndEditing, notification.object );
}

- (void)comboBoxSelectionDidChange:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( comboBoxSelectionDidChange, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( comboBoxSelectionDidChange, notification.object );
}

- (void)comboBoxSelectionIsChanging:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( comboBoxSelectionIsChanging, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( comboBoxSelectionIsChanging, notification.object );
}

- (void)comboBoxWillDismiss:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( comboBoxWillDismiss, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( comboBoxWillDismiss, notification.object );
}

- (void)comboBoxWillPopUp:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( comboBoxWillPopUp, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( comboBoxWillPopUp, notification.object );
}

#pragma mark - combobox data source methods
- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox {
    return [comboBoxData count];
}

- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index {
    return [comboBoxData objectAtIndex:index];
}
@end

CocoaComboBoxDelegate *CocoaComboBoxDelegateGet()
{
    static CocoaComboBoxDelegate *sCocoaComboBoxDelegate = nil;
    if ( !sCocoaComboBoxDelegate ) sCocoaComboBoxDelegate = [[CocoaComboBoxDelegate alloc] init];
    return sCocoaComboBoxDelegate;
}

// private
NSComboBox *ComboBoxWithTag( SInt32 tag )
{
    NSComboBox *combo = (NSComboBox *)CocoaViewWithTag( tag );
    if ( combo ) {
        if ( [combo class] == [NSComboBox class] ) return combo;
    }
    return nil;
}

// public
void ComboBoxSetHasVerticalScroller( SInt32 tag, Boolean flag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo setHasVerticalScroller:flag];
}

void ComboBoxSetIntercellSpacing( SInt32 tag, CGSize size )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo setIntercellSpacing:NSSizeFromCGSize(size)];
}

void ComboBoxSetBordered( SInt32 tag, Boolean flag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo setBordered:flag];
}

void ComboBoxSetItemHeight( SInt32 tag, CGFloat height )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo setItemHeight:height];
}

void ComboBoxSetNumberOfVisibleItems( SInt32 tag, SInt32 value )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo setNumberOfVisibleItems:value];
}

void ComboBoxSetUsesDataSource( SInt32 tag, Boolean flag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) {
        [combo setUsesDataSource:flag];
        if ( flag ) {
            [combo setDataSource:CocoaComboBoxDelegateGet()];
        }
    }
}

void ComboBoxSetCompletes( SInt32 tag, Boolean flag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo setCompletes:flag];
}

void ComboBoxAddItems( SInt32 tag, CFArrayRef items )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo addItemsWithObjectValues:(NSArray *)items];
}

void ComboBoxAddItem( SInt32 tag, CFTypeRef item )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo addItemWithObjectValue:(id)item];
}

void ComboBoxInsertItem( SInt32 tag, CFTypeRef item, SInt32 index )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo insertItemWithObjectValue:(id)item atIndex:index];
}

CFArrayRef ComboBoxValues( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return (CFArrayRef)[combo objectValues];
    return NULL;
}

void ComboBoxRemoveAllItems( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo removeAllItems];
}

void ComboBoxRemoveItemAtIndex( SInt32 tag, SInt32 index )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo removeItemAtIndex:index];
}

void ComboBoxRemoveItemWithValue( SInt32 tag, CFTypeRef value )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo removeItemWithObjectValue:(id)value];
}

SInt32 ComboBoxNumberOfItems( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return [combo numberOfItems];
    return 0;
}

SInt32 ComboBoxIndexOfItemWithValue( SInt32 tag, CFTypeRef value )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return [combo indexOfItemWithObjectValue:(id)value];
    return 0;
}

CFTypeRef ComboBoxItemValueAtIndex( SInt32 tag, SInt32 index )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return (id)[combo itemObjectValueAtIndex:index];
    return NULL;
}

void ComboBoxNoteNumberOfItemsChanged( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo noteNumberOfItemsChanged];
}

void ComboBoxReloadData( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo reloadData];
}

void ComboBoxScrollItemAtIndexToTop( SInt32 tag, SInt32 index )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo scrollItemAtIndexToTop:index];
}

void ComboBoxScrollItemAtIndexToVisible( SInt32 tag, SInt32 index )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo scrollItemAtIndexToVisible:index];
}

SInt32 ComboBoxIndexOfSelectedItem( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return [combo indexOfSelectedItem];
    return 0;
}

CFTypeRef ComboBoxValueOfSelectedItem( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return (CFTypeRef)[combo objectValueOfSelectedItem];
    return NULL;
}

void ComboBoxSelectItemAtIndex( SInt32 tag, SInt32 index )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo selectItemAtIndex:index];
}
void ComboBoxSelectItemWithValue( SInt32 tag, CFTypeRef value )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [combo selectItemWithObjectValue:(id)value];
}


// custom
CFMutableArrayRef ComboBoxData( SInt32 tag )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) return (CFMutableArrayRef)[CocoaComboBoxDelegateGet() comboBoxData];
    return nil;
}

void ComboBoxSetData( SInt32 tag, CFMutableArrayRef array )
{
    NSComboBox *combo = ComboBoxWithTag( tag );
    if ( combo ) [CocoaComboBoxDelegateGet() setComboBoxData:(NSMutableArray *)array];
}


void CocoaComboBox( SInt32 inTag, SInt32 inEnabled, CFTypeRef inItems, CFStringRef inText, CGRect inRect, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *text = (NSString *)inText;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSComboBox *combo = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( combo ) {    // already exists
            
            // make sure it's same view type
            if ( [combo class] != [NSComboBox class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [combo setEnabled:inEnabled];
            
            // text
            if ( text ) [combo setStringValue:text];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [combo setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 16, 99, 26 );
            
            // init
            combo = [[NSComboBox alloc] initWithFrame:rect];
            
            // default autoresizing
            [combo setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [combo setTag:inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [combo setEnabled:inEnabled];
            
            // items
            if ( inItems ) {
                NSArray *items = nil;
                
                if ( CFGetTypeID(inItems) == CFStringGetTypeID() ) {
                    items = [(NSString *)inItems componentsSeparatedByString:@";"];
                    
                } else if ( CFGetTypeID(inItems) == CFArrayGetTypeID() ) {
                    items = (NSArray *)inItems;
                }
                
                if ( items ) {
                    [combo addItemsWithObjectValues:items];
                }
            }
            
            // text
            if ( text ) [combo setStringValue:text];
            
            // wndTag
            [combo setWndTag:inWndTag];
            
            // fix font to match I.B.
            CocoaControlFixFont( combo );
            
            // delegate
            [combo setDelegate:CocoaComboBoxDelegateGet()];
            
            // add to window
            [contentView addSubview:combo];
            [combo release];
        }
        
        [combo setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - datepicker
///////////////////////////////////////////////////
//                  cocoa datepicker             //
///////////////////////////////////////////////////

// private
NSDatePicker *DatePickerWithTag( SInt32 tag )
{
    NSDatePicker *view = (NSDatePicker *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSDatePicker class] ) return view;
    }
    return nil;
}

void DatePickerSetMode( SInt32 tag, SInt32 mode )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setDatePickerMode:mode];
}

CFDateRef DatePickerDateValue( SInt32 tag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) return (CFDateRef)[picker dateValue];
    return NULL;
}

void DatePickerSetTimeZone( SInt32 tag, CFTimeZoneRef zone )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setTimeZone:(NSTimeZone *)zone];
}

void DatePickerSetMinDate( SInt32 tag, CFDateRef dt )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setMinDate:(NSDate *)dt];
}

void DatePickerSetMaxDate( SInt32 tag, CFDateRef dt )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setMaxDate:(NSDate *)dt];
}

void DatePickerSetBezeled( SInt32 tag, Boolean flag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setBezeled:flag];
}

void DatePickerSetBordered( SInt32 tag, Boolean flag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setBordered:flag];
}

void DatePickerSetDrawsBackground( SInt32 tag, Boolean flag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setDrawsBackground:flag];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void DatePickerSetBackgroundColor( SInt32 tag, CGColorRef col )
{
    if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
        NSDatePicker *picker = DatePickerWithTag( tag );
        if ( picker ) [picker setBackgroundColor:[NSColor colorWithCGColor:col]];   // macOS 10.8
    }
}

void DatePickerSetTextColor( SInt32 tag, CGColorRef col )
{
    if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
        NSDatePicker *picker = DatePickerWithTag( tag );
        if ( picker ) [picker setTextColor:[NSColor colorWithCGColor:col]];   // macOS 10.8
    }
}
#endif

void DatePickerSetLocale( SInt32 tag, CFLocaleRef locale )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setLocale:(NSLocale *)locale];
}

CFTimeInterval DatePickerTimeInterval( SInt32 tag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) return [picker timeInterval];
    return 0.0;
}

void DatePickerSetTimeInterval( SInt32 tag, CFTimeInterval ti )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) [picker setTimeInterval:ti];
}

CFDateRef DatePickerMinDate( SInt32 tag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) return (CFDateRef)[picker minDate];
    return NULL;
}

CFDateRef DatePickerMaxDate( SInt32 tag )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) return (CFDateRef)[picker maxDate];
    return NULL;
}

// custom
void DatePickerSetTextRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) {
        [picker setTextColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]];
    }
}

void DatePickerSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSDatePicker *picker = DatePickerWithTag( tag );
    if ( picker ) {
        [picker setBackgroundColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]];
    }
}


void CocoaDatePicker( SInt32 inTag, SInt32 inEnabled, CFDateRef inDate, CGRect inRect, SInt32 inStyle, SInt32 inElements, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSDate *date = (NSDate *)inDate;
    NSRect rect = NSRectFromCGRect( inRect );
    BOOL emptyRect = NSIsEmptyRect( rect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSDatePicker *picker = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( picker ) {    // already exists
            
            // make sure it's same view type
            if ( [picker class] != [NSDatePicker class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [picker setEnabled:inEnabled];
            
            // date
            if ( date ) [picker setDateValue:date];
            
            // rect
            if ( !emptyRect ) [picker setFrame:rect];
            
        } else {            // create
            
            // rect
            if ( emptyRect ) rect = NSMakeRect( 20, 20, 99, 27 );
            
            // init
            picker = [[NSDatePicker alloc] initWithFrame:rect];
            
            // default autoresizing
            [picker setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [picker setTag:inTag];
            
            // default draws background
            [picker setDrawsBackground:YES];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [picker setEnabled:inEnabled];

            // date
            if ( date ) [picker setDateValue:date];
            
            // style
            if ( inStyle != kFBParamMissing ) [picker setDatePickerStyle:inStyle];
            
            // elements
            if ( inElements == kFBParamMissing ) {
                [picker setDatePickerElements:NSYearMonthDayDatePickerElementFlag];
            } else {
                [picker setDatePickerElements:inElements];
            }
            
            // size to fit
            if ( emptyRect ) [picker sizeToFit];
            
            // wndTag
            [picker setWndTag:inWndTag];
            
            // target/action
            [picker setTarget:CocoaControlTargetObj()];
            [picker setAction:@selector(controlAction:)];

            // add to window
            [contentView addSubview:picker];
            [picker release];
            
        }
        
        [picker setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - graphics
CGContextRef GraphicsCurrentCGContext()
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
    return [[NSGraphicsContext currentContext] CGContext];
#else
    return [[NSGraphicsContext currentContext] graphicsPort];
#endif
    return NULL;
}


#pragma mark - imageview
///////////////////////////////////////////////////
//                  cocoa imageview              //
///////////////////////////////////////////////////

// private
NSImageView *ImageViewWithTag( SInt32 tag )
{
    NSImageView *view = (NSImageView *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSImageView class] ) return view;
    }
    return nil;
}

void ImageViewSetAnimates( SInt32 tag, Boolean flag )
{
    NSImageView *view = ImageViewWithTag( tag );
    if ( view ) [view setAnimates:flag];
}

void ImageViewSetEditable( SInt32 tag, Boolean flag )
{
    NSImageView *view = ImageViewWithTag( tag );
    if ( view ) [view setEditable:flag];
}

void ImageViewSetAllowsCutCopyPaste( SInt32 tag, Boolean flag )
{
    NSImageView *view = ImageViewWithTag( tag );
    if ( view ) [view setAllowsCutCopyPaste:flag];
}

void ImageViewSetCGImage( SInt32 tag, CGImageRef inImage )
{
    NSImageView *view = ImageViewWithTag( tag );
    if ( view ) {
        NSImage *image = [[NSImage alloc] initWithCGImage:inImage size:NSZeroSize];
        if ( image ) {
            [view setImage:image];
            [image release];
        }
    }
}

void CocoaImageView( SInt32 inTag, SInt32 inEnabled, CFStringRef inImageName, CGRect inRect, SInt32 inScaling, SInt32 inAlignment, SInt32 inFrameStyle, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *imageName = (NSString *)inImageName;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSImageView *view = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( view ) {    // already exists
            
            // make sure it's same view type
            if ( [view class] != [NSImageView class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [view setEnabled:inEnabled];
            
            // image name
            if ( imageName ) [view setImage:[NSImage imageNamed:imageName]];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [view setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 48, 48 );
            
            // init
            view = [[NSImageView alloc] initWithFrame:rect];
            
            // default autoresizing
            [view setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [view setTag:inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [view setEnabled:inEnabled];
            
            // image name
            if ( imageName ) [view setImage:[NSImage imageNamed:imageName]];
            
            // scaling
            if ( inScaling != kFBParamMissing ) [view setImageScaling:inScaling];
            
            // alignment
            if ( inAlignment != kFBParamMissing ) [view setImageAlignment:inAlignment];
            
            // frame style
            if ( inFrameStyle != kFBParamMissing ) [view setImageFrameStyle:inFrameStyle];
            
            // wndTag
            [view setWndTag:inWndTag];
            
            // add to window
            [contentView addSubview:view];
            [view release];
        }
        
        [view setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - text
///////////////////////////////////////////////////
//                  cocoa text                   //
///////////////////////////////////////////////////

CFStringRef TextString( SInt32 tag )
{
    NSText *txtObj = (NSText *)CocoaViewWithTag( tag );
    if ( txtObj ) return (CFStringRef)[txtObj string];
    return NULL;
}


#pragma mark - label
///////////////////////////////////////////////////
//                  cocoa textlabel              //
///////////////////////////////////////////////////

void CocoaTextLabel( SInt32 inTag, CFStringRef inText, CGRect inRect, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *text = (NSString *)inText;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSTextField *fld = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( fld ) {    // text field already exists
            
            // make sure it's same view type
            if ( [fld class] != [NSTextField class] ) return;
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [fld setFrame:rect];
            
        } else {        // create text field
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 18, 20, 37, 17 );
            
            // init
            fld = [[NSTextField alloc] initWithFrame:rect];
            
            [fld setDrawsBackground:NO];
            [fld setEditable:NO];
            [fld setSelectable:NO];
            [fld setBezeled:NO];
            
            // default autoresizing
            [fld setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [fld setTag: inTag];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // wndTag
            [fld setWndTag:inWndTag];
            
            // fix font to match I.B.
            CocoaControlFixFont( fld );
            
            // add to window
            [contentView addSubview:fld];
            [fld release];
        }
        
        [fld setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - levelindicator
///////////////////////////////////////////////////
//              cocoa levelindicator             //
///////////////////////////////////////////////////

// private
NSLevelIndicator *LevelIndicatorWithTag( SInt32 tag )
{
    NSLevelIndicator *view = (NSLevelIndicator *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSLevelIndicator class] ) return view;
    }
    return nil;
}

void LevelIndicatorSetEditable( SInt32 tag, Boolean flag )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) {
        [[ind cell] setEditable:flag];
        if ( flag ) {
            [ind setTarget:CocoaControlTargetObj()];
            [ind setAction:@selector(controlAction:)];
        } else {
            [ind setTarget:nil];
            [ind setAction:nil];
        }
    }
}

void LevelIndicatorSetTickMarkPosition( SInt32 tag, NSTickMarkPosition position )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) [ind setTickMarkPosition:position];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void LevelIndicatorSetStyle( SInt32 tag, NSLevelIndicatorStyle style )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) [ind setLevelIndicatorStyle:style];
}
#endif

double LevelIndicatorMinValue( SInt32 tag )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) return [ind minValue];
    return 0.0;
}

double LevelIndicatorMaxValue( SInt32 tag )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) return [ind maxValue];
    return 0.0;
}

double LevelIndicatorWarningValue( SInt32 tag )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) return [ind warningValue];
    return 0.0;
}

double LevelIndicatorCriticalValue( SInt32 tag )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) return [ind criticalValue];
    return 0.0;
}


// custom
void LevelIndicatorSetCGImage( SInt32 tag, CGImageRef inImage )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) {
        NSImage *image = [[NSImage alloc] initWithCGImage:inImage size:NSZeroSize];
        if ( image ) {
            [[ind cell] setImage:image];
            [image release];
        }
    }
}

void LevelIndicatorSetImage( SInt32 tag, CFStringRef imageName )
{
    NSLevelIndicator *ind = LevelIndicatorWithTag( tag );
    if ( ind ) {
        [[ind cell] setImage:[NSImage imageNamed:(NSString *)imageName]];
    }
}


void CocoaLevelIndicator( SInt32 inTag, SInt32 inEnabled, double inValue, CGRect inRect, double inMinValue, double inMaxValue, double inWarningValue, double inCriticalValue, SInt32 inTickMarks, SInt32 inMajorTickMarks, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSLevelIndicator *ind = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( ind ) {
            
            // make sure it's same view type
            if ( [ind class] != [NSLevelIndicator class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [ind setEnabled:inEnabled];
            
            // min
            if ( (int)inMinValue != kFBParamMissing ) [ind setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue != kFBParamMissing ) [ind setMaxValue:inMaxValue];
            
            SInt32 maxValue = [ind maxValue];
            
            // value
            if ( (int)inValue != kFBParamMissing ) {
                if ( inValue <= maxValue ) [ind setDoubleValue:inValue];
            }
            
            // warning
            if ( (int)inWarningValue != kFBParamMissing ) [ind setWarningValue:inWarningValue];
            
            // critical
            if ( (int)inCriticalValue != kFBParamMissing ) [ind setCriticalValue:inCriticalValue];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [ind setFrame:rect];
            
        } else {
            
            // tick marks
            if ( inTickMarks == kFBParamMissing ) inTickMarks = 0;
            if ( inTickMarks > 0 ) {
                if ( inMajorTickMarks == kFBParamMissing ) inMajorTickMarks = 0;
            }
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                if ( inTickMarks ) {
                    rect = NSMakeRect( 20, 18, 96, 25 );
                } else {
                    rect = NSMakeRect( 20, 18, 96, 18 );
                }
            }
            
            // init
            ind = [[NSLevelIndicator alloc] initWithFrame:rect];
            
            // default autoresizing
            [ind setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [ind setTag:inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [ind setEnabled:inEnabled];
            
            // min
            if ( (int)inMinValue == kFBParamMissing ) inMinValue = 0.0;
            [ind setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue == kFBParamMissing ) inMaxValue = 2.0;
            [ind setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue == kFBParamMissing ) inValue = 2;
            if ( inValue <= inMaxValue ) [ind setDoubleValue:inValue];
            
            // warning
            if ( (int)inWarningValue != kFBParamMissing ) [ind setWarningValue:inWarningValue];
            
            // critical
            if ( (int)inCriticalValue != kFBParamMissing ) [ind setCriticalValue:inCriticalValue];
            
            // tick marks
            if ( inTickMarks ) {
                [ind setNumberOfTickMarks:inTickMarks];
                [ind setNumberOfMajorTickMarks:inMajorTickMarks];
            }
            
            // wndTag
            [ind setWndTag:inWndTag];
            
            // add to window
            [contentView addSubview:ind];
            [ind release];
        }
        
        [ind setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - object
///////////////////////////////////////////////////
//                cocoa object                   //
///////////////////////////////////////////////////

@implementation NSObject (Additions)
@dynamic wndTagNumber;

- (NSInteger)wndTag {
    return [objc_getAssociatedObject(self, @selector(wndTagNumber)) integerValue];
}

- (void)setWndTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(wndTagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end



#pragma mark - panel
///////////////////////////////////////////////////
//                  cocoa panel                  //
///////////////////////////////////////////////////

// private
NSPanel *PanelWithTag( SInt32 tag )
{
    NSPanel *panel = (NSPanel *)CocoaViewWithTag( tag );
    if ( panel ) {
        if ( [panel class] == [NSPanel class] ) return panel;
    }
    return nil;
}

void PanelSetFloatingPanel( SInt32 tag, Boolean flag )
{
    NSPanel *w = PanelWithTag( tag );
    if ( w ) [w setFloatingPanel:flag];
}

void PanelSetBecomesKeyOnlyIfNeeded( SInt32 tag, Boolean flag )
{
    NSPanel *w = PanelWithTag( tag );
    if ( w ) [w setBecomesKeyOnlyIfNeeded:flag];
}

void PanelSetWorksWhenModal( SInt32 tag, Boolean flag )
{
    NSPanel *w = PanelWithTag( tag );
    if ( w ) [w setWorksWhenModal:flag];
}


void CocoaPanel( SInt32 inTag, CFStringRef inTitle, CGRect inRect, SInt32 inStyle )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    if ( CocoaObjectIsPopover( inTag ) ) return;
    
    if ( !IsCocoaApplication() ) {
        NSApplicationLoad();
        
#if !__LP64__
        if ( CocoaWindowIsFBWindow( inTag ) ) return;
#endif  //__LP64__
        
    }
    
    NSString *title = (NSString *)inTitle;
    NSRect rect = NSRectFromCGRect( inRect );
    
    id w = CocoaWindowWithTag( inTag );
    
    if ( w ) {      // exists
        
        // title
        if ( title ) {
            [w setTitle:title];
        }
        
        // rect
        if ( !NSIsEmptyRect( rect ) ) {
            [w setFrame:[w frameRectForContentRect:rect] display:YES];
        }
        
    } else {    // create
        
        BOOL center = NO;
        
        // rect
        if ( NSIsEmptyRect( rect ) ) {
            rect = NSMakeRect( 0, 0, 276, 378 );
        } else {
            center = ( rect.origin.x == 0.0 && rect.origin.y == 0.0 );
        }
        
        // style
        if ( (int)inStyle == kFBParamMissing ) {
            inStyle = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSUtilityWindowMask;
        }
        
        // init
        w = [[NSPanel alloc] initWithContentRect:rect styleMask:inStyle backing:NSBackingStoreBuffered defer:NO];
        
        // floating panel
        [w setFloatingPanel:YES];
        
        // content view
        /* create content view subclass for:
         1. drawing focus rect around text controls (Carbon apps only).
         2. intercepting content view drawRect: method to issue dialog viewDrawRect event.
         */
        //        if ( !IsCocoaApplication() ) { // only for Carbon apps
        // create content view subclass for drawing focus rect around text controls
        CocoaWindowContentView *contentView = [[CocoaWindowContentView alloc] initWithFrame:[[w contentView] frame]];
        [w setContentView:contentView];
        
        [contentView setIdentifier:[NSString stringWithFormat:@"%d",windowContentViewTag]];
        
        // key view loop
        [w setAutorecalculatesKeyViewLoop:YES];
        
        // tag
        [w setTag:inTag];
        
        // title
        if ( !title ) title = @"Untitled";
        [w setTitle:title];
        
        // center
        if ( center ) [w center];
        
        // delegate
        [w setDelegate:CocoaWindowDelegateGet()];
        
        // app delegate
        CocoaApplicationDelegate *appDelegate = CocoaApplicationDelegateGet();
        //        CocoaApplicationDelegate *appDelegate = (CocoaApplicationDelegate *)[NSApp delegate];
        if ( appDelegate ) {
            [[appDelegate windows] addObject:w];
        }
    }
    
    // show
    if ( show ) {
        [w makeKeyAndOrderFront:nil];
        
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101100
        [[w contentView] setNeedsDisplay:YES];  // force drawRect:
#endif
        
    } else {
        [w orderOut:nil];
    }
    
    CocoaOutputWindowSetTag( inTag );
}





#pragma mark - popover
///////////////////////////////////////////////////
//                  cocoa popover                //
///////////////////////////////////////////////////

@implementation CocoaPopoverDelegate

@synthesize detachableWindow;

- (BOOL)windowShouldClose:(NSWindow *)window {
    if ( window == detachableWindow ) {
        [window orderOut:nil];
        return NO;
    }
    return YES;
}

- (NSWindow *)detachableWindowForPopover:(NSPopover *)popover {
    NSWindow *window;
    SInt32 detachableWndTag = 0;
    
    DialogEventSetLong(detachableWndTag);
    if ( detachableWindow ) DialogEventSetLong([detachableWindow tag]);
    
    CallUserDefinedOnDialogFunction( popoverDetachableWindow, [popover tag], [popover tag] );

    detachableWndTag = DialogEventLong();
    window = CocoaWindowWithTag(detachableWndTag);
    CocoaOutputWindowSetTag( detachableWndTag );

    return window;
}

- (BOOL)popoverShouldClose:(NSPopover *)popover {
    DialogEventSetBool( YES );
    CallUserDefinedOnDialogFunction( popoverShouldClose, [popover tag], [popover tag] );
    return DialogEventBool();
}

- (void)popoverWillShow:(NSNotification *)notification {
    CallUserDefinedOnDialogFunction( popoverWillShow, [notification.object tag], [notification.object tag] );
}

- (void)popoverDidShow:(NSNotification *)notification {
    CallUserDefinedOnDialogFunction( popoverDidShow, [notification.object tag], [notification.object tag] );
}

- (void)popoverWillClose:(NSNotification *)notification {
    CallUserDefinedOnDialogFunction( popoverWillClose, [notification.object tag], [notification.object tag] );
}

- (void)popoverDidClose:(NSNotification *)notification {
    CallUserDefinedOnDialogFunction( popoverDidClose, [notification.object tag], [notification.object tag] );
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
- (void)popoverDidDetach:(NSPopover *)popover {
    CallUserDefinedOnDialogFunction( popoverDidDetach, [popover tag], [popover tag] );
}

- (BOOL)popoverShouldDetach:(NSPopover *)popover {
    DialogEventSetBool( YES );
    CallUserDefinedOnDialogFunction( popoverShouldDetach, [popover tag], [popover tag] );
    return DialogEventBool();
}
#endif

@end

@implementation NSPopover (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

static NSMutableArray *sCocoaPopovers;

NSPopover *CocoaPopoverForView( id view )
{
    NSPopover *popover = nil;
    if ( [view class] == [NSView class] ) {
        NSResponder *viewNextResponder = [view nextResponder];
        if ( viewNextResponder ) {
            if ( [viewNextResponder class] == [NSViewController class] ) {
                NSResponder *viewControllerNextResponder = [viewNextResponder nextResponder];
                if ( viewControllerNextResponder ) {
                    if ( [viewControllerNextResponder class] == [NSPopover class] ) {
                        popover = (NSPopover *)viewControllerNextResponder;
                    }
                }
            }
        }
    } else {
        id superview = [view superview];
        if ( superview ) popover = CocoaPopoverForView( superview );
    }
    return popover;
}

void CocoaPopoverAdd( NSPopover *popover, SInt32 tag )
{
    if ( !sCocoaPopovers ) sCocoaPopovers = [[NSMutableArray alloc] initWithCapacity:0];
    NSDictionary *poDict = [NSDictionary dictionaryWithObject:popover forKey:[NSString stringWithFormat:@"%ld",(long)tag]];
    [sCocoaPopovers addObject:poDict];
    
    CocoaPopoverDelegate *delegate = [[CocoaPopoverDelegate alloc] init];
    [popover setDelegate:delegate];
}

NSPopover *CocoaPopoverWithTag( SInt32 tag )
{
    if ( sCocoaPopovers ) {
        for ( NSDictionary *poDict in sCocoaPopovers ) {
            NSPopover *popover = [poDict objectForKey:[NSString stringWithFormat:@"%ld",(long)tag]];
            if ( popover ) return popover;
        }
    }
    return nil;
}

void PopoverAddSubview( SInt32 tag, SInt32 subviewTag )
{
    NSPopover *popover = CocoaPopoverWithTag( tag );
    if ( popover ) {
        NSViewController *controller = [popover contentViewController];
        if ( controller ) {
            NSView *view = [controller view];
            if ( view ) {
                NSView *subview = CocoaViewWithTag( subviewTag );
                if ( subview ) {
                    [view addSubview:subview];
                    [(NSButton *)subview setWndTag:tag];
                }
            }
        }
    }
}

void PopoverShow( SInt32 tag, CGRect inRect, SInt32 viewTag, CGRectEdge preferredEdge )
{
    NSPopover *popover = CocoaPopoverWithTag( tag );
    if ( popover ) {
        NSView *view = CocoaViewWithTag( viewTag );
        if ( view ) {
            NSRect rect = NSRectFromCGRect( inRect );
            [popover showRelativeToRect:rect ofView:view preferredEdge:(NSRectEdge)preferredEdge];
        }
    }
}

void PopoverClose( SInt32 tag )
{
    NSPopover *popover = CocoaPopoverWithTag( tag );
    if ( popover ) [popover close];
}

Boolean PopoverIsShown( SInt32 tag )
{
    NSPopover *popover = CocoaPopoverWithTag( tag );
    if ( popover ) return [popover isShown];
    return false;
}

void PopoverSetDetachableWindow( SInt32 popoverTag, SInt32 wndTag )
{
    NSPopover *popover = CocoaPopoverWithTag( popoverTag );
    if ( popover ) {
        NSWindow *w = CocoaWindowWithTag( wndTag );
        if ( w ) {
            CocoaPopoverDelegate *delegate = (CocoaPopoverDelegate *)[popover delegate];
            delegate.detachableWindow = w;
            [w setDelegate:delegate];
        }
    }
}

void CocoaPopover( SInt32 inTag, CGRect inRect, SInt32 inBehavior, SInt32 inAnimates )
{
    if ( CocoaObjectIsWindow( inTag ) ) return;
    
#if !__LP64__
    if ( CocoaWindowIsFBWindow( inTag ) ) return;// if an FB Carbon window exists with same id/tag, alert and abort
#endif  //__LP64__
    
    NSPopover *popover = CocoaPopoverWithTag( inTag );
    if ( !popover ) {
        
        // rect
        NSRect rect = NSRectFromCGRect( inRect );
        if ( NSIsEmptyRect( rect ) ) {
            rect = NSMakeRect( 0, 0, 163, 96 );
        }
        
        // create popover view
        id view;
        if ( IsCocoaApplication() ) {
            view = [[NSView alloc] initWithFrame:rect];
        } else {
            view = [[CocoaWindowContentView alloc] initWithFrame:rect];
            [view setIdentifier:[NSString stringWithFormat:@"%d",windowContentViewTag]];
        }
        
        // create popover view controller
        NSViewController *controller = [[NSViewController alloc] init];
        [controller setView:view];
        [view release];
        
        // create popover
        popover = [[NSPopover alloc] init];
        
        // behavior
        if ( inBehavior != kFBParamMissing ) [popover setBehavior:inBehavior];
        
        // animates
        if ( inAnimates != kFBParamMissing ) [popover setAnimates:inAnimates];
        
        // set controller
        [popover setContentViewController:controller];
        [controller release];
        
        // add to list of popovers
        CocoaPopoverAdd( popover, inTag );
        [popover release];
        
        // tag
        [popover setTag:inTag];
    }
    CocoaOutputWindowSetTag( inTag );
}



#pragma mark - popupbutton
///////////////////////////////////////////////////
//                  cocoa popupbutton            //
///////////////////////////////////////////////////

// private
NSPopUpButton *PopUpButtonWithTag( SInt32 tag )
{
    NSPopUpButton *view = (NSPopUpButton *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSPopUpButton class] ) return view;
    }
    return nil;
}

void PopUpButtonAddItemWithTitle( SInt32 tag, CFStringRef title )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn addItemWithTitle:(NSString *)title];
}

void PopUpButtonAddItemsWithTitles( SInt32 tag, CFArrayRef titles )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn addItemsWithTitles:(NSArray *)titles];
}

void PopUpButtonInsertItemWithTitle( SInt32 tag, CFStringRef title, SInt32 index )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn insertItemWithTitle:(NSString *)title atIndex:index];
}

void PopUpButtonRemoveAllItems( SInt32 tag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn removeAllItems];
}

void PopUpButtonRemoveItemWithTitle( SInt32 tag, CFStringRef title )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn removeItemWithTitle:(NSString *)title];
}

void PopUpButtonRemoveItemAtIndex( SInt32 tag, SInt32 index )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn removeItemAtIndex:index];
}

CFStringRef PopUpButtonTitleOfSelectedItem( SInt32 tag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) return (CFStringRef)[btn titleOfSelectedItem];
    return NULL;
}

SInt32 PopUpButtonIndexOfSelectedItem( SInt32 tag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) return [btn indexOfSelectedItem];
    return 0;
}

void PopUpButtonSelectItemAtIndex( SInt32 tag, SInt32 index )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn selectItemAtIndex:index];
}

void PopUpButtonSelectItemWithTitle( SInt32 tag, CFStringRef title )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn selectItemWithTitle:(NSString *)title];
}

SInt32 PopUpButtonNumberOfItems( SInt32 tag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) return [btn numberOfItems];
    return 0;
}

CFStringRef PopUpButtonItemTitleAtIndex( SInt32 tag, SInt32 index )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) return (CFStringRef)[btn itemTitleAtIndex:index];
    return NULL;
}

CFArrayRef PopUpButtonItemTitles( SInt32 tag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) return (CFArrayRef)[btn itemTitles];
    return NULL;
}

void PopUpButtonSetMenu( SInt32 tag, SInt32 menuIndex )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) {
        NSMenu *menu = MenuAtIndex( menuIndex );
        if ( menu ) {
            [btn setMenu:menu];
        }
    }
}


void PopUpButtonSetTitle( SInt32 tag, CFStringRef title )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn setTitle:(NSString *)title];
}

void PopUpButtonSetPreferredEdge( SInt32 tag, CGRectEdge edge )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn setPreferredEdge:(NSRectEdge)edge];
}

void PopUpButtonSetAutoenablesItems( SInt32 tag, Boolean flag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) [btn setAutoenablesItems:flag];
}

void PopUpButtonItemSetEnabled( SInt32 tag, SInt32 index, Boolean flag )
{
    NSPopUpButton *btn = PopUpButtonWithTag( tag );
    if ( btn ) {
        NSMenuItem *item = [btn itemAtIndex:index];
        if ( item ) [item setEnabled:flag];
    }
}



void CocoaPopUpButton( SInt32 inTag, SInt32 inEnabled, SInt32 inIndex, CFTypeRef inItems, CGRect inRect, SInt32 inPullsDown, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSPopUpButton *btn = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( btn ) {
            
            // make sure it's same view type
            if ( [btn class] != [NSPopUpButton class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [btn setEnabled:inEnabled];
            
            // index
            if ( inIndex != kFBParamMissing ) {
                if ( inIndex < [btn numberOfItems] ) [btn selectItemAtIndex:inIndex];
            }
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [btn setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 18, 17, 76, 26 );
            }
            
            // init
            if ( inPullsDown == kFBParamMissing ) inPullsDown = false;
            btn = [[NSPopUpButton alloc] initWithFrame:rect pullsDown:inPullsDown];
            
            // default autoresizing
            [btn setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [btn setTag: inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [btn setEnabled:inEnabled];
            
            // items
            if ( inItems ) {
                NSArray *items = nil;
                if ( CFGetTypeID(inItems) == CFStringGetTypeID() ) {
                    items = [(NSString *)inItems componentsSeparatedByString:@";"];
                } else if ( CFGetTypeID(inItems) == CFArrayGetTypeID() ) {
                    items = (NSArray *)inItems;
                }
                if ( items ) {
                    [btn addItemsWithTitles:items];
                }
            }
            
            // index
            if ( inIndex != kFBParamMissing ) {
                if ( inIndex < [btn numberOfItems] ) [btn selectItemAtIndex:inIndex];
            }
            
            // wndTag
            [btn setWndTag:inWndTag];
            
            // fix font
            CocoaControlFixFont( btn );
            
            // target/action
            [btn setTarget:CocoaControlTargetObj()];
            [btn setAction:@selector(controlAction:)];

            // add to window
            [contentView addSubview:btn];
            [btn release];
        }
        
        [btn setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - progress indicator
///////////////////////////////////////////////////
//           cocoa progress indicator            //
///////////////////////////////////////////////////

@implementation NSProgressIndicator (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

// private
NSProgressIndicator *ProgressIndicatorWithTag( SInt32 tag )
{
    NSProgressIndicator *view = (NSProgressIndicator *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSProgressIndicator class] ) return view;
    }
    return nil;
}

void ProgressIndicatorSetStyle( SInt32 tag, UInt32 style )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c setStyle:style];
}

void ProgressIndicatorSetIndeterminate( SInt32 tag, Boolean flag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c setIndeterminate:flag];
}

void ProgressIndicatorSetDisplayWhenStopped( SInt32 tag, Boolean flag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c setDisplayedWhenStopped:flag];
}

void ProgressIndicatorStartAnimation( SInt32 tag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c startAnimation:nil];
}

void ProgressIndicatorStopAnimation( SInt32 tag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c stopAnimation:nil];
}

void ProgressIndicatorSetUsesThreadedAnimation( SInt32 tag, Boolean flag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c setUsesThreadedAnimation:flag];
}

void ProgressIndicatorIncrementBy( SInt32 tag, double delta )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) [c incrementBy:delta];
}


double ProgressIndicatorDoubleValue( SInt32 tag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) return [c doubleValue];
    return 0.0;
}

double ProgressIndicatorMinValue( SInt32 tag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) return [c minValue];
    return 0.0;
}

double ProgressIndicatorMaxValue( SInt32 tag )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    if ( c ) return [c maxValue];
    return 0.0;
}

void ProgressIndicatorSetDoubleValue( SInt32 tag, double value )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    [c setDoubleValue:value];
}

void ProgressIndicatorSetMinValue( SInt32 tag, double value )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    [c setMinValue:value];
}

void ProgressIndicatorSetMaxValue( SInt32 tag, double value )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    [c setMaxValue:value];
}


void ProgressIndicatorSetControlSize( SInt32 tag, NSControlSize size )
{
    NSProgressIndicator *c = ProgressIndicatorWithTag( tag );
    [c setControlSize:size];
}


void CocoaProgressIndicator( SInt32 inTag, double inValue, CGRect inRect, double inMinValue, double inMaxValue, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSProgressIndicator *ind = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( ind ) {
            
            // make sure it's same view type
            if ( [ind class] != [NSProgressIndicator class] ) return;
            
            // min
            if ( (int)inMinValue != kFBParamMissing ) [ind setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue != kFBParamMissing ) [ind setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue != kFBParamMissing ) {
                if ( inValue <= inMaxValue ) [ind setDoubleValue:inValue];
            }
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [ind setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 20, 19, 100, 20 );
            }
            
            // init
            NSProgressIndicator *ind = [[NSProgressIndicator alloc] initWithFrame:rect];
            
            // default autoresizing
            [ind setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [ind setTag:inTag];
            
            // min
            if ( (int)inMinValue == kFBParamMissing ) inMinValue = 0.0;
            [ind setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue == kFBParamMissing ) inMaxValue = 100.0;
            [ind setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue == kFBParamMissing ) inValue = inMinValue;
            if ( inValue <= inMaxValue ) [ind setDoubleValue:inValue];
            
            // indeterminate
            [ind setIndeterminate:NO];
            
            // displayWhenStopped
            [ind setDisplayedWhenStopped:YES];
            
            // wndTag
            [ind setWndTag:inWndTag];
            
            // add to window
            [contentView addSubview:ind];
            [ind release];
        }
        
        [ind setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - scrollview
///////////////////////////////////////////////////
//                  cocoa scrollview             //
///////////////////////////////////////////////////

@implementation NSScrollView (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

// private
NSScrollView *ScrollViewWithTag( SInt32 tag )
{
    NSScrollView *view = (NSScrollView *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSScrollView class] ) return view;
    }
    return nil;
}

void ScrollViewSetHasHorizontalScroller( SInt32 tag, Boolean flag )
{
    NSScrollView *scrollView = ScrollViewWithTag( tag );
    if ( scrollView ) {
        [scrollView setHasHorizontalScroller:flag];
    }
}

void ScrollViewSetHasVerticalScroller( SInt32 tag, Boolean flag )
{
    NSScrollView *scrollView = ScrollViewWithTag( tag );
    if ( scrollView ) {
        [scrollView setHasVerticalScroller:flag];
    }
}

void ScrollViewSetAutohidesScrollers( SInt32 tag, Boolean flag )
{
    NSScrollView *scrollView = ScrollViewWithTag( tag );
    if ( scrollView ) {
        [scrollView setAutohidesScrollers:flag];
    }
}

void ScrollViewFlashScrollers( SInt32 tag )
{
    NSScrollView *scrollView = ScrollViewWithTag( tag );
    if ( scrollView ) {
        [scrollView flashScrollers];
    }
}

void ScrollViewSetDocumentView( SInt32 scrollTag, SInt32 docTag )
{
    NSScrollView *scrollView = ScrollViewWithTag( scrollTag );
    if ( scrollView ) {
        NSView *docView = CocoaViewWithTag( docTag );
        if ( docView ) [scrollView setDocumentView:docView];
    }
}


void CocoaScrollView( SInt32 inTag, CGRect inRect, SInt32 inBorderType, SInt32 inDocTag, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSScrollView *scrollView = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( scrollView ) { // already exists
            
            // make sure it's same view type
            if ( [scrollView class] != [NSScrollView class] ) return;
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [scrollView setFrame:rect];
            
        } else {            // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 240, 135 );
            
            // init scroll view
            scrollView = [[NSScrollView alloc] initWithFrame:rect];
            
            // vertical scroller
            [scrollView setHasVerticalScroller:YES];

            // border type
            if ( inBorderType != kFBParamMissing) [scrollView setBorderType:inBorderType];
            
            // document view
            if ( inDocTag != 0 ) {
                NSView *docView = CocoaViewSubviewWithTag( contentView, inDocTag );
                if ( docView ) {
                    [docView retain];
                    [docView removeFromSuperview];
                    [scrollView setDocumentView:docView];
                    [docView release];
                }
            }
            
            // default autoresizing
            [scrollView setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [scrollView setTag:inTag];
            
            // wndTag
            [scrollView setWndTag:inWndTag];
            
            // add to window
            [contentView addSubview:scrollView];
            [scrollView release];
        }
        
        [scrollView setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - search field
///////////////////////////////////////////////////
//                  cocoa search field           //
///////////////////////////////////////////////////

@implementation CocoaSearchFieldDelegate

- (void)controlTextDidBeginEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidBeginEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidBeginEditing, notification.object );
}

- (void)controlTextDidChange:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidChange, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidChange, notification.object );
}

- (void)controlTextDidEndEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidEndEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidEndEditing, notification.object );
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
- (void)searchFieldDidStartSearching:(NSSearchField *)sender {
//    CocoaOutputWindowSetTag( [sender wndTag] );
//    CallUserDefinedOnDialogFunction( searchFieldDidStartSearching, [sender tag], [sender wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( searchFieldDidStartSearching, sender );
}

- (void)searchFieldDidEndSearching:(NSSearchField *)sender {
//    CocoaOutputWindowSetTag( [sender wndTag] );
//    CallUserDefinedOnDialogFunction( searchFieldDidEndSearching, [sender tag], [sender wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( searchFieldDidEndSearching, sender );
}
#endif  //__MAC_OS_X_VERSION_MAX_ALLOWED > 101000

@end

CocoaSearchFieldDelegate *CocoaSearchFieldDelegateGet()
{
    static CocoaSearchFieldDelegate *sCocoaSearchFieldDelegate = nil;
    if ( !sCocoaSearchFieldDelegate ) sCocoaSearchFieldDelegate = [[CocoaSearchFieldDelegate alloc] init];
    return sCocoaSearchFieldDelegate;
}

// private
NSSearchField *SearchFieldWithTag( SInt32 tag )
{
    NSSearchField *view = (NSSearchField *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSSearchField class] ) return view;
    }
    return nil;
}


#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void SearchFieldSetSendsSearchStringImmediately( SInt32 tag, Boolean flag )
{
    NSSearchField *fld = SearchFieldWithTag( tag );
    if ( fld ) {
        [fld setSendsSearchStringImmediately:flag];
    }
}

void SearchFieldSetSendsWholeSearchString( SInt32 tag, Boolean flag )
{
    NSSearchField *fld = SearchFieldWithTag( tag );
    if ( fld ) {
        [fld setSendsWholeSearchString:flag];
    }
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
void SearchFieldSetCentersPlaceholder( SInt32 tag, Boolean flag )
{
    NSSearchField *fld = SearchFieldWithTag( tag );
    if ( fld ) {
        [fld setCentersPlaceholder:flag];
    }
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void SearchFieldSetMaximumRecents( SInt32 tag, SInt32 value )
{
    NSSearchField *fld = SearchFieldWithTag( tag );
    if ( fld ) {
        [fld setMaximumRecents:value];
    }
}
#endif

void SearchFieldSetRecentsAutosaveName( SInt32 tag, CFStringRef name )
{
    NSSearchField *fld = SearchFieldWithTag( tag );
    if ( fld ) {
        [fld setRecentsAutosaveName:(NSString *)name];
    }
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void SearchFieldSetSearchMenuTemplate( SInt32 tag, SInt32 menuIndex )
{
    NSSearchField *fld = SearchFieldWithTag( tag );
    if ( fld ) {
        NSMenu *menu = MenuAtIndex( menuIndex );
        if ( menu ) {
            [fld setSearchMenuTemplate:menu];
        }
    }
}
#endif



void CocoaSearchField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *text = (NSString *)inText;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSSearchField *fld = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( fld ) {    // already exists
            
            // make sure it's same view type
            if ( [fld class] != [NSSearchField class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [fld setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 96, 22 );
            
            // init
            fld = [[NSSearchField alloc] initWithFrame:rect];
            
            // default autoresizing
            [fld setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [fld setTag: inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // wndTag
            [fld setWndTag:inWndTag];
            
            // fix font to match I.B.
            CocoaControlFixFont( fld );
            
            // delegate
            [fld setDelegate:CocoaSearchFieldDelegateGet()];
            
            // add to window
            [contentView addSubview:fld];
            [fld release];
        }
        
        [fld setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - secure text field
///////////////////////////////////////////////////
//             cocoa secure text field           //
///////////////////////////////////////////////////

// private
NSSecureTextField *SecureTextFieldWithTag( SInt32 tag )
{
    NSSecureTextField *view = (NSSecureTextField *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSSecureTextField class] ) return view;
    }
    return nil;
}

void SecureTextFieldSetEchosBullets( SInt32 tag, Boolean flag )
{
    NSSecureTextField *fld = SecureTextFieldWithTag( tag );
    if ( fld ) [[fld cell] setEchosBullets:flag];
}


void CocoaSecureTextField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *text = (NSString *)inText;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSSecureTextField *fld = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( fld ) {    // already exists
            
            // make sure it's same view type
            if ( [fld class] != [NSSecureTextField class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [fld setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 96, 22 );
            
            // init
            fld = [[NSSecureTextField alloc] initWithFrame:rect];
            
            // default autoresizing
            [fld setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [fld setTag: inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // wndTag
            [fld setWndTag:inWndTag];
            
            // fix font to match I.B.
            CocoaControlFixFont( fld );
            
            // delegate
            [fld setDelegate:CocoaTextFieldDelegateGet()];
            
            // add to window
            [contentView addSubview:fld];
            [fld release];
        }
        
        [fld setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - segmented control
///////////////////////////////////////////////////
//            cocoa segmentedcontrol             //
///////////////////////////////////////////////////

// private
NSSegmentedControl *SegmentedControlWithTag( SInt32 tag )
{
    NSSegmentedControl *view = (NSSegmentedControl *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSSegmentedControl class] ) return view;
    }
    return nil;
}

SInt32 SegmentedControlSegmentCount( SInt32 tag )
{
    NSSegmentedControl *c = SegmentedControlWithTag( tag );
    if ( c ) return [c segmentCount];
    return 0;
}

void SegmentedControlSetSegment( SInt32 tag, SInt32 index, SInt32 enabled, CFStringRef label, CFStringRef imageName, SInt32 imageScaling, SInt32 width )
{
    // target window
    id w = CocoaOutputWindow();
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSSegmentedControl *c = CocoaViewSubviewWithTag( contentView, tag );
        if ( c ) {
            NSSegmentedCell *cell = (NSSegmentedCell *)[c cell];
            
            // label
            if ( label ) [cell setLabel:(NSString *)label forSegment:index];
            
            // image
            if ( imageName ) {
                [cell setImage:[NSImage imageNamed:(NSString *)imageName] forSegment:index];
                
                // image scaling
                [cell setImageScaling:imageScaling forSegment:index];
            }
            
            // width
            if ( width ) [cell setWidth:width forSegment:index];
            
            // enabled
            [cell setEnabled:enabled forSegment:index];
            
        }
    }
}

SInt32 SegmentedControlSelectedSegment( SInt32 tag )
{
    NSSegmentedControl *c = SegmentedControlWithTag( tag );
    if ( c ) return [c selectedSegment];
    return -1;
}

void SegmentedControlSetMenu( SInt32 tag, SInt32 index, SInt32 menuIndex )
{
    NSSegmentedControl *c = SegmentedControlWithTag( tag );
    if ( c ) {
        NSMenu *menu = MenuAtIndex( menuIndex );
        if ( menu ) {
            [c setMenu:menu forSegment:index];
        }
    }
}

void SegmentedControlSetStyle( SInt32 tag, NSSegmentStyle style )
{
    NSSegmentedControl *c = SegmentedControlWithTag( tag );
    if ( c ) [c setSegmentStyle:style];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101002
void SegmentedControlSetTrackingMode( SInt32 tag, NSSegmentSwitchTracking mode )
{
    NSSegmentedControl *c = SegmentedControlWithTag( tag );
    if ( c ) [c setTrackingMode:mode];
}
#endif

void CocoaSegmentedControl( SInt32 inTag, SInt32 inEnabled, SInt32 inIndex, CGRect inRect, SInt32 inSegments, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSSegmentedControl *seg = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( seg ) {
            // make sure it's same view type
            if ( [seg class] != [NSSegmentedControl class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [seg setEnabled:inEnabled];
            
            // index
            if ( inIndex != kFBParamMissing ) [seg setSelected:YES forSegment:inIndex];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [seg setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 18, 18, 104, 24 );
            
            // init
            seg = [[NSSegmentedControl alloc] initWithFrame:rect];
            
            // default autoresizing
            [seg setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [seg setTag:inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [seg setEnabled:inEnabled];
                        
            // segment count
            if ( inSegments == kFBParamMissing ) inSegments = 3;
            [seg setSegmentCount:inSegments];
            
            for ( NSInteger index = 0; index < inSegments; index++ ) {
                [seg setWidth:rect.size.width/inSegments forSegment:index];
            }
            
            // wndTag
            [seg setWndTag:inWndTag];
            
            // index
            if ( inIndex != kFBParamMissing ) [seg setSelected:YES forSegment:inIndex];
            
            // target/action
            [seg setTarget:CocoaControlTargetObj()];
            [seg setAction:@selector(controlAction:)];

            // add to window
            [contentView addSubview:seg];
            [seg release];
        }
        
        [seg setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - slider
///////////////////////////////////////////////////
//                  cocoa slider                 //
///////////////////////////////////////////////////

// private
NSSlider *SliderWithTag( SInt32 tag )
{
    NSSlider *view = (NSSlider *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSSlider class] ) return view;
    }
    return nil;
}

void SliderSetType( SInt32 tag, NSSliderType type )
{
    NSSlider *slider = SliderWithTag( tag );
    if ( slider ) [[slider cell] setSliderType:type];
}

void SliderSetAllowsTickMarkValuesOnly( SInt32 tag, Boolean flag )
{
    NSSlider *slider = SliderWithTag( tag );
    if ( slider ) [slider setAllowsTickMarkValuesOnly:flag];
}

void SliderSetTickMarkPosition( SInt32 tag, NSTickMarkPosition position )
{
    NSSlider *slider = SliderWithTag( tag );
    if ( slider ) [slider setTickMarkPosition:position];
}

void SliderSetAltIncrementValue( SInt32 tag, double value )
{
    NSSlider *slider = SliderWithTag( tag );
    if ( slider ) [slider setAltIncrementValue:value];
}

void SliderSetKnobThickness( SInt32 tag, CGFloat thickness )
{
    NSSlider *slider = SliderWithTag( tag );
    if ( slider ) [slider setKnobThickness:thickness];
}

void CocoaSlider( SInt32 inTag, SInt32 inEnabled, double inValue, CGRect inRect, double inMinValue, double inMaxValue, SInt32 inTickMarks, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSSlider *slider = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( slider ) {
            
            // make sure it's same view type
            if ( [slider class] != [NSSlider class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [slider setEnabled:inEnabled];
            
            // min
            if ( (int)inMinValue != kFBParamMissing ) [slider setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue != kFBParamMissing ) [slider setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue != kFBParamMissing ) {
                if ( inValue <= inMaxValue ) [slider setDoubleValue:inValue];
            }
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [slider setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                if ( inTickMarks > 0 ) {
                    rect = NSMakeRect( 18, 17, 24, 96 );
                } else {
                    rect = NSMakeRect( 18, 17, 19, 96 );
                }
            }
            
            // init
            slider = [[NSSlider alloc] initWithFrame:rect];
            
            // default autoresizing
            [slider setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [slider setTag:inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [slider setEnabled:inEnabled];
            
            // min
            if ( (int)inMinValue == kFBParamMissing ) inMinValue = 0.0;
            [slider setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue == kFBParamMissing ) inMaxValue = 100.0;
            [slider setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue == kFBParamMissing ) inValue = inMinValue;
            if ( inValue <= inMaxValue ) [slider setDoubleValue:inValue];
            
            // tick marks
            if ( inTickMarks == kFBParamMissing ) inTickMarks = 0;
            [slider setNumberOfTickMarks:inTickMarks];
            
            // continuous
            [slider setContinuous:NO];
            
            // wndTag
            [slider setWndTag:inWndTag];
            
            // target/action
            [slider setTarget:CocoaControlTargetObj()];
            [slider setAction:@selector(controlAction:)];

            // add to window
            [contentView addSubview:slider];
            [slider release];
        }
        
        [slider setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - splitview
///////////////////////////////////////////////////
//                  cocoa splitview              //
///////////////////////////////////////////////////

@implementation NSSplitView (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

@implementation CocoaSplitViewDelegate

//- (void)splitView:(NSSplitView *)splitView resizeSubviewsWithOldSize:(NSSize)oldSize {
//    DialogEventSetNSSize( oldSize );
//    CocoaOutputWindowSetTag( [splitView wndTag] );
//    CallUserDefinedOnDialogFunction( splitViewResizeSubviewsWithOldSize, [splitView tag], [splitView wndTag] );
//}

- (void)splitViewWillResizeSubviews:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object wndTag] );
    CallUserDefinedOnDialogFunction( splitViewWillResizeSubviews, [notification.object tag], [notification.object wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewWillResizeSubviews, notification.object );
}

- (void)splitViewDidResizeSubviews:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object wndTag] );
    CallUserDefinedOnDialogFunction( splitViewDidResizeSubviews, [notification.object tag], [notification.object wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewDidResizeSubviews, notification.object );
}

- (BOOL)splitView:(NSSplitView *)splitView canCollapseSubview:(NSView *)subview {
    DialogEventSetBool( NO );
    DialogEventSetViewTag( [subview tag] );
    CocoaOutputWindowSetTag( [splitView wndTag] );
    CallUserDefinedOnDialogFunction( splitViewCanCollapseSubview, [splitView tag], [splitView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewCanCollapseSubview, splitView );
    return DialogEventBool();
}

- (BOOL)splitView:(NSSplitView *)splitView shouldCollapseSubview:(NSView *)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex {
    DialogEventSetBool( NO );
    DialogEventSetViewTag( [subview tag] );
    DialogEventSetLong( dividerIndex );
    CocoaOutputWindowSetTag( [splitView wndTag] );
    CallUserDefinedOnDialogFunction( splitViewShouldCollapseSubview, [splitView tag], [splitView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewShouldCollapseSubview, splitView );
    return DialogEventBool();
}

- (BOOL)splitView:(NSSplitView *)splitView shouldAdjustSizeOfSubview:(NSView *)view {
    DialogEventSetBool( YES );
    DialogEventSetViewTag( [view tag] );
    CocoaOutputWindowSetTag( [splitView wndTag] );
    CallUserDefinedOnDialogFunction( splitViewShouldAdjustSizeOfSubview, [splitView tag], [splitView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewShouldAdjustSizeOfSubview, splitView );
    return DialogEventBool();
}

- (NSRect)splitView:(NSSplitView *)splitView effectiveRect:(NSRect)proposedEffectiveRect forDrawnRect:(NSRect)drawnRect ofDividerAtIndex:(NSInteger)dividerIndex {
    
    return proposedEffectiveRect;
}

- (BOOL)splitView:(NSSplitView *)splitView shouldHideDividerAtIndex:(NSInteger)dividerIndex {
    DialogEventSetBool( YES );
    DialogEventSetLong( dividerIndex );
    CocoaOutputWindowSetTag( [splitView wndTag] );
    CallUserDefinedOnDialogFunction( splitViewShouldHideDividerAtIndex, [splitView tag], [splitView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewShouldHideDividerAtIndex, splitView );
    return DialogEventBool();
}

//- (NSRect)splitView:(NSSplitView *)splitView additionalEffectiveRectOfDividerAtIndex:(NSInteger)dividerIndex {
//
//    return rect;
//}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex {
    DialogEventSetFloat( proposedMaximumPosition );
    DialogEventSetLong( dividerIndex );
    CocoaOutputWindowSetTag( [splitView wndTag] );
    CallUserDefinedOnDialogFunction( splitViewConstrainMaxCoordinate, [splitView tag], [splitView wndTag] );
 //   CocoaControlCallUserDefinedOnDialogOrCallback( splitViewConstrainMaxCoordinate, splitView );
    return DialogEventFloat();
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex {
    DialogEventSetFloat( proposedMinimumPosition );
    DialogEventSetLong( dividerIndex );
    CocoaOutputWindowSetTag( [splitView wndTag] );
    CallUserDefinedOnDialogFunction( splitViewConstrainMinCoordinate, [splitView tag], [splitView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( splitViewConstrainMinCoordinate, splitView );
    return DialogEventFloat();
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainSplitPosition:(CGFloat)proposedPosition ofSubviewAt:(NSInteger)dividerIndex {
    
    return proposedPosition;
}

@end

CocoaSplitViewDelegate *CocoaSplitViewDelegateGet()
{
    static CocoaSplitViewDelegate *sCocoaSplitViewDelegate = nil;
    if ( !sCocoaSplitViewDelegate ) sCocoaSplitViewDelegate = [[CocoaSplitViewDelegate alloc] init];
    return sCocoaSplitViewDelegate;
}

// private
NSSplitView *SplitViewWithTag( SInt32 tag )
{
    NSSplitView *view = (NSSplitView *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSSplitView class] ) return view;
    }
    return nil;
}


void SplitViewAdjustSubviews( SInt32 tag )
{
    NSSplitView *splitView = SplitViewWithTag( tag );
    if ( splitView ) [splitView adjustSubviews];
}

BOOL SplitViewIsPaneCollapsed( SInt32 tag, SInt32 index )
{
    NSSplitView *splitView = SplitViewWithTag( tag );
    if ( splitView ) {
        NSArray *views = [splitView subviews];
        NSView *view = [views objectAtIndex:index];
        return [splitView isSubviewCollapsed:view];
    }
    return NO;
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void SplitViewSetHoldingPriority( SInt32 tag, SInt32 index, float priority )
{
    NSSplitView *splitView = SplitViewWithTag( tag );
    if ( splitView ) {
        if ( [splitView respondsToSelector:@selector(setHoldingPriority:forSubviewAtIndex:)] ) {
            [splitView setHoldingPriority:priority forSubviewAtIndex:index];    // macOS 10.8
        }
    }
}
#endif

CGFloat SplitViewDividerThickness( SInt32 tag )
{
    NSSplitView *splitView = SplitViewWithTag( tag );
    if ( splitView ) {
        return [splitView dividerThickness];
    }
    return 0.0;
}

void SplitViewSetPositionOfDivider( SInt32 tag, SInt32 index, CGFloat position )
{
    NSSplitView *splitView = SplitViewWithTag( tag );
    if ( splitView ) {
        [splitView setPosition:position ofDividerAtIndex:index];
    }
}

void SplitViewAddSubview( SInt32 splitViewTag, SInt32 index, SInt32 subviewTag )
{
    NSSplitView *splitView = SplitViewWithTag( splitViewTag );
    if ( splitView ) {
        NSArray *views = [splitView subviews];
        NSView *view = [views objectAtIndex:index];
        id subview = CocoaViewWithTag( subviewTag );
        if ( subview ) {
            [view addSubview:subview];
        }
    }
}


void CocoaSplitView( SInt32 inTag, CGRect inRect, SInt32 inVertical, SInt32 inDividerStyle, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSSplitView *splitView = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( splitView ) {    // exists
            
            // make sure it's same view type
            if ( [splitView class] != [NSSplitView class] ) return;
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [splitView setFrame:rect];
            
        } else {        // create
            
            BOOL vertical = NO;
            if ( inVertical != kFBParamMissing ) vertical = inVertical;
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 163, 96 );
            
            // init
            splitView = [[NSSplitView alloc] initWithFrame:rect];
            
            // default autoresizing
            [splitView setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [splitView setTag:inTag];
            
            // vertical
            if ( vertical ) [splitView setVertical:YES];
            
            // divider style
            if ( inDividerStyle == kFBParamMissing ) {
                [splitView setDividerStyle:NSSplitViewDividerStylePaneSplitter];
            } else {
                [splitView setDividerStyle:inDividerStyle];
            }
            
            // subviews
            CGFloat dividerThickness = [splitView dividerThickness];
            CGFloat viewWidth, viewHeight;
            CGFloat view1x = 0.0;
            CGFloat view1y = 0.0;
            
            if ( vertical ) {
                viewWidth = (rect.size.width - dividerThickness) / 2;
                viewHeight = rect.size.height;
                view1x = viewWidth+dividerThickness;
            } else {
                viewWidth = rect.size.width;
                viewHeight = (rect.size.height - dividerThickness) / 2;
                view1y = viewHeight+dividerThickness;
            }
            
            NSView *view0 = [[NSView alloc] initWithFrame:NSMakeRect(0,0,viewWidth,viewHeight)];
            NSView *view1 = [[NSView alloc] initWithFrame:NSMakeRect(view1x,view1y,viewWidth,viewHeight)];
            
            [splitView addSubview:view0];
            [splitView addSubview:view1];
            
            // wndTag
            [splitView setWndTag:inWndTag];
            
            // delegate
            [splitView setDelegate:CocoaSplitViewDelegateGet()];
            
            // add to window
            [contentView addSubview:splitView];
            [splitView release];
        }
        
        [splitView setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}




#pragma mark - stepper
///////////////////////////////////////////////////
//                  cocoa stepper                //
///////////////////////////////////////////////////

// private
NSStepper *StepperWithTag( SInt32 tag )
{
    NSStepper *view = (NSStepper *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSStepper class] ) return view;
    }
    return nil;
}

void StepperSetAutorepeat( SInt32 tag, Boolean flag )
{
    NSStepper *stepper = StepperWithTag( tag );
    if ( stepper ) [stepper setAutorepeat:flag];
}

void StepperSetValueWraps( SInt32 tag, Boolean flag )
{
    NSStepper *stepper = StepperWithTag( tag );
    if ( stepper ) [stepper setValueWraps:flag];
}

void StepperSetContinuous( SInt32 tag, Boolean flag )
{
    NSStepper *stepper = StepperWithTag( tag );
    if ( stepper ) [stepper setContinuous:flag];
}

void StepperSetIncrement( SInt32 tag, double increment )
{
    NSStepper *stepper = StepperWithTag( tag );
    if ( stepper ) [stepper setIncrement:increment];
}

void CocoaStepper( SInt32 inTag, SInt32 inEnabled, double inValue, CGRect inRect, double inMinValue, double inMaxValue, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSStepper *stepper = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( stepper ) {
            
            // make sure it's same view type
            if ( [stepper class] != [NSStepper class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [stepper setEnabled:inEnabled];
            
            // min
            if ( (int)inMinValue != kFBParamMissing ) [stepper setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue != kFBParamMissing ) [stepper setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue != kFBParamMissing ) {
                if ( inValue <= inMaxValue ) [stepper setDoubleValue:inValue];
            }
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [stepper setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 17, 18, 19, 27 );
            }
            
            // init
            stepper = [[NSStepper alloc] initWithFrame:rect];
            
            // default autoresizing
            [stepper setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [stepper setTag:inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [stepper setEnabled:inEnabled];
            
            // min
            if ( (int)inMinValue == kFBParamMissing ) inMinValue = 0.0;
            [stepper setMinValue:inMinValue];
            
            // max
            if ( (int)inMaxValue == kFBParamMissing ) inMaxValue = 100.0;
            [stepper setMaxValue:inMaxValue];
            
            // value
            if ( (int)inValue == kFBParamMissing ) inValue = inMinValue;
            if ( inValue <= inMaxValue ) [stepper setDoubleValue:inValue];
            
            // value wraps
            [stepper setValueWraps:NO];
            
            // wndTag
            [stepper setWndTag:inWndTag];
            
            // target/action
            [stepper setTarget:CocoaControlTargetObj()];
            [stepper setAction:@selector(controlAction:)];

            // add to window
            [contentView addSubview:stepper];
            [stepper release];
        }
        
        [stepper setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - tableview

///////////////////////////////////////////////////
//                  cocoa tableview              //
///////////////////////////////////////////////////

@implementation CocoaTableViewDelegate

@synthesize table;
@synthesize tableData;

- (id)init {
    if ( self = [super init] ) {
        self.tableData = [[NSMutableArray alloc] initWithCapacity:0];
    }
    return self;
}

- (void)singleAction:(id)sender {
    CocoaOutputWindowSetTag( [sender wndTag] );
//    CallUserDefinedOnDialogFunction( 1, [sender tag], [sender wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( 1, sender );
}

- (void)doubleAction:(id)sender {
    CocoaOutputWindowSetTag( [sender wndTag] );
//    CallUserDefinedOnDialogFunction( tableViewDoubleAction, [table tag], [table wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( tableViewDoubleAction, sender );
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( tableViewSelectionDidChange, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( tableViewSelectionDidChange, notification.object );
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return [tableData count];
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    
    NSString *identifier = [tableColumn identifier];
    NSTableCellView *cellView = [tableView makeViewWithIdentifier:identifier owner:self];
    
    NSDictionary *rowDict = [tableData objectAtIndex:row];
    id obj = [rowDict objectForKey:identifier];
    
    [cellView.textField setStringValue:obj];
    if ( [cellView.textField isEditable] ) {
        [cellView.textField setDelegate:self];
    }
    
    return cellView;
}


- (void)controlTextDidEndEditing:(NSNotification *)notification {
    NSTextField *textField = [notification object];
    NSInteger row = [table rowForView:textField];
    NSInteger col = [table columnForView:textField];
    NSTableColumn *column = [[table tableColumns] objectAtIndex:col];
    NSString *identifier = [column identifier];
    NSMutableDictionary *dict = (NSMutableDictionary *)[tableData objectAtIndex:row];
    [dict setObject:[textField stringValue] forKey:identifier];
}

@end

// private
NSTableView *TableViewWithTag( SInt32 tag )
{
    NSTableView *tableView = (NSTableView *)CocoaViewWithTag( tag );
    if ( tableView ) {
        if ( [tableView class] == [NSTableView class] ) return tableView;
    }
    return nil;
}

// public
void TableViewSetData( SInt32 tag, CFMutableArrayRef array )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) {
        CocoaTableViewDelegate *delegate = (CocoaTableViewDelegate *)[tableView delegate];
        [delegate setTableData:(NSMutableArray *)array];
        [tableView reloadData];
    }
}

CFMutableArrayRef TableViewData( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) {
        CocoaTableViewDelegate *delegate = (CocoaTableViewDelegate *)[tableView delegate];
        return (CFMutableArrayRef)delegate.tableData;
    }
    return nil;
}

void TableViewReloadData( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) {
        if ( [tableView class] == [NSTableView class] ) {
            [tableView reloadData];
        }
    }
}

void TableViewScrollRowToVisible( SInt32 tag, SInt32 row )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) [tableView scrollRowToVisible:row];
}

void TableViewScrollColumnToVisible( SInt32 tag, SInt32 col )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) [tableView scrollColumnToVisible:col];
}

SInt32 TableViewSelectedRow( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView selectedRow];
    return -1;
}

SInt32 TableViewSelectedColumn( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView selectedColumn];
    return -1;
}

CFArrayRef TableViewSelectedRowIndexes( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) {
        NSIndexSet *set = [tableView selectedRowIndexes];
        if ( set ) {
            NSMutableArray *array = [NSMutableArray arrayWithCapacity:0];
            [set enumerateIndexesUsingBlock:^(NSUInteger index, BOOL *stop) {
                [array addObject:[NSNumber numberWithInteger:index]];
            }];
            return (CFArrayRef)array;
        }
    }
    return nil;
}

SInt32 TableViewNumberOfSelectedRows( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView numberOfSelectedRows];
    return 0;
}

Boolean TableViewIsRowSelected( SInt32 tag, SInt32 row )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView isRowSelected:row];
    return false;
}

SInt32 TableViewClickedRow( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView clickedRow];
    return 0;
}

SInt32 TableViewClickedColumn( SInt32 tag )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView clickedColumn];
    return 0;
}

// custom
void *TableViewView( SInt32 tag, SInt32 row, SInt32 col )
{
    NSTableView *tableView = TableViewWithTag( tag );
    if ( tableView ) return [tableView viewAtColumn:col row:row makeIfNecessary:YES];
    return nil;
}

#pragma mark - tabview
///////////////////////////////////////////////////
//                  cocoa tabview                //
///////////////////////////////////////////////////

@implementation NSTabView (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

@implementation CocoaTabViewDelegate

@synthesize boolVal;
@synthesize indexVal;

- (void)tabViewDidChangeNumberOfTabViewItems:(NSTabView *)tabView {
    CocoaOutputWindowSetTag( [tabView wndTag] );
    CallUserDefinedOnDialogFunction( tabViewDidChangeNumberOfTabViewItems, [tabView tag], [tabView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( tabViewDidChangeNumberOfTabViewItems, tabView );
}

- (BOOL)tabView:(NSTabView *)tabView shouldSelectTabViewItem:(NSTabViewItem *)tabViewItem {
    DialogEventSetBool( YES );
    CocoaOutputWindowSetTag( [tabView wndTag] );
    CallUserDefinedOnDialogFunction( tabViewShouldSelectTabViewItem, [tabView tag], [tabView wndTag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( tabViewShouldSelectTabViewItem, tabView );
    return DialogEventBool();
}

- (void)tabView:(NSTabView *)tabView willSelectTabViewItem:(NSTabViewItem *)tabViewItem {
    DialogEventSetLong( [tabView indexOfTabViewItem:tabViewItem] );
    CocoaOutputWindowSetTag( [tabView wndTag] );
    CallUserDefinedOnDialogFunction( tabViewWillSelectTabViewItem, [tabView tag], [tabView wndTag] );
 //   CocoaControlCallUserDefinedOnDialogOrCallback( tabViewWillSelectTabViewItem, tabView );
}

- (void)tabView:(NSTabView *)tabView didSelectTabViewItem:(NSTabViewItem *)tabViewItem {
    DialogEventSetLong( [tabView indexOfTabViewItem:tabViewItem] );
    CocoaOutputWindowSetTag( [tabView wndTag] );
    CallUserDefinedOnDialogFunction( 1, [tabView tag], [tabView wndTag] );// 1 == kFBBtnClick
//    CocoaControlCallUserDefinedOnDialogOrCallback( 1, tabView );
}
@end

CocoaTabViewDelegate *CocoaTabViewDelegateGet()
{
    static CocoaTabViewDelegate *sCocoaTabViewDelegate = nil;
    if ( !sCocoaTabViewDelegate ) sCocoaTabViewDelegate = [[CocoaTabViewDelegate alloc] init];
    return sCocoaTabViewDelegate;
}


// private
NSTabView *TabViewWithTag( SInt32 tag )
{
    NSTabView *tabView = (NSTabView *)CocoaViewWithTag( tag );
    if ( tabView ) {
        if ( [tabView class] == [NSTabView class] ) return tabView;
    }
    return nil;
}



void TabViewSetType( SInt32 tag, SInt32 type )
{
    NSTabView *c = TabViewWithTag( tag );
    if ( c ) [c setTabViewType:type];
}

SInt32 TabViewIndex( SInt32 tag )
{
    NSTabView *c = TabViewWithTag( tag );
    if ( c ) {
        NSTabViewItem *item = [c selectedTabViewItem];
        if ( item ) return [c indexOfTabViewItem:item];
    }
    return 0;
}

void TabViewSelectNextItem( SInt32 tag )
{
    NSTabView *tab = TabViewWithTag( tag );
    if ( tab ) [tab selectNextTabViewItem:nil];
}

void TabViewSelectPreviousItem( SInt32 tag )
{
    NSTabView *tab = TabViewWithTag( tag );
    if ( tab ) [tab selectPreviousTabViewItem:nil];
}

void TabViewSetFont( SInt32 tag, CTFontRef font )
{
    NSTabView *tab = TabViewWithTag( tag );
    if ( tab ) [tab setFont:(NSFont *)font];
}

void TabViewSetControlTint( SInt32 tag, NSControlTint tint )
{
    NSTabView *tab = TabViewWithTag( tag );
    if ( tab ) [tab setControlTint:tint];
}

void TabViewSetDrawsBackground( SInt32 tag, Boolean flag )
{
    NSTabView *tab = TabViewWithTag( tag );
    if ( tab ) [tab setDrawsBackground:flag];
}

CGRect TabViewContentRect( SInt32 tag )
{
    NSTabView *tab = TabViewWithTag( tag );
    if ( tab ) return NSRectToCGRect([tab contentRect]);
    return CGRectZero;
}


// custom
void TabViewAddSubview( SInt32 tabTag, SInt32 index, SInt32 subviewTag )
{
    NSTabView *tabView = TabViewWithTag( tabTag );
    if ( tabView ) {
        NSTabViewItem *item = [tabView tabViewItemAtIndex:index];
        if ( item ) {
            id subview = CocoaViewWithTag( subviewTag );
            if ( subview ) {
                [[item view] addSubview:subview];
            }
        }
    }
}





void CocoaTabView( SInt32 inTag, SInt32 inIndex, CFTypeRef inItems, CGRect inRect, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSTabView *tab = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( tab ) {    // exists
            
            // make sure it's same view type
            if ( [tab class] != [NSTabView class] ) return;
            
            // index
            if ( inIndex != kFBParamMissing ) {
                if ( inIndex < [tab numberOfTabViewItems] ) [tab selectTabViewItemAtIndex:inIndex];
            }
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [tab setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 13, 2, 177, 124 );
            
            // init
            tab = [[NSTabView alloc] initWithFrame:rect];
            
            // default autoresizing
            [tab setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [tab setTag:inTag];
            
            // items
            NSArray *items = nil;
            
            if ( inItems ) {
                if ( CFGetTypeID(inItems) == CFStringGetTypeID() ) {
                    items = [(NSString *)inItems componentsSeparatedByString:@";"];
                    
                } else if ( CFGetTypeID(inItems) == CFArrayGetTypeID() ) {
                    items = (NSArray *)inItems;
                }
            } else {
                items = [NSArray arrayWithObjects:@"Tab 1",@"Tab 2",nil];
            }
            
            NSInteger count = [items count];
            for ( NSInteger index = 0; index < count; index++ ) {
                NSTabViewItem *item = [[NSTabViewItem alloc] initWithIdentifier:[NSString stringWithFormat:@"%ld",(long)index+1]];
                [item setLabel:[items objectAtIndex:index]];
                [[item view] setFrame:[tab contentRect]];
                [tab addTabViewItem:item];
            }
            
            // index
            if ( inIndex != kFBParamMissing ) {
                if ( inIndex < [tab numberOfTabViewItems] ) [tab selectTabViewItemAtIndex:inIndex];
            }
            
            // wndTag
            [tab setWndTag:inWndTag];
            
            // fix font
            // CocoaControlFixFont( tab );
            
            // target
            [tab setDelegate:CocoaTabViewDelegateGet()];
            
            // add to window
            [contentView addSubview:tab];
            [tab release];
        }
        
        [tab setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - textfield formatter
///////////////////////////////////////////////////
//        cocoa textfield formatter              //
///////////////////////////////////////////////////
@implementation CocoaTextFieldFormatter

@synthesize set, pass, length;

- (id)initWithCharacters:(NSString *)inCharacters pass:(BOOL)inPass length:(NSUInteger)inLength caseSensitive:(BOOL)inCaseSensitive {
    if ( self = [super init] ) {
        
        NSMutableString *string = [inCharacters mutableCopy];
        if ( !inCaseSensitive ) {
            [string appendString:[string uppercaseString]];
            [string appendString:[string lowercaseString]];
        }
        
        if ( inPass ) {
            set = [[NSMutableCharacterSet characterSetWithCharactersInString:string] invertedSet];
        } else {
            set = [NSMutableCharacterSet characterSetWithCharactersInString:string];
        }
        
        self.pass = inPass;
        self.length = inLength;
        
        [string release];
    }
    return self;
}

- (NSString *)stringForObjectValue:(id)obj {
    if ( [obj isKindOfClass:[NSString class]] ) return [NSString stringWithString:obj];
    return nil;
}

- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString **)error {
    if ( string != nil ) {
        *obj = [NSString stringWithString:string];
        return YES;
    }
    return NO;
}

- (BOOL)isPartialStringValid:(NSString **)partialStringPtr proposedSelectedRange:(NSRangePointer)proposedSelRangePtr originalString:(NSString *)origString originalSelectedRange:(NSRange)origSelRange errorDescription:(NSString **)error {
    
    if ( length ) {
        if ([*partialStringPtr length] > length) {
            return NO;
        }
    }
    
    NSString *resultString = [[*partialStringPtr componentsSeparatedByCharactersInSet:set] componentsJoinedByString:@""];
    if ( ![resultString isEqualToString:*partialStringPtr] ) {
        *partialStringPtr = resultString;
        return NO;
    }
    return YES;
}

@end


#pragma mark - textfield
///////////////////////////////////////////////////
//                  cocoa textfield              //
///////////////////////////////////////////////////

@implementation CocoaTextFieldDelegate
- (void)controlTextDidBeginEditing:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidBeginEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidBeginEditing, notification.object );
}

- (void)controlTextDidChange:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidChange, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidChange, notification.object );
}

- (void)controlTextDidEndEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidEndEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidEndEditing, notification.object );
}
@end

CocoaTextFieldDelegate *CocoaTextFieldDelegateGet()
{
    static CocoaTextFieldDelegate *sCocoaTextFieldDelegate = nil;
    if ( !sCocoaTextFieldDelegate ) sCocoaTextFieldDelegate = [[CocoaTextFieldDelegate alloc] init];
    return sCocoaTextFieldDelegate;
}


// private
NSTextField *TextFieldWithTag( SInt32 tag )
{
    NSTextField *textField = (NSTextField *)CocoaViewWithTag( tag );
    if ( textField ) {
        if ( [textField class] == [NSTextField class] ) return textField;
    }
    return nil;
}



void TextFieldSetEditable( SInt32 tag, Boolean flag )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld setEditable:flag];
}

void TextFieldSetSelectable( SInt32 tag, Boolean flag )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld setSelectable:flag];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void TextFieldSetPlaceholderString( SInt32 tag, CFStringRef string )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) {
        if ( [fld respondsToSelector:@selector(setPlaceholderString:)] ) {
            [fld setPlaceholderString:(NSString *)string];   // macOS 10.10
        }
    }
}

void TextFieldSetPlaceholderAttributedString( SInt32 tag, CFAttributedStringRef string )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld setPlaceholderAttributedString:(NSAttributedString *)string];
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void TextFieldSetTextColor( SInt32 tag, CGColorRef col )
{
    if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
        NSTextField *fld = TextFieldWithTag( tag );
        if ( fld ) [fld setTextColor:[NSColor colorWithCGColor:col]];   // macOS 10.8
    }
}

void TextFieldSetBackgroundColor( SInt32 tag, CGColorRef col )
{
    if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
        NSTextField *fld = TextFieldWithTag( tag );
        if ( fld ) [fld setBackgroundColor:[NSColor colorWithCGColor:col]];   // macOS 10.8
    }
}
#endif

void TextFieldSetDrawsBackground( SInt32 tag, Boolean flag )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld setDrawsBackground:flag];
}

void TextFieldSelectText( SInt32 tag )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld selectText:nil];
}

void TextFieldSetAllowsEditingTextAttributes( SInt32 tag, Boolean flag )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld setAllowsEditingTextAttributes:flag];
}

void TextFieldSetImportsGraphics( SInt32 tag, Boolean flag )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) [fld setImportsGraphics:flag];
}


// custom
void TextFieldSetTextRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) {
        NSColor *col = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
        [fld setTextColor:col];
    }
}

void TextFieldSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) {
        NSColor *col = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
        [fld setBackgroundColor:col];
    }
}

void TextFieldSetFormat( SInt32 tag, CFStringRef characters, Boolean pass, UInt32 length, Boolean caseSensitive )
{
    NSTextField *fld = TextFieldWithTag( tag );
    if ( fld ) {
        CocoaTextFieldFormatter *formatter = [[CocoaTextFieldFormatter alloc] initWithCharacters:(NSString *)characters pass:pass length:length caseSensitive:caseSensitive];
        [fld setFormatter:formatter];
        [formatter release];
    }
}


void CocoaTextField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *text = (NSString *)inText;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSTextField *fld = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( fld ) {    // text field already exists
            
            // make sure it's same view type
            if ( [fld class] != [NSTextField class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [fld setFrame:rect];
            
        } else {        // create text field
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 96, 22 );
            
            // init
            fld = [[NSTextField alloc] initWithFrame:rect];
            
            // default autoresizing
            [fld setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [fld setTag: inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // wndTag
            [fld setWndTag:inWndTag];
            
            // fix font to match I.B.
            CocoaControlFixFont( fld );
            
            // delegate
            [fld setDelegate:CocoaTextFieldDelegateGet()];
            
            // add to window
            [contentView addSubview:fld];
            [fld release];
        }
        
        [fld setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}





#pragma mark - textview
///////////////////////////////////////////////////
//                  cocoa textview               //
///////////////////////////////////////////////////

@implementation NSTextView (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end

// private
NSTextView *TextViewWithTag( SInt32 tag )
{
    NSTextView *view = (NSTextView *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSTextView class] ) return view;
    }
    return nil;
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void TextViewSetBackgroundColor( SInt32 tag, CGColorRef col )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) {
        if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
            [textView setBackgroundColor:[NSColor colorWithCGColor:col]];  // macOS 10.8
        }
    }
}
#endif

void TextViewSetDrawsBackground( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setDrawsBackground:flag];
}

void TextViewSetEditable( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setEditable:flag];
}

void TextViewSetSelectable( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setSelectable:flag];
}

CFStringRef TextViewString( SInt32 tag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) {
        return (CFStringRef)[textView string];
    }
    return NULL;
}

void TextViewSetString( SInt32 tag, CFStringRef string )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setString:(NSString *)string];
}

void TextViewSetUsesRuler( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setUsesRuler:flag];
}

void TextViewSetRulerVisible( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setRulerVisible:flag];
}

void TextViewSetUsesInspectorBar( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setUsesInspectorBar:flag];
}

void TextViewSetSelectedRange( SInt32 tag, CFRange range )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) {
        [textView setSelectedRange:NSMakeRange(range.location,range.length)];
    }
}

void TextViewSetContinuousSpellCheckingEnabled( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setContinuousSpellCheckingEnabled:flag];
}

void TextViewSetGrammarCheckingEnabled( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setGrammarCheckingEnabled:flag];
}

void TextViewStartSpeaking( SInt32 tag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView startSpeaking:nil];
}

void TextViewStopSpeaking( SInt32 tag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView stopSpeaking:nil];
}

void TextViewSetUsesFontPanel( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setUsesFontPanel:flag];
}

void TextViewSetUsesFindPanel( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setUsesFindPanel:flag];
}

void TextViewSetUsesFindBar( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setUsesFindBar:flag];
}

CFMutableAttributedStringRef TextViewTextStorage( SInt32 tag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) return (CFMutableAttributedStringRef)[textView textStorage];
    return NULL;
}

void TextViewSetRichText( SInt32 tag, Boolean flag )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) [textView setRichText:flag];
}

// custom
void TextViewPerformFindPanelAction( SInt32 tag, NSTextFinderAction action )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) {
        NSControl *c = [[NSControl alloc] init];
        [c setTag:action];
        [textView performFindPanelAction:c];
        [c release];
    }
}

void TextViewSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSTextView *textView = TextViewWithTag( tag );
    if ( textView ) {
        [textView setBackgroundColor:[NSColor colorWithCalibratedRed:r green:g blue:b alpha:a]];
    }
}


void CocoaTextView( SInt32 inTag, CGRect inRect, SInt32 inScrollTag, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSTextView *textView = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( textView ) {    // already exists
            
            // make sure it's same view type
            if ( [textView class] != [NSTextView class] ) return;
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [textView setFrame:rect];
            
        } else {            // create
            
            NSScrollView *scrollView = nil;
            if ( inScrollTag ) {
                scrollView = CocoaViewSubviewWithTag( contentView, inScrollTag );
            }
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                if ( scrollView ) {
                    NSRect scrollRect = [scrollView frame];
                    rect = NSMakeRect( 0, 0, scrollRect.size.width, scrollRect.size.height );
                } else {
                    rect = NSMakeRect( 20, 20, 240, 135 );
                }
            }
            
            // init
            textView = [[NSTextView alloc] initWithFrame:rect];
            
            // max size
            [textView setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
            
            // vertically resizable
            [textView setVerticallyResizable:YES];
            
            // autoresizing
            [textView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
            
            // tag
            [textView setTag:inTag];
            
            // add to superview
            if ( scrollView ) {
                [scrollView setDocumentView:textView];
            } else {
                [contentView addSubview:textView];
            }
            
            // wndTag
            [textView setWndTag:inWndTag];
            
            [textView release];
        }
        [textView setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}



#pragma mark - tokenfield
///////////////////////////////////////////////////
//                  cocoa tokenfield             //
///////////////////////////////////////////////////

@implementation CocoaTokenFieldDelegate

@synthesize boolVal;

- (void)controlTextDidBeginEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidBeginEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidBeginEditing, notification.object );
}

- (void)controlTextDidChange:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidChange, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidChange, notification.object );
}

- (void)controlTextDidEndEditing:(NSNotification *)notification {
//    CocoaOutputWindowSetTag( [notification.object wndTag] );
//    CallUserDefinedOnDialogFunction( textFieldDidEndEditing, [notification.object tag], [notification.object wndTag] );
    CocoaControlCallUserDefinedOnDialogOrCallback( textFieldDidEndEditing, notification.object );
}

/*
 - (NSString *)tokenField:(NSTokenField *)tokenField displayStringForRepresentedObject:(id)representedObject {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSString *string = nil;
 
 
 return string;
 }
 
 - (NSTokenStyle)tokenField:(NSTokenField *)tokenField styleForRepresentedObject:(id)representedObject {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSTokenStyle style = 0;
 
 return style;
 }
 
 - (NSArray *)tokenField:(NSTokenField *)tokenField completionsForSubstring:(NSString *)substring indexOfToken:(NSInteger)tokenIndex indexOfSelectedItem:(NSInteger *)selectedIndex {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSArray *completions = nil;
 
 return completions;
 }
 
 - (NSString *)tokenField:(NSTokenField *)tokenField editingStringForRepresentedObject:(id)representedObject {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSString *string = nil;
 
 return string;
 }
 
 - (id)tokenField:(NSTokenField *)tokenField representedObjectForEditingString:(NSString *)editingString {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 id obj = nil;
 
 return obj;
 }
 
 - (NSArray *)tokenField:(NSTokenField *)tokenField shouldAddObjects:(NSArray *)tokens atIndex:(NSUInteger)index {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSArray *objects = nil;
 
 return objects;
 }
 
 - (NSArray *)tokenField:(NSTokenField *)tokenField readFromPasteboard:(NSPasteboard *)pboard {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSArray *objects = nil;
 
 return objects;
 }
 
 - (BOOL)tokenField:(NSTokenField *)tokenField writeRepresentedObjects:(NSArray *)objects toPasteboard:(NSPasteboard *)pboard {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 BOOL flag = NO;
 
 return flag;
 }
 
 - (BOOL)tokenField:(NSTokenField *)tokenField hasMenuForRepresentedObject:(id)representedObject {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 BOOL flag = NO;
 
 return flag;
 }
 
 - (NSMenu *)tokenField:(NSTokenField *)tokenField menuForRepresentedObject:(id)representedObject {
 CocoaOutputWindowSetTag( [tokenField wndTag] );
 NSMenu *menu = nil;
 
 return menu;
 }
 */

@end

CocoaTokenFieldDelegate *CocoaTokenFieldDelegateGet()
{
    static CocoaTokenFieldDelegate *sCocoaTokenFieldDelegate = nil;
    if ( !sCocoaTokenFieldDelegate ) sCocoaTokenFieldDelegate = [[CocoaTokenFieldDelegate alloc] init];
    return sCocoaTokenFieldDelegate;
}

// private
NSTokenField *TokenFieldWithTag( SInt32 tag )
{
    NSTokenField *view = (NSTokenField *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [NSTokenField class] ) return view;
    }
    return nil;
}

void TokenFieldSetTokenizingCharacterSet( SInt32 tag, CFCharacterSetRef set )
{
    NSTokenField *fld = TokenFieldWithTag( tag );
    if ( fld ) [fld setTokenizingCharacterSet:(NSCharacterSet *)set];
}



void CocoaTokenField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inStyle, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *text = (NSString *)inText;
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        NSTokenField *fld = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( fld ) {    // already exists
            
            // make sure it's same view type
            if ( [fld class] != [NSTokenField class] ) return;
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [fld setFrame:rect];
            
        } else {        // create
            
            // rect
            if ( NSIsEmptyRect( rect ) ) rect = NSMakeRect( 20, 20, 96, 22 );
            
            // init
            fld = [[NSTokenField alloc] initWithFrame:rect];
            
            // default autoresizing
            [fld setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [fld setTag: inTag];
            
            // enabled
            if ( inEnabled != kFBParamMissing ) [fld setEnabled:inEnabled];
            
            // text
            if ( text ) [fld setStringValue:text];
            
            // style
            if ( inStyle != kFBParamMissing ) [fld setTokenStyle:inStyle];
            
            // wndTag
            [fld setWndTag:inWndTag];
            
            // fix font to match I.B.
            CocoaControlFixFont( fld );
            
            // delegate
            [fld setDelegate:CocoaTokenFieldDelegateGet()];
            
            // add to window
            [contentView addSubview:fld];
            [fld release];
        }
        
        [fld setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}




#pragma mark - toolbar
///////////////////////////////////////////////////
//               cocoa toolbar                   //
///////////////////////////////////////////////////

@implementation CocoaToolbarItemTarget
- (void)toolbarItemAction:(id)sender {
    CallUserDefinedOnDialogFunction( toolbarItemClick, [sender tag], CocoaOutputWindowTag() );
//    CocoaControlCallUserDefinedOnDialogOrCallback( toolbarItemClick, sender );
}
@end

// private
CocoaToolbarItemTarget *CocoaToolbarItemTargetGet()
{
    static CocoaToolbarItemTarget *sCocoaToolbarItemTarget = nil;
    if ( !sCocoaToolbarItemTarget ) sCocoaToolbarItemTarget = [[CocoaToolbarItemTarget alloc] init];
    return sCocoaToolbarItemTarget;
}


@implementation CocoaToolbarDelegate

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar {
    SInt32 tag = [[toolbar identifier] integerValue];
    CocoaToolbarObj *obj = CocoaToolbarObjWithTag( tag );
    if ( obj ) {
        return [obj allowedIdentifiers];
    }
    return nil;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar {
    SInt32 tag = [[toolbar identifier] integerValue];
    CocoaToolbarObj *obj = CocoaToolbarObjWithTag( tag );
    if ( obj ) {
        return [obj defaultIdentifiers];
    }
    return nil;
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag {
    NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier];
    SInt32 tag = [[toolbar identifier] integerValue];
    CocoaToolbarObj *obj = CocoaToolbarObjWithTag( tag );
    if ( obj ) {
        NSDictionary *itemDict = [[obj items] objectForKey:itemIdentifier];
        if ( itemDict ) {
            
            SInt32 itemTag = [[itemDict objectForKey:@"Tag"] integerValue];
            NSString *label = [itemDict objectForKey:@"Label"];
            NSString *imageName = [itemDict objectForKey:@"ImageName"];
            
            [item setTag:itemTag];
            if ( label ) [item setLabel:label];
            if ( imageName ) [item setImage:[NSImage imageNamed:imageName]];
            
            // target
            [item setTarget:CocoaToolbarItemTargetGet()];
            
            // action
            [item setAction:@selector(toolbarItemAction:)];
        }
    }
    return item;
}

@end

CocoaToolbarDelegate *CocoaToolbarDelegateGet()
{
    static CocoaToolbarDelegate *sCocoaToolbarDelegate = nil;
    if ( !sCocoaToolbarDelegate ) sCocoaToolbarDelegate = [[CocoaToolbarDelegate alloc] init];
    return sCocoaToolbarDelegate;
}


static NSMutableArray *sCocoaToolbars;

@implementation CocoaToolbarObj

@synthesize toolbar;
@synthesize items;
@synthesize allowedIdentifiers;
@synthesize defaultIdentifiers;

- (id)initWithToolbar:(NSToolbar *)tb {
    if ( self = [super init] ) {
        self.toolbar = tb;
        self.items = [[NSMutableDictionary alloc] initWithCapacity:0];
        self.allowedIdentifiers = [[NSMutableArray alloc] initWithCapacity:0];
        self.defaultIdentifiers = [[NSMutableArray alloc] initWithCapacity:0];
    }
    return self;
}

@end

CocoaToolbarObj *CocoaToolbarObjWithTag( SInt32 tag )
{
    if ( sCocoaToolbars ) {
        NSString *identifier = [NSString stringWithFormat:@"%ld",(long)tag];
        for ( CocoaToolbarObj *obj in sCocoaToolbars ) {
            NSToolbar *toolbar = [obj toolbar];
            if ( [[toolbar identifier] isEqualToString:identifier] ) return obj;
        }
    }
    return nil;
}

void CocoaToolbarObjAdd( NSToolbar *tb )
{
    if ( !sCocoaToolbars ) sCocoaToolbars = [[NSMutableArray alloc] initWithCapacity:0];
    CocoaToolbarObj *obj = [[CocoaToolbarObj alloc] initWithToolbar:tb];
    [sCocoaToolbars addObject:obj];
    //[obj release];
}

void CocoaToolbarObjAddItem( CocoaToolbarObj *obj, NSString *identifier, SInt32 tag, NSString *label, NSString *imageName, Boolean allowed, Boolean dflt )
{
    NSDictionary *itemDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInteger:tag], @"Tag", label, @"Label", imageName, @"ImageName", nil];
    [[obj items] setObject:itemDict forKey:identifier];
    if ( allowed ) [[obj allowedIdentifiers] addObject:identifier];
    if ( dflt ) [[obj defaultIdentifiers] addObject:identifier];
}

NSToolbar *CocoaToolbarWithTag( SInt32 tag )
{
    CocoaToolbarObj *obj = CocoaToolbarObjWithTag( tag );
    if ( obj ) return [obj toolbar];
    return nil;
}

SInt32 CocoaToolbarLastTag()
{
    if ( sCocoaToolbars ) {
        CocoaToolbarObj *obj = [sCocoaToolbars lastObject];
        if ( obj ) {
            return [[[obj toolbar] identifier] integerValue];
        }
    }
    return 0;
}



//void ToolbarInsertItem( SInt32 toolbarTag, SInt32 itemTag, CFStringRef inIdentifier, CFStringRef inLabel, CFStringRef inImageName, Boolean allowed, Boolean dflt )
//{
//    CocoaToolbarObj *obj = CocoaToolbarObjWithTag( toolbarTag );
//    if ( obj ) {
//        NSToolbar *toolbar = [obj toolbar];
//        if ( toolbar ) {
//            if ( inIdentifier ) {
//                NSString *identifier = (NSString *)inIdentifier;
//                NSString *label = (NSString *)inLabel;
//                NSString *imageName = (NSString *)inImageName;
//                
//                [toolbar insertItemWithItemIdentifier:identifier atIndex:0];
//                
//                CocoaToolbarObjAddItem( obj, identifier, itemTag, label, imageName, allowed, dflt );
//            }
//        }
//    }
//}


void ToolbarSetSizeMode( SInt32 tag, NSToolbarSizeMode size )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( tag );
    if ( toolbar ) {
        [toolbar setSizeMode:size];
    }
}


void ToolbarSetDisplayMode( SInt32 tag, NSToolbarDisplayMode mode )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( tag );
    if ( toolbar ) [toolbar setDisplayMode:mode];
}

void ToolbarSetAllowsUserCustomization( SInt32 tag, Boolean flag )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( tag );
    if ( toolbar ) [toolbar setAllowsUserCustomization:flag];
}

void ToolbarSetShowsBaselineSeparator( SInt32 tag, Boolean flag )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( tag );
    if ( toolbar ) [toolbar setShowsBaselineSeparator:flag];
}

void ToolbarSetAutosavesConfiguration( SInt32 tag, Boolean flag )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( tag );
    if ( toolbar ) [toolbar setAutosavesConfiguration:flag];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void ToolbarSetAllowsExtensionItems( SInt32 tag, Boolean flag )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( tag );
    if ( toolbar ) [toolbar setAllowsExtensionItems:flag];
}
#endif


void CocoaToolbar( SInt32 inTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSString *identifier = [NSString stringWithFormat:@"%ld",(long)inTag];
    
    NSToolbar *toolbar = CocoaToolbarWithTag( inTag );
    
    if ( toolbar ) {    // exists
        
        // ... don't think there's anything we can do here
        
    } else {        // create
        
        // init
        toolbar = [[NSToolbar alloc] initWithIdentifier:identifier];
                
        // delegate
        [toolbar setDelegate:CocoaToolbarDelegateGet()];
        
        // add to toolbars array
        CocoaToolbarObjAdd( toolbar );
    }
    
    [toolbar setVisible:show];
    
    // workaround for cosmetic in Carbon
    if ( !IsCocoaApplication() ) {
        CocoaWindowCarbonRuntimeCosmeticFix( CocoaOutputWindow() );
    }
}


#pragma mark - toolbaritem
///////////////////////////////////////////////////
//               cocoa toolbaritem               //
///////////////////////////////////////////////////

void CocoaToolbarItem( SInt32 inTag, NSString *inIdentifier, CFStringRef inLabel, CFStringRef inImageName, SInt32 inAllowed, SInt32 inDefault, SInt32 inToolbarTag )
{
    // get target toolbar
    if ( inToolbarTag == 0 ) inToolbarTag = CocoaToolbarLastTag();
    
    if ( inToolbarTag ) {
        CocoaToolbarObj *obj = CocoaToolbarObjWithTag( inToolbarTag );
        if ( obj ) {
            NSToolbar *toolbar = [obj toolbar];
            
            NSString *identifier = (NSString *)inIdentifier;
            NSString *label = (NSString *)inLabel;
            NSString *imageName = (NSString *)inImageName;
            
            BOOL allowed = YES;
            BOOL dflt = YES;
            if ( inAllowed != kFBParamMissing ) allowed = inAllowed;
            if ( inDefault != kFBParamMissing ) dflt = inDefault;
            
            // insert item
            [toolbar insertItemWithItemIdentifier:identifier atIndex:0];
            
            CocoaToolbarObjAddItem( obj, identifier, inTag, label, imageName, allowed, dflt );
        }
    }
}


void ToolbarItemSetMinSize( SInt32 toolbarTag, SInt32 itemTag, CGSize size )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( toolbarTag );
    if ( toolbar ) {
        NSArray *items = [toolbar items];
        for ( NSToolbarItem *item in items ) {
            if ( [item tag] == itemTag ) {
                [item setMinSize:NSSizeFromCGSize( size )];
                break;
            }
        }
    }
}

void ToolbarItemSetMaxSize( SInt32 toolbarTag, SInt32 itemTag, CGSize size )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( toolbarTag );
    if ( toolbar ) {
        NSArray *items = [toolbar items];
        for ( NSToolbarItem *item in items ) {
            if ( [item tag] == itemTag ) {
                [item setMaxSize:NSSizeFromCGSize( size )];
                break;
            }
        }
    }
}

void ToolbarItemSetToolTip( SInt32 toolbarTag, SInt32 itemTag, CFStringRef toolTip )
{
    NSToolbar *toolbar = CocoaToolbarWithTag( toolbarTag );
    if ( toolbar ) {
        NSArray *items = [toolbar items];
        for ( NSToolbarItem *item in items ) {
            if ( [item tag] == itemTag ) {
                [item setToolTip:(NSString *)toolTip];
                break;
            }
        }
    }
}


#pragma mark - view
///////////////////////////////////////////////////
//                  cocoa view                   //
///////////////////////////////////////////////////

@implementation CocoaViewSubclass

@synthesize flip;
@synthesize responderFlag;

//- (void)setTrackingAreaRect:(NSRect)rect options:(NSTrackingAreaOptions)options {
//    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:rect options:options owner:self userInfo:nil];
//    [self addTrackingArea:trackingArea];
//    [trackingArea release];
//}

- (void)viewWillMoveToWindow:(NSWindow *)newWindow {
    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways;
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:options owner:self userInfo:nil];
    [self addTrackingArea:trackingArea];
    //[trackingArea release];
}

- (void)updateTrackingAreas {
    NSArray *areas = [self trackingAreas];
    if ( [areas count] > 0 ) {
        NSTrackingArea *trackingArea = [areas objectAtIndex:0];
        [self removeTrackingArea:trackingArea];
        [trackingArea release];
        NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways;
        trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:options owner:self userInfo:nil];
        [self addTrackingArea:trackingArea];
        //[trackingArea release];
    }
    [super updateTrackingAreas];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    NSWindow *window = [self window];
    DialogEventSetNSRect( dirtyRect );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewDrawRect, CocoaViewTag(self), [window tag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( viewDrawRect, self );
}

- (void)mouseDown:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewMouseDown, CocoaViewTag(self), [window tag] );
//    CocoaControlCallUserDefinedOnDialogOrCallback( viewMouseDown, self );
    CocoaEventSetEvent( nil );
}

- (void)mouseUp:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewMouseUp, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)mouseEntered:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewMouseEntered, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)mouseExited:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewMouseExited, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)mouseDragged:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewMouseDragged, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)mouseMoved:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewMouseMoved, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)rightMouseDown:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewRightMouseDown, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)rightMouseUp:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewRightMouseUp, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

- (void)rightMouseDragged:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    CallUserDefinedOnDialogFunction( viewRightMouseDragged, CocoaViewTag(self), [window tag] );
    CocoaEventSetEvent( nil );
}

// this is required for key events
- (BOOL)acceptsFirstResponder {
    return responderFlag;
}

- (void)keyDown:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    DialogEventSetBool(NO);
    CallUserDefinedOnDialogFunction( viewKeyDown, CocoaViewTag(self), [window tag] );
    if ( !DialogEventBool() ) [super keyDown:event];
    CocoaEventSetEvent( nil );
}

- (void)keyUp:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    DialogEventSetBool(NO);
    CallUserDefinedOnDialogFunction( viewKeyUp, CocoaViewTag(self), [window tag] );
    if ( !DialogEventBool() ) [super keyUp:event];
    CocoaEventSetEvent( nil );
}

- (void)flagsChanged:(NSEvent *)event {
    NSWindow *window = [self window];
    CocoaEventSetEvent( event );
    CocoaOutputWindowSetTag( [window tag] );
    DialogEventSetBool(NO);
    CallUserDefinedOnDialogFunction( viewFlagsChanged, CocoaViewTag(self), [window tag] );
    if ( !DialogEventBool() ) [super flagsChanged:event];
    CocoaEventSetEvent( nil );
}

- (BOOL)isFlipped {
    return flip;
}
@end


id ViewSubviewWithIdentifier( id superview, CFStringRef identifier )
{
    NSArray *subviews = [superview subviews];
    for (id obj in subviews ) {
        if ( class_respondsToSelector( [obj class], NSSelectorFromString( @"identifier" ) ) ) {
            if ( [obj identifier] != nil ) {
                if ( [(NSString *)[obj identifier] compare:(NSString *)identifier] == NSOrderedSame ) {
                    return obj;
                }
            }
            obj = ViewSubviewWithIdentifier( obj, identifier );
            if ( obj != nil ) return obj;
        }
    }
    return nil;
}


SInt32 CocoaViewTag( id view )
{
    NSInteger tag = 0;
    if ( [view class] == [NSView class] || [view class] == [CocoaViewSubclass class] ) {
        tag = [[view identifier] integerValue];
    } else {
        tag = [view tag];
    }
    return tag;
}

id CocoaViewSubviewWithTag( NSView *superview, SInt32 tag )
{
    id obj = nil;
    
    if ( [superview isKindOfClass:[NSTabView class]] ) {
        NSArray *items = [(NSTabView *)superview tabViewItems];
        for ( NSTabViewItem *item in items ) {
            NSView *itemView = [item view];
            obj = CocoaViewSubviewWithTag( itemView, tag );
            if ( obj ) return obj;
        }
    } else if ( [superview class] == [NSSegmentedControl class] ) { // don't descend into segmented control because segment views are given tag values by macOS < 10.13
        return nil;
    } else {
        NSArray *subviews = [superview subviews];
        for ( obj in subviews ) {
            
            NSInteger viewTag = 0;
            if ( [obj class] == [NSView class] || [obj class] == [CocoaViewSubclass class] ) {
                viewTag = [[obj identifier] integerValue];
            } else {
                viewTag = [obj tag];
            }
            
            if ( viewTag == tag ) return obj;
            obj = CocoaViewSubviewWithTag( obj, tag );
            if ( obj ) return obj;
            
        }
    }
    
    return nil;
}

NSView *CocoaViewWithTag( SInt32 tag )
{
    NSView *view = nil;
    NSView *contentView = CocoaOutputWindowContentView();
    if ( !contentView ) {
        NSWindow *w = CocoaFrontWindow();
        contentView = [w contentView];
    }
    if ( contentView ) {
        if ( tag == windowContentViewTag ) return contentView;
        view = CocoaViewSubviewWithTag( contentView, tag );
    }
    return view;
}


SInt32 ViewSuperview( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        NSView *superview = [view superview];
        if ( superview ) return [superview tag];
    }
    return 0;
}

SInt32 ViewWindow( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        NSWindow *w = [view window];
        if ( w ) return [w tag];
    }
    return 0;
}

void ViewSetFrameOrigin( SInt32 tag, CGPoint origin )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setFrameOrigin:NSPointFromCGPoint( origin )];
    }
}

void ViewSetFrameSize( SInt32 tag, CGSize size )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setFrameSize:NSSizeFromCGSize( size )];
    }
}

void ViewSetFrameRotation( SInt32 tag, CGFloat rotation )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setFrameRotation:rotation];
    }
}

CGRect ViewBounds( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        return NSRectToCGRect( [view bounds] );
    }
    return CGRectZero;
}

void ViewSetBounds( SInt32 tag, CGRect bounds )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setBounds:NSRectFromCGRect( bounds )];
    }
}

void ViewSetBoundsOrigin( SInt32 tag, CGPoint origin )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setBoundsOrigin:NSPointFromCGPoint( origin )];
    }
}

void ViewSetBoundsSize( SInt32 tag, CGSize size )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setBoundsSize:NSSizeFromCGSize( size )];
    }
}

void ViewSetBoundsRotation( SInt32 tag, CGFloat rotation )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setBoundsRotation:rotation];
    }
}

CGFloat ViewFrameRotation( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) return [view frameRotation];
    return 0.0;
}

CGFloat ViewBoundsRotation( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) return [view boundsRotation];
    return 0.0;
}

void ViewSetWantsLayer( SInt32 tag, Boolean flag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [view setWantsLayer:flag];
}

CGFloat ViewAlphaValue( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        return [view alphaValue];
    }
    return 0;
}

void ViewSetAlphaValue( SInt32 tag, CGFloat value )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setAlphaValue:value];
    }
}

CGFloat ViewFrameCenterRotation( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        return [view frameCenterRotation];
    }
    return 0;
}

void ViewSetFrameCenterRotation( SInt32 tag, CGFloat rotation )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setFrameCenterRotation:rotation];
    }
}

void ViewPrint( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view print:nil];
    }
}

void ViewSetNeedsDisplay( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view setNeedsDisplay:YES];
    }
}

void ViewRotateByAngle( SInt32 tag, CGFloat angle )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        [view rotateByAngle:angle];
    }
}


void ViewAddSubview( SInt32 viewTag, SInt32 subviewTag )
{
    NSView *view = CocoaViewWithTag( viewTag );
    if ( view ) {
        NSView *subview = CocoaViewWithTag( subviewTag );
        if ( subview ) [view addSubview:subview];
    }
}

void ViewAddSubviewPositioned( SInt32 superviewTag, SInt32 subviewTag, NSWindowOrderingMode position, SInt32 otherViewTag )
{
    NSView *superview = CocoaViewWithTag( superviewTag );
    if ( superview ) {
        NSView *subview = CocoaViewWithTag( subviewTag );
        if ( subview ) {
            NSView *otherView = nil;
            if ( otherViewTag ) otherView = CocoaViewWithTag( otherViewTag );
            [superview addSubview:subview positioned:position relativeTo:otherView];
        }
    }
}

void ViewRemoveFromSuperview( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [view removeFromSuperview];
}

void ViewSetAutoresizingMask( SInt32 tag, SInt32 mask )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [(NSView *)view setAutoresizingMask:mask];
}

CGRect ViewFrame( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) return NSRectToCGRect([view frame]);
    return NSRectToCGRect( NSZeroRect );
}

void ViewSetFrame( SInt32 tag, CGRect r )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [view setFrame:NSRectFromCGRect(r)];
}

void ViewSetToolTip( SInt32 tag, CFStringRef string )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [(NSView *)view setToolTip:(NSString *)string];
}

Boolean ViewIsHidden( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) return [view isHidden];
    return NO;
}

Boolean ViewIsHiddenOrHasHiddenAncestor( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) return [view isHiddenOrHasHiddenAncestor];
    return NO;
}

SInt32 ViewNextKeyView( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        NSView *nextView = [view nextKeyView];
        if ( nextView ) {
            return CocoaViewTag( nextView );
        }
    }
    return 0;
}

SInt32 ViewNextValidKeyView( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        NSView *nextView = [view nextValidKeyView];
        if ( nextView ) {
            return CocoaViewTag( nextView );
        }
    }
    return 0;
}

SInt32 ViewPreviousKeyView( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        NSView *prevView = [view previousKeyView];
        if ( prevView ) {
            return CocoaViewTag( prevView );
        }
    }
    return 0;
}

SInt32 ViewPreviousValidKeyView( SInt32 tag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        NSView *prevView = [view previousValidKeyView];
        if ( prevView ) {
            return CocoaViewTag( prevView );
        }
    }
    return 0;
}

void ViewScrollRect( SInt32 tag, CGRect r, CGSize size )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        [view scrollRect:NSRectFromCGRect(r) by:NSSizeFromCGSize(size)];
    }
}

void ViewSetNeedsDisplayInRect( SInt32 tag, CGRect r )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        [view setNeedsDisplayInRect:NSRectFromCGRect(r)];
    }
}


// custom
void ViewSetAcceptsFirstResponder( SInt32 tag, Boolean flag )
{
    NSView *view = (NSView *)CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [CocoaViewSubclass class] ) {
            [(CocoaViewSubclass *)view setResponderFlag:flag];
        } else if ( [view class] == [CocoaWindowContentView class] ) {
            if ( flag ) [(CocoaWindowContentView *)view setSubclassFlag:flag];
            [(CocoaWindowContentView *)view setResponderFlag:flag];
        }
    }
}


// generic functions
CFStringRef CocoaViewText( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view isKindOfClass:[NSControl class]] ) {
            return (CFStringRef)[(NSControl *)view stringValue];
        }
        
        if ( [view class] == [NSBox class] ) {
            return (CFStringRef)[(NSBox *)view title];
        }
        
        if ( [view class] == [NSButton class] ) {
            return (CFStringRef)[(NSButton *)view title];
        }
        
        if ( [view class] == [NSPopUpButton class] ) {
            return (CFStringRef)[(NSPopUpButton *)view title];
        }
        
        if ( [view isKindOfClass:[NSText class]] ) {
            return (CFStringRef)[(NSText *)view string];
        }
    }
    return NULL;
}

void *ViewAnimator( SInt32 tag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) return [view animator];
    return NULL;
}

void ViewAnimatorSetFrame( SInt32 tag, CGRect r )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setFrame:NSRectFromCGRect(r)];
}

void ViewAnimatorSetFrameOrigin( SInt32 tag, CGPoint origin )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setFrameOrigin:NSPointFromCGPoint(origin)];
}

void ViewAnimatorSetFrameSize( SInt32 tag, CGSize size )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setFrameSize:NSSizeFromCGSize(size)];
}

void ViewAnimatorSetFrameRotation( SInt32 tag, CGFloat rot )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setFrameRotation:rot];
}

void ViewAnimatorSetBounds( SInt32 tag, CGRect r )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setBounds:NSRectFromCGRect(r)];
}

void ViewAnimatorSetBoundsOrigin( SInt32 tag, CGPoint origin )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setBoundsOrigin:NSPointFromCGPoint(origin)];
}

void ViewAnimatorSetBoundsSize( SInt32 tag, CGSize size )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setBoundsSize:NSSizeFromCGSize(size)];
}

void ViewAnimatorSetBoundsRotation( SInt32 tag, CGFloat rot )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setBoundsRotation:rot];
}

void ViewAnimatorSetFrameCenterRotation( SInt32 tag, CGFloat rot )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setFrameCenterRotation:rot];
}

void ViewAnimatorSetAlphaValue( SInt32 tag, CGFloat value )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) [[view animator] setAlphaValue:value];
}


//void ViewSetTrackingArea( SInt32 tag, CGRect r, NSTrackingAreaOptions options )
//{
//    NSView *view = CocoaViewWithTag( tag );
//    if ( view ) {
//        if ( [view class] == [CocoaViewSubclass class] ) {
//            [(CocoaViewSubclass *)view setTrackingAreaRect:NSRectFromCGRect(r) options:options];
//        }
//    }
//}

// custom
id ViewWithTag( SInt32 tag )
{ return (id)CocoaViewWithTag( tag ); }

void ViewSetFlipped( SInt32 tag, Boolean flag )
{
    NSView *view = CocoaViewWithTag( tag );
    if ( view ) {
        if ( [view class] == [CocoaViewSubclass class] ) {
            [(CocoaViewSubclass *)view setFlip:flag];
        } else if ( [view class] == [CocoaWindowContentView class] ) {
            [(CocoaWindowContentView *)view setFlip:flag];
        }
    }
}

void ViewsEmbedInView( SInt32 tag, ... )
{
    SInt32 inWndTag = 0;
    id w = CocoaTargetWindow( &inWndTag );
    if ( w ) {
        va_list ap;
        va_start(ap,tag);
        
        CGFloat minX = 10000;
        CGFloat minY = 10000;
        CGFloat maxX = 0;
        CGFloat maxY = 0;
        
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:0];
        while ( tag ) {
            id view = CocoaViewWithTag( tag );
            if ( view ) {
                [array addObject:view];
                NSRect r = [view frame];
                CGFloat x1 = r.origin.x;
                CGFloat y1 = r.origin.y;
                CGFloat x2 = x1 + r.size.width;
                CGFloat y2 = y1 + r.size.height;
                
                if ( x1 < minX ) minX = x1;
                if ( y1 < minY ) minY = y1;
                if ( x2 > maxX ) maxX = x2;
                if ( y2 > maxY ) maxY = y2;
            }
            tag = va_arg( ap, SInt32 );
        }
        
        NSView *superview = [[NSView alloc] initWithFrame:NSMakeRect(minX,minY,minX+(maxX-minX),minY+(maxY-minY))];
        for ( id view in array ) {
            NSRect r = [view frame];
            r.origin.x -= minX;
            r.origin.y -= minY;
            [view setFrame:r];
            [superview addSubview:view];
        }
        [[w contentView] addSubview:superview];
        va_end(ap);
    }
}



void CocoaView( SInt32 inTag, CGRect inRect, SInt32 inSubclass, SInt32 inWndTag )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    NSRect rect = NSRectFromCGRect( inRect );
    
    // target window
    id w = CocoaTargetWindow( &inWndTag );
    
    if ( w ) {
        NSView *contentView = CocoaTargetContentView( w );
        id view = CocoaViewSubviewWithTag( contentView, inTag );
        
        if ( view ) {    // view already exists
            
            // make sure it's same view type
            if ( [view class] != [NSView class] && [view class] != [CocoaViewSubclass class] ) return;
            
            // rect
            if ( !NSIsEmptyRect( rect ) ) [view setFrame:rect];
            
        } else {
            
            // rect
            if ( NSIsEmptyRect( rect ) ) {
                rect = NSMakeRect( 20, 20, 163, 96 );
            }
            
            // subclass
            BOOL subclass = NO;
            if ( inSubclass != kFBParamMissing ) subclass = inSubclass;
            
            // init
            if ( subclass ) {
                view = [[CocoaViewSubclass alloc] initWithFrame:rect];
            } else {
                view = [[NSView alloc] initWithFrame:rect];
            }
            
            // default autoresizing
            [(NSView *)view setAutoresizingMask:NSViewMinYMargin];
            
            // tag
            [view setIdentifier:[NSString stringWithFormat:@"%ld",(long)inTag]];
            
            // wndTag
            [view setWndTag:inWndTag];
            
            // add to window
            [contentView addSubview:view];
            [view release];
        }
        
        [view setHidden:!show];
    } else {
        CocoaShowAlert( NSWarningAlertStyle, @"Window error.", @"Cocoa window does not exist." );
    }
}


#pragma mark - window
///////////////////////////////////////////////////
//                  cocoa window                 //
///////////////////////////////////////////////////

#pragma mark - output window
static SInt32 sCocoaOutputWindowTag;

SInt32 CocoaOutputWindowTag()
{ return sCocoaOutputWindowTag; }

void CocoaOutputWindowSetTag( SInt32 tag )
{ sCocoaOutputWindowTag = tag; }

id CocoaOutputWindow()
{
    id w = nil;
    SInt32 tag = CocoaOutputWindowTag();
    if ( tag ) {
        w = CocoaWindowWithTag( tag );
        
        if ( !w ) {
            w = CocoaPopoverWithTag( tag );
        }
        
    }
    return w;
}

NSView *CocoaOutputWindowContentView()
{
    NSView *contentView = nil;
    
    id w = CocoaOutputWindow();
    if ( w ) {
        
        if ( [w class] == [NSWindow class] || [w class] == [NSPanel class] ) {
            contentView = [w contentView];
        } else if ( [w class] == [NSPopover class] ) {
            contentView = [[w contentViewController] view];
        } else {
            // ... it's not a window, panel or popover
        }
    }
    
    return contentView;
}


SInt32 CocoaParentTagForView( id view )
{
    NSPopover *popover = CocoaPopoverForView( [view superview] );
    if ( popover ) {
        return [popover tag];
    } else {
        return [[view window] tag];
    }
}

id CocoaTargetWithTag( SInt32 tag )
{
    id target = CocoaWindowWithTag( tag );
    if ( !target ) {
        target = CocoaPopoverWithTag( tag );
    }
    return target;
}

id CocoaTargetWindow( SInt32 *inWndTag )
{
    id w = nil;
    if ( *inWndTag ) {
        w = CocoaTargetWithTag( *inWndTag ); // get window or popover
    } else {
        w = CocoaOutputWindow();
        if ( !w ) {
            w = CocoaFrontWindow();
        }
        if ( w ) *inWndTag = [w tag];
    }
    return w;
}

NSView *CocoaTargetContentView( id w )
{
    NSView *contentView = nil;
    
    if ( [w class] == [NSWindow class] || [w class] == [NSPanel class] ) {
        contentView = [w contentView];
    } else if ( [w class] == [NSPopover class] ) {
        contentView = [[w contentViewController] view];
    } else {
        // ... it isn't a window, panel or popover
    }
    
    return contentView;
}



#pragma mark - CocoaWindowContentView
@implementation CocoaWindowContentView

@synthesize flip;
@synthesize subclassFlag;
@synthesize responderFlag;

- (void)viewWillMoveToWindow:(NSWindow *)newWindow {
    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways;
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:options owner:self userInfo:nil];
    [self addTrackingArea:trackingArea];
    //[trackingArea release];
}

- (void)updateTrackingAreas {
    NSArray *areas = [self trackingAreas];
    if ( [areas count] > 0 ) {
        NSTrackingArea *trackingArea = [areas objectAtIndex:0];
        [self removeTrackingArea:trackingArea];
        [trackingArea release];
        NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways;
        trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:options owner:self userInfo:nil];
        [self addTrackingArea:trackingArea];
        //[trackingArea release];
    }
    [super updateTrackingAreas];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    NSWindow *window = [self window];
    
    //
    //#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
    //    CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
    //#else
    //    CGContextRef ctx = [[NSGraphicsContext currentContext] graphicsPort];
    //#endif
    //    DialogEventSetCGContext( ctx );
    //
    
    if ( subclassFlag ) {
        DialogEventSetNSRect( dirtyRect );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewDrawRect, windowContentViewTag, [window tag] );
    }
    
    // fix focus ring when running in Carbon
    if ( !IsCocoaApplication() ) {
        if ( ![window isKeyWindow] ) return;
        id responder = [window firstResponder];
        if ( responder ) {
            if ( [responder respondsToSelector:@selector(superview)] ) {
                id focusView = [[responder superview] superview];// ugh! I don't like this - bw
                if ( focusView ) {
                    if ( [focusView respondsToSelector:@selector(focusRingType)] ) {
                        if ( [focusView focusRingType] != NSFocusRingTypeNone ) {
                            NSRect rect = [focusView frame];
                            
                            // fix focus ring rect for embedded views
                            NSPoint pt = [[focusView superview] convertPoint:[focusView frame].origin toView:nil];
                            rect.origin = pt;
                            
                            if ( [focusView class] == [NSTextField class] || [focusView class] == [NSTokenField class] || [focusView class] == [NSSecureTextField class] ) {
                                NSSetFocusRingStyle(NSFocusRingOnly);
                                NSRectFill(rect);
                            } else {
                                NSBezierPath *path = nil;
                                if ( [focusView class] == [NSSearchField class] ) {
                                    rect = NSMakeRect( rect.origin.x-1,rect.origin.y-1,rect.size.width+2,rect.size.height+2);
                                    path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:5.0 yRadius:5.0];
                                    [path setLineWidth:3];
                                } else if ( [focusView class] == [NSComboBox class] ) {
                                    rect = NSMakeRect( rect.origin.x-0.5,rect.origin.y+3,rect.size.width-2,rect.size.height-4);
                                    path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:1.5 yRadius:1.5];
                                    [path setLineWidth:4];
                                } else {
                                    // more text type views... (maybe)
                                }
                                if ( path ) {
                                    NSColor *c = [NSColor colorWithCalibratedRed:(131.0/256) green:(180.0/256) blue:(240.0/256) alpha:1.0];
                                    [c set];
                                    [path stroke];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
    
    NSWindow *window = [self window];
    if ( [window class] == [NSWindow class] ) {
        // close any transient or semitransient popovers when running in Carbon
        if ( !IsCocoaApplication() ) {
            if ( sCocoaPopovers ) {
                for ( NSDictionary *poDict in sCocoaPopovers ) {
                    NSArray *popovers = [poDict allValues];
                    NSPopover *popover = [popovers objectAtIndex:0];
                    if ( [popover behavior] != NSPopoverBehaviorApplicationDefined ) {
                        if ( [popover isShown] ) [popover close];
                    }
                }
            }
        }
    }
    
    if ( subclassFlag ) {
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewMouseDown, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    }
}

- (void)mouseUp:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewMouseUp, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super mouseUp:event];
    }
}

- (void)mouseEntered:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewMouseEntered, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super mouseEntered:event];
    }
}

- (void)mouseExited:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewMouseExited, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super mouseExited:event];
    }
}

- (void)mouseDragged:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewMouseDragged, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super mouseDragged:event];
    }
}

- (void)mouseMoved:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewMouseMoved, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super mouseMoved:event];
    }
}

- (void)rightMouseDown:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewRightMouseDown, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super rightMouseDown:event];
    }
}

- (void)rightMouseUp:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewRightMouseUp, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super rightMouseUp:event];
    }
}

- (void)rightMouseDragged:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        CallUserDefinedOnDialogFunction( viewRightMouseDragged, windowContentViewTag, [window tag] );
        CocoaEventSetEvent( nil );
    } else {
        [super rightMouseDragged:event];
    }
}


// this is required for key events
- (BOOL)acceptsFirstResponder {
    return responderFlag;
}

- (void)keyDown:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        DialogEventSetBool(NO);
        CallUserDefinedOnDialogFunction( viewKeyDown, windowContentViewTag, [window tag] );
        if ( !DialogEventBool() ) [super keyDown:event];
        CocoaEventSetEvent( nil );
    } else {
        [super keyDown:event];
    }
}

- (void)keyUp:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        DialogEventSetBool(NO);
        CallUserDefinedOnDialogFunction( viewKeyUp, windowContentViewTag, [window tag] );
        if ( !DialogEventBool() ) [super keyUp:event];
        CocoaEventSetEvent( nil );
    } else {
        [super keyUp:event];
    }
}

- (void)flagsChanged:(NSEvent *)event {
    if ( subclassFlag ) {
        NSWindow *window = [self window];
        CocoaEventSetEvent( event );
        CocoaOutputWindowSetTag( [window tag] );
        DialogEventSetBool(NO);
        CallUserDefinedOnDialogFunction( viewFlagsChanged, windowContentViewTag, [window tag] );
        if ( !DialogEventBool() ) [super flagsChanged:event];
        CocoaEventSetEvent( nil );
    } else {
        [super flagsChanged:event];
    }
}

- (BOOL)isFlipped {
    return flip;
}

@end


//@implementation CocoaWindowPrintView
//
//@synthesize string;
//
//- (id)initWithFrame:(NSRect)frame {
//    if ( self = [super initWithFrame:frame] ) {
//        string = [[NSMutableString alloc] initWithCapacity:0];
//    }
//    return self;
//}
//
//- (void)printString:(NSString *)s {
//    [string appendFormat:@"%@\n",s];
//    [self setNeedsDisplay:YES];
//}
//
//- (void)printWithFormat:(NSString *)format arguments:(va_list)ap {
//    NSString *s = [[NSString alloc] initWithFormat:format arguments:ap];
//    [self printString:s];
//}
//
//- (void)printClear {
//    [string setString:@""];
//    [self setNeedsDisplay:YES];
//}
//
//- (void)drawRect:(NSRect)dirtyRect {
//    [super drawRect:dirtyRect];
//    if ( string ) [string drawAtPoint:NSMakePoint(0,0) withAttributes:nil];
//}
//

//- (BOOL)isFlipped {
//    return flip;
//}

//@end


//void CocoaWindowAddPrintView( NSWindow *w )
//{
//    NSView *contentView = [w contentView];
//    NSRect printViewRect = [contentView frame];
//    printViewRect.origin.x += 5;
//    printViewRect.size.width -= 10;
//    CocoaWindowPrintView *printView = [[CocoaWindowPrintView alloc] initWithFrame:printViewRect];
//    [printView setIdentifier:@"PrintView"];
//    [printView setAutoresizingMask:NSViewWidthSizable + NSViewHeightSizable];
//    [contentView addSubview:printView positioned:NSWindowBelow relativeTo:[w contentView]];
//}


//void WindowPrint( CFStringRef string )
//{
//    id w = CocoaFrontWindow();
//    if ( w ) {
//        CocoaWindowContentView *contentView = [w contentView];
//        NSView *printView = ViewSubviewWithIdentifier( contentView, (CFStringRef)@"PrintView" );
//        if ( printView ) {
//            if ( [printView class] == [CocoaWindowPrintView class] ) {
//                [(CocoaWindowPrintView *)printView printString:(NSString *)string];
//            }
//        }
//    }
//}
//
//void WindowPrintWithFormat( CFStringRef format, ... )
//{
//    id w = CocoaFrontWindow();
//    if ( w ) {
//        CocoaWindowContentView *contentView = [w contentView];
//        NSView *printView = ViewSubviewWithIdentifier( contentView, (CFStringRef)@"PrintView" );
//        if ( printView ) {
//            if ( [printView class] == [CocoaWindowPrintView class] ) {
//                va_list ap;
//                va_start( ap, format );
//                [(CocoaWindowPrintView *)printView printWithFormat:(NSString *)format arguments:ap];
//                va_end( ap );
//            }
//        }
//    }
//}
//
//void WindowPrintClear()
//{
//    id w = CocoaFrontWindow();
//    if ( w ) {
//        CocoaWindowContentView *contentView = [w contentView];
//        NSView *printView = ViewSubviewWithIdentifier( contentView, (CFStringRef)@"PrintView" );
//        if ( printView ) {
//            if ( [printView class] == [CocoaWindowPrintView class] ) {
//                [(CocoaWindowPrintView *)printView printClear];
//            }
//        }
//    }
//}



@implementation CocoaWindowDelegate

@synthesize boolVal;
@synthesize sizeVal;
@synthesize rectVal;

@synthesize allWindowsCallback;


#pragma mark - window delegate methods

- (void)window:(NSWindow *)w forwardEvent:(SInt32)event {
    if ( [w cocoaWindowCallback] ) {
        CocoaWindowCallbackType callback = [w cocoaWindowCallback];
        (*callback)([w tag],event);
    } else if ( allWindowsCallback ) {
        (*allWindowsCallback)([w tag],event);
    } else {
        CallUserDefinedOnDialogFunction( event, [w tag], [w tag] );
    }
}



- (void)windowWillBeginSheet:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowWillBeginSheet];
//    CallUserDefinedOnDialogFunction( windowWillBeginSheet, [notification.object tag], [notification.object tag] );
}

- (void)windowDidEndSheet:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidEndSheet];
//    CallUserDefinedOnDialogFunction( windowDidEndSheet, [notification.object tag], [notification.object tag] );
}

- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize {
    CocoaOutputWindowSetTag( [sender tag] );
    DialogEventSetNSSize( frameSize );
    [self window:sender forwardEvent:windowWillResize];
//    CallUserDefinedOnDialogFunction( windowWillResize, [sender tag], [sender tag] );
    return DialogEventNSSize();
}

- (void)windowDidResize:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowDidResize];
//    CallUserDefinedOnDialogFunction( windowDidResize, [notification.object tag], [notification.object tag] );
}

- (void)windowWillStartLiveResize:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowWillStartLiveResize];
//    CallUserDefinedOnDialogFunction( windowWillStartLiveResize, [notification.object tag], [notification.object tag] );
}

- (void)windowDidEndLiveResize:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidEndLiveResize];
//    CallUserDefinedOnDialogFunction( windowDidEndLiveResize, [notification.object tag], [notification.object tag] );
}

- (void)windowWillMiniaturize:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowWillMiniaturize];
//    CallUserDefinedOnDialogFunction( windowWillMiniaturize, [notification.object tag], [notification.object tag] );
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowDidMiniaturize];
//    CallUserDefinedOnDialogFunction( windowDidMiniaturize, [notification.object tag], [notification.object tag] );
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidDeminiaturize];
//    CallUserDefinedOnDialogFunction( windowDidDeminiaturize, [notification.object tag], [notification.object tag] );
}

- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame {
    DialogEventSetBool( YES );
    DialogEventSetNSRect( newFrame );
    [self window:window forwardEvent:windowShouldZoom];
//    CallUserDefinedOnDialogFunction( windowShouldZoom, [window tag], [window tag] );
    return DialogEventBool();
}

- (void)windowWillEnterFullScreen:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowWillEnterFullScreen];
//    CallUserDefinedOnDialogFunction( windowWillEnterFullScreen, [notification.object tag], [notification.object tag] );
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidEnterFullScreen];
//    CallUserDefinedOnDialogFunction( windowDidEnterFullScreen, [notification.object tag], [notification.object tag] );
}

- (void)windowWillExitFullScreen:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowWillExitFullScreen];
//    CallUserDefinedOnDialogFunction( windowWillExitFullScreen, [notification.object tag], [notification.object tag] );
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidExitFullScreen];
//    CallUserDefinedOnDialogFunction( windowDidExitFullScreen, [notification.object tag], [notification.object tag] );
}

- (void)windowDidFailToEnterFullScreen:(NSWindow *)window {
    [self window:window forwardEvent:windowDidFailToEnterFullScreen];
//    CallUserDefinedOnDialogFunction( windowDidFailToEnterFullScreen, [window tag], [window tag] );
}

- (void)windowDidFailToExitFullScreen:(NSWindow *)window {
    [self window:window forwardEvent:windowDidFailToExitFullScreen];
//    CallUserDefinedOnDialogFunction( windowDidFailToExitFullScreen, [window tag], [window tag] );
}

- (void)windowWillMove:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowWillMove];
//    CallUserDefinedOnDialogFunction( windowWillMove, [notification.object tag], [notification.object tag] );
}

- (void)windowDidMove:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidMove];
//    CallUserDefinedOnDialogFunction( windowDidMove, [notification.object tag], [notification.object tag] );
}

- (void)windowDidChangeScreen:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidChangeScreen];
//    CallUserDefinedOnDialogFunction( windowDidChangeScreen, [notification.object tag], [notification.object tag] );
}

- (BOOL)windowShouldClose:(NSWindow *)window {
    DialogEventSetBool( YES );
    [self window:window forwardEvent:windowShouldClose];
//    CallUserDefinedOnDialogFunction( windowShouldClose, [window tag], [window tag] );
    return DialogEventBool();
}

- (void)windowWillClose:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowWillClose];
//    CallUserDefinedOnDialogFunction( windowWillClose, [notification.object tag], [notification.object tag] );
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidBecomeKey];
//    CallUserDefinedOnDialogFunction( windowDidBecomeKey,[notification.object tag], [notification.object tag] );
}

- (void)windowDidResignKey:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowDidResignKey];
//    CallUserDefinedOnDialogFunction( windowDidResignKey, [notification.object tag], [notification.object tag] );
}

- (void)windowDidBecomeMain:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidBecomeMain];
//    CallUserDefinedOnDialogFunction( windowDidBecomeMain, [notification.object tag], [notification.object tag] );
}

- (void)windowDidResignMain:(NSNotification *)notification {
    [self window:notification.object forwardEvent:windowDidResignMain];
//    CallUserDefinedOnDialogFunction( windowDidResignMain, [notification.object tag], [notification.object tag] );
}

- (void)windowDidUpdate:(NSNotification *)notification {
    CocoaOutputWindowSetTag( [notification.object tag] );
    [self window:notification.object forwardEvent:windowDidUpdate];
//    CallUserDefinedOnDialogFunction( windowDidUpdate, [notification.object tag], [notification.object tag] );
}

@end

CocoaWindowDelegate *CocoaWindowDelegateGet()
{
    static CocoaWindowDelegate *sCocoaWindowDelegate = nil;
    if ( !sCocoaWindowDelegate ) sCocoaWindowDelegate = [[CocoaWindowDelegate alloc] init];
    return sCocoaWindowDelegate;
}


@implementation NSWindow (Additions)
@dynamic tagNumber;
@dynamic callbackValue;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (CocoaWindowCallbackType)cocoaWindowCallback {
    return [objc_getAssociatedObject(self, @selector(callbackValue)) pointerValue];
}

- (void)setCocoaWindowCallback:(CocoaWindowCallbackType)callback {
    objc_setAssociatedObject(self, @selector(callbackValue), [NSValue valueWithPointer:callback], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end


@implementation NSPanel (Additions)
@dynamic tagNumber;

- (NSInteger)tag {
    return [objc_getAssociatedObject(self, @selector(tagNumber)) integerValue];
}

- (void)setTag:(NSInteger)tag {
    objc_setAssociatedObject(self, @selector(tagNumber), [NSNumber numberWithInteger:tag], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

@end

// private

#if !__LP64__

BOOL CocoaWindowIsFBWindow( SInt32 tag )
{
    WindowRef fbw = FBWindowFindByID( tag );
    if ( fbw ) {
        CocoaShowAlert( NSWarningAlertStyle, @"Window tag error.", @"FB and Cocoa windows cannot have same tag values." );
        return YES;
    }
    return NO;
}

BOOL FBWindowIsCocoaWindow( SInt32 tag )
{
    NSWindow *w = CocoaTargetWithTag( tag );
    if ( w ) {
        CocoaShowAlert( NSWarningAlertStyle, @"Window tag error.", @"Cocoa and FB windows cannot have same tag values." );
        return YES;
    }
    return NO;
}

#endif  //!__LP64__


BOOL CocoaObjectIsPopover( SInt32 tag )
{
    if ( CocoaPopoverWithTag( tag ) ) {
        CocoaShowAlert( NSWarningAlertStyle, @"Window tag error.", @"Windows and Popovers cannot have same tag values." );
        return YES;
    }
    return NO;
}

BOOL CocoaObjectIsWindow( SInt32 tag )
{
    if ( CocoaWindowWithTag( tag ) ) {
        CocoaShowAlert( NSWarningAlertStyle, @"Popover tag error.", @"Popovers and Windows cannot have same tag values." );
        return YES;
    }
    return NO;
}




id CocoaWindowWithTag( NSInteger tag )
{
    id w = nil;
    NSArray *windows = [NSApp windows];
    if ( windows ) {
        for ( id obj in windows ) {
            if ( [obj class] == [NSWindow class] || [obj class] == [NSPanel class] ) {
                if ( [obj tag] == tag ) {
                    w = obj;
                    break;
                }
            }
        }
    }
    return w;
}

NSWindow *CocoaWindowWithIdentifier( NSString *identifier )
{
    NSArray *windows = [NSApp windows];
    for ( NSWindow *w in windows ) {
        if ( [[w identifier] isEqualToString:identifier] ) return w;
    }
    return nil;
}

NSWindow *CocoaFrontWindow()
{
    NSWindow *w = nil;
    NSArray *windows = [NSApp orderedWindows];
    for ( id obj in windows ) {
        if ( [obj class] == [NSWindow class] ) {
            w = obj;
            break;
        }
    }
    return w;
}

BOOL WindowIsCocoa( SInt32 tag )
{
    if ( CocoaWindowWithTag( tag ) ) return YES;
    if ( CocoaPopoverWithTag( tag ) ) return YES;
    return NO;
}

void CocoaWindowSetTitle( SInt32 tag, CFStringRef title )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setTitle:(NSString *)title];
}

void CocoaWindowMakeFirstTextViewResponder( NSWindow *w )
{
    NSArray *views = [[w contentView] subviews];
    for ( NSView *view in views ) {
        if ( [view acceptsFirstResponder] ) {
            Class cls = [view class];
            if ( cls == [NSTextField class] || cls == [NSSearchField class] || cls == [NSComboBox class] || cls == [NSTokenField class]  || cls == [NSSecureTextField class] ) {
                [w makeFirstResponder:view];
                break;
            }
        }
    }
}


/*
 kludge to fix glitches in Carbon
 
 - black marks in top left and top right window border
 - black window background when attaching toolbar
 */
void CocoaWindowCarbonRuntimeCosmeticFix( NSWindow *w )
{
    if ( !IsCocoaApplication() ) {
        NSRect r = [w frame];
        NSRect r2 = r;
        r2.size.width++;
        [w setFrame:r2 display:YES];
        [w setFrame:r display:YES];
    }
}



// public
void WindowSetAlphaValue( SInt32 tag, CGFloat value )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setAlphaValue:value];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void WindowSetBackgroundColor( SInt32 tag, CGColorRef col )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        if ( [[NSColor class] respondsToSelector:@selector(colorWithCGColor:)] ) {
            [w setBackgroundColor:[NSColor colorWithCGColor:col]];  // macOS 10.8
        }
    }
}
#endif

void WindowSetContentBorderThickness( SInt32 tag, CGFloat thickness, CGRectEdge edge )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        switch ( edge ) {
            case CGRectMaxYEdge: {
                UInt32 style = [w styleMask];
                if ( (style & NSTexturedBackgroundWindowMask) == 0 ) break;
            }
            case CGRectMinYEdge:
                [w setContentBorderThickness:thickness forEdge:(NSRectEdge)edge];
                break;
            default:
                break;
        }
    }
}

SInt32 WindowAttachedSheet( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        NSWindow *sheet = [w attachedSheet];
        if ( sheet ) {
            return [sheet tag];
        }
    }
    return 0;
}


void WindowSetAspectRatio( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setAspectRatio:NSSizeFromCGSize( size )];
}

CFStringRef WindowTitle( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return (CFStringRef)[w title];
    return NULL;
}


CFURLRef WindowRepresentedURL( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return (CFURLRef)[w representedURL];
    return NULL;
}

void WindowSetRepresentedURL( SInt32 tag, CFURLRef url )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setRepresentedURL:(NSURL *)url];
}


void WindowCenter( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w center];
}

void WindowClose( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        //        CocoaApplicationDelegate *appDelegate = CocoaApplicationDelegateGet();
        //        if ( appDelegate ) {
        //            NSMutableArray *windows = [appDelegate windows];
        //            [windows removeObject:w];
        //        }
        [w close];
    }
}

CGRect WindowFrame( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return NSRectToCGRect([w frame]);
    return CGRectZero;
}

CGRect WindowContentRect( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return NSRectToCGRect([w contentRectForFrameRect:[w frame]]);
    return CGRectZero;
}

void WindowSetFrameTopLeftPoint( SInt32 tag, CGPoint pt )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setFrameTopLeftPoint:NSPointFromCGPoint( pt )];
}

void WindowSetMinSize( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setMinSize:NSSizeFromCGSize(size)];
}

void WindowSetMaxSize( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setMaxSize:NSSizeFromCGSize(size)];
}

void WindowSetContentAspectRatio( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setContentAspectRatio:NSSizeFromCGSize( size )];
}

void WindowSetContentMinSize( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setContentMinSize:NSSizeFromCGSize(size)];
}

void WindowSetContentMaxSize( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setContentMaxSize:NSSizeFromCGSize(size)];
}

void WindowSetFrameAutosaveName( SInt32 tag, CFStringRef name )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setFrameAutosaveName:(NSString *)name];
}

void WindowSetFrameUsingName( SInt32 tag, CFStringRef name )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setFrameUsingName:(NSString *)name];
}

void WindowBeginSheet( SInt32 tag, SInt32 sheetTag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1080
        NSWindow *sheet = CocoaWindowWithTag( sheetTag );
        if ( sheet ) {
            [w beginSheet:sheet completionHandler:^(NSModalResponse returnCode) {
                if ( returnCode == NSOKButton ) {
                    // ???
                }
            }];
        }
#else
        
        // alternative to beginSheet: here
        
#endif
    }
}

void WindowEndSheet( SInt32 tag, SInt32 sheetTag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1080
        
        NSWindow *sheet = CocoaWindowWithTag( sheetTag );
        if ( sheet ) [w endSheet:sheet];
        
#else
        
        // alternative to endSheet: here
        
#endif
    }
}

void WindowFlush( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w flushWindow];
}


void WindowMakeFirstResponder( SInt32 wndTag, SInt32 viewTag )
{
    NSWindow *w = CocoaWindowWithTag( wndTag );
    if ( w ) {
        id view = CocoaViewSubviewWithTag( [w contentView], viewTag );
        if ( view ) {
            if ( [view acceptsFirstResponder] ) {
                [w makeFirstResponder:view];
            }
        }
    }
}


void WindowSetToolbar( SInt32 wndTag, SInt32 toolbarTag )
{
    NSWindow *w = CocoaWindowWithTag( wndTag );
    if ( w ) {
        NSToolbar *toolbar = CocoaToolbarWithTag( toolbarTag );
        if ( toolbar ) {
            [w setToolbar:toolbar];
            
            // workaround for cosmetic in Carbon
            if ( !IsCocoaApplication() ) {
                CocoaWindowCarbonRuntimeCosmeticFix( w );
            }
            
        }
    }
}

SInt32 WindowToolbar( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        NSToolbar *toolbar = [w toolbar];
        if ( toolbar ) return [[toolbar identifier] integerValue];
    }
    return 0;
}

void WindowSetDocumentEdited( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setDocumentEdited:flag];
}

Boolean WindowDocumentEdited( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w isDocumentEdited];
    return NO;
}

void WindowToggleFullScreen( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w toggleFullScreen:nil];
}

void WindowSetCanHide( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setCanHide:flag];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void WindowSetTitlebarAppearsTransparent( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setTitlebarAppearsTransparent:flag];
}
#endif

void WindowSetAllowsToolTipsWhenApplicationIsInactive( SInt32 tag, Boolean flag)
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setAllowsToolTipsWhenApplicationIsInactive:flag];
}

void WindowSetTitleWithRepresentedFilename( SInt32 tag, CFStringRef filename )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setTitleWithRepresentedFilename:(NSString *)filename];
}

void WindowAddChildWindow( SInt32 parTag, SInt32 childTag, NSWindowOrderingMode ordered )
{
    NSWindow *parW = CocoaWindowWithTag( parTag );
    if ( parW ) {
        NSWindow *childW = CocoaWindowWithTag( childTag );
        if ( childW ) {
            [parW addChildWindow:childW ordered:ordered];
        }
    }
}

void WindowRemoveChildWindow( SInt32 parTag, SInt32 childTag )
{
    NSWindow *parW = CocoaWindowWithTag( parTag );
    if ( parW ) {
        NSWindow *childW = CocoaWindowWithTag( childTag );
        if ( childW ) {
            [parW removeChildWindow:childW];
        }
    }
}

SInt32 WindowParentWindow( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        return [[w parentWindow] tag];
    }
    return 0;
}



// 20171018
void WindowSetHidesOnDeactivate( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setHidesOnDeactivate:flag];
}

void WindowSetHasShadow( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setHasShadow:flag];
}

void WindowSetFrameOrigin( SInt32 tag, CGPoint pt )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setFrameOrigin:NSPointFromCGPoint(pt)];
}

void WindowSetFrame( SInt32 tag, CGRect frame )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setFrame:NSRectFromCGRect(frame) display:YES];
}

void WindowCascadeTopLeftFromPoint( SInt32 tag, CGPoint pt )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w cascadeTopLeftFromPoint:NSPointFromCGPoint(pt)];
}

Boolean WindowIsZoomed( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w isZoomed];
    return NO;
}

void WindowPerformZoom( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w performZoom:nil];
}

void WindowZoom( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w zoom:nil];
}

void WindowSetShowsResizeIndicator( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w setShowsResizeIndicator:flag];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
void WindowSetMaxFullScreenContentSize( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w setMaxFullScreenContentSize:NSSizeFromCGSize(size)];
}

void WindowSetMinFullScreenContentSize( SInt32 tag, CGSize size )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w setMinFullScreenContentSize:NSSizeFromCGSize(size)];
}
#endif

void WindowOrderBack( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w orderBack:nil];
}

Boolean WindowIsVisible( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) return [w isVisible];
    return NO;
}

void WindowRemoveFrameUsingName( CFStringRef name )
{ [NSWindow removeFrameUsingName:(NSString *)name]; }

void WindowSetExcludedFromWindowsMenu( SInt32 tag, Boolean flag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setExcludedFromWindowsMenu:flag];
}

CGContextRef WindowGraphicsContext( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
        return [[w graphicsContext] CGContext];
#else
        return [[w graphicsContext] graphicsPort];
#endif
    }
    return NULL;
}


// custom

void WindowSetTitleWithRepresentedURL( SInt32 tag, CFURLRef url )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) [w setTitleWithRepresentedFilename:[(NSURL *)url path]];
}

Boolean WindowExists( SInt32 tag )
{ return CocoaTargetWithTag( tag ) != nil; }

id WindowWithTag( SInt32 tag )
{ return (id)CocoaWindowWithTag( tag ); }

void WindowSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        NSColor *col = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
        [w setBackgroundColor:col];
    }
}

void WindowSetDelegateCallback( SInt32 tag, void *callback )
{
    if ( tag == 0 ) {   // all windows
        CocoaWindowDelegate *delegate = CocoaWindowDelegateGet();
        [delegate setAllWindowsCallback:callback];
    } else {            // one window
        NSWindow *w = CocoaWindowWithTag( tag );
        if ( w ) {
            [w setCocoaWindowCallback:callback];
        }
    }
}

void WindowSubclassContentView( SInt32 tag )
{
    NSWindow *w = CocoaWindowWithTag( tag );
    if ( w ) {
        CocoaWindowContentView *view = (CocoaWindowContentView *)[w contentView];
        [(CocoaWindowContentView *)view setSubclassFlag:YES];
    }
}


void CocoaWindow( SInt32 inTag, CFStringRef inTitle, CGRect inRect, SInt32 inStyle )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    if ( CocoaObjectIsPopover( inTag ) ) return;
    
    if ( !IsCocoaApplication() ) {
        NSApplicationLoad();
        
#if !__LP64__
        if ( CocoaWindowIsFBWindow( inTag ) ) return;
#endif  //__LP64__
        
    }
    
    NSString *title = (NSString *)inTitle;
    NSRect rect = NSRectFromCGRect( inRect );
    
    id w = CocoaWindowWithTag( inTag );
    
    if ( w ) {      // exists
        
        // title
        if ( title ) {
            [w setTitle:title];
        }
        
        // rect
        if ( !NSIsEmptyRect( rect ) ) {
            [w setFrame:[w frameRectForContentRect:rect] display:YES];
        }
        
    } else {    // create
        
        BOOL center = NO;
        
        // rect
        if ( NSIsEmptyRect( rect ) ) {
            rect = NSMakeRect( 0, 0, 550, 400 );
        } else {
            center = ( rect.origin.x == 0.0 && rect.origin.y == 0.0 );
        }
        
        if ( (int)inStyle == kFBParamMissing ) {
            inStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
        }
        
        // init
        w = [[NSWindow alloc] initWithContentRect:rect styleMask:inStyle backing:NSBackingStoreBuffered defer:NO];
        
        /* create content view subclass for:
         1. drawing focus rect around text controls (Carbon apps only).
         2. intercepting content view drawRect: method to issue dialog viewDrawRect event.
         */
        
        CocoaWindowContentView *contentView = [[CocoaWindowContentView alloc] initWithFrame:[[w contentView] frame]];
        [w setContentView:contentView];
        
        [contentView setIdentifier:[NSString stringWithFormat:@"%d",windowContentViewTag]];
        
        // print view
        //        CocoaWindowAddPrintView( w );
        
        // key view loop
        [w setAutorecalculatesKeyViewLoop:YES];
        
        // tag
        [w setTag:inTag];
        
        // title
        if ( !title ) title = @"Untitled";
        [w setTitle:title];
        
        // center
        if ( center ) [w center];
        
        // delegate
        [w setDelegate:CocoaWindowDelegateGet()];
        
        // app delegate
        CocoaApplicationDelegate *appDelegate = CocoaApplicationDelegateGet();
        if ( appDelegate ) {
            [[appDelegate windows] addObject:w];
        }
    }
    
    // show
    if ( show ) {
        [w makeKeyAndOrderFront:nil];
        
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101100
        [[w contentView] setNeedsDisplay:YES];  // force drawRect:
#endif
        
    } else {
        [w orderOut:nil];
    }
    
    CocoaOutputWindowSetTag( inTag );
}






#pragma mark - cocoa nibs

static NSMutableArray *sCocoaNibObjects;

@implementation CocoaNibObject
- (id)initWithName:(NSString *)name objects:(NSArray *)tlObjects {
    if ( self = [super init] ) {
        nibName = name;
        topLevelObjects = tlObjects;
    }
    return self;
}

@synthesize nibName;
@synthesize topLevelObjects;
@end

CocoaNibObject *CocoaNibObjectWithName( NSString *name )
{
    if ( sCocoaNibObjects ) {
        for ( CocoaNibObject *nibObj in sCocoaNibObjects ) {
            if ( [[nibObj nibName] isEqualToString:name] ) return nibObj;
        }
    }
    return nil;
}

void CocoaNibObjectAdd( NSString *name, NSArray *objects )
{
    CocoaNibObject *nibObj = CocoaNibObjectWithName( name );
    if ( !nibObj ) {
        nibObj = [[CocoaNibObject alloc] initWithName:name objects:objects];
        if ( !sCocoaNibObjects ) sCocoaNibObjects = [[NSMutableArray alloc] initWithCapacity:0];
        [sCocoaNibObjects addObject:nibObj];
        [nibObj release];
    }
}



#pragma mark - nibmenu
///////////////////////////////////////////////////
//                  cocoa nib menu               //
///////////////////////////////////////////////////

void CocoaNibMenuSetup( NSMenu *menu )
{
    NSMenu *supermenu = [menu supermenu];
    if ( supermenu ) {
        NSInteger index = [supermenu indexOfItemWithSubmenu:menu];
        NSMenuItem *submenuItem = [supermenu itemAtIndex:index];
        NSInteger tag = [submenuItem tag];
        if ( tag > 100 ) {
            [menu setTag:tag];
            CocoaOtherMenuAdd( menu );
        }
    }
    
    NSArray *items = [menu itemArray];
    for ( NSMenuItem *item in items ) {
        if ( ![item action] ) {
            [item setTarget:CocoaMenuItemTargetGet()];
            [item setAction:@selector(menuItemAction:)];
        }
        NSMenu *submenu = [item submenu];
        if ( submenu ) CocoaNibMenuSetup( submenu );
    }
}

void CocoaNibMenu( CFStringRef inNibName )
{
    if ( !IsCocoaApplication() ) {
        CocoaShowAlert( NSWarningAlertStyle, @"Runtime error.", @"NibMenu can only be used with Cocoa runtime." );
        return;
    }
    
    NSString *nibName = (NSString *)inNibName;
    
    CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
    if ( !nibObj ) {
        NSNib *nib = [[NSNib alloc] initWithNibNamed:nibName bundle:nil];
        if ( nib ) {
            NSArray *objects = nil;
            
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
            if ( [nib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)]) {
                [nib instantiateWithOwner:nil topLevelObjects:&objects];
            } else {
                [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
            }
#else
            [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
#endif
            
            CocoaNibObjectAdd( nibName, objects );
            
            CocoaNibMenuSetup( [NSApp mainMenu] );
        }
    }
}




#pragma mark - nibwindow

void CocoaNibViewSetup( SInt32 wndTag, NSView *superview )
{
    NSArray *subviews = [superview subviews];
    for ( id subview in subviews ) {
        
        // wndTag
        [subview setWndTag:wndTag];
        
        // get view tag
        NSInteger tag = 0;
        if ( [subview respondsToSelector:@selector(tag)] ) tag = [subview tag];
        
        if ( tag <= 0 ) {   // try to get tag from identifier
            NSString *identifier = [subview identifier];
            if ( identifier ) {
                if ( [subview respondsToSelector:@selector(setTag:)] ) {
                    tag = [identifier integerValue];
                    [subview setTag:tag];
                }
            }
        }
        
        if ( [subview class] == [NSButton class] ) {
            if ( [subview action] == 0 ) {
                if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
                if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            }
            continue;
        }
        
        if ( [subview class] == [NSColorWell class] ) {
            if ( [subview action] == 0 ) {
                if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
                if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            }
            continue;
        }

        if ( [subview class] == [NSComboBox class] ) {
            if ( [subview delegate] == nil ) [subview setDelegate:CocoaComboBoxDelegateGet()];
            continue;
        }
        
        if ( [subview class] == [NSDatePicker class] ) {
            if ( [subview action] == 0 ) {
                [subview setTarget:CocoaControlTargetObj()];
                [subview setAction:@selector(controlAction:)];
            }
            continue;
        }
        
        if ( [subview class] == [NSLevelIndicator class] ) {
            if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
            if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            continue;
        }
        
        if ( [subview class] == [NSPopUpButton class] ) {
            if ( [subview action] == 0 ) {
                if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
                if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            }
            continue;
        }
        
        if ( [subview class] == [NSSearchField class] ) {
            if ( [subview delegate] == nil ) [subview setDelegate:CocoaSearchFieldDelegateGet()];
            continue;
        }
        
        if ( [subview class] == [NSSecureTextField class] ) {
            if ( [subview delegate] == nil ) [subview setDelegate:CocoaTextFieldDelegateGet()];
            continue;
        }
        
        if ( [subview class] == [NSSegmentedControl class] ) {
            if ( [subview action] == 0 ) {
                if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
                if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            }
            continue;
        }
        
        if ( [subview class] == [NSSlider class] ) {
            if ( [subview action] == 0 ) {
                if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
                if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            }
            continue;
        }
        
        if ( [subview class] == [NSSplitView class] ) {
            if ( [subview delegate] == nil ) [subview setDelegate:CocoaSplitViewDelegateGet()];
            continue;
        }
        
        if ( [subview class] == [NSStepper class] ) {
            if ( [subview action] == 0 ) {
                if ( [subview target] == nil ) [subview setTarget:CocoaControlTargetObj()];
                if ( [subview action] == 0 ) [subview setAction:@selector(controlAction:)];
            }
            continue;
        }
        
        if ( [subview class] == [NSTableView class] ) {
            if ( [subview delegate] == nil || [subview dataSource] == nil ) {
                CocoaTableViewDelegate *delegate = [[CocoaTableViewDelegate alloc] init];
                if ( [subview delegate] == nil ) {
                    [subview setDelegate:delegate];
                    [delegate setTable:subview];
                }
                if ( [subview action] == 0 ) {
                    [subview setTarget:delegate];
                    if ( ![subview action] ) [subview setAction:@selector(singleAction:)];
                    [subview setDoubleAction:@selector(doubleAction:)];
                }
                if ( [subview dataSource] == nil ) [subview setDataSource:delegate];
            }
            continue;
        }
        
        if ( [subview class] == [NSTabView class] ) {
            if ( [subview delegate] == nil ) [subview setDelegate:CocoaTabViewDelegateGet()];
            // special handling for tab view items
            for ( NSTabViewItem *item in [subview tabViewItems] ) {
                CocoaNibViewSetup( wndTag, [item view] );
            }
            goto Done;
        }
        
        if ( [subview class] == [NSTextField class] ) {
            if ( [subview isEditable] ) [subview setDelegate:CocoaTextFieldDelegateGet()];
            continue;
        }
        
        if ( [subview class] == [NSTokenField class] ) {
            if ( [subview delegate] == nil ) [subview setDelegate:CocoaTokenFieldDelegateGet()];
            continue;
        }
        
        if ( [subview class] == [NSView class] ) {
            goto Done;  // container view - check subviews
        }
        
    Done:
        CocoaNibViewSetup( wndTag, subview );
    }
}


id CocoaWindowFromNibObjects( SInt32 tag, NSString *nibName, NSString *wndIdentifier )
{
    id w = nil;
    
    CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
    if ( nibObj ) {
        
        NSArray *objects = [nibObj topLevelObjects];
        
        for ( id obj in objects ) {
            w = nil;
            if ( [obj class] == [NSWindow class] || [obj class] == [NSPanel class] ) {
                
                if ( wndIdentifier ) {
                    if ( class_respondsToSelector( [obj class], NSSelectorFromString( @"identifier" ) ) ) {
                        if ( [obj identifier] != nil ) {
                            if ( [(NSString *)[obj identifier] isEqualToString:(NSString *)wndIdentifier] ) {
                                w = obj;
                            }
                        }
                    }
                } else {
                    w = obj;
                }
                
                if ( w ) {
                    if ( [obj tag] ) return w;
                    
                    /* create content view subclass for:
                     1. drawing focus rect around text controls (Carbon apps only).
                     2. intercepting content view drawRect: method to issue dialog viewDrawRect event.
                     */
                    // copy views to new content view
                    NSView *contentView = [obj contentView];
                    NSArray *subviews = [[contentView subviews] copy];
                    CocoaWindowContentView *newContentView = [[CocoaWindowContentView alloc] initWithFrame:[[obj contentView] frame]];
                    [newContentView setSubviews:subviews];
                    [obj setContentView:newContentView];
                    
                    [newContentView setIdentifier:[NSString stringWithFormat:@"%d",windowContentViewTag]];
                    
                    [subviews release];
                    
                    // tag
                    [obj setTag:tag];
                    
                    // window delegate
                    [obj setDelegate:CocoaWindowDelegateGet()];
                    
                    // app delegate
                    CocoaApplicationDelegate *appDelegate = CocoaApplicationDelegateGet();
                    if ( appDelegate ) {
                        [[appDelegate windows] addObject:obj];
                    }
                    
                    // set subviews action, target, etc.
                    CocoaNibViewSetup( tag, [obj contentView] );
                    
                    // workaround for cosmetic in Carbon (black marks in top left and top right window border)
                    if ( !IsCocoaApplication() ) {
                        CocoaWindowCarbonRuntimeCosmeticFix( obj );
                    }
                    
                    // focus first editable view
                    CocoaWindowMakeFirstTextViewResponder( obj );
                    
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101100
                    [[obj contentView] setNeedsDisplay:YES];  // force drawRect:
#endif
                    
                    break;
                }
                
                
                
                //
                //                if ( class_respondsToSelector( [obj class], NSSelectorFromString( @"identifier" ) ) ) {
                //                    if ( [obj identifier] != nil ) {
                //
                //                        if ( [(NSString *)[obj identifier] isEqualToString:(NSString *)wndIdentifier] ) {
                //
                //                            w = obj;
                //
                //                            if ( [obj tag] ) return w;// window with this identifier has already been initialised
                //
                //                            /* create content view subclass for:
                //                             1. drawing focus rect around text controls (Carbon apps only).
                //                             2. intercepting content view drawRect: method to issue dialog viewDrawRect event.
                //                             */
                //                                // copy views to new content view
                //                                NSView *contentView = [obj contentView];
                //                                NSArray *subviews = [[contentView subviews] copy];
                //                                CocoaWindowContentView *newContentView = [[CocoaWindowContentView alloc] initWithFrame:[[obj contentView] frame]];
                //                                [newContentView setSubviews:subviews];
                //                                [obj setContentView:newContentView];
                //                                [subviews release];
                //
                //                            // tag
                //                            [obj setTag:tag];
                //
                //                            // window delegate
                //                            [obj setDelegate:CocoaWindowDelegateGet()];
                //
                //                            // app delegate
                //                            CocoaApplicationDelegate *appDelegate = CocoaApplicationDelegateGet();
                //                            if ( appDelegate ) {
                //                                [[appDelegate windows] addObject:obj];
                //                            }
                //
                //                            // set subviews action, target, etc.
                //                            CocoaNibViewSetup( tag, [obj contentView] );
                //
                //                            // workaround for cosmetic in Carbon (black marks in top left and top right window border)
                //                            if ( !IsCocoaApplication() ) {
                //                                CocoaWindowCarbonRuntimeCosmeticFix( obj );
                //                            }
                //
                //                            // focus first editable view
                //                            CocoaWindowMakeFirstTextViewResponder( obj );
                //
                //#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101100
                //                            [[obj contentView] setNeedsDisplay:YES];  // force drawRect:
                //#endif
                //
                //                        }
                //                    }
                //                }
            }
        }
        
    }
    
    return w;
}




void CocoaNibWindow( SInt32 inTag, CFStringRef inNibName, CFStringRef inWndIdentifier )
{
    BOOL show = ( inTag > 0 );
    inTag = ABS(inTag);
    
    if ( CocoaObjectIsPopover( inTag ) ) return;
    
    if ( !IsCocoaApplication() ) {
        NSApplicationLoad();
        
#if !__LP64__
        if ( CocoaWindowIsFBWindow( inTag ) ) return;
#endif  // __LP64__
    }
    
    NSWindow *w = CocoaWindowWithTag( inTag );
    if ( !w ) {             // ensure another cocoa window doesn't already exist
        
        NSString *nibName = (NSString *)inNibName;
        NSString *wndIdentifier = (NSString *)inWndIdentifier;
        
        CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
        
        if ( !nibObj ) {
            
            NSNib *nib = [[NSNib alloc] initWithNibNamed:nibName bundle:nil];
            if ( nib ) {
                NSArray *objects = nil;
                
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
                if ( [nib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)]) {
                    [nib instantiateWithOwner:nil topLevelObjects:&objects];
                } else {
                    [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
                }
#else
                [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
#endif
                
                CocoaNibObjectAdd( nibName, objects );
            }
        }
        
        w = CocoaWindowFromNibObjects( inTag, nibName, wndIdentifier );
        
        if ( ![w isVisible] ) {
            show = NO;
        }
        
        // print view
        //        w = CocoaWindowWithTag( inTag );
        //        CocoaWindowAddPrintView( w );
    }
    CocoaOutputWindowSetTag( inTag );
    
    // show
    if ( show ) {
        [w makeKeyAndOrderFront:nil];
        
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101100
        [[w contentView] setNeedsDisplay:YES];  // force drawRect:
#endif
        
    } else {
        [w orderOut:nil];
    }
    
}




void CocoaViewFromNibObjects( SInt32 tag, NSString *nibName, NSString *viewIdentifier, NSPoint origin, NSView *superview )
{
    NSView *view = nil;
    
    CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
    if ( nibObj ) {
        NSArray *objects = [nibObj topLevelObjects];
        
        for ( id obj in objects ) {
            view = nil;
            if ( [obj class] == [NSView class] ) {
                if ( viewIdentifier ) {
                    if ( class_respondsToSelector( [obj class], NSSelectorFromString( @"identifier" ) ) ) {
                        if ( [obj identifier] != nil ) {
                            if ( [(NSString *)[obj identifier] isEqualToString:viewIdentifier] ) {
                                view = obj;
                            }
                        }
                    }
                } else {
                    view = obj;
                }
                
                if ( view ) {
                    
                    // origin
                    if ( origin.x != 0.0 || origin.y != 0.0 ) [view setFrameOrigin:origin];
                    [superview addSubview:view];
                    
                    // tag
                    if ( tag ) {
                        [view setIdentifier:[NSString stringWithFormat:@"%ld",(long)tag]];
                    }
                    
                    // set subviews action, target, etc.
                    CocoaNibViewSetup( [[superview window] tag], view );
                    
                    break;
                }
                
                
                
                //                if ( class_respondsToSelector( [obj class], NSSelectorFromString( @"identifier" ) ) ) {
                //                    if ( [obj identifier] != nil ) {
                //
                //                        if ( [(NSString *)[obj identifier] isEqualToString:viewIdentifier] ) {
                //
                //                            // origin
                //                            if ( origin.x != 0.0 || origin.y != 0.0 ) [obj setFrameOrigin:origin];
                //                            [superview addSubview:obj];
                //
                //                            // tag
                //                            if ( tag ) {
                //                                [obj setIdentifier:[NSString stringWithFormat:@"%ld",(long)tag]];
                //                            }
                //
                //                            // set subviews action, target, etc.
                //                            CocoaNibViewSetup( [[superview window] tag], obj );
                //
                //                            break;
                //                        }
                //                    }
                //                }
                
                
                
            }
        }
        
    }
}



void CocoaNibView( SInt32 inTag, CFStringRef inNibName, CFStringRef inViewIdentifier, CGPoint inOrigin, SInt32 inSuperviewTag )
{
    inTag = ABS(inTag);
    
    NSPoint origin = NSPointFromCGPoint( inOrigin );
    NSView *superview = nil;
    
    SInt32 dummyTag = 0;
    id w = CocoaTargetWindow( &dummyTag );
    if ( w ) {
        superview = CocoaTargetContentView( w );
        if ( inSuperviewTag ) {
            superview = CocoaViewSubviewWithTag( superview, inSuperviewTag );
        }
    }
    
    if ( superview ) {
        
        NSString *nibName = (NSString *)inNibName;
        NSString *viewIdentifier = (NSString *)inViewIdentifier;
        
        CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
        if ( !nibObj ) {
            NSNib *nib = [[NSNib alloc] initWithNibNamed:nibName bundle:nil];
            if ( nib ) {
                NSArray *objects = nil;
                
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
                if ( [nib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)]) {
                    [nib instantiateWithOwner:nil topLevelObjects:&objects];
                } else {
                    [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
                }
#else
                [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
#endif
                
                CocoaNibObjectAdd( nibName, objects );
            }
        }
        
        CocoaViewFromNibObjects( inTag, nibName, viewIdentifier, origin, superview );
    }
}



#pragma mark - nibpopover

///////////////////////////////////////////////////
//                  cocoa nibpopover             //
///////////////////////////////////////////////////

void CocoaPopoverFromNibObjects( SInt32 tag, NSString *nibName )
{
    CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
    if ( nibObj ) {
        NSArray *objects = [nibObj topLevelObjects];
        
        for ( id obj in objects ) {
            if ( [obj class] == [NSPopover class] ) {
                
                // tag
                if ( tag ) [obj setTag:tag];
                
                // add popover
                CocoaPopoverAdd( obj, tag );
                
                // set subviews action, target, etc.
                NSView *view = [[obj contentViewController] view];
                CocoaNibViewSetup( tag, view );
                
                break;
            }
        }
        
    }
}



void CocoaNibPopover( SInt32 inTag, CFStringRef inNibName )
{
    inTag = ABS(inTag);
    
    if ( CocoaObjectIsWindow( inTag ) ) return;
    
#if !__LP64__
    if ( CocoaWindowIsFBWindow( inTag ) ) return;// if an FB Carbon window exists with same id/tag, alert and abort
#endif  //__LP64__
    
    NSPopover *popover = CocoaPopoverWithTag( inTag );
    if ( !popover ) {
        
        NSString *nibName = (NSString *)inNibName;
        
        CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
        if ( !nibObj ) {
            NSNib *nib = [[NSNib alloc] initWithNibNamed:nibName bundle:nil];
            if ( nib ) {
                NSArray *objects = nil;
                
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
                if ( [nib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)]) {
                    [nib instantiateWithOwner:nil topLevelObjects:&objects];
                } else {
                    [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
                }
#else
                [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
#endif
                
                CocoaNibObjectAdd( nibName, objects );
            }
        }
        
        CocoaPopoverFromNibObjects( inTag, nibName );
        
    }
}


#pragma mark - nibtoolbar
///////////////////////////////////////////////////
//                  cocoa nibtoolbar             //
///////////////////////////////////////////////////

void CocoaToolbarFromNibObjects( SInt32 tag, NSString *nibName )
{
    CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
    if ( nibObj ) {
        NSArray *objects = [nibObj topLevelObjects];
        
        for ( id obj in objects ) {
            if ( [obj class] == [NSToolbar class] ) {
                
                // set item action, target, etc.
                NSArray *items = [obj items];
                for ( NSToolbarItem *item in items ) {
                    if ( [item tag] > 0 ) {
                        [item setTarget:CocoaToolbarItemTargetGet()];
                        [item setAction:@selector(toolbarItemAction:)];
                    }
                }
                
                // add to toolbars list
                CocoaToolbarObjAdd( obj );
                
                break;
            }
        }
        
    }
}

void CocoaNibToolbar( SInt32 inTag, CFStringRef inNibName )
{
    inTag = ABS(inTag);
    
    NSToolbar *toolbar = CocoaToolbarWithTag( inTag );
    if ( !toolbar ) {
        
        NSString *nibName = (NSString *)inNibName;
        
        CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
        if ( !nibObj ) {
            NSNib *nib = [[NSNib alloc] initWithNibNamed:nibName bundle:nil];
            if ( nib ) {
                NSArray *objects = nil;
                
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
                if ( [nib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)]) {
                    [nib instantiateWithOwner:nil topLevelObjects:&objects];
                } else {
                    [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
                }
#else
                [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
#endif
                
                CocoaNibObjectAdd( nibName, objects );
            }
        }
        
        CocoaToolbarFromNibObjects( inTag, nibName );
    }
}

#endif // __MAC_OS_X_VERSION_MAX_ALLOWED > 1060



#pragma mark - nib <everything>
void CocoaNib( CFStringRef inNibName )
{
    NSString *nibName = (NSString *)inNibName;
    
    CocoaNibObject *nibObj = CocoaNibObjectWithName( nibName );
    if ( !nibObj ) {
        NSNib *nib = [[NSNib alloc] initWithNibNamed:nibName bundle:nil];
        if ( nib ) {
            NSArray *objects = nil;
            
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
            if ( [nib respondsToSelector:@selector(instantiateWithOwner:topLevelObjects:)]) {
                [nib instantiateWithOwner:nil topLevelObjects:&objects];
            } else {
                [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
            }
#else
            [nib instantiateNibWithOwner:nil topLevelObjects:&objects];
#endif
            
            CocoaNibObjectAdd( nibName, objects );
            
            // menus
            
            // windows
            
            // popovers
            
            // toolbars
            
            // views

        }
    }
}




