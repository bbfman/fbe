//
//  WindowDelegate.h
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>
//#import "SharedVariables.h"

@interface WindowDelegate : NSObject <NSWindowDelegate>
{
    NSWindow *window;
    NSString *currFileName;
    NSTextView *txtView;
}
@property(retain) NSWindow *window;
@property (retain) NSTextView *txtView;

@end
