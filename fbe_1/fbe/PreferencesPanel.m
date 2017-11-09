//
//  PreferencesPanel.m
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "PreferencesPanel.h"

@implementation PreferencesPanel
@synthesize txtView;
@synthesize lineNumView;
@synthesize window;

-(void)dealloc {
    
    [txtView release];
    [lineNumView release];
    [window release];
    [super dealloc];
}

- (void) retrieveArchivedData
{
    BOOL isDir;
    NSError *err;
    NSFileManager *fm = [[NSFileManager alloc] init];
    // ****** Check to see if an archive file already exists ****** //
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *applicationSupportDirectory = [paths firstObject];
    prefsPath = [applicationSupportDirectory stringByAppendingPathComponent:@"FBE"];
    
    BOOL FBEDirExists = [fm fileExistsAtPath:prefsPath isDirectory:&isDir];
    if (!FBEDirExists) {
        BOOL fbeDirCreated = [fm createDirectoryAtPath:prefsPath withIntermediateDirectories:NO attributes:nil error:&err];
        NSLog(@"created = %d",fbeDirCreated);
    }
    
    // ****** Create prefs.plist if one doesn't already exist ****** //
    prefsPlistPath = [[prefsPath stringByAppendingPathComponent: @"FBE_prefs.plist"]retain];
    BOOL plistExists = [fm fileExistsAtPath:prefsPlistPath];
    if ( plistExists > 0 )
    {
        prefs = [[[NSDictionary alloc] initWithContentsOfFile:prefsPlistPath]retain];
    } else {  // **** If no archive plist file exists, create one using Defaults  **** //
        // prefs = [NSMutableDictionary dictionary];
        background = [NSMutableDictionary dictionary];
        text = [NSMutableDictionary dictionary];
        prefs = [[NSDictionary dictionaryWithObjectsAndKeys: background,@"bkgrnd",text,@"txt",nil]retain];
        [[prefs objectForKey:@"bkgrnd"] setObject:@"NSCalibratedRGBColorSpace" forKey:@"bkgrndColorSpaceType"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:1.0] forKey:@"bkgrndRed"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:0.0] forKey:@"bkgrndGreen"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:0.0] forKey:@"bkgrndBlue"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:1.0] forKey:@"bkgrndAlpha"];
        
        [[prefs objectForKey:@"txt"] setObject: @"Menlo" forKey:@"baseFont"];
        [[prefs objectForKey:@"txt"] setObject: @"12" forKey:@"fontSize"];
        [[prefs objectForKey:@"txt"] setObject:@"NSCalibratedRGBColorSpace" forKey:@"txtColorSpaceType"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:0.0] forKey:@"txtRed"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:0.0] forKey:@"txtGreen"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:0.0] forKey:@"txtBlue"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:1.0] forKey:@"txtAlpha"];
        
        BOOL prefsWritten = [prefs writeToFile:prefsPlistPath atomically:YES];
        if (!prefsWritten) { NSLog(@"Unable to create prefs plist."); }
    }
    NSNumber *conversionFactor = [prefs objectForKey:@"fontSize"];
    CGFloat fontSize = [conversionFactor floatValue];
    [txtView setFont:[NSFont fontWithName:[[prefs objectForKey:@"txt"] objectForKey:@"baseFont"] size: fontSize]];
    [lineNumView setFont:[NSFont fontWithName:[[prefs objectForKey:@"txt"] objectForKey:@"baseFont"] size: fontSize]];
    [fm release];
}

