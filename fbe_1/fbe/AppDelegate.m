//
//  AppDelegate.m
//  fbe
//
//  Created by steve on 11/5/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "AppDelegate.h"

BOOL CompilerIsRunning()
{
    NSArray *runningApps = [[NSWorkspace sharedWorkspace] runningApplications];
    for ( NSRunningApplication *app in runningApps ) {
        if ( [[app bundleIdentifier] isEqualToString:@"com.FBtoC"] ) {
            return YES;
        }
    }
    return NO;
}

void CompilerPostNotification( NSString *name, NSString *object, NSDictionary *userInfo )
{
    if ( CompilerIsRunning() ) {
        CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter(), (CFStringRef)name, (CFStringRef)object, (CFDictionaryRef)userInfo, true );
    } else {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:0];
        [array addObject:object];
        NSString *filePath = [userInfo objectForKey:@"FilePath"];
        if ( filePath ) [array addObject:filePath];
        NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:array,NSWorkspaceLaunchConfigurationArguments,nil];
        // find FBtoC in our bundle resource
        NSURL *fbtocURL = [[NSBundle mainBundle] URLForResource:@"FBtoC" withExtension:@"app" subdirectory:@"FBtoC"];
        
        [[NSWorkspace sharedWorkspace] launchApplicationAtURL:fbtocURL options:NSWorkspaceLaunchDefault configuration:dict error:nil];
    }
}

void CompilerNotificationObserver( CFNotificationCenterRef center, void *observer, CFStringRef name, void *object, CFDictionaryRef userInfo )
{
    // NSLog(@"%@",userInfo);// do something with userInfo
}

@implementation AppDelegate

- (void) dealloc
{
    [window release];
    [splitDelegate release];
    [super dealloc];
}


- (void) showProjMgrPanel
{
    [projMgrPanel showProjMgrPanel];
}


- (void) showPreferencesPanel
{
    [preferencesPanel showPrefsPanel];
}

- (void) autoSaveBeforeCompile {
    
    NSString *viewStr = [[txtView textStorage] string];
    NSURL *url = [window representedURL];
    if ( url ) {
        NSString *currFileName = [url path];
        SharedVariables *sharedManager = [SharedVariables sharedVariablesManager];
        sharedManager.currFileName = currFileName;
        NSError *err;
        BOOL fileSaved = [viewStr writeToFile: currFileName atomically:YES encoding:NSUTF8StringEncoding error:&err];
        if (!fileSaved) {
            NSLog(@"Unable to save file due to error: %@", err);
        } else {
            NSLog(@"File %@ was saved.", currFileName );
            [window setDocumentEdited:NO];
            [self runAction];
        }
    } else {
        [self showSavePanel];
    }
}


- (void)runAction {
    NSURL *url = [window representedURL];
    if ( url ) {
        if ( [[NSFileManager defaultManager] fileExistsAtPath:[url path]] ) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:[url path] forKey:@"FilePath"];
            CompilerPostNotification( @"EditorNotification", @"Run", userInfo );
        }
    }
}

- (void)buildAction {
    NSURL *url = [window representedURL];
    if ( url ) {
        if ( [[NSFileManager defaultManager] fileExistsAtPath:[url path]] ) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:[url path] forKey:@"FilePath"];
            CompilerPostNotification( @"EditorNotification", @"Build", userInfo );
        }
    }
}

- (void)analyzeAction {
    NSURL *url = [window representedURL];
    if ( url ) {
        if ( [[NSFileManager defaultManager] fileExistsAtPath:[url path]] ) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:[url path] forKey:@"FilePath"];
            CompilerPostNotification( @"EditorNotification", @"Analyze", userInfo );
        }
    }
}

- (void)showFBtoCPreferencesAction:(id)sender {
    CompilerPostNotification( @"EditorNotification", @"ShowSettings", nil );
}


