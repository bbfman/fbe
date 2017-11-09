//
//  SplitViewDelegate.m
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "SplitViewDelegate.h"

@implementation SplitViewDelegate
- (void)splitView:(NSSplitView *)splitView resizeSubviewsWithOldSize:(NSSize)oldSize {
    [splitView adjustSubviews];
    [splitView setPosition:50 ofDividerAtIndex:0];
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger)dividerIndex {
    return 0;
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)dividerIndex {
    return 50;
}
@end
