#include "OSPanel.h"
#include "ArrayIndices.h"

static OSPanelHandler *sOSPanelHandler;
static NSView *sOSPanelAccessoryView;
static NSString *sOSPanelTitle;
static NSString *sOSPanelNameFieldLabel;
static NSArray *sOSPanelTagNames;

@implementation OSPanelHandler

@synthesize parentWindow = _parentWindow;
@synthesize callback = _callback;
@synthesize userData = _userData;

#pragma mark - private
- (id)initWithParentWindow:(NSWindow *)parentWindow callback:(OSPanelCallbackType)callback userData:(void *)userData {
    if ( self = [super init] ) {
        _parentWindow = parentWindow;
        _callback = callback;
        _userData = userData;
    }
    return self;
}

@end

id OSPanel_priv( NSUInteger type, NSUInteger options, NSString *message, id allowedFileTypes, NSString *nameFieldStringValue, NSString *prompt, NSURL *dirURL )
{
    if ( ![NSApp mainMenu] ) NSApplicationLoad();     // Cocoa in Carbon
    id panel;
    
    // type & options
    switch ( type ) {
        case OSPanelTypeSave:
            panel = [NSSavePanel savePanel];
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1080
            if ( options & OSPanelDoesntShowTagField ) [panel setShowsTagField:NO];
#endif
            //if ( options & OSPanelExtensionVisible ) [panel setExtensionHidden:NO]; // bw 20170517
            [panel setExtensionHidden:((options & OSPanelExtensionVisible) == 0)];	// bw 20170517
            if ( nameFieldStringValue ) [panel setNameFieldStringValue:nameFieldStringValue];
            break;
        default:
            panel = [NSOpenPanel openPanel];
            if ( options & OSPanelCantChooseFiles ) [panel setCanChooseFiles:NO];
            if ( options & OSPanelCanChooseDirectories ) [panel setCanChooseDirectories:YES];
            if ( options & OSPanelAllowsMultipleSelection ) [panel setAllowsMultipleSelection:YES];
            if ( options & OSPanelDoesntResolveAliases ) [panel setResolvesAliases:NO];
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1090
            if ( options & OSPanelCantDownloadUbiquitousContents ) [panel setCanDownloadUbiquitousContents:NO];
            if ( options & OSPanelCantResolveUbiquitousConflicts ) [panel setCanResolveUbiquitousConflicts:NO];
#endif
            break;
    }
    
    if ( options & OSPanelCanCreateDirectories ) [panel setCanCreateDirectories:YES];
    if ( options & OSPanelCantCreateDirectories ) [panel setCanCreateDirectories:NO];
    if ( options & OSPanelShowsHiddenFiles ) [panel setShowsHiddenFiles:YES];
    if ( options & OSPanelCanSelectHiddenExtension ) [panel setCanSelectHiddenExtension:YES];
    if ( options & OSPanelAllowsOtherFileTypes ) [panel setAllowsOtherFileTypes:YES];
    if ( options & OSPanelTreatsFilePackagesAsDirectories ) [panel setTreatsFilePackagesAsDirectories:YES];
    
    // message
    if ( message ) [panel setMessage:message];
    
    // allowedFileTypes
    if ( allowedFileTypes ) {
        if ( [allowedFileTypes isKindOfClass:[NSString class]] ) {
            [panel setAllowedFileTypes:[allowedFileTypes componentsSeparatedByString:@";"]];
        } else if ( [allowedFileTypes isKindOfClass:[NSArray class]] ) {
            [panel setAllowedFileTypes:allowedFileTypes];
        }
    }
    
    // prompt
    if ( prompt ) [panel setPrompt:prompt];
    
    // directoryURL
    if ( dirURL ) [panel setDirectoryURL:dirURL];
    
    // title
    if ( sOSPanelTitle ) {
        [panel setTitle:sOSPanelTitle];
        [sOSPanelTitle release];
        sOSPanelTitle = nil;
    }
    
    // nameFieldLabel
    if ( sOSPanelNameFieldLabel ) {
        [panel setNameFieldLabel:sOSPanelNameFieldLabel];
        [sOSPanelNameFieldLabel release];
        sOSPanelNameFieldLabel = nil;
    }
    
    // tagNames
    if ( sOSPanelTagNames ) {
#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1080
        [panel setTagNames:sOSPanelTagNames];
#endif
        [sOSPanelTagNames release];
        sOSPanelTagNames = nil;
    }
    
    // accessoryView
    if ( sOSPanelAccessoryView ) {
        [panel setAccessoryView:sOSPanelAccessoryView];
        [sOSPanelAccessoryView release];
        sOSPanelAccessoryView = nil;
    }
    
    // run panel
    if ( sOSPanelHandler ) {
        [panel beginSheetModalForWindow:sOSPanelHandler.parentWindow completionHandler:^(NSInteger result) {
            if ( [panel allowsMultipleSelection] ) {
                if ( sOSPanelHandler.callback ) (*sOSPanelHandler.callback)( result, [panel URLs], sOSPanelHandler.userData );
            } else {
                if ( sOSPanelHandler.callback ) (*sOSPanelHandler.callback)( result, [panel URL], sOSPanelHandler.userData );
            }
            [sOSPanelHandler release];
            sOSPanelHandler = nil;
        }];
    } else {
        if ( [panel runModal] == NSOKButton ) {
            if ( [panel allowsMultipleSelection] ) {
                return [panel URLs];
            } else {
                return [panel URL];
            }
        }
    }
    
    return nil;
}

