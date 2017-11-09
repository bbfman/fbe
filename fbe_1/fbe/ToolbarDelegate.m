//
//  ToolbarDelegate.m
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "ToolbarDelegate.h"


static NSString *RunToolbarItemIdentifier = @"RunToolbarItemIdentifier";
static NSString *OpenToolbarItemIdentifier = @"OpenToolbarItemIdentifier";
static NSString *SaveToolbarItemIdentifier = @"SaveToolbarItemIdentifier";
static NSString *SaveAsToolbarItemIdentifier = @"SaveAsToolbarItemIdentifier";
static NSString *RefreshToolbarItemIdentifier = @"RefreshToolbarItemIdentifier";
static NSString *InsertToolbarItemIdentifier = @"InsertToolbarItemIdentifier";
static NSString *TemplatesToolbarItemIdentifier = @"TemplatesToolbarItemIdentifier";
static NSString *ControlsToolbarItemIdentifier = @"ControlsToolbarItemIdentifier";
static NSString *SourceFileToolbarItemIdentifier = @"SourceFileToolbarItemIdentifier";
static NSString *ClearToolbarItemIdentifier = @"ClearToolbarItemIdentifier";
static NSString *QuitToolbarItemIdentifier = @"QuitToolbarItemIdentifier";

@implementation ToolbarDelegate

@synthesize window;
@synthesize txtView;
@synthesize lineNumView;

-(instancetype)init
{
    self = [super init];
    if (self){
     }
    return self;
}

- (void)runAction
{
   //Method is in AppDelegate; makes compiler happy
}

- (void)showSavePanel
{
    //Method is in AppDelegate; makes compiler happy
}

- (void)showOpenPanel
{
    //Method is in AppDelegate; makes compiler happy
}

- (void) dealloc
{
    [window release];
    [txtView release];
    [lineNumView release];
    //  [prefsPlistPath release];
    [self release];
    [super dealloc];
}

- (NSArray *) toolbarAllowedItemIdentifiers: (NSToolbar *) toolbar
{
    return [NSArray arrayWithObjects:
            OpenToolbarItemIdentifier,
            RunToolbarItemIdentifier,
            SaveToolbarItemIdentifier,
            SaveAsToolbarItemIdentifier,
            RefreshToolbarItemIdentifier,
            InsertToolbarItemIdentifier,
            TemplatesToolbarItemIdentifier,
            ControlsToolbarItemIdentifier,
            //NSToolbarPrintItemIdentifier,
            //NSToolbarShowFontsItemIdentifier,
            //NSToolbarCustomizeToolbarItemIdentifier,
            //NSToolbarFlexibleSpaceItemIdentifier,
            //NSToolbarSeparatorItemIdentifier,
            SourceFileToolbarItemIdentifier,
            ClearToolbarItemIdentifier,
            QuitToolbarItemIdentifier, nil];
}

- (NSArray *) toolbarDefaultItemIdentifiers: (NSToolbar *) toolbar
{
    return [NSArray arrayWithObjects:
            OpenToolbarItemIdentifier,
            RunToolbarItemIdentifier,
            SaveToolbarItemIdentifier,
            SaveAsToolbarItemIdentifier,
            //NSToolbarPrintItemIdentifier,
            //NSToolbarSeparatorItemIdentifier,
            //NSToolbarShowFontsItemIdentifier,
            RefreshToolbarItemIdentifier,
            InsertToolbarItemIdentifier,
            TemplatesToolbarItemIdentifier,
            ControlsToolbarItemIdentifier,
            //NSToolbarFlexibleSpaceItemIdentifier,
            SourceFileToolbarItemIdentifier,
            ClearToolbarItemIdentifier,
            QuitToolbarItemIdentifier, nil];
}

