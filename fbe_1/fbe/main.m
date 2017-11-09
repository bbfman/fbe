//
//  main.m
//  fbe
//
//  Created by steve on 11/5/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"


int main() {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    NSApplication *application = [NSApplication sharedApplication];
    AppDelegate *appDelegate = [[[AppDelegate alloc] init] autorelease];
    [application setDelegate:appDelegate];
    [application run];
    [pool drain];
    
    return 0;
}