- (void) archivePrefsData
{
    NSString *bkgrndColorSpace = [[bkgrndColorWell color]colorSpaceName];
    [[prefs objectForKey:@"bkgrnd"] setObject:bkgrndColorSpace forKey:@"bkgrndColorSpaceType"];
    if([bkgrndColorSpace isEqualToString:@"NSCalibratedRGBColorSpace"] || [bkgrndColorSpace isEqualToString:@"NSCustomColorSpace"]) {
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:[[bkgrndColorWell color] redComponent]] forKey:@"bkgrndRed"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:[[bkgrndColorWell color] greenComponent]] forKey:@"bkgrndGreen"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:[[bkgrndColorWell color] blueComponent]] forKey:@"bkgrndBlue"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:[[bkgrndColorWell color] alphaComponent]] forKey:@"bkgrndAlpha"];
    }
    if([bkgrndColorSpace isEqualToString:@"NSCalibratedWhiteColorSpace"]) {
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:[[bkgrndColorWell color] whiteComponent]] forKey:@"bkgrndWhite"];
        [[prefs objectForKey:@"bkgrnd"] setObject: [NSNumber numberWithFloat:[[bkgrndColorWell color] alphaComponent]] forKey:@"bkgrndAlpha"];
    }
    
    [[prefs objectForKey:@"txt"] setObject: [fontDisplayFld stringValue] forKey:@"baseFont"];
    [[prefs objectForKey:@"txt"] setObject: [sizeDisplayFld stringValue] forKey:@"fontSize"];
    
    NSString *txtColorSpace = [[txtColorWell color]colorSpaceName];
    [[prefs objectForKey:@"text"] setObject:txtColorSpace forKey:@"txtColorSpaceType"];
    if([txtColorSpace isEqualToString:@"NSCalibratedRGBColorSpace"] || [txtColorSpace isEqualToString:@"NSCustomColorSpace"]) {
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:[[txtColorWell color] redComponent]] forKey:@"txtRed"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:[[txtColorWell color] greenComponent]] forKey:@"txtGreen"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:[[txtColorWell color] blueComponent]] forKey:@"txtBlue"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:[[txtColorWell color] alphaComponent]] forKey:@"txtAlpha"];
    }
    
    if([txtColorSpace isEqualToString:@"NSCalibratedWhiteColorSpace"]) {
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:[[txtColorWell color] whiteComponent]] forKey:@"txtWhite"];
        [[prefs objectForKey:@"txt"] setObject: [NSNumber numberWithFloat:[[txtColorWell color] alphaComponent]] forKey:@"txtAlpha"];
    }
    
    BOOL written = [prefs writeToFile:prefsPlistPath atomically:YES];
    if (!written) { NSLog(@"Unable to write prefs to file.");}
}