- (NSToolbarItem *) toolbar:(NSToolbar *)toolbar  itemForItemIdentifier:(NSString *)itemIdentifier  willBeInsertedIntoToolbar:(BOOL)flag
{
    NSToolbarItem *toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier: itemIdentifier] autorelease];
    
    if ([itemIdentifier isEqual: OpenToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"Open"];
        [toolbarItem setToolTip:@"Open file"];
        [toolbarItem setImage:[NSImage imageNamed:@"open.png"]];
        [toolbarItem setTarget:[NSApp delegate]];
        [toolbarItem setAction:@selector(showOpenPanel)];
    }
    else if ([itemIdentifier isEqual: RunToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"Run"];
        [toolbarItem setToolTip:@"Build and run"];
        [toolbarItem setImage:[NSImage imageNamed:@"run.png"]];
        //      [toolbarItem setKeyEquivalent:@"r"];
        [toolbarItem setTarget:[NSApp delegate]];
        [toolbarItem setAction:@selector(runAction)];
    }
    else if ([itemIdentifier isEqual: SaveToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"Save"];
        [toolbarItem setToolTip:@"Save file"];
        [toolbarItem setImage:[NSImage imageNamed:@"save.png"]];
        [toolbarItem setTarget:self];
        [toolbarItem setAction:@selector(save)];
    }
    else if ([itemIdentifier isEqual: SaveAsToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"SaveAs"];
        [toolbarItem setToolTip:@"Save file"];
        [toolbarItem setImage:[NSImage imageNamed:@"save.png"]];
        [toolbarItem setTarget:[NSApp delegate]];
        [toolbarItem setAction:@selector(showSavePanel)];
    }

    else if ([itemIdentifier isEqual: RefreshToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"Refresh"];
        [toolbarItem setToolTip:@"Refresh Source Code"];
        [toolbarItem setImage:[NSImage imageNamed:@"refresh.png"]];
        [toolbarItem setTarget:self];
        [toolbarItem setAction:@selector(refresh)];
    }
    else if ([itemIdentifier isEqual: InsertToolbarItemIdentifier])
    {
        NSArray *menuItems = [NSArray arrayWithObjects: @"Insert", @"NSLog",@"Class", @"Import AppKit",@"Include NSLog.h",@"Include Tlbx CocoaUI.incl",@"Curly Braces", nil];
        NSPopUpButton *autoInsert = [[NSPopUpButton alloc] initWithFrame:NSMakeRect( 290, 30, 60, 30 )];
        [autoInsert setPullsDown:YES];
        [autoInsert setBordered:NO];
        [autoInsert addItemsWithTitles:menuItems];
        [toolbarItem setView:autoInsert];
        [toolbarItem setTarget:self];
        [toolbarItem setAction:@selector(autoInsertHandler:)];
        [toolbarItem setToolTip:@"Auto insert"];
        [autoInsert release];
    }
    else if ([itemIdentifier isEqual: TemplatesToolbarItemIdentifier])
    {
        tmplPopUp = [[NSPopUpButton alloc] initWithFrame:NSMakeRect( 90, 46, 125, 30 )];
        [tmplPopUp setFont: [NSFont fontWithName:@"Lucida Grande" size:12.0]];
        [tmplPopUp setPullsDown:YES];
        rsrcFolderPath = [[NSBundle mainBundle] resourcePath];
        NSString *tmplFolderPath = [rsrcFolderPath stringByAppendingString:@"/Templates"];
        NSError *error;
        NSArray *tmplContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath: tmplFolderPath error: &error ];
        [tmplPopUp addItemsWithTitles: tmplContents];
        [[tmplPopUp cell] setTitle:@"Templates"];
        [toolbarItem setView:tmplPopUp];
        [tmplPopUp setTarget:self];
        [tmplPopUp setAction:@selector(myTemplateAction)];
        [tmplPopUp release];

    }    
    else if ([itemIdentifier isEqual: ControlsToolbarItemIdentifier])
    {
        ctrlPopUp = [[NSPopUpButton alloc] initWithFrame:NSMakeRect( 210, 46, 100, 30 )];
        [ctrlPopUp setFont: [NSFont fontWithName:@"Lucida Grande" size:12.0]];
        [ctrlPopUp setPullsDown:YES];
        NSString *ctrlFolder = [rsrcFolderPath stringByAppendingString:@"/Controls"];
        NSError *err2;
        NSArray *ctrlContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath: ctrlFolder error: &err2 ];
        [ctrlPopUp addItemsWithTitles: ctrlContents];
        [toolbarItem setView:ctrlPopUp];
        [ctrlPopUp setTarget:self];
        [ctrlPopUp setAction:@selector(myControlsAction)];
        
        [ctrlPopUp release];
    }
    else if ([itemIdentifier isEqual: ClearToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"Clear"];
        [toolbarItem setImage:[NSImage imageNamed:@"clear.png"]];
        [toolbarItem setTarget:self];
        [toolbarItem setAction:@selector(clearTxtView)];
    }
    else if ([itemIdentifier isEqual: QuitToolbarItemIdentifier])
    {
        [toolbarItem setLabel:@"Quit"];
        [toolbarItem setImage:[NSImage imageNamed:@"quit.png"]];
        [toolbarItem setTarget:self];
        [toolbarItem setAction:@selector(safeToQuit)];
        // [toolbarItem setAction:@selector(terminate:)];
    }
    else
    {
        // itemIdentifier referred to a toolbar item that is not supported
        // Return nil to inform toolbar that this kind of item is not supported
        toolbarItem = nil;
    }
    return toolbarItem;
}

