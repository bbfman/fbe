//
//  PreferencesPanel.h
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PreferencesPanel : NSPanel
{
    NSTextField *fontDisplayFld;
    NSPanel *prefsPanel;
    NSTextView *txtView;
    NSButton *showFontBtn;
    NSButton *saveBtn;
    NSFontManager *fontManager;
    NSTextField *sizeDisplayFld;
    NSTextView *lineNumView;
    NSDictionary *prefs;
    NSMutableDictionary *background;
    NSMutableDictionary *text;
    NSString *prefsPath;
    NSString *prefsPlistPath;
    NSWindow *window;
    NSColorWell *bkgrndColorWell;
    NSColorWell *txtColorWell;
    
    float red;
    float green;
    float blue;
    float white;
    float alpha;
    NSColor *bkgrndArchivedColor;
    NSColor *txtArchivedColor;
}
@property (retain) NSTextView *txtView;
@property (retain) NSTextView *lineNumView;
@property (retain) NSWindow *window;

- (void) buildPrefsPanel;
- (void) showPrefsPanel;
- (void) hidePrefsPanel;
- (void) retrieveArchivedData;

@end
