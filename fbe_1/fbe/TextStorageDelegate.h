//
//  TextStorageDelegate.h
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface TextStorageDelegate : NSObject  <NSTextStorageDelegate>
{
    NSWindow *window;
}
@property(retain)NSWindow *window;
@end
