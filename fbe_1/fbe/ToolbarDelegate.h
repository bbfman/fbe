//
//  ToolbarDelegate.h
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>
#import "SharedVariables.h"

@interface ToolbarDelegate : NSObject <NSToolbarDelegate>
{
    NSPopUpButton *tmplPopUp;
    NSPopUpButton *ctrlPopUp;
    NSButton *quitBtn;
    NSPopUpButton *runTypePopUp;
    NSTextField *sourceFld;
    NSString *currFileName;
     NSString *rsrcPath;
    // NSString *prefsPlistPath;
    NSTextView *txtView;
    NSTextView *lineNumView;
    NSWindow *window;
    NSDictionary *prefs;
    NSString *rsrcFolderPath;
    
    float red;
    float green;
    float blue;
    float white;
    float alpha;
    NSColor *bkgrndArchivedColor;
    NSColor *txtArchivedColor;
}
@property(retain)NSTextView *txtView;
@property(retain)NSTextView *lineNumView;
@property(retain)NSWindow *window;

void NSLog(NSString *formatString, ... );

-(void)retrieveArchivedData;
-(void)clearTxtView;
-(void)autoInsertHandler:(id)sender;
-(void)showSavePanel;
-(void)showOpenPanel;
@end