// obj-c
id OSPanelOpen( UInt32 options, NSString *message, id allowedFileTypes, NSString *prompt, NSURL *dirURL )
{ return OSPanel_priv( OSPanelTypeOpen, options, message, allowedFileTypes, nil, prompt, dirURL ); }
 
NSURL *OSPanelSave( UInt32 options, NSString *message, id allowedFileTypes, NSString *nameFieldStringValue, NSString *prompt, NSURL *dirURL )
{ return OSPanel_priv( OSPanelTypeSave, options, message, allowedFileTypes, nameFieldStringValue, prompt, dirURL ); }

void OSPanelSetTitle( NSString *string )
{
    if ( sOSPanelTitle ) [sOSPanelTitle release];
    sOSPanelTitle = [string retain];
}

void OSPanelSetNameFieldLabel( NSString *string )
{
    if ( sOSPanelNameFieldLabel ) [sOSPanelNameFieldLabel release];
    sOSPanelNameFieldLabel = [string retain];
}

void OSPanelSetTagNames( id tagNames )
{
    if ( sOSPanelTagNames ) [sOSPanelTagNames release];
    if ( [tagNames isKindOfClass:[NSString class]] ) {
        sOSPanelTagNames = [[tagNames componentsSeparatedByString:@";"] retain];
    } else if ( [(id)tagNames isKindOfClass:[NSArray class]] ) {
        sOSPanelTagNames = [tagNames retain];
    }
}

// fb
CFTypeRef OSpanelOpen( UInt32 options, CFStringRef message, CFTypeRef allowedFileTypes, CFStringRef prompt, CFURLRef dirURL )
{ return (CFTypeRef)OSPanelOpen( options, (NSString *)message, (id)allowedFileTypes, (NSString *)prompt, (NSURL *)dirURL ); }

CFURLRef OSpanelSave( UInt32 options, CFStringRef message, CFTypeRef allowedFileTypes, CFStringRef nameFieldStringValue, CFStringRef prompt, CFURLRef dirURL )
{ return (CFURLRef)OSPanelSave( options, (NSString *)message, (id)allowedFileTypes, (NSString *)nameFieldStringValue, (NSString *)prompt, (NSURL *)dirURL ); }

void OSpanelSetTitle( CFStringRef string )
{ OSPanelSetTitle( (NSString *)string ); }

void OSpanelSetNameFieldLabel( CFStringRef string )
{ OSPanelSetNameFieldLabel( (NSString *)string ); }

void OSpanelSetTagNames( CFTypeRef tagNames )
{ OSPanelSetTagNames( (id)tagNames ); }

// obj-c & fb
void OSPanelInstallHandler( void *parentWindow, void *callback, void *userData )
{
    if ( sOSPanelHandler ) [sOSPanelHandler release];
    if ( parentWindow ) {
        if ( ![(id)parentWindow isKindOfClass:[NSWindow class]] ) {
            if ( [(id)parentWindow isKindOfClass:NSClassFromString(@"NSHIObject")] ) {
                NSApplicationLoad();
                parentWindow = [[NSWindow alloc] initWithWindowRef:parentWindow];
            } else {
                parentWindow = nil;
            }
        }
    }
    sOSPanelHandler = [[OSPanelHandler alloc] initWithParentWindow:(NSWindow *)parentWindow callback:callback userData:userData];
}

void OSPanelSetAccessoryView( id accessoryView )
{
    if ( sOSPanelAccessoryView ) [sOSPanelAccessoryView release];
    sOSPanelAccessoryView = [(NSView *)accessoryView retain];
}

