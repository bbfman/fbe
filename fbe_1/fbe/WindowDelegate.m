//
//  WindowDelegate.m
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "WindowDelegate.h"

@implementation WindowDelegate
@synthesize window;
@synthesize txtView;

- (void) dealloc
{
    [txtView release];
    [window release];
    [self release];
    [super dealloc];
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
    if (!fileSaved) {
        NSLog(@"Unable to save file due to error: %@", err);
    } else {
        NSLog(@"File %@ was saved.", currFileName );
        [window setDocumentEdited:NO];
    }
}

- (void)saveAlert
{
    NSAlert *alert = [[NSAlert alloc] init] ;
    [alert addButtonWithTitle:@"Save changes..."];
    [alert addButtonWithTitle:@"Cancel"];
    [alert addButtonWithTitle:@"Quit"];
    [alert setMessageText:@"There are unsaved changes. Do you wish to save them?"];
    [alert setInformativeText:@"If you quit without saving, changes will be lost."];
    [alert setAlertStyle: NSAlertStyleWarning];
    NSUInteger result = [alert runModal];
    switch ( result )
    {
        case 1000: // Save button
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
            NSAssert1( NO, @"Unknown button return: %ld", returnCode );
            break;
    }
}

- (BOOL)windowShouldClose:(id)wnd
{
    if ( [wnd isDocumentEdited] ) {
        [self saveAlert];
        return NO;
    }
    else {
        [NSApp terminate:self];
        return YES; }
}

@end