- (void)printView
{
    NSPrintInfo *printInfo = [NSPrintInfo sharedPrintInfo];
    [printInfo setVerticallyCentered:NO];
    [printInfo setHorizontallyCentered:YES];
    [printInfo setTopMargin:60];
    [printInfo setBottomMargin:30];
    [printInfo setLeftMargin:45];
    [printInfo setRightMargin:45];
    //[printInfo setOrientation:NSPortraitOrientation];
    printInfo.orientation = NSPortraitOrientation;
    printInfo.scalingFactor = 0.80;
    
    NSPrintOperation *printOp;
    printOp = [NSPrintOperation printOperationWithView:txtView printInfo:printInfo];
    //[printOp setShowsPrintPanel:YES];
    //-- Run print operation ---
    [printOp runOperation];
}


- (void)aboutHandler
{
    NSString *creditStr =
    @"Creates 64-bit apps from FB source code.\n\n\t\tWritten by FB Team.\n";
    NSAttributedString *attribStr = [[[NSMutableAttributedString alloc] initWithString: creditStr ] autorelease];
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
                             attribStr,         @"Credits",
                             @"FutureBASIC Editor",     @"ApplicationName",
                             NULL,            @"ApplicationIcon",
                             @"",  @"Version",
                             @"0.0.1",           @"ApplicationVersion",
                             @"",  @"Copyright", nil];
    [[NSApplication sharedApplication] orderFrontStandardAboutPanelWithOptions:options];
}


- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename
{
    return YES;
}

- (void) showOpenPanel
{
    NSOpenPanel *op = [NSOpenPanel openPanel];
    [op setTitle:@"Open file."];
    [op setCanChooseFiles:YES];
    // [op setAllowedFileTypes:[NSArray arrayWithObjects:@"txt", @"bas", @"incl", @"main", @"glbl", @"c",@"m",@"h",@"plist",@" ", nil]];
    [op setAllowedFileTypes:[NSArray arrayWithObjects:@"m", @"c", @"txt", @"", @"h", @"bas",@"plist", nil]];
    
    NSInteger result = [op runModal];
    if (result == NSFileHandlingPanelOKButton) {
        NSURL *openURL = [[[op URLs] objectAtIndex:0] retain];
        NSLog(@"openURL = %@", openURL);
        NSString *currFileName = [[openURL path ] retain];
        SharedVariables *sharedManager = [SharedVariables sharedVariablesManager];
        sharedManager.currFileName = currFileName;
        NSLog(@"currFileName = %@", currFileName);
       BOOL opened = [self application:NSApp openFile:currFileName];
        // Restore this
        if (opened == YES) {
          [docController noteNewRecentDocumentURL:openURL];
        }
        
        NSError *err;
        NSString *fileStr = [[NSString alloc] initWithContentsOfURL:openURL encoding:NSASCIIStringEncoding error:&err];
        
        if (!fileStr) { NSLog(@"Unable to open file %@", err);
        } else {
            [txtView setString:fileStr];
            [fileStr release];
            [window setTitleWithRepresentedFilename:[openURL path]];
        }
        [self calcLineNumbers];
        [window setDocumentEdited:NO];
    }
}


-(void) showSavePanel
{
    NSSavePanel *sp = [NSSavePanel savePanel];
    [sp setTitle:@"Save contents to file"];
    [sp setAllowedFileTypes:[NSArray arrayWithObjects:@"",@"txt", nil]];
    [sp setCanSelectHiddenExtension:YES];
    [sp setExtensionHidden:NO];
    [sp setAllowsOtherFileTypes:YES];
    [sp setCanCreateDirectories:YES];
  
  //  if ([currFileName length]> 0) { [sp setNameFieldStringValue: [currFileName lastPathComponent]]; }
    NSInteger result = [sp runModal];
    if (result == NSFileHandlingPanelOKButton) {
        NSURL *url = [[sp URL]retain];
        // ****** Must be retained ***** //
        NSString *currFileName = [[url path]retain];
        NSLog(@"showSavePanel currFileName = %@", currFileName);
        SharedVariables *sharedManager = [SharedVariables sharedVariablesManager];
        sharedManager.currFileName = currFileName;
        [window setTitleWithRepresentedFilename:[url path]];
        NSText* editor = [sp fieldEditor:NO forObject:nil];
        if (editor)  {
            NSString *viewStr = [[txtView textStorage] string];
            NSError *err;
            BOOL fileSaved = [viewStr writeToURL:url atomically:YES encoding:NSUTF8StringEncoding error:&err];
            NSNumber *creatorCode = [NSNumber numberWithUnsignedLong:'FB^e'];
            NSNumber *typeCode = [NSNumber numberWithUnsignedLong:'TEXT'];
            NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:creatorCode, NSFileHFSCreatorCode, typeCode, NSFileHFSTypeCode, nil];
            [[NSFileManager defaultManager] setAttributes:attributes ofItemAtPath:[[sp URL] path] error:nil];

            if (!fileSaved) {NSLog(@"Unable to save file due to error: %@", err);}
        }
        [window setDocumentEdited:NO];
    }
}

