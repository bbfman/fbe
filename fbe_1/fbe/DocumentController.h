//
//  DocumentController.h
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface DocumentController : NSObject
{
    NSTextView *txtView;
    NSMenu *openRecentMenu;
}

@property(retain)  NSTextView *txtView;
@property(retain) NSMenu *openRecentMenu;

- (void) myMenuAction:(id)sender;
@end
