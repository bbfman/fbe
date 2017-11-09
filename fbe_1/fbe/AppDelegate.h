//
//  AppDelegate.h
//  fbe
//
//  Created by steve on 11/5/17.
//  Copyright © 2017 vsquared. All rights reserved.
//


#import <Cocoa/Cocoa.h>
#import "ToolbarDelegate.h"
#import "TextStorageDelegate.h"
#import "WindowDelegate.h"
#import "SplitViewDelegate.h"
#import "DocumentController.h"
//#import "TrackView.h"
#import "SharedVariables.h"
#import "PreferencesPanel.h"
#import "ProjectManagerPanel.h"
#import "TextDelegate.h"

@class PreferencesPanel;
@class ResourcesPanel;

@interface AppDelegate : NSObject <NSApplicationDelegate, NSTextViewDelegate>
{
    NSWindow *window;
    NSButton *clearBtn;
    NSFontManager *fontManager;
    NSTextFinder *txtFinder;
    TextStorageDelegate *txtStorageDelegate;
    WindowDelegate *windowDelegate;
    TextDelegate *txtDelegate;
    SplitViewDelegate *splitDelegate;
    ToolbarDelegate *toolbarDelegate;
    NSMenu *mFont;
    DocumentController *docController;
    NSScrollView *txtScrlView;
    NSScrollView *lineNumScrlView;
    NSTextView *lineNumView;
    NSPanel *rsrcPanel;
    NSTextView *rsrcTextView;
    NSTextField *appFld;
    NSTextView *txtView;
    PreferencesPanel *preferencesPanel;
    ProjectManagerPanel *projMgrPanel;
    NSDictionary *prefs;
    
    float red;
    float green;
    float blue;
    float white;
    float alpha;
    NSColor *bkgrndArchivedColor;
    NSColor *txtArchivedColor;
    
}

- (void) buildMenu;
- (void) buildWindow;
- (void) calcLineNumbers;
- (void) runAction;
- (void) buildAction;
- (void) analyzeAction;
@end
