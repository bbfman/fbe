//
//  SharedVariables.h
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface SharedVariables : NSObject
{
    NSString *currFileName;
 }
@property(nonatomic, strong) NSString *currFileName;

+(id)sharedVariablesManager;

@end
