/*
 CocoaUI.h
 
 Bernie Wylde 20170712
 
 updated: 20171026
 */

#import <AppKit/AppKit.h>
#include "FBtoC.h"
#include "Runtime.h"
#import <objc/runtime.h>

#ifndef COCOAUI_H
#define COCOAUI_H


void CocoaAppRun();
BOOL IsCocoaApplication();


#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060

// application events
enum {
    appWillFinishLaunching = 1,
    appDidFinishLaunching,
    appShouldTerminate,
    appShouldTerminateAfterLastWindowClosed,
    appWillTerminate,
    appWillBecomeActive,
    appDidBecomeActive,
    appWillResignActive,
    appDidResignActive,
    appWillHide,
    appDidHide,
    appWillUnhide,
    appDidUnhide,
    appWillUpdate,
    appDidUpdate,
    appShouldHandleReopen,
    appDockMenu,
    appWillPresentError,
    appDidChangeScreenParameters,
//    appOpenFile,
    appOpenFileWithoutUI,
    appOpenTempFile,
//    appOpenFiles,
    appOpenUntitledFile,
    appShouldOpenUntitledFile,
    appPrintFile,
    appPrintFiles,
    appDidRegisterForRemoteNotificationsWithDeviceToken,
    appDidFailToRegisterForRemoteNotifications,
    appDidReceiveRemoteNotification,
    appDidDecodeRestorableState,
    appWillEncodeRestorableState,
    appWillContinueUserActivityWithType,
    appContinueUserActivity,
    appDidFailToContinueUserActivityWithType,
    appDidUpdateUserActivity,
    appDidChangeOcclusionState,
    appOpenURLs
};

typedef BOOL (*CocoaTimerCallbackType)(NSTimer*);

@interface CocoaApplicationDelegate : NSObject <NSApplicationDelegate> {
    NSMutableArray *windows;
    BOOL boolVal;
    long longVal;
    NSString *stringVal;
    NSArray *arrayVal;
    NSDictionary *dictVal;
    NSURL *urlVal;
    NSMenu *menuObj;
}
@property (assign) NSMutableArray *windows;
@property (assign) BOOL boolVal;
@property (assign) long longVal;
@property (assign) NSString *stringVal;
@property (assign) NSArray *arrayVal;
@property (assign) NSDictionary *dictVal;
@property (assign) NSURL *urlVal;
@property (assign) NSMenu *menuObj;
@end

// events
void AppEventSetBool( Boolean value );
void AppEventSetLong( long value );
CFStringRef AppEventString();
CFArrayRef AppEventArray();
CFDictionaryRef AppEventDictionary();
CFURLRef AppEventURL();
void AppEventSetMenu( NSMenu *obj );


SInt32 AppKeyWindow();
SInt32 AppMainWindow();
Boolean AppIsHidden();
void AppHide();
void AppUnhide();
void AppUnhideWithoutActivation();
Boolean AppIsActive();
void AppActivateIgnoringOtherApps( Boolean flag );
void AppHideOtherApplications();
void AppUnhideAllApplications();
void AppShowHelp();

// custom
void CocoaAppInit();
void CocoaInit();
SInt32 AppOutputWindow();

void AppSetTimer( NSTimeInterval interval, CocoaTimerCallbackType callback, BOOL repeats );

// event
long FBAppEventFunction( long val );                                          // cocoaui 20170704 bw
void CallUserDefinedOnAppEventFunction( long appEventKind, long appEventID ); // cocoaui 20170704 bw

#if __LP64__

// menu
long FBMenuFunction( long val );
void InstallFBAppCommandProcessHandler( void );// FBtoC generates a call to this function but we don't use it

// dialog
long FBDialogFunction( long val );
void CallUserDefinedOnDialogFunction(long fbEventKind, long fbEventID, long fbWndNum);
void InstallFBAppDialogHandler( void ); // FBtoC generates a call to this function but we don't use it

#endif  // __LP64__


#pragma mark - menu
///////////////////////////////////////////////////
//                  cocoa menu                   //
///////////////////////////////////////////////////

typedef BOOL (*CocoaValidateMenuItemCallbackType)(SInt32,SInt32);

void BuildMinimalMenu();

@interface CocoaMenuItemTarget : NSObject {
    CocoaValidateMenuItemCallbackType validateCallback;
    BOOL boolVal;
}
@property (assign) CocoaValidateMenuItemCallbackType validateCallback;
@property (assign) BOOL boolVal;
@end

void MenuEventSetBool( Boolean value );

void MenuSetValidateItemsCallback( CocoaValidateMenuItemCallbackType callback );



NSInteger CocoaOtherMenuIndex( NSMenu *menu );

CocoaMenuItemTarget *CocoaMenuItemTargetGet();