//handles line wraps
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
        [lineNumString appendFormat:@"%ld\n", (long)numberOfLines+1];
    }
    [lineNumView setString:lineNumString];
    [lineNumString release];
}

- (void)scrollViewContentBoundsDidChange:(NSNotification *)notification
{
    NSPoint leftOrigin = [[lineNumScrlView contentView] bounds].origin;
    NSPoint rightOrigin = [[txtScrlView contentView] bounds].origin;
    if ( [notification object] == [txtScrlView contentView] ) {
        leftOrigin.y = rightOrigin.y;
        [[lineNumScrlView contentView] scrollToPoint:leftOrigin];
        [self calcLineNumbers];
    } else {
        rightOrigin.y = leftOrigin.y;
        [[txtScrlView contentView] scrollToPoint:rightOrigin];
        [txtScrlView reflectScrolledClipView:[txtScrlView contentView]];
    }
}

- (void) performTextFinderAction: (id) sender
{
    [txtFinder performAction:[sender tag]];
}


- (void) buildMenu
{
    // ******** Menu and menubar ********//
    NSMenu *menubar = [[NSMenu new] autorelease];
    NSMenuItem *menuBarItem = [[NSMenuItem new] autorelease];
    [menubar addItem:menuBarItem];
    [NSApp setMainMenu:menubar];
    
    // ******** App Menu ******* //
    NSMenu *mApp = [[NSMenu new] autorelease];
    NSMenuItem *aboutMenuItem = [[[NSMenuItem alloc] initWithTitle:@"About..." action:@selector(aboutHandler) keyEquivalent:@""]autorelease];
    [mApp addItem:aboutMenuItem];
    [mApp addItem:[NSMenuItem separatorItem]];
    NSMenuItem *prefsMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Preferences..."
                                                            action:@selector(showPreferencesPanel) keyEquivalent:@""] autorelease];
    [mApp addItem:prefsMenuItem];
    [mApp addItem:[NSMenuItem separatorItem]];
    NSMenuItem *quitMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Quit"
                                                           action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
    [mApp addItem:quitMenuItem];
    [menuBarItem setSubmenu:mApp];

    // ******* File menu ********* //
    NSMenuItem *fileItem = [menubar addItemWithTitle:@"File" action:nil keyEquivalent:@""];
    [menubar addItemWithTitle:@"File" action:nil keyEquivalent:@""];
    NSMenu *mFile = [[NSMenu alloc] initWithTitle:@"File"];
    [menubar setSubmenu:mFile forItem:fileItem];
    NSMenuItem *openRecentItem = [mFile addItemWithTitle:@"Open Recent" action: nil keyEquivalent:@""];
    NSMenu *openRecentMenu = [[[NSMenu alloc] init] autorelease];
    [openRecentItem setSubmenu:openRecentMenu];
    [docController setOpenRecentMenu:openRecentMenu];
    [mFile addItem:[NSMenuItem separatorItem]];
    [mFile addItemWithTitle:@"Print..." action:@selector(printView) keyEquivalent:@"p"];

    // ******** Edit menu ******* //
    NSMenuItem *editItem = [menubar addItemWithTitle:@"Edit" action:nil keyEquivalent:@""];
    NSMenu *mEdit = [[NSMenu alloc] initWithTitle:@"Edit"];
    [menubar setSubmenu:mEdit forItem:editItem];
    [mEdit addItemWithTitle:@"Undo" action:@selector(undo) keyEquivalent:@"z"];
    NSMenuItem *redoItem = [mEdit addItemWithTitle:@"Redo" action:@selector(redo) keyEquivalent:@"Z"];
    [redoItem setKeyEquivalentModifierMask:NSEventModifierFlagShift | NSEventModifierFlagCommand];
    [mEdit addItem:[NSMenuItem separatorItem]];
    [mEdit addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
    [mEdit addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
    [mEdit addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];
    [mEdit addItemWithTitle:@"Delete" action:@selector(delete:) keyEquivalent:@""];
    [mEdit addItemWithTitle:@"Select All" action:@selector(selectAll:) keyEquivalent:@"a"];
    [mEdit addItem:[NSMenuItem separatorItem]];
    NSMenuItem *findItem = [mEdit addItemWithTitle:@"Find" action:@selector(performTextFinderAction:) keyEquivalent:@"f"];
    [findItem setTag: NSTextFinderActionShowFindInterface];
    
    // ******** Command Menu  ******* //
    NSMenuItem *commandMenuItem = [[[NSMenuItem alloc] init] autorelease];
    [menubar addItem:commandMenuItem];
    NSMenu *commandMenu = [[NSMenu alloc] initWithTitle:@"Command"];
    [commandMenuItem setEnabled:YES];
    [commandMenuItem setSubmenu:commandMenu];
    [commandMenu addItemWithTitle:@"Run" action:@selector(runAction) keyEquivalent:@"r"];
    [commandMenu addItemWithTitle:@"Build" action:@selector(buildAction) keyEquivalent:@"b"];
    [commandMenu addItemWithTitle:@"Analyze" action:@selector(analyzeAction) keyEquivalent:@"A"];

    // **** Project Manager menu **** //
    NSMenuItem* projMgrMenuItem = [menubar addItemWithTitle:@"" action:nil keyEquivalent:@""];
    NSMenu *projMgrMenu = [[[NSMenu alloc] initWithTitle:@"Project Manager"]autorelease];
    [menubar setSubmenu:projMgrMenu forItem: projMgrMenuItem];
    [projMgrMenu addItemWithTitle:@"Show Manager" action:@selector(showProjMgrPanel) keyEquivalent:@""];

}

- (void) initDelegates
{
    // ***** Delegates ****** //
    toolbarDelegate = [[ToolbarDelegate alloc]init];
    txtStorageDelegate = [[TextStorageDelegate alloc]init];
    windowDelegate = [[WindowDelegate alloc]init];
    splitDelegate = [[SplitViewDelegate alloc]init];
    txtDelegate = [[TextDelegate alloc]init];
    [docController = [DocumentController alloc]init];
    [txtStorageDelegate setWindow: window];
    preferencesPanel = [[PreferencesPanel alloc]init];
    projMgrPanel = [[ProjectManagerPanel alloc]init];
}

- (void) buildWindow
{
#define _wndW 825
#define _wndH 700
    
    
     window = [[NSWindow alloc] initWithContentRect: NSMakeRect( 0, 0, _wndW, _wndH )
     styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
     backing: NSBackingStoreBuffered
     defer: NO];

    [window center];
    [window setTitle: @"FB Editor"];
    [window makeKeyAndOrderFront: nil];
    [window setDelegate:windowDelegate];
    [windowDelegate setWindow: window];

    [toolbarDelegate setWindow: window];
    [txtDelegate setWindow: window];
    [preferencesPanel setWindow:window];
    
    // ***** Toolbar ******* //
    NSToolbar *toolbar = [[NSToolbar alloc] initWithIdentifier:@"Toolbar1"];
    [toolbar setDelegate:(id<NSToolbarDelegate>)toolbarDelegate];
    [toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
    [toolbar setShowsBaselineSeparator:YES];
    [window setToolbar:toolbar];
    [toolbar release];

    // ***** LineNum Scroll View ***** //
    lineNumScrlView = [[NSScrollView alloc] initWithFrame: NSMakeRect( 0, 0, 50, 0 )];
    
    [lineNumScrlView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    
    [lineNumScrlView setBorderType:NSBezelBorder];
    
    // ***** LineNum View ***** //
    lineNumView = [[NSTextView alloc]initWithFrame:NSMakeRect( 0, 0, 50, 0 )];
    [lineNumView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    
    // Must be same font as text view!!
    [lineNumView setFont:[NSFont fontWithName:@"Menlo" size: 12]];
    [lineNumView setTextColor:[NSColor lightGrayColor]];
    [lineNumView setSelectable:NO];
    [lineNumView setAlignment:NSTextAlignmentRight];
    [lineNumScrlView setDocumentView:lineNumView];
    [lineNumView release];
    
    // ****** Text Scroll View ****** //
    txtScrlView = [[NSScrollView alloc] initWithFrame:NSMakeRect( 0, 0, _wndW, _wndH )];
    
    [txtScrlView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [txtScrlView setBorderType:NSBezelBorder];
    [txtScrlView setHasVerticalScroller: YES];
    [txtScrlView setAutohidesScrollers:YES];
    [txtScrlView setAutoresizesSubviews:YES];
    
    // **** Text View **** //
    txtView = [[NSTextView alloc] initWithFrame:NSMakeRect( 0, 0, _wndW, _wndH )];
    [txtView setDelegate:txtDelegate];
    [docController setTxtView:txtView];
    [windowDelegate setTxtView:txtView];
    [preferencesPanel setTxtView:txtView];
    [toolbarDelegate setTxtView:txtView];
    [txtView setFont:[NSFont fontWithName:@"Menlo" size: 12]];
    [txtView setEditable:YES];
    [txtView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
    [[txtView textStorage] setDelegate:txtStorageDelegate];
   
 
    txtView.automaticQuoteSubstitutionEnabled = NO;
    [txtView setAllowsUndo: YES];
    [txtView setUsesFindBar:YES];
    txtFinder = [[NSTextFinder alloc]init];
    [txtFinder setClient:(id<NSTextFinderClient>)txtView];
    [txtFinder setFindBarContainer:(id<NSTextFinderBarContainer>)txtScrlView];
    [txtScrlView setDocumentView: txtView];
    [txtView release];
    
    // ****** NSSplitView - Vertical ******* //
    NSSplitView *vertSplitView = [[NSSplitView alloc] initWithFrame:NSMakeRect( 0, 0, _wndW, _wndH)];
    
    [vertSplitView setAutoresizingMask: NSViewHeightSizable | NSViewWidthSizable ];
    
    [vertSplitView setDelegate: splitDelegate];
    [vertSplitView setVertical:YES];
    [vertSplitView setDividerStyle: NSSplitViewDividerStyleThin];
    [vertSplitView addSubview:lineNumScrlView];
    [vertSplitView addSubview:txtScrlView];
    [vertSplitView adjustSubviews];
    
    
    [vertSplitView setPosition:60.0 ofDividerAtIndex:0];
    
    [[window contentView]addSubview:vertSplitView];
    
    // ******* Synchronize the two Scroll Views ******** //
    [[txtScrlView contentView] setPostsBoundsChangedNotifications:YES];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(scrollViewContentBoundsDidChange:)
                                                 name:NSViewBoundsDidChangeNotification
                                               object:[txtScrlView contentView]];
    
    [[lineNumScrlView contentView] setPostsBoundsChangedNotifications:YES];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(scrollViewContentBoundsDidChange:)
                                                 name:NSViewBoundsDidChangeNotification
                                               object:[lineNumScrlView contentView]];
}


- (void)textDidChange:(NSNotification *)notification {
    [self calcLineNumbers];
}

- (void) applicationWillFinishLaunching: (NSNotification *)notification
{
    [self initDelegates];
    [self buildMenu];
    [toolbarDelegate retrieveArchivedData];
    [self buildWindow];
    [preferencesPanel retrieveArchivedData];
    [preferencesPanel buildPrefsPanel];
    [projMgrPanel buildProjMgrPanel];

}

- (void) applicationDidFinishLaunching: (NSNotification *)notification
{
// Launches FBtoC messaging
CFNotificationCenterAddObserver( CFNotificationCenterGetDistributedCenter(), NULL, (void*)&CompilerNotificationObserver, CFSTR("CompilerNotification"), NULL, CFNotificationSuspensionBehaviorDeliverImmediately );
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *) theApp
{
    return YES;
}

@end

