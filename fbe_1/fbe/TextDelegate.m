//
//  TextDelegate.m
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "TextDelegate.h"

@implementation TextDelegate
@synthesize window;

-(void)dealloc
{
    [window release];
    [super dealloc];
}

- (void)textDidBeginEditing:(NSNotification *)notification {
    [window setDocumentEdited:YES];
}

- (void)textDidEndEditing:(NSNotification *)notification {
    [window setDocumentEdited:YES];
}

@end