@interface NSMenu (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;
- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end


CFStringRef MenuTitle( SInt32 index );
CFStringRef MenuItemTitle( SInt32 menuIndex, SInt32 itemIndex );

void MenuSetSubmenu( SInt32 parMenuIndex, SInt32 parItemIndex, SInt32 submenuIndex );

void MenuSetAutoenablesItems( SInt32 menuIndex, Boolean flag );

void MenuBarSetVisible( Boolean flag );
CGFloat MenuBarHeight();
void MenuRemoveItem( SInt32 menuIndex, SInt32 itemIndex );
void MenuRemoveAllItems( SInt32 menuIndex );
SInt32 MenuItemWithTitle( SInt32 menuIndex, CFStringRef title );

SInt32 MenuNumberOfItems( SInt32 menuIndex );

Boolean MenuPopUp( SInt32 menuIndex, SInt32 itemIndex, CGPoint location, SInt32 viewTag );
void MenuSetShowsStateColumn( SInt32 menuIndex, Boolean flag );

void MenuSetFont( SInt32 menuIndex, CTFontRef font );

void MenuItemSetState( SInt32 menuIndex, SInt32 itemIndex, NSCellStateValue state );

// custom
void MenuSetTag( SInt32 menuIndex, SInt32 itemIndex, SInt32 tag );
void MenuItemSetAction( SInt32 menuIndex, SInt32 itemIndex, CFStringRef actionName );


void CocoaMenu( SInt32 inMenuIndex, SInt32 inItemIndex, SInt32 inEnabled, CFStringRef inTitle, CFStringRef inKeyEquivalent, SInt32 inKeyEquivalentModifier );

void CocoaEditMenu( SInt32 inIndex );
void CocoaWindowMenu( SInt32 inIndex );

NSMenu *MenuAtIndex( SInt32 index );

void MenuSetFontWithName( SInt32 menuIndex, CFStringRef inName, CGFloat size );



#pragma mark - cocoa event    // 20180829 bw
///////////////////////////////////////////////////
//                  cocoa event                 //
///////////////////////////////////////////////////

@interface CocoaEvent : NSObject {
    NSEvent *nsEvent;
}

@property (assign) NSEvent* nsEvent;

@end

CGPoint EventLocationInWindow();

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
NSEventModifierFlags EventModifierFlags();
#else
UInt32 EventModifierFlags();
#endif

CFTimeInterval EventTimestamp();
NSEventType eventType();// lowercase 'e' to avoid conflict with Carbon's 'EventType'
void *EventWindow();
CFTimeInterval EventKeyRepeatDelay();
CFTimeInterval EventKeyRepeatInterval();
CFStringRef EventCharacters();
CFStringRef EventCharactersIgnoringModifiers();
Boolean EventIsARepeat();
unsigned short EventKeyCode();
UInt32 EventPressedMouseButtons();
CFTimeInterval EventDoubleClickInterval();
CGPoint EventMouseLocation();
SInt32 EventClickCount();

// custom
CGPoint EventLocationInView( SInt32 tag );



#pragma mark - dialog event    // 20180825 bw
///////////////////////////////////////////////////
//                  dialog event                 //
///////////////////////////////////////////////////

@interface CocoaDialogEvent : NSObject {
    BOOL boolVal;
    long longVal;
    float floatVal;
    double doubleVal;
    NSSize sizeVal;
    NSRect rectVal;
    SInt32 viewTag;
//    CGContextRef cgContext;
    //NSEvent *nsEvent;
}

@property (assign) BOOL boolVal;
@property (assign) long longVal;
@property (assign) float floatVal;
@property (assign) double doubleVal;
@property (assign) NSSize sizeVal;
@property (assign) NSRect rectVal;
@property (assign) SInt32 viewTag;
//@property (assign) CGContextRef cgContext;
//@property (assign) NSEvent* nsEvent;

@end

CocoaDialogEvent *CocoaDialogEventObj();

BOOL DialogEventBool();
void DialogEventSetBool( BOOL value );
long DialogEventLong();
void DialogEventSetLong( long value );
float DialogEventFloat();
void DialogEventSetFloat( float value );
double DialogEventDouble();
void DialogEventSetDouble( double value );
CGSize DialogEventSize();
void DialogEventSetSize( CGSize value );
CGRect DialogEventRect();
void DialogEventSetRect( CGRect value );
SInt32 DialogEventViewTag();
void DialogEventSetViewTag( SInt32 value );
//CGContextRef DialogEventCGContext();
//void DialogEventSetCGContext( CGContextRef ctx );
//NSEvent *DialogEventNSEvent();
//void DialogEventSetNSEvent( NSEvent *ev );


#pragma mark - output window
SInt32 CocoaOutputWindowTag();
void CocoaOutputWindowSetTag( SInt32 tag );
id CocoaOutputWindow();
NSView *CocoaOutputWindowContentView();


#pragma mark - target window/popover
id CocoaTargetWithTag( SInt32 tag );



#pragma mark - alert
///////////////////////////////////////////////////
//                  cocoa alert                  //
///////////////////////////////////////////////////

// dialog events
enum {
    alertDidEnd = 241,
    alertShowHelp
};

@interface CocoaAlertDelegate : NSObject <NSAlertDelegate> {
    BOOL boolVal;
}
@property (assign) BOOL boolVal;
@end

CocoaAlertDelegate *CocoaAlertDelegateGet();

@interface NSAlert (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;
@property (nonatomic,retain) NSNumber *sheetNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
- (BOOL)sheet;
- (void)setSheet:(BOOL)sheet;

@end

void AlertSetAccessoryView( SInt32 alertTag, SInt32 viewTag );
void AlertSetShowsHelp( SInt32 tag, Boolean flag );
CFStringRef AlertHelpAnchor( SInt32 tag );
void AlertSetHelpAnchor( SInt32 tag, CFStringRef anchorName );
void AlertSetShowsSuppressionButton( SInt32 tag, Boolean flag );
void AlertSuppressionButtonSetState( SInt32 tag, NSCellStateValue value );
NSCellStateValue AlertSuppressionButtonState( SInt32 tag );
void AlertSuppressionButtonSetTitle( SInt32 tag, CFStringRef title );
void AlertSetIcon( SInt32 tag, CFStringRef imageName );
void AlertSetIconCGImage( SInt32 tag, CGImageRef cgImage );

// custom
void AlertRemove( SInt32 tag );

NSInteger CocoaAlert( SInt32 inTag, SInt32 inStyle, CFStringRef inMsg, CFStringRef inInfo, CFTypeRef inButtonTitles, SInt32 inSheetFlag );


#pragma mark - control
///////////////////////////////////////////////////
//                  cocoa control                //
///////////////////////////////////////////////////

// dialog events
enum {
    controlTextDidBeginEditing = 151,
    controlTextDidChange,
    controlTextDidEndEditing
};

typedef void (*CocoaControlCallbackType)(SInt32,SInt32,SInt32);

@interface NSControl (Additions)
@property (nonatomic,retain) NSValue *callbackValue;

- (CocoaControlCallbackType)cocoaControlCallback;
- (void)setCocoaControlCallback:(CocoaControlCallbackType)cccb;
@end


@interface CocoaControlTarget : NSObject {
    CocoaControlCallbackType allControlsCallback;
}
@property (assign) CocoaControlCallbackType allControlsCallback;
@end

CocoaControlTarget *CocoaControlTargetObj();


// private
// fix font to match I.B.
void CocoaControlFixFont( id c );


// public
double ControlDoubleValue( SInt32 tag );
void ControlSetDoubleValue( SInt32 tag, double value );

SInt32 ControlIntegerValue( SInt32 tag );
void ControlSetIntegerValue( SInt32 tag, SInt32 value );

CFStringRef ControlStringValue( SInt32 tag );
void ControlSetStringValue( SInt32 tag, CFStringRef string );

void ControlSetSize( SInt32 tag, NSControlSize size );
void ControlSetAlignment( SInt32 tag, NSTextAlignment alignment );
void ControlSizeToFit( SInt32 tag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void ControlSetLineBreakMode( SInt32 tag, NSLineBreakMode mode );
#endif

void ControlSetFont( SInt32 tag, CTFontRef font );
void ControlSetAttributedStringValue( SInt32 tag, CFAttributedStringRef string );

void ControlTakeDoubleValueFrom( SInt32 tag, SInt32 fromViewTag );
void ControlTakeIntegerValueFrom( SInt32 tag, SInt32 fromViewTag );
void ControlTakeStringValueFrom( SInt32 tag, SInt32 fromViewTag );

void ControlSetEnabled( SInt32 tag, Boolean flag );

Boolean ControlIsEnabled( SInt32 tag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
CGSize ControlSizeThatFits( SInt32 tag, CGSize size );
#endif

void ControlSetContinuous( SInt32 tag, Boolean flag );
void ControlPerformClick( SInt32 tag );

// custom
void ControlSetFontWithName( SInt32 tag, CFStringRef inName, CGFloat size );
void ControlSetAction( SInt32 tag, CFStringRef actionName );
void ControlSetActionCallback( SInt32 tag, void *callback );


#pragma mark - box
///////////////////////////////////////////////////
//                  cocoa box                    //
///////////////////////////////////////////////////

@interface NSBox (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

void BoxSetBorderType( SInt32 tag, SInt32 type );
void BoxSetTitlePosition( SInt32 tag, SInt32 position );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void BoxSetBorderColor( SInt32 tag, CGColorRef col );
void BoxSetFillColor( SInt32 tag, CGColorRef col );
#endif

void BoxSetBorderWidth( SInt32 tag, CGFloat width );
void BoxSetCornerRadius( SInt32 tag, CGFloat radius );

void BoxSetTransparent( SInt32 tag, Boolean flag );
void BoxSetTitleFont( SInt32 tag, CTFontRef font );
void BoxSizeToFit( SInt32 tag );

// custom
void BoxSetBorderRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );
void BoxSetFillRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );
void BoxSetTitleFontWithName( SInt32 tag, CFStringRef inName, CGFloat size );

void CocoaBox( SInt32 inTag, CFStringRef inTitle, CGRect inRect, SInt32 inType, SInt32 inWndTag );

#pragma mark - button
///////////////////////////////////////////////////
//                  cocoa button                 //
///////////////////////////////////////////////////

//@interface CocoaButtonTarget : NSObject
//@end
//
//CocoaButtonTarget *CocoaButtonTargetGet();

SInt32 ButtonState( SInt32 tag );
Boolean ButtonIsEnabled( SInt32 tag );
CFStringRef ButtonTitle( SInt32 tag );


void ButtonSetSound( SInt32 tag, CFStringRef sndName );

void ButtonSetKeyEquivalent( SInt32 tag, CFStringRef key );
void ButtonSetKeyEquivalentModifierMask( SInt32 tag, SInt32 mask );

void ButtonSetCGImage( SInt32 tag, CGImageRef cgImage );
void ButtonSetImage( SInt32 tag, CFStringRef imageName );
void ButtonSetImagePosition( SInt32 tag, NSCellImagePosition position );
void ButtonSetImageScaling( SInt32 tag, NSImageScaling scaling );

void ButtonSetBordered( SInt32 tag, Boolean flag );

void ButtonSetShowsBorderOnlyWhileMouseInside( SInt32 tag, Boolean flag );

void ButtonPerformClick( SInt32 tag );

void ButtonSetAttributedTitle( SInt32 tag, CFAttributedStringRef title );


void ButtonSetAlternateTitle( SInt32 tag, CFStringRef title );
void ButtonSetPeriodicDelay( SInt32 tag, float delay, float interval );
void ButtonSetAttributedAlternateTitle( SInt32 tag, CFAttributedStringRef title );
void ButtonSetAlternateImage( SInt32 tag, CFStringRef imageName );
void ButtonSetTransparent( SInt32 tag, Boolean flag );
void ButtonSetAllowsMixedState( SInt32 tag, Boolean flag );
void ButtonSetNextState( SInt32 tag );
void ButtonHighlight( SInt32 tag, Boolean flag );

void ButtonSetHighlightsBy( SInt32 tag, UInt32 mask );

void ButtonSetImageDimsWhenDisabled( SInt32 tag, Boolean flag );



// custom
void ButtonClose( SInt32 tag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void ButtonSetTitleColor( SInt32 tag, CGColorRef col );
void ButtonSetBackgroundColor( SInt32 tag, CGColorRef col );
#endif

void ButtonSetTitleRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );
void ButtonSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );



void CocoaButton( SInt32 inTag, SInt32 inEnabled, SInt32 inState, CFStringRef inTitle, CGRect inRect, SInt32 inType, SInt32 inStyle, SInt32 inWndTag );



#pragma mark - colorpanel
///////////////////////////////////////////////////
//                  cocoa panel                  //
///////////////////////////////////////////////////

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070

void ColorPanelSetColor( CGColorRef col );
CGColorRef ColorPanelColor();
void ColorPanelSetMode( NSColorPanelMode mode );
void ColorPanelSetShowsAlpha( Boolean flag );
CGFloat ColorPanelAlpha();

// custom
void ColorPanelShow();
void ColorPanelClose();
void ColorPanelSetRGB( CGFloat r, CGFloat g, CGFloat b, CGFloat a );

#endif


#pragma mark - colorwell
///////////////////////////////////////////////////
//                  cocoa color well             //
///////////////////////////////////////////////////

//@interface CocoaColorWellTarget : NSObject
//@end
//
//CocoaColorWellTarget *CocoaColorWellTargetGet();

CGColorRef ColorWellColor( SInt32 tag );
void ColorWellTakeColorFrom( SInt32 tag, SInt32 senderTag );

void ColorWellActivate( SInt32 tag, Boolean exclusive );
void ColorWellDeactivate( SInt32 tag );
void ColorWellDrawWellInside( SInt32 tag, CGRect insideRect );

// custom
void ColorWellSetRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );

void CocoaColorWell( SInt32 inTag, SInt32 inEnabled, CGColorRef inColor, CGRect inRect, SInt32 inBordered, SInt32 inWndTag );


#pragma mark - combobox
///////////////////////////////////////////////////
//                  cocoa combobox               //
///////////////////////////////////////////////////

// dialog events
enum {
    comboBoxSelectionDidChange = 201,
    comboBoxSelectionIsChanging,
    comboBoxWillDismiss,
    comboBoxWillPopUp
};

@interface CocoaComboBoxDelegate : NSObject <NSComboBoxDelegate,NSComboBoxDataSource> {
    NSMutableArray *comboBoxData;
}
@property (assign) NSMutableArray *comboBoxData;
@end

void ComboBoxSetHasVerticalScroller( SInt32 tag, Boolean flag );
void ComboBoxSetIntercellSpacing( SInt32 tag, CGSize size );
void ComboBoxSetBordered( SInt32 tag, Boolean flag );
void ComboBoxSetItemHeight( SInt32 tag, CGFloat height );
void ComboBoxSetNumberOfVisibleItems( SInt32 tag, SInt32 value );
void ComboBoxSetCompletes( SInt32 tag, Boolean flag );

void ComboBoxAddItems( SInt32 tag, CFArrayRef items );
void ComboBoxAddItem( SInt32 tag, CFTypeRef item );
void ComboBoxInsertItem( SInt32 tag, CFTypeRef item, SInt32 index );
CFArrayRef ComboBoxValues( SInt32 tag );
void ComboBoxRemoveAllItems( SInt32 tag );
void ComboBoxRemoveItemAtIndex( SInt32 tag, SInt32 index );
void ComboBoxRemoveItemWithValue( SInt32 tag, CFTypeRef value );
SInt32 ComboBoxNumberOfItems( SInt32 tag );
SInt32 ComboBoxIndexOfItemWithValue( SInt32 tag, CFTypeRef value );
CFTypeRef ComboBoxItemValueAtIndex( SInt32 tag, SInt32 index );
void ComboBoxScrollItemAtIndexToTop( SInt32 tag, SInt32 index );
void ComboBoxScrollItemAtIndexToVisible( SInt32 tag, SInt32 index );
SInt32 ComboBoxIndexOfSelectedItem( SInt32 tag );
CFTypeRef ComboBoxValueOfSelectedItem( SInt32 tag );
void ComboBoxSelectItemAtIndex( SInt32 tag, SInt32 index );
void ComboBoxSelectItemWithValue( SInt32 tag, CFTypeRef value );

void ComboBoxSetUsesDataSource( SInt32 tag, Boolean flag );
void ComboBoxNoteNumberOfItemsChanged( SInt32 tag );
void ComboBoxReloadData( SInt32 tag );

// custom
CFMutableArrayRef ComboBoxData( SInt32 tag );
void ComboBoxSetData( SInt32 tag, CFMutableArrayRef array );


void CocoaComboBox( SInt32 inTag, SInt32 inEnabled, CFTypeRef inItems, CFStringRef inText, CGRect inRect, SInt32 inWndTag );



#pragma mark - datepicker
///////////////////////////////////////////////////
//                  cocoa datepicker             //
///////////////////////////////////////////////////

@interface CocoaDatePickerTarget : NSObject
@end

void DatePickerSetMode( SInt32 tag, SInt32 mode );

CFDateRef DatePickerDateValue( SInt32 tag );
void DatePickerSetTimeZone( SInt32 tag, CFTimeZoneRef zone );
void DatePickerSetMinDate( SInt32 tag, CFDateRef dt );
void DatePickerSetMaxDate( SInt32 tag, CFDateRef dt );

void DatePickerSetBezeled( SInt32 tag, Boolean flag );
void DatePickerSetBordered( SInt32 tag, Boolean flag );
void DatePickerSetDrawsBackground( SInt32 tag, Boolean flag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void DatePickerSetBackgroundColor( SInt32 tag, CGColorRef col );
void DatePickerSetTextColor( SInt32 tag, CGColorRef col );
#endif

void DatePickerSetLocale( SInt32 tag, CFLocaleRef locale );
CFTimeInterval DatePickerTimeInterval( SInt32 tag );
void DatePickerSetTimeInterval( SInt32 tag, CFTimeInterval ti );
CFDateRef DatePickerMinDate( SInt32 tag );
CFDateRef DatePickerMaxDate( SInt32 tag );

// custom
void DatePickerSetTextRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );
void DatePickerSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );

