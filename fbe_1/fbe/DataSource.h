//
//  DataSource.h
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface DataSource : NSObject <NSOutlineViewDataSource>
{
    NSDictionary *headersDict;
    NSDictionary *resourcesDict;
    NSDictionary *globalsDict;
    NSDictionary *includesDict;
    NSDictionary *mainDict;
    NSMutableArray *headersArray;
    NSMutableArray *resourcesArray;
    NSMutableArray *globalsArray;
    NSMutableArray *includesArray;
    NSMutableArray *mainArray;
    NSMutableArray *files;
    NSInteger startIndex;
    NSString *srcType;
}

@end
