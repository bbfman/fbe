//
//  ProjectManagerPanel.m
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "ProjectManagerPanel.h"

@implementation ProjectManagerPanel

- (void)buildProjMgrPanel
{
#define _projMgrPanelW  345
#define _projMgrPanelH  500
    
    projMgrPanel = [[NSPanel alloc] initWithContentRect:NSMakeRect( 30, 750, _projMgrPanelW, _projMgrPanelH )
                                            styleMask:NSWindowStyleMaskUtilityWindow | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                              backing:NSBackingStoreBuffered defer:NO];
    [projMgrPanel setTitle:@"Project Manager"];

    // ***** NSScrollView to hold OutlineView ****** //
    NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame:NSMakeRect( 20, 20, 300, _projMgrPanelH - 40 )];
    [scrollView setBorderType:NSBezelBorder];
    [scrollView setHasVerticalScroller:YES];
    [scrollView setHasHorizontalScroller:YES];
    [scrollView setAutohidesScrollers:YES];
    [scrollView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    
    // ******* NSOutlineView ******* //
    NSOutlineView *outlineView = [[NSOutlineView alloc] initWithFrame: NSMakeRect( 0, 0, 0, 0)];
    
    // ***** Data Source **** //
    dataSource = [[DataSource alloc] init];
    [outlineView setDataSource: dataSource];
    [outlineView registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];
    
    // ***** Column one ***** //
    NSTableColumn *column1 = [[NSTableColumn alloc] initWithIdentifier: @"Col1"];
    [column1 setEditable: NO];
    [column1 setMinWidth: 295.0];
    [[column1 headerCell] setStringValue:@"Files"];
    [outlineView addTableColumn: column1];
    // **** Won't see triangles without this ***** //
    [outlineView setOutlineTableColumn:column1];
    [column1 release];
    
    // ***** Embed OutlineView into scroll view **** //
    [scrollView setDocumentView:outlineView];
    [[projMgrPanel contentView] addSubview: scrollView];
    [outlineView release];
    [scrollView release];

}

- (void) showProjMgrPanel
{
    [projMgrPanel makeKeyAndOrderFront:self];
    
}

- (void) hideProjMgrPanel
{
    [projMgrPanel orderOut:self];
}

@end