void CocoaDatePicker( SInt32 inTag, SInt32 inEnabled, CFDateRef inDate, CGRect inRect, SInt32 inStyle, SInt32 inElements, SInt32 inWndTag );



#pragma mark - graphics
CGContextRef GraphicsCurrentCGContext();



#pragma mark - imageview
///////////////////////////////////////////////////
//                  cocoa imageview              //
///////////////////////////////////////////////////
void ImageViewSetAnimates( SInt32 tag, Boolean flag );
void ImageViewSetEditable( SInt32 tag, Boolean flag );
void ImageViewSetAllowsCutCopyPaste( SInt32 tag, Boolean flag );

void ImageViewSetCGImage( SInt32 tag, CGImageRef inImage );

void CocoaImageView( SInt32 inTag, SInt32 inEnabled, CFStringRef inImageName, CGRect inRect, SInt32 inScaling, SInt32 inAlignment, SInt32 inFrameStyle, SInt32 inWndTag );


#pragma mark - text
///////////////////////////////////////////////////
//                  cocoa text                   //
///////////////////////////////////////////////////

CFStringRef TextString( SInt32 tag );



#pragma mark - label
///////////////////////////////////////////////////
//                  cocoa textlabel              //
///////////////////////////////////////////////////

void CocoaTextLabel( SInt32 inTag, CFStringRef inText, CGRect inRect, SInt32 inWndTag );