- (void) retrieveArchivedData
{
    BOOL isDir;
    NSError *err;
    NSFileManager *fm = [[NSFileManager alloc] init];
    // ****** Check to see if an archive file already exists ****** //
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *applicationSupportDirectory = [paths firstObject];
    NSString *prefsPath = [applicationSupportDirectory stringByAppendingPathComponent:@"FBE"];
    
    BOOL FBEDirExists = [fm fileExistsAtPath:prefsPath isDirectory:&isDir];
    if (!FBEDirExists) {
        BOOL fbeDirCreated = [fm createDirectoryAtPath:prefsPath withIntermediateDirectories:NO attributes:nil error:&err];
        NSLog(@"created = %d",fbeDirCreated);
    }
    
    // ****** Create prefs.plist if one doesn't already exist ****** //
    NSString *prefsPlistPath = [[prefsPath stringByAppendingPathComponent: @"FBE_prefs.plist"]retain];
    BOOL plistExists = [fm fileExistsAtPath:prefsPlistPath];
    if ( plistExists > 0 )
    {
        prefs = [[[NSDictionary alloc] initWithContentsOfFile:prefsPlistPath]retain];
        // NSLog(@"baseFont = %@", [prefs objectForKey:@"baseFont"]);
        
    } else {  // **** If no archive plist file exists, create one using Defaults  **** //
        // prefs = [NSMutableDictionary dictionary];
        NSMutableDictionary *background = [NSMutableDictionary dictionary];
        NSMutableDictionary *text = [NSMutableDictionary dictionary];
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

- (void) refresh
{
    [self retrieveArchivedData];
    [window setDocumentEdited:YES];
    NSNumber *conversionFactor = [[prefs objectForKey:@"txt"] objectForKey:@"fontSize"];
    CGFloat fontSize = [conversionFactor floatValue];
    [lineNumView setFont:[NSFont fontWithName:[[prefs objectForKey:@"txt"] objectForKey:@"baseFont"] size: fontSize]];
    [txtView setFont:[NSFont fontWithName:[[prefs objectForKey:@"txt"] objectForKey:@"baseFont"] size: fontSize]];
    
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
    
    [txtView setBackgroundColor:bkgrndArchivedColor];
    [txtView setTextColor:txtArchivedColor];
    
}

- (void) myTemplateAction
{
    NSInteger selected = [tmplPopUp indexOfSelectedItem];
    NSString *selectedObject = [tmplPopUp itemTitleAtIndex:selected];
    [sourceFld setStringValue:@""];
    NSString *rsrcFolder = [[NSBundle mainBundle] resourcePath];
    NSString *tmplFolder = [rsrcFolder stringByAppendingString:@"/Templates"];
    NSString *tmplStr = [tmplFolder stringByAppendingPathComponent:selectedObject];
    NSString *fileStr = [[NSString alloc] initWithContentsOfFile:tmplStr encoding:NSUTF8StringEncoding error:NULL];
    [txtView setString:fileStr];
    [fileStr release];
}

- (void) myControlsAction
{
    NSInteger selectedItem = [ctrlPopUp indexOfSelectedItem];
    NSString *selectedObject = [ctrlPopUp itemTitleAtIndex:selectedItem];
    NSLog(@"sel = %@",selectedObject);
    [sourceFld setStringValue:@""];
    NSString *rsrcFolder = [[NSBundle mainBundle] resourcePath];
    NSString *ctrlFolder = [rsrcFolder stringByAppendingString:@"/Controls"];
    NSString *ctrlStr = [ctrlFolder stringByAppendingPathComponent:selectedObject];
    NSString *fileStr = [[NSString alloc] initWithContentsOfFile:ctrlStr encoding:NSUTF8StringEncoding error:NULL];
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:fileStr replacementRange:NSMakeRange(insertionPoint,0)];
    [fileStr release];
}

//_original handles line wraps
- (void) calcLineNumbers
{
    NSMutableString *lineNumString = [[NSMutableString alloc] initWithCapacity:0];
    NSLayoutManager *layoutManager = [txtView layoutManager];
    NSUInteger numberOfLines, index, numberOfGlyphs = [layoutManager numberOfGlyphs];
    NSRange lineRange;
    for (numberOfLines = 0, index = 0; index < numberOfGlyphs; numberOfLines++)
    {
        (void) [layoutManager lineFragmentRectForGlyphAtIndex:index effectiveRange: &lineRange];
        index = NSMaxRange(lineRange);
        [lineNumString appendFormat:@"%lu\n", numberOfLines+1];
    }
    [lineNumView setString:lineNumString];
    [lineNumString release];
}

// Not currently used
- (void) autoSaveBeforeCompile {
    
    NSString *viewStr = [[txtView textStorage] string];
    currFileName = [sourceFld stringValue];
 //   compileOutput = [[currFileName stringByDeletingPathExtension]retain];
    SharedVariables *sharedManager = [SharedVariables sharedVariablesManager];
    sharedManager.currFileName = currFileName;
  //  sharedManager.compileOutput = compileOutput;
    NSError *err;
    BOOL fileSaved = [viewStr writeToFile: currFileName atomically:YES encoding:NSUTF8StringEncoding error:&err];
    if (!fileSaved) {
        NSLog(@"Unable to save file due to error: %@", err);
    } else {
        NSLog(@"File %@ was saved.", currFileName );
        [window setDocumentEdited:NO];
    //    AppDelegate *appDelegate = [(AppDelegate *)[NSApp delegate]];
     //    AppDelegate *appDelegate = [[NSApp] delegate];
   //     [[NSApp delegate] runAction:];
    //    [(AppDelegate *)[[NSApplication sharedApplication] delegate] runAction:]
      //  [appDelegate runAction:];
       // [toolbarItem setTarget:[NSApp delegate]];
        //   [toolbarItem setAction:@selector(runAction:)];

       }
}

- (void) clearTxtView
{
    [txtView setString:@""];
    [sourceFld setStringValue:@""];
    currFileName = @"";
}

- (void) safeToQuit
{
    if ( [window isDocumentEdited] ) {
        [self saveAlert];
    } else {
        [NSApp terminate:self];}
}

- (void) saveAlert
{
    NSAlert *alert = [[NSAlert alloc] init] ;
    [alert addButtonWithTitle:@"Save changes..."];
    [alert addButtonWithTitle:@"Cancel"];
    [alert addButtonWithTitle:@"Quit"];
    [alert setMessageText:@"You have unsaved changes. Do you wish to save them?"];
    [alert setInformativeText:@"If you quit without saving, changes will be lost."];
    [alert setAlertStyle: NSAlertStyleWarning];
    NSUInteger result = [alert runModal];
    switch ( result )
    {
        case 1000: // Save button
            // Show save panel here
            [self save];
            [window setDocumentEdited:NO];
            [NSApp replyToApplicationShouldTerminate:YES];
            break;
            
        case 1001: // Cancel button
            [NSApp replyToApplicationShouldTerminate:NO];
            break;
            
        case 1002: // Quit button
            [window setDocumentEdited:NO];
            [NSApp replyToApplicationShouldTerminate:YES];
            [NSApp terminate:self];
            break;
    }
    [alert release];
}

- (void)alertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    switch ( returnCode )
    {
        case NSAlertFirstButtonReturn: // Save button
            // Show save panel here
            [self save];
            [window setDocumentEdited:NO];
            [NSApp replyToApplicationShouldTerminate:YES];
            break;
            
        case NSAlertSecondButtonReturn: // Cancel button
            [NSApp replyToApplicationShouldTerminate:NO];
            break;
            
        case NSAlertThirdButtonReturn: // Quit button
            [window setDocumentEdited:NO];
            [NSApp replyToApplicationShouldTerminate:YES];
            [NSApp terminate:self];
            break;
            
        default:
            NSAssert1( NO, @"Unknown button return: %ld", (long)returnCode );
            break;
    }
}