- (void) buildPrefsPanel
{
#define _prefsPanelW  480
#define _prefsPanelH  350
    
    prefsPanel = [[NSPanel alloc] initWithContentRect:NSMakeRect( 30, 750, _prefsPanelW, _prefsPanelH )
                                            styleMask:NSWindowStyleMaskUtilityWindow | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                              backing:NSBackingStoreBuffered defer:NO];
    [prefsPanel setTitle:@"Preferences"];
    
    // ***** Background Color Well Label ****** //
    NSTextField *bkgrndLabel = [[NSTextField alloc] initWithFrame:NSMakeRect( 20, _prefsPanelH - 50, 135, 24 )];
    [[prefsPanel contentView] addSubview:bkgrndLabel];
    [bkgrndLabel setBackgroundColor: [NSColor  clearColor]];
    [bkgrndLabel setSelectable:NO];
    [bkgrndLabel setBordered:NO];
    [bkgrndLabel setFont:[NSFont fontWithName: @"Menlo" size:12]];
    [bkgrndLabel setStringValue:@"Background color:"];
    [bkgrndLabel release];
    
    // ****** Background ColorWell ****** //
    bkgrndColorWell = [[NSColorWell alloc] initWithFrame:NSMakeRect( 160, _prefsPanelH - 50, 70, 30 )];
    [[prefsPanel contentView] addSubview:bkgrndColorWell];
    
    // ***** Text Color Well Label ****** //
    NSTextField *textLabel = [[NSTextField alloc] initWithFrame:NSMakeRect( 250, _prefsPanelH - 50, 95, 24 )];
    [[prefsPanel contentView] addSubview:textLabel];
    [textLabel setBackgroundColor: [NSColor  clearColor]];
    [textLabel setSelectable:NO];
    [textLabel setBordered:NO];
    [textLabel setFont:[NSFont fontWithName: @"Menlo" size:12]];
    [textLabel setStringValue:@"Text color:"];
    [textLabel release];
    
    // ***** Text ColorWell ***** //
    txtColorWell = [[NSColorWell alloc] initWithFrame:NSMakeRect( 350, _prefsPanelH - 50, 70, 30 )];
    [[prefsPanel contentView] addSubview:txtColorWell];
    
    NSString *bkgrndColorSpace = [[prefs objectForKey:@"bkgrnd"] objectForKey: @"bkgrndColorSpaceType"];
    
    if ([bkgrndColorSpace isEqualToString: @"NSCalibratedRGBColorSpace"] || [bkgrndColorSpace isEqualToString:@"NSCustomColorSpace"]) {
        red = [[[prefs objectForKey:@"bkgrnd"] objectForKey:@"bkgrndRed"]floatValue];
        green = [[[prefs objectForKey:@"bkgrnd"] objectForKey:@"bkgrndGreen"]floatValue];
        blue = [[[prefs objectForKey:@"bkgrnd"] objectForKey:@"bkgrndBlue"]floatValue];
        alpha = [[[prefs objectForKey:@"bkgrnd"] objectForKey:@"bkgrndAlpha"]floatValue];
        bkgrndArchivedColor = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:alpha ];
    }
    
    if ([bkgrndColorSpace isEqualToString: @"NSCalibratedWhiteColorSpace"]) {
        white = [[[prefs objectForKey:@"bkgrnd"] objectForKey:@"bkgrndWhite"]floatValue];
        alpha = [[[prefs objectForKey:@"bkgrnd"] objectForKey:@"bkgrndAlpha"]floatValue];
        bkgrndArchivedColor = [NSColor colorWithCalibratedWhite:white alpha:alpha];
    }
    
    NSString *txtColorSpace = [[prefs objectForKey:@"txt"] objectForKey: @"txtColorSpaceType"];
    
    if ([txtColorSpace isEqualToString: @"NSCalibratedRGBColorSpace"] || [txtColorSpace isEqualToString:@"NSCustomColorSpace"]) {
        red = [[[prefs objectForKey:@"txt"] objectForKey:@"txtRed"]floatValue];
        green = [[[prefs objectForKey:@"txt"] objectForKey:@"txtGreen"]floatValue];
        blue = [[[prefs objectForKey:@"txt"] objectForKey:@"txtBlue"]floatValue];
        alpha = [[[prefs objectForKey:@"txt"] objectForKey:@"txtAlpha"]floatValue];
        txtArchivedColor = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:alpha ];
    }
    
    [bkgrndColorWell setColor:bkgrndArchivedColor];
    [txtView setBackgroundColor:bkgrndArchivedColor];
    [txtView setTextColor:txtArchivedColor];
    [bkgrndColorWell release];
    
    [txtColorWell setColor:txtArchivedColor];
    [txtColorWell release];
    
    // ***** Base Font Label ****** //
    NSTextField *fontLabel = [[NSTextField alloc] initWithFrame:NSMakeRect( 20, _prefsPanelH - 150, 135, 24 )];
    [[prefsPanel contentView] addSubview:fontLabel];
    [fontLabel setBackgroundColor: [NSColor  clearColor]];
    [fontLabel setSelectable:NO];
    [fontLabel setBordered:NO];
    [fontLabel setFont:[NSFont fontWithName: @"Menlo" size:12]];
    [fontLabel setStringValue:@"Current font/size:"];
    [fontLabel release];
    
    // **** Font Display Field ***** //
    fontDisplayFld = [[NSTextField alloc]initWithFrame:NSMakeRect(160, _prefsPanelH - 150, 220, 24)];
    [[prefsPanel contentView] addSubview:fontDisplayFld];
    [fontDisplayFld setSelectable:NO];
    [fontDisplayFld setStringValue: [NSString stringWithFormat:@"%@",[[prefs objectForKey:@"txt"] objectForKey:@"baseFont"]]];
    [fontDisplayFld release];
    
    // **** Size Display Field ***** //
    sizeDisplayFld = [[NSTextField alloc]initWithFrame:NSMakeRect(390, _prefsPanelH - 150, 50, 24)];
    [[prefsPanel contentView] addSubview:sizeDisplayFld];
    [sizeDisplayFld setSelectable:NO];
    [sizeDisplayFld setStringValue: [NSString stringWithFormat:@"%@",[[prefs objectForKey:@"txt"] objectForKey:@"fontSize"]]];
    [sizeDisplayFld release];
    
    // ****** Show Font Btn ***** //
    showFontBtn = [[NSButton alloc]initWithFrame:NSMakeRect( 30, _prefsPanelH - 210, 110, 30)];
    [showFontBtn setBezelStyle:NSRoundedBezelStyle ];
    [showFontBtn setTitle: @"Show Fonts"];
    [showFontBtn setTarget: self];
    [showFontBtn setAction: @selector (showFontPanel)];
    [[prefsPanel contentView] addSubview: showFontBtn];
    [showFontBtn release];
    
    // ****** Save to Archive Btn ***** //
    saveBtn = [[NSButton alloc]initWithFrame:NSMakeRect( 250, _prefsPanelH - 210, 120, 30)];
    [saveBtn setBezelStyle:NSRoundedBezelStyle ];
    [saveBtn setTitle: @"Save"];
    [saveBtn setTarget: self];
    [saveBtn setAction: @selector (archivePrefsData)];
    [[prefsPanel contentView] addSubview: saveBtn];
    [saveBtn release];
    
    // ***** Quit btn ***** //
    NSButton *quitBtn = [[NSButton alloc]initWithFrame:NSMakeRect( _prefsPanelW - 50, 5, 40, 40 )];
    [quitBtn setBezelStyle:NSCircularBezelStyle ];
    [quitBtn setTitle: @"Q" ];
    [quitBtn setTarget: self];
    [quitBtn setAction:@selector(hidePrefsPanel)];
    [[prefsPanel contentView] addSubview: quitBtn];
    [quitBtn release];
    
}