#pragma mark - level indicator
///////////////////////////////////////////////////
//              cocoa level indicator            //
///////////////////////////////////////////////////

//@interface CocoaLevelIndicatorTarget : NSObject
//@end
//
//CocoaLevelIndicatorTarget *CocoaLevelIndicatorTargetGet();

void LevelIndicatorSetEditable( SInt32 tag, Boolean flag );
void LevelIndicatorSetTickMarkPosition( SInt32 tag, NSTickMarkPosition position );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void LevelIndicatorSetStyle( SInt32 tag, NSLevelIndicatorStyle style );
#endif

double LevelIndicatorMinValue( SInt32 tag );
double LevelIndicatorMaxValue( SInt32 tag );
double LevelIndicatorWarningValue( SInt32 tag );
double LevelIndicatorCriticalValue( SInt32 tag );

// custom
void LevelIndicatorSetCGImage( SInt32 tag, CGImageRef image );
void LevelIndicatorSetImage( SInt32 tag, CFStringRef imageName );

void CocoaLevelIndicator( SInt32 inTag, SInt32 inEnabled, double inValue, CGRect inRect, double inMinValue, double inMaxValue, double inWarningValue, double inCriticalValue, SInt32 inTickMarks, SInt32 inMajorTickMarks, SInt32 inWndTag );



#pragma mark - object
///////////////////////////////////////////////////
//                  cocoa object                 //
///////////////////////////////////////////////////
@interface NSObject (Additions)
@property (nonatomic,retain) NSNumber *wndTagNumber;

- (NSInteger)wndTag;
- (void)setWndTag:(NSInteger)tag;
@end


#pragma mark - panel
///////////////////////////////////////////////////
//                  cocoa panel                  //
///////////////////////////////////////////////////

