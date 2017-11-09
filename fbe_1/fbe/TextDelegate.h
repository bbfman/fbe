//
//  TextDelegate.h
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface TextDelegate : NSObject <NSTextViewDelegate>
{
    NSWindow *window;
}
@property(retain) NSWindow *window;

@end