- (void)showFontPanel
{
    if ([[showFontBtn title] isEqualToString:@"Show Fonts"]) {
        fontManager = [NSFontManager sharedFontManager];
        [fontManager orderFrontFontPanel:self];
        [fontManager setTarget:self];
        [fontManager setAction:@selector(changeFont:)];
        [window makeFirstResponder:txtView];
        // [txtView setSelectedRange:NSMakeRange(0, [txtView textStorage].length)];
        [txtView selectAll:txtView];
        [window makeKeyAndOrderFront: nil];
        [showFontBtn setTitle:@"Hide Fonts"];
    } else {
        [[fontManager fontPanel:NO] orderOut: self];
        [showFontBtn setTitle:@"Show Fonts"];
    }
}

// ***** Automatically called by fontPanel ****** //
- (void)changeFont:(id)sender
{
    NSLog(@"-changeFont: called.");
    NSFont *oldFont = [txtView font];
    NSFont *newFont = [sender convertFont:oldFont];
    [txtView setFont:newFont];
    [lineNumView setFont:newFont];
    [fontDisplayFld setStringValue:[NSString stringWithFormat:@"%@", [newFont fontName]]];
    [sizeDisplayFld setStringValue:[NSString stringWithFormat:@"%.0f", [newFont pointSize]]];
    return;
}

- (NSUInteger)validModesForFontPanel:(NSFontPanel *)fontPanel
{
    return NSFontPanelAllModesMask;
}

- (void) showPrefsPanel
{
    [prefsPanel makeKeyAndOrderFront:self];
    
}

- (void) hidePrefsPanel
{
    [prefsPanel orderOut:self];
}

@end

/*
 NSTextStorage *textStorage = [notification object];
 NSString *text = [textStorage string];
 NSRange range = NSMakeRange(0, text.length);
 */

