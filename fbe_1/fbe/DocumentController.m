//
//  DocumentController.m
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "DocumentController.h"

@implementation DocumentController
@synthesize openRecentMenu;
@synthesize txtView;

- (void) dealloc
{
    [openRecentMenu release];
    [txtView release];
    [super dealloc];
}

- (void) myMenuAction:(id)sender
{
    NSString *selectedItem = [sender title];
    NSString *fileStr = [[NSString alloc] initWithContentsOfFile: selectedItem encoding:NSUTF8StringEncoding error:nil];
    if (fileStr) {
        [txtView setString:fileStr];
        [fileStr release];
    }
}

- (void)noteNewRecentDocumentURL:(NSURL *)url
{
    [openRecentMenu addItemWithTitle:[url path] action:@selector(myMenuAction:) keyEquivalent:@""];
}

@end
