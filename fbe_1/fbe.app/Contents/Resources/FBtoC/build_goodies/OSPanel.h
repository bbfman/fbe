#import <AppKit/AppKit.h>
#include "FBtoC.h"
#include "Runtime.h"

#ifndef OSPANEL_H
#define OSPANEL_H

// options
enum {
    //open
    OSPanelCantChooseFiles                 = 1 << 0,
    OSPanelCanChooseDirectories            = 1 << 1,
    OSPanelAllowsMultipleSelection         = 1 << 2,
    OSPanelDoesntResolveAliases            = 1 << 3,
    OSPanelCantDownloadUbiquitousContents  = 1 << 4,
    OSPanelCantResolveUbiquitousConflicts  = 1 << 5,

    // save
    OSPanelDoesntShowTagField              = 1 << 6,
    OSPanelExtensionVisible                = 1 << 7,
    OSPanelCanSelectHiddenExtension        = 1 << 8,// works with open panel, but only makes sense with save
    OSPanelAllowsOtherFileTypes            = 1 << 9,

    // open & save
    OSPanelCanCreateDirectories            = 1 << 10,
    OSPanelCantCreateDirectories           = 1 << 11,
    OSPanelShowsHiddenFiles                = 1 << 12,
    OSPanelTreatsFilePackagesAsDirectories = 1 << 13
};

typedef void (*OSPanelCallbackType)(NSInteger,id,void*);

@interface OSPanelHandler : NSObject <NSOpenSavePanelDelegate> {
    NSWindow *_parentWindow;
    OSPanelCallbackType _callback;
    void *_userData;
}
@property (assign) NSWindow *parentWindow;
@property (assign) OSPanelCallbackType callback;
@property (assign) void *userData;

- (id)initWithParentWindow:(NSWindow *)parentWindow callback:(OSPanelCallbackType)callback userData:(void *)userData;
@end

enum {
    OSPanelTypeOpen,
    OSPanelTypeSave
};

// obj-c
id OSPanelOpen( UInt32 options, NSString *message, id allowedFileTypes, NSString *prompt, NSURL *dirURL );
NSURL *OSPanelSave( UInt32 options, NSString *message, id allowedFileTypes, NSString *nameFieldStringValue, NSString *prompt, NSURL *dirURL );
void OSPanelSetTitle( NSString *string );
void OSPanelSetNameFieldLabel( NSString *string );
void OSPanelSetTagNames( id tagNames );

// fb
CFTypeRef OSpanelOpen( UInt32 options, CFStringRef message, CFTypeRef allowedFileTypes, CFStringRef prompt, CFURLRef dirURL );
CFURLRef OSpanelSave( UInt32 options, CFStringRef message, CFTypeRef allowedFileTypes, CFStringRef nameFieldStringValue, CFStringRef prompt, CFURLRef dirURL );
void OSpanelSetTitle( CFStringRef string );
void OSpanelSetNameFieldLabel( CFStringRef string );
void OSpanelSetTagNames( CFTypeRef tagNames );

// obj-c & fb
void OSPanelInstallHandler( void *parentWindow, void *callback, void *userData );
void OSPanelSetAccessoryView( id accessoryView );

#endif //OSPANEL_H