@interface NSPanel (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;
- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

void PanelSetFloatingPanel( SInt32 tag, Boolean flag );
void PanelSetBecomesKeyOnlyIfNeeded( SInt32 tag, Boolean flag );
void PanelSetWorksWhenModal( SInt32 tag, Boolean flag );


void CocoaPanel( SInt32 inTag, CFStringRef inTitle, CGRect inRect, SInt32 inStyle );




#pragma mark - popover
///////////////////////////////////////////////////
//                  cocoa popover                //
///////////////////////////////////////////////////

// dialog events
enum {
    popoverDetachableWindow = 261,
    popoverShouldClose,
    popoverWillShow,
    popoverDidShow,
    popoverWillClose,
    popoverDidClose,
    popoverDidDetach,// macOS 10.10
    popoverShouldDetach// macOS 10.10
};

@interface CocoaPopoverDelegate : NSObject <NSPopoverDelegate,NSWindowDelegate> {
    NSWindow *detachableWindow;
}
@property (assign) NSWindow *detachableWindow;
@end

@interface NSPopover (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;
- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

NSPopover *CocoaPopoverForView( id view );

NSPopover *CocoaPopoverWithTag( SInt32 tag );

void PopoverAddSubview( SInt32 tag, SInt32 subviewTag );

void PopoverShow( SInt32 tag, CGRect inRelativeRect, SInt32 viewTag, CGRectEdge preferredEdge );
void PopoverClose( SInt32 tag );
Boolean PopoverIsShown( SInt32 tag );

void PopoverSetDetachableWindow( SInt32 popoverTag, SInt32 wndTag );

void CocoaPopover( SInt32 inTag, CGRect inRect, SInt32 inBehavior, SInt32 inAnimates );


#pragma mark - popupbutton
///////////////////////////////////////////////////
//                  cocoa popupbutton            //
///////////////////////////////////////////////////

//@interface CocoaPopUpButtonTarget : NSObject
//@end


void PopUpButtonAddItemWithTitle( SInt32 tag, CFStringRef title );
void PopUpButtonAddItemsWithTitles( SInt32 tag, CFArrayRef titles );
void PopUpButtonInsertItemWithTitle( SInt32 tag, CFStringRef title, SInt32 index );
void PopUpButtonRemoveAllItems( SInt32 tag );
void PopUpButtonRemoveItemWithTitle( SInt32 tag, CFStringRef title );
void PopUpButtonRemoveItemAtIndex( SInt32 tag, SInt32 index );
CFStringRef PopUpButtonTitleOfSelectedItem( SInt32 tag );

SInt32 PopUpButtonIndexOfSelectedItem( SInt32 tag );

void PopUpButtonSelectItemAtIndex( SInt32 tag, SInt32 index );

void PopUpButtonSelectItemWithTitle( SInt32 tag, CFStringRef title );

SInt32 PopUpButtonNumberOfItems( SInt32 tag );
CFStringRef PopUpButtonItemTitleAtIndex( SInt32 tag, SInt32 index );
CFArrayRef PopUpButtonItemTitles( SInt32 tag );

void PopUpButtonSetMenu( SInt32 tag, SInt32 menuIndex );

void PopUpButtonSetTitle( SInt32 tag, CFStringRef title );
void PopUpButtonSetPreferredEdge( SInt32 tag, CGRectEdge edge );
void PopUpButtonSetAutoenablesItems( SInt32 tag, Boolean flag );
void PopUpButtonItemSetEnabled( SInt32 tag, SInt32 index, Boolean flag );


void CocoaPopUpButton( SInt32 inTag, SInt32 inEnabled, SInt32 inIndex, CFTypeRef inItems, CGRect inRect, SInt32 inPullsDown, SInt32 inWndTag );







#pragma mark - progressindicator
///////////////////////////////////////////////////
//           cocoa progress indicator            //
///////////////////////////////////////////////////

@interface NSProgressIndicator (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end


void ProgressIndicatorSetStyle( SInt32 tag, UInt32 style );
void ProgressIndicatorSetIndeterminate( SInt32 tag, Boolean flag );

void ProgressIndicatorSetDisplayWhenStopped( SInt32 tag, Boolean flag );

void ProgressIndicatorStartAnimation( SInt32 tag );
void ProgressIndicatorStopAnimation( SInt32 tag );
void ProgressIndicatorSetUsesThreadedAnimation( SInt32 tag, Boolean flag );
void ProgressIndicatorIncrementBy( SInt32 tag, double delta );

double ProgressIndicatorDoubleValue( SInt32 tag );
double ProgressIndicatorMinValue( SInt32 tag );
double ProgressIndicatorMaxValue( SInt32 tag );
void ProgressIndicatorSetDoubleValue( SInt32 tag, double value );
void ProgressIndicatorSetMinValue( SInt32 tag, double value );
void ProgressIndicatorSetMaxValue( SInt32 tag, double value );

void ProgressIndicatorSetControlSize( SInt32 tag, NSControlSize size );


void CocoaProgressIndicator( SInt32 inTag, double inValue, CGRect inRect, double inMinValue, double inMaxValue, SInt32 inWndTag );



#pragma mark - scrollview
///////////////////////////////////////////////////
//                  cocoa scrollview             //
///////////////////////////////////////////////////

@interface NSScrollView (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

void ScrollViewSetHasHorizontalScroller( SInt32 tag, Boolean flag );
void ScrollViewSetHasVerticalScroller( SInt32 tag, Boolean flag );
void ScrollViewSetAutohidesScrollers( SInt32 tag, Boolean flag );
void ScrollViewFlashScrollers( SInt32 tag );

void ScrollViewSetDocumentView( SInt32 scrollTag, SInt32 docTag );

void CocoaScrollView( SInt32 inTag, CGRect inRect, SInt32 inBorderType, SInt32 inDocTag, SInt32 inWndTag );


#pragma mark - searchfield
///////////////////////////////////////////////////
//                  cocoa searchfield            //
///////////////////////////////////////////////////

// dialog events
enum {
    searchFieldDidStartSearching = 161,
    searchFieldDidEndSearching
};

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
@interface CocoaSearchFieldDelegate : NSObject <NSSearchFieldDelegate>
@end
#else   //__MAC_OS_X_VERSION_MAX_ALLOWED > 101000
@interface CocoaSearchFieldDelegate : NSObject <NSTextFieldDelegate>
@end
#endif  //__MAC_OS_X_VERSION_MAX_ALLOWED > 101000

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void SearchFieldSetSendsSearchStringImmediately( SInt32 tag, Boolean flag );
void SearchFieldSetSendsWholeSearchString( SInt32 tag, Boolean flag );
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
void SearchFieldSetCentersPlaceholder( SInt32 tag, Boolean flag );
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void SearchFieldSetMaximumRecents( SInt32 tag, SInt32 value );
#endif

void SearchFieldSetRecentsAutosaveName( SInt32 tag, CFStringRef name );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void SearchFieldSetSearchMenuTemplate( SInt32 tag, SInt32 menuIndex );
#endif

void CocoaSearchField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inWndTag );


#pragma mark - securetextfield
///////////////////////////////////////////////////
//             cocoa secure text field           //
///////////////////////////////////////////////////

void SecureTextFieldSetEchosBullets( SInt32 tag, Boolean flag );

void CocoaSecureTextField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inWndTag );



#pragma mark - segmentedcontrol
///////////////////////////////////////////////////
//             cocoa segmentedcontrol            //
///////////////////////////////////////////////////

@interface CocoaSegmentedControlTarget : NSObject
@end

void SegmentedControlSetSegment( SInt32 tag, SInt32 index, SInt32 enabled, CFStringRef label, CFStringRef imageName, SInt32 imageScaling, SInt32 width );

SInt32 SegmentedControlSegmentCount( SInt32 tag );

SInt32 SegmentedControlSelectedSegment( SInt32 tag );

void SegmentedControlSetMenu( SInt32 tag, SInt32 index, SInt32 menuIndex );

void SegmentedControlSetStyle( SInt32 tag, NSSegmentStyle style );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101002
void SegmentedControlSetTrackingMode( SInt32 tag, NSSegmentSwitchTracking mode );
#endif

void CocoaSegmentedControl( SInt32 inTag, SInt32 inEnabled, SInt32 inIndex, CGRect inRect, SInt32 inSegments, SInt32 inWndTag );



#pragma mark - slider
///////////////////////////////////////////////////
//                  cocoa slider                 //
///////////////////////////////////////////////////

@interface CocoaSliderTarget : NSObject
@end

void SliderSetType( SInt32 tag, NSSliderType type );
void SliderSetAllowsTickMarkValuesOnly( SInt32 tag, Boolean flag );
void SliderSetTickMarkPosition( SInt32 tag, NSTickMarkPosition position );

void SliderSetAltIncrementValue( SInt32 tag, double value );
void SliderSetKnobThickness( SInt32 tag, CGFloat thickness );

void CocoaSlider( SInt32 inTag, SInt32 inEnabled, double inValue, CGRect inRect, double inMinValue, double inMaxValue, SInt32 inTickMarks, SInt32 inWndTag );


#pragma mark - splitview
///////////////////////////////////////////////////
//                  cocoa splitview              //
///////////////////////////////////////////////////

// dialog events
enum {
    splitViewResizeSubviewsWithOldSize = 221,
    splitViewWillResizeSubviews,
    splitViewDidResizeSubviews,
    splitViewCanCollapseSubview,
    splitViewShouldCollapseSubview,
    splitViewShouldAdjustSizeOfSubview,
    splitViewEffectiveRect,
    splitViewShouldHideDividerAtIndex,
    splitViewAdditionalEffectiveRectOfDividerAtIndex,
    splitViewConstrainMaxCoordinate,
    splitViewConstrainMinCoordinate,
    splitViewConstrainSplitPosition
};

@interface NSSplitView (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

@interface CocoaSplitViewDelegate : NSObject <NSSplitViewDelegate>
@end

void SplitViewAdjustSubviews( SInt32 tag );
BOOL SplitViewIsPaneCollapsed( SInt32 tag, SInt32 paneIndex );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void SplitViewSetHoldingPriority( SInt32 tag, SInt32 paneIndex, float priority );
#endif

CGFloat SplitViewDividerThickness( SInt32 tag );
void SplitViewSetPositionOfDivider( SInt32 tag, SInt32 index, CGFloat position );
void SplitViewAddSubview( SInt32 tag, SInt32 index, SInt32 subviewTag );

void CocoaSplitView( SInt32 inTag, CGRect inRect, SInt32 inVertical, SInt32 inDividerStyle, SInt32 inWndTag );



#pragma mark - stepper
///////////////////////////////////////////////////
//                  cocoa stepper                //
///////////////////////////////////////////////////

@interface CocoaStepperTarget : NSObject
@end

void StepperSetAutorepeat( SInt32 tag, Boolean flag );
void StepperSetValueWraps( SInt32 tag, Boolean flag );
void StepperSetContinuous( SInt32 tag, Boolean flag );
void StepperSetIncrement( SInt32 tag, double increment );

void CocoaStepper( SInt32 inTag, SInt32 inEnabled, double inValue, CGRect inRect, double inMinValue, double inMaxValue, SInt32 inWndTag );


#pragma mark - tableview

///////////////////////////////////////////////////
//                  cocoa tableview              //
///////////////////////////////////////////////////

// dialog events
enum {
    tableViewDoubleAction = 251,
    tableViewSelectionDidChange
};

@interface CocoaTableViewDelegate : NSObject <NSTableViewDelegate,NSTableViewDataSource,NSTextFieldDelegate> {
    NSTableView *table;
    NSMutableArray *tableData;
}
@property (assign) NSTableView *table;
@property (assign) NSMutableArray *tableData;

- (void)doubleAction:(id)sender;
@end

void TableViewSetData( SInt32 tag, CFMutableArrayRef array );
CFMutableArrayRef TableViewData( SInt32 tag );
void TableViewReloadData( SInt32 tag );
void TableViewScrollRowToVisible( SInt32 tag, SInt32 row );
void TableViewScrollColumnToVisible( SInt32 tag, SInt32 col );

SInt32 TableViewSelectedRow( SInt32 tag );
SInt32 TableViewSelectedColumn( SInt32 tag );

CFArrayRef TableViewSelectedRowIndexes( SInt32 tag );
SInt32 TableViewNumberOfSelectedRows( SInt32 tag );
Boolean TableViewIsRowSelected( SInt32 tag, SInt32 row );
SInt32 TableViewClickedRow( SInt32 tag );
SInt32 TableViewClickedColumn( SInt32 tag );

// custom
void *TableViewView( SInt32 tag, SInt32 row, SInt32 col );


#pragma mark - tabview
///////////////////////////////////////////////////
//                  cocoa tabview                //
///////////////////////////////////////////////////

@interface NSTabView (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

// dialog events
enum {
    tabViewDidChangeNumberOfTabViewItems = 191,
    tabViewShouldSelectTabViewItem,
    tabViewWillSelectTabViewItem,
    tabViewDidSelectTabViewItem
};


@interface CocoaTabViewDelegate : NSObject <NSTabViewDelegate> {
    BOOL boolVal;
    NSInteger indexVal;
}
@property (assign) BOOL boolVal;
@property (assign) NSInteger indexVal;
@end

void TabViewSetType( SInt32 tag, SInt32 type );
SInt32 TabViewIndex( SInt32 tag );

void TabViewSelectNextItem( SInt32 tag );
void TabViewSelectPreviousItem( SInt32 tag );
void TabViewSetFont( SInt32 tag, CTFontRef font );
void TabViewSetControlTint( SInt32 tag, NSControlTint tint );
void TabViewSetDrawsBackground( SInt32 tag, Boolean flag );
CGRect TabViewContentRect( SInt32 tag );


// custom
void TabViewAddSubview( SInt32 tabTag, SInt32 index, SInt32 subviewTag );

void CocoaTabView( SInt32 inTag, SInt32 inIndex, CFTypeRef inItems, CGRect inRect, SInt32 inWndTag )
;



#pragma mark - textfield formatter
///////////////////////////////////////////////////
//        cocoa textfield formatter              //
///////////////////////////////////////////////////
@interface CocoaTextFieldFormatter : NSFormatter {
    NSCharacterSet *set;
    BOOL pass;
    NSUInteger length;
}

@property (assign) NSCharacterSet *set;
@property (assign) BOOL pass;
@property (assign) NSUInteger length;

- (id)initWithCharacters:(NSString *)inCharacters pass:(BOOL)inPass length:(NSUInteger)inLength caseSensitive:(BOOL)inCaseSensitive;
@end







#pragma mark - textfield
///////////////////////////////////////////////////
//                  cocoa textfield              //
///////////////////////////////////////////////////

// dialog events
enum {
    textFieldDidBeginEditing = controlTextDidBeginEditing,
    textFieldDidChange = controlTextDidChange,
    textFieldDidEndEditing = controlTextDidEndEditing
};

@interface CocoaTextFieldDelegate : NSObject <NSTextFieldDelegate>
@end

CocoaTextFieldDelegate *CocoaTextFieldDelegateGet();

void TextFieldSetEditable( SInt32 tag, Boolean flag );
void TextFieldSetSelectable( SInt32 tag, Boolean flag );

void TextFieldMakeFirstResponder( SInt32 inTag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void TextFieldSetPlaceholderString( SInt32 tag, CFStringRef string );
void TextFieldSetPlaceholderAttributedString( SInt32 tag, CFAttributedStringRef string );
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void TextFieldSetTextColor( SInt32 tag, CGColorRef col );
void TextFieldSetBackgroundColor( SInt32 tag, CGColorRef col );
#endif

void TextFieldSetDrawsBackground( SInt32 tag, Boolean flag );

void TextFieldSelectText( SInt32 tag );
void TextFieldSetAllowsEditingTextAttributes( SInt32 tag, Boolean flag );
void TextFieldSetImportsGraphics( SInt32 tag, Boolean flag );



// custom
void TextFieldSetTextRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );
void TextFieldSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );

void TextFieldSetFormat( SInt32 tag, CFStringRef characters, Boolean pass, UInt32 length, Boolean caseSensitive );




void CocoaTextField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inWndTag );


#pragma mark - textview
///////////////////////////////////////////////////
//                  cocoa textview               //
///////////////////////////////////////////////////

@interface NSTextView (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;

- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
@end

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void TextViewSetBackgroundColor( SInt32 tag, CGColorRef col );
#endif

void TextViewSetDrawsBackground( SInt32 tag, Boolean flag );

void TextViewSetEditable( SInt32 tag, Boolean flag );
void TextViewSetSelectable( SInt32 tag, Boolean flag );

CFStringRef TextViewString( SInt32 tag );
void TextViewSetString( SInt32 tag, CFStringRef string );

void TextViewSetUsesRuler( SInt32 tag, Boolean flag );
void TextViewSetRulerVisible( SInt32 tag, Boolean flag );
void TextViewSetUsesInspectorBar( SInt32 tag, Boolean flag );
void TextViewSetSelectedRange( SInt32 tag, CFRange range );
void TextViewSetContinuousSpellCheckingEnabled( SInt32 tag, Boolean flag );
void TextViewSetGrammarCheckingEnabled( SInt32 tag, Boolean flag );
void TextViewStartSpeaking( SInt32 tag );
void TextViewStopSpeaking( SInt32 tag );
void TextViewSetUsesFontPanel( SInt32 tag, Boolean flag );
void TextViewSetUsesFindPanel( SInt32 tag, Boolean flag );
void TextViewSetUsesFindBar( SInt32 tag, Boolean flag );

CFMutableAttributedStringRef TextViewTextStorage( SInt32 tag );
void TextViewSetRichText( SInt32 tag, Boolean flag );

// custom
void TextViewPerformFindPanelAction( SInt32 tag, NSTextFinderAction action );

void CocoaTextView( SInt32 inTag, CGRect inRect, SInt32 inScrollTag, SInt32 inWndTag );

void TextViewSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );




#pragma mark - tokenfield
///////////////////////////////////////////////////
//                  cocoa tokenfield             //
///////////////////////////////////////////////////

// dialog events
enum {
    tokenFieldDisplayStringForRepresentedObject = 171,
    tokenFieldStyleForRepresentedObject,
    tokenFieldCompletionsForSubstring,
    tokenFieldEditingStringForRepresentedObject,
    tokenFieldRepresentedObjectForEditingString,
    tokenFieldShouldAddObjects,
    tokenFieldReadFromPasteboard,
    tokenFieldWriteRepresentedObjectToPasteboard,
    tokenFieldHasMenuForRepresentedObject,
    tokenFieldMenuForRepresentedObject
};

@interface CocoaTokenFieldDelegate : NSObject <NSTokenFieldDelegate> {
    BOOL boolVal;
}
@property (assign) BOOL boolVal;
@end

CocoaTokenFieldDelegate *CocoaTokenFieldDelegateGet();

void TokenFieldSetTokenizingCharacterSet( SInt32 tag, CFCharacterSetRef set );

void CocoaTokenField( SInt32 inTag, SInt32 inEnabled, CFStringRef inText, CGRect inRect, SInt32 inStyle, SInt32 inWndTag );


#pragma mark - toolbar
///////////////////////////////////////////////////
//                  cocoa toolbar                //
///////////////////////////////////////////////////

// toolbarItem events
enum {
    toolbarItemClick = 211
};

@interface CocoaToolbarItemTarget : NSObject
@end

CocoaToolbarItemTarget *CocoaToolbarItemTargetGet();


@interface CocoaToolbarDelegate : NSObject <NSToolbarDelegate>
@end

CocoaToolbarDelegate *CocoaToolbarDelegateGet();


@interface CocoaToolbarObj : NSObject {
    NSToolbar *toolbar;
    NSMutableDictionary *items;
    NSMutableArray *allowedIdentifiers;
    NSMutableArray *defaultIdentifiers;
}

@property (assign) NSToolbar *toolbar;
@property (assign) NSMutableDictionary *items;
@property (assign) NSMutableArray *allowedIdentifiers;
@property (assign) NSMutableArray *defaultIdentifiers;

- (id)initWithToolbar:(NSToolbar *)tb;

@end

CocoaToolbarObj *CocoaToolbarObjWithTag( SInt32 tag );

NSToolbar *CocoaToolbarWithTag( SInt32 tag );


//void ToolbarInsertItem( SInt32 toolbarTag, SInt32 itemTag, CFStringRef inIdentifier, CFStringRef inLabel, CFStringRef inImageName, Boolean allowed, Boolean dflt );


void ToolbarSetSizeMode( SInt32 tag, NSToolbarSizeMode size );

void ToolbarSetDisplayMode( SInt32 tag, NSToolbarDisplayMode mode );
void ToolbarSetAllowsUserCustomization( SInt32 tag, Boolean flag );
void ToolbarSetShowsBaselineSeparator( SInt32 tag, Boolean flag );
void ToolbarSetAutosavesConfiguration( SInt32 tag, Boolean flag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void ToolbarSetAllowsExtensionItems( SInt32 tag, Boolean flag );
#endif

void CocoaToolbar( SInt32 inTag );



#pragma mark - toolbaritem
///////////////////////////////////////////////////
//               cocoa toolbaritem               //
///////////////////////////////////////////////////

void CocoaToolbarItem( SInt32 inTag, NSString *inIdentifier, CFStringRef inLabel, CFStringRef inImageName, SInt32 inAllowed, SInt32 inDefault, SInt32 inToolbarTag );

void ToolbarItemSetMinSize( SInt32 toolbarTag, SInt32 itemTag, CGSize size );
void ToolbarItemSetMaxSize( SInt32 toolbarTag, SInt32 itemTag, CGSize size );

void ToolbarItemSetToolTip( SInt32 toolbarTag, SInt32 itemTag, CFStringRef toolTip );



#pragma mark - view
///////////////////////////////////////////////////
//                  cocoa view                   //
///////////////////////////////////////////////////

// dialog events
enum {
    viewDrawRect = 231,
    viewMouseDown,
    viewMouseDragged,
    viewMouseUp,
    viewMouseMoved,
    viewMouseEntered,
    viewMouseExited,
    viewRightMouseDown,
    viewRightMouseDragged,
    viewRightMouseUp,
    viewKeyDown,
    viewKeyUp,
    viewFlagsChanged
};

@interface CocoaViewSubclass : NSView {
    BOOL flip;
    BOOL responderFlag;
}

@property (assign) BOOL flip;
@property (assign) BOOL responderFlag;

//- (void)setTrackingAreaRect:(NSRect)rect options:(NSTrackingAreaOptions)options;

@end


    
SInt32 CocoaViewTag( id view );
id CocoaViewSubviewWithTag( NSView *superview, SInt32 tag );
NSView *CocoaViewWithTag( SInt32 tag );

SInt32 ViewSuperview( SInt32 tag );
SInt32 ViewWindow( SInt32 tag );
void ViewSetFrameOrigin( SInt32 tag, CGPoint origin );
void ViewSetFrameSize( SInt32 tag, CGSize size );
void ViewSetFrameRotation( SInt32 tag, CGFloat rotation );
CGRect ViewBounds( SInt32 tag );
void ViewSetBounds( SInt32 tag, CGRect bounds );
void ViewSetBoundsOrigin( SInt32 tag, CGPoint origin );
void ViewSetBoundsSize( SInt32 tag, CGSize size );
void ViewSetBoundsRotation( SInt32 tag, CGFloat rotation );

CGFloat ViewFrameRotation( SInt32 tag );
CGFloat ViewBoundsRotation( SInt32 tag );

void ViewSetWantsLayer( SInt32 tag, Boolean flag );

CGFloat ViewAlphaValue( SInt32 tag );
void ViewSetAlphaValue( SInt32 tag, CGFloat value );
CGFloat ViewFrameCenterRotation( SInt32 tag );
void ViewSetFrameCenterRotation( SInt32 tag, CGFloat rotation );
void ViewPrint( SInt32 tag );
void ViewSetNeedsDisplay( SInt32 tag );
void ViewRotateByAngle( SInt32 tag, CGFloat angle );


void ViewAddSubview( SInt32 viewTag, SInt32 subviewTag );

void ViewAddSubviewPositioned( SInt32 superviewTag, SInt32 subviewTag, NSWindowOrderingMode position, SInt32 otherViewTag );

void ViewRemoveFromSuperview( SInt32 tag );

void ViewSetAutoresizingMask( SInt32 tag, SInt32 mask );

CGRect ViewFrame( SInt32 tag );
void ViewSetFrame( SInt32 tag, CGRect r );

void ViewSetToolTip( SInt32 tag, CFStringRef string );

Boolean ViewIsHidden( SInt32 tag );
Boolean ViewIsHiddenOrHasHiddenAncestor( SInt32 tag );
SInt32 ViewNextKeyView( SInt32 tag );
SInt32 ViewNextValidKeyView( SInt32 tag );
SInt32 ViewPreviousKeyView( SInt32 tag );
SInt32 ViewPreviousValidKeyView( SInt32 tag );

void ViewScrollRect( SInt32 tag, CGRect r, CGSize size );
void ViewSetNeedsDisplayInRect( SInt32 tag, CGRect r );

void ViewSetAcceptsFirstResponder( SInt32 tag, Boolean flag );


void CocoaView( SInt32 inTag, CGRect inRect, SInt32 inSubclass, SInt32 inWndTag );

// generic functions
CFStringRef ViewText( SInt32 tag );

// animator
void *ViewAnimator( SInt32 tag );
void ViewAnimatorSetFrame( SInt32 tag, CGRect r );
void ViewAnimatorSetFrameOrigin( SInt32 tag, CGPoint origin );
void ViewAnimatorSetFrameSize( SInt32 tag, CGSize size );
void ViewAnimatorSetFrameRotation( SInt32 tag, CGFloat rot );
void ViewAnimatorSetBounds( SInt32 tag, CGRect r );
void ViewAnimatorSetBoundsOrigin( SInt32 tag, CGPoint origin );
void ViewAnimatorSetBoundsSize( SInt32 tag, CGSize size );
void ViewAnimatorSetBoundsRotation( SInt32 tag, CGFloat rot );
void ViewAnimatorSetFrameCenterRotation( SInt32 tag, CGFloat rot );
void ViewAnimatorSetAlphaValue( SInt32 tag, CGFloat value );

//void ViewSetTrackingArea( SInt32 tag, CGRect r, NSTrackingAreaOptions options );


// custom
id ViewWithTag( SInt32 tag );

void ViewSetFlipped( SInt32 tag, Boolean flag );

void ViewsEmbedInView( SInt32 tag1, ... );


#pragma mark - window
///////////////////////////////////////////////////
//                  cocoa window                 //
///////////////////////////////////////////////////

// content view tag
enum {
    windowContentViewTag = -1437226411
};

id CocoaTargetWindow( SInt32 *inWndTag );
NSView *CocoaTargetContentView( id w );

@interface CocoaWindowContentView : NSView {
    BOOL flip;
    BOOL subclassFlag;
    BOOL responderFlag;
}

@property (assign) BOOL flip;
@property (assign) BOOL subclassFlag;
@property (assign) BOOL responderFlag;

@end


//@interface CocoaWindowPrintView : NSView {
//    NSMutableString *string;
//}
//@property (assign) NSMutableString *string;
//
//- (void)printString:(NSString *)s;
//- (void)printWithFormat:(NSString *)format arguments:(va_list)ap;
//- (void)printClear;
//@end

//void WindowPrint( CFStringRef string );
//void WindowPrintWithFormat( CFStringRef format, ... );
//void WindowPrintClear();


// dialog events
enum {
    windowWillBeginSheet = 101,
    windowDidEndSheet,
    windowWillResize,           // get/set CGSize
    windowDidResize,
    windowWillStartLiveResize,
    windowDidEndLiveResize,
    windowWillMiniaturize,
    windowDidMiniaturize,
    windowDidDeminiaturize,
    windowShouldZoom,          // get CGRect, set Boolean
    windowWillEnterFullScreen,
    windowDidEnterFullScreen,
    windowWillExitFullScreen,
    windowDidExitFullScreen,
    windowDidFailToEnterFullScreen,
    windowDidFailToExitFullScreen,
    windowWillMove,
    windowDidMove,
    windowDidChangeScreen,
    windowShouldClose,         // set Boolean
    windowWillClose,
    windowDidBecomeKey,
    windowDidResignKey,
    windowDidBecomeMain,
    windowDidResignMain,
    windowDidUpdate
};

typedef void (*CocoaWindowCallbackType)(SInt32,SInt32);


@interface CocoaWindowDelegate : NSObject <NSWindowDelegate> {
    BOOL boolVal;
    NSSize sizeVal;
    NSRect rectVal;
    CocoaWindowCallbackType allWindowsCallback;
}
@property (assign) BOOL boolVal;
@property (assign) NSSize sizeVal;
@property (assign) NSRect rectVal;
@property (assign) CocoaWindowCallbackType allWindowsCallback;
@end

CocoaWindowDelegate *CocoaWindowDelegateGet();

@interface NSWindow (Additions)
@property (nonatomic,retain) NSNumber *tagNumber;
@property (nonatomic,retain) NSValue *callbackValue;
- (NSInteger)tag;
- (void)setTag:(NSInteger)tag;
- (CocoaWindowCallbackType)cocoaWindowCallback;
- (void)setCocoaWindowCallback:(CocoaWindowCallbackType)cb;
@end

// private
#if !__LP64__
BOOL CocoaWindowIsFBWindow( SInt32 tag );
BOOL FBWindowIsCocoaWindow( SInt32 tag );
#endif  //!__LP64__

BOOL CocoaObjectIsPopover( SInt32 tag );
BOOL CocoaObjectIsWindow( SInt32 tag );

NSWindow *CocoaWindowWithTag( NSInteger tag );
NSWindow *CocoaFrontWindow();
BOOL WindowIsCocoa( SInt32 tag );

void CocoaWindowCarbonRuntimeCosmeticFix( NSWindow *w );

void CocoaWindowSetTitle( SInt32 tag, CFStringRef title );

// public
void CocoaWindowSetTarget( SInt32 tag );
void WindowSetAlphaValue( SInt32 tag, CGFloat value );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1070
void WindowSetBackgroundColor( SInt32 tag, CGColorRef col );
#endif

void WindowSetContentBorderThickness( SInt32 tag, CGFloat thickness, CGRectEdge edge );

SInt32 WindowAttachedSheet( SInt32 tag );

void WindowSetAspectRatio( SInt32 tag, CGSize size );

CFStringRef WindowTitle( SInt32 tag );

CFURLRef WindowRepresentedURL( SInt32 tag );
void WindowSetRepresentedURL( SInt32 tag, CFURLRef url );

void WindowCenter( SInt32 tag );

void WindowClose( SInt32 tag );

CGRect WindowFrame( SInt32 tag );
CGRect WindowContentRect( SInt32 tag );

void WindowSetFrameTopLeftPoint( SInt32 tag, CGPoint pt );

void WindowSetMinSize( SInt32 tag, CGSize size );
void WindowSetMaxSize( SInt32 tag, CGSize size );

void WindowSetContentAspectRatio( SInt32 tag, CGSize size );

void WindowSetContentMinSize( SInt32 tag, CGSize size );
void WindowSetContentMaxSize( SInt32 tag, CGSize size );
void WindowSetFrameAutosaveName( SInt32 tag, CFStringRef name );
void WindowSetFrameUsingName( SInt32 tag, CFStringRef name );

void WindowBeginSheet( SInt32 tag, SInt32 sheetTag );
void WindowEndSheet( SInt32 tag, SInt32 sheetTag );

void WindowFlush( SInt32 tag );

void WindowMakeFirstResponder( SInt32 wndTag, SInt32 viewTag );

void WindowSetToolbar( SInt32 wndTag, SInt32 toolbarTag );
SInt32 WindowToolbar( SInt32 tag );

void WindowSetDocumentEdited( SInt32 tag, Boolean flag );
Boolean WindowDocumentEdited( SInt32 tag );


void WindowToggleFullScreen( SInt32 tag );
void WindowSetCanHide( SInt32 tag, Boolean flag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
void WindowSetTitlebarAppearsTransparent( SInt32 tag, Boolean flag );
#endif

void WindowSetAllowsToolTipsWhenApplicationIsInactive( SInt32 tag, Boolean flag);

void WindowSetTitleWithRepresentedFilename( SInt32 tag, CFStringRef filename );

void WindowAddChildWindow( SInt32 parTag, SInt32 childTag, NSWindowOrderingMode ordered );
void WindowRemoveChildWindow( SInt32 parTag, SInt32 childTag );
SInt32 WindowParentWindow( SInt32 tag );


// 20171018
void WindowSetHidesOnDeactivate( SInt32 tag, Boolean flag );
void WindowSetHasShadow( SInt32 tag, Boolean flag );
void WindowSetFrameOrigin( SInt32 tag, CGPoint pt );
void WindowSetFrame( SInt32 tag, CGRect frame );
void WindowCascadeTopLeftFromPoint( SInt32 tag, CGPoint pt );
Boolean WindowIsZoomed( SInt32 tag );
void WindowPerformZoom( SInt32 tag );
void WindowZoom( SInt32 tag );
void WindowSetShowsResizeIndicator( SInt32 tag, Boolean flag );

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 101000
void WindowSetMaxFullScreenContentSize( SInt32 tag, CGSize size );
void WindowSetMinFullScreenContentSize( SInt32 tag, CGSize size );
#endif

void WindowOrderBack( SInt32 tag );
Boolean WindowIsVisible( SInt32 tag );
void WindowRemoveFrameUsingName( CFStringRef name );
void WindowSetExcludedFromWindowsMenu( SInt32 tag, Boolean flag );

CGContextRef WindowGraphicsContext( SInt32 tag );


// custom
void WindowSetTitleWithRepresentedURL( SInt32 tag, CFURLRef url );

Boolean WindowExists( SInt32 tag );

id WindowWithTag( SInt32 tag );

void WindowSetBackgroundRGB( SInt32 tag, CGFloat r, CGFloat g, CGFloat b, CGFloat a );

void WindowSetDelegateCallback( SInt32 tag, void *callback );

void WindowSubclassContentView( SInt32 tag );


void CocoaWindow( SInt32 inTag, CFStringRef inTitle, CGRect inRect, SInt32 inStyle );


#pragma mark - cocoa nibs

@interface CocoaNibObject : NSObject {
    NSString *nibName;
    NSArray *topLevelObjects;
}
- (id)initWithName:(NSString *)name objects:(NSArray *)objects;

@property (nonatomic,assign) NSString *nibName;
@property (nonatomic,assign) NSArray *topLevelObjects;
@end

#pragma mark - nibmenu
void CocoaNibMenu( CFStringRef inNibName );

#pragma mark - nibpopover
void CocoaNibPopover( SInt32 inTag, CFStringRef inNibName );

#pragma mark - nibtoolbar
void CocoaNibToolbar( SInt32 inTag, CFStringRef inNibName );

#pragma mark - nibwindow
void CocoaNibWindow( SInt32 inTag, CFStringRef inNibName, CFStringRef inWndIdentifier );

#pragma mark - nibview
void CocoaNibView( SInt32 inTag, CFStringRef inNibName, CFStringRef inViewIdentifier, CGPoint inPoint, SInt32 inSuperviewTag );

#pragma mark - nib <everything>
void CocoaNib( CFStringRef inNibName );


#endif //__MAC_OS_X_VERSION_MAX_ALLOWED > 1060


#endif  // COCOAUI_H





