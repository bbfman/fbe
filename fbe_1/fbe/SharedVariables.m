//
//  SharedVariables.m
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "SharedVariables.h"

@implementation SharedVariables
@synthesize currFileName;

+(id)sharedVariablesManager {
    static SharedVariables *sharedVariablesClass = nil;
    @synchronized(self) {
        if (sharedVariablesClass == nil) {
            sharedVariablesClass = [[self alloc] init];
        }
    }
    return sharedVariablesClass;
}

-(id)init {
    if (self = [super init]) {
        currFileName = [[NSString alloc] init];
    }
    return self;
}

-(void)dealloc {
    [currFileName release];
    [super dealloc];
}

@end