- (void) save {
    NSURL *url = [window representedURL];
    if ( url ) {
        currFileName = [url path];
    } else {
        NSLog(@"Unable to find file name.");
    }
    NSString *viewStr = [[txtView textStorage] string];
    NSError *err;
    BOOL fileSaved = [viewStr writeToFile: currFileName atomically:YES encoding:NSUTF8StringEncoding error:&err];
    NSNumber *creatorCode = [NSNumber numberWithUnsignedLong:'FB^e'];
    NSNumber *typeCode = [NSNumber numberWithUnsignedLong:'TEXT'];
    NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:creatorCode, NSFileHFSCreatorCode, typeCode, NSFileHFSTypeCode, nil];
    [[NSFileManager defaultManager] setAttributes:attributes ofItemAtPath:currFileName error:nil];

    if (!fileSaved) {
        NSLog(@"Unable to save file due to error: %@", err);
    } else {
        NSLog(@"File %@ was saved.", currFileName );
        [window setDocumentEdited:NO];
    }
}


- (void) autoInsertHandler:(id)sender
{
    NSInteger selectedItem = [sender indexOfSelectedItem];
    switch (selectedItem) {
        case 1: [self insertNSLog];
            break;
        case 2: [self insertClass];
            break;
        case 3: [self insertAppKit];
            break;
        case 4: [self includeNSLog];
            break;
        case 5: [self includeCocoaUI];
            break;
        case 6: [self curlyProcedure];
            break;
    }
}

- (void) insertNSLog
{
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"NSLog (@\"\");\n" replacementRange:NSMakeRange(insertionPoint,0)];
}

- (void) insertClass
{
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"@interface\n@end\n\n@implementation\n@end\n" replacementRange:NSMakeRange(insertionPoint,0)];
}

- (void) insertClassPlusPlus
{
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"class  {\rpublic:\r\rprivate:\r};\n" replacementRange:NSMakeRange(insertionPoint,0)];
}

- (void) insertAppKit
{
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"#import <AppKit/AppKit.h>\n"  replacementRange:NSMakeRange(insertionPoint,0)];
}

- (void) includeNSLog
{
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"include \"NSLog.incl\"\n"  replacementRange:NSMakeRange(insertionPoint,0)];
}

- (void) includeCocoaUI
{
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"include \"Tlbx CocoaUI.incl\"\n"  replacementRange:NSMakeRange(insertionPoint,0)];
}

- (void) curlyProcedure {
    NSInteger insertionPoint = [[[txtView selectedRanges] objectAtIndex:0] rangeValue].location;
    [txtView insertText:@"{\n\n}" replacementRange:NSMakeRange(insertionPoint,0)];
}

@end
