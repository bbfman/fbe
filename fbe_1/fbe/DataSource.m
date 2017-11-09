//
//  DataSource.m
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "DataSource.h"

@implementation DataSource
-(id)init
{
    if (self = [super init])
    {
        // *** Retain is important **** //
        headersArray = [NSMutableArray array];
        headersDict = [[[NSDictionary alloc] initWithObjectsAndKeys:@"headers", @"type", headersArray, @"fileName", nil ] retain];
        
        resourcesArray = [NSMutableArray array];
        resourcesDict = [[[NSDictionary alloc] initWithObjectsAndKeys:@"resources", @"type", resourcesArray, @"fileName", nil ] retain];
        
        globalsArray = [NSMutableArray array];
        globalsDict = [[[NSDictionary alloc] initWithObjectsAndKeys:@"globals", @"type", globalsArray, @"fileName", nil ] retain];
        
        includesArray = [NSMutableArray array];
        includesDict = [[[NSDictionary alloc] initWithObjectsAndKeys:@"includes", @"type", includesArray, @"fileName", nil ] retain];
        
        mainArray = [NSMutableArray array];
        mainDict = [[[NSDictionary alloc] initWithObjectsAndKeys:@"main", @"type", mainArray, @"fileName", nil ] retain];
        
        files = [[NSMutableArray arrayWithObjects: headersDict, resourcesDict, globalsDict, includesDict, mainDict, nil] retain];
        
    }
    return self;
}

- (void)dealloc
{
    [headersDict release];
    [resourcesDict release];
    [globalsDict release];
    [includesDict release];
    [mainDict release];
    [super dealloc];
}

// ****** These four methods are required ****** //

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem: (id)item
{
    // ** root node ** //
    if (item == nil) {
        return [files objectAtIndex:index];
    }
    
    if ([item isKindOfClass:[NSDictionary class]]) {
        return [[item objectForKey:@"fileName"] objectAtIndex: index];
    }
    
    return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
    if ( [item isKindOfClass:[NSDictionary class] ] ) {
        return YES;
    } else {
        return NO;
    }
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem: (id)item
{
    if (item == nil) {
        return [files count];
    }
    if ([item isKindOfClass:[NSDictionary class]]) {
        return [[item objectForKey:@"fileName"]count];
    }
    return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
    if ( [[tableColumn identifier] isEqualToString: @"Col1"] ) {
        if ([item isKindOfClass:[NSDictionary class]]) { return [item objectForKey:@"type"]; }
        if ([item isKindOfClass:[NSArray class]]) { return [item objectForKey:@"fileName"]; }
        return item;
    }
    return nil;
}

- (BOOL)outlineView:(NSOutlineView*)olv writeItems:(NSArray*)items toPasteboard:(NSPasteboard*)pboard
{
    [pboard declareTypes:[NSArray arrayWithObject:NSFilenamesPboardType] owner:self];
    [pboard setPropertyList:items forType:NSFilenamesPboardType];
    NSLog(@"items = %@", items);
    // NSLog(@"category type = %@", [items objectForKey:@"type"]);
    return YES;
}

//NSDraggingInfo
- (NSDragOperation)outlineView:(NSOutlineView*)outlineView validateDrop:(id)info proposedItem:(id)item proposedChildIndex:(NSInteger)index
{
    // NSLog(@"proposedChildIndex = %d", [outlineView childIndexForItem:item]);
    // NSLog(@"selectedRow = %d", [outlineView selectedRow]);
    id selectedItem = [outlineView itemAtRow:[outlineView selectedRow]];
    //NSInteger selectedIndex=[myArray indexOfObject:selectedItem];
    
    id parent = [outlineView parentForItem:selectedItem];
    NSLog(@"++++++++++++");
    NSLog(@"selectedItem= %@", selectedItem);
    NSLog(@"info = %@", info);
    NSLog(@"proposed item = %@", item);
    NSLog(@"proposed index = %ld",(long) index);
    
    NSLog(@"parent for item = %@", [outlineView parentForItem:selectedItem]);
    NSLog(@" srcType = %@", [parent objectForKey:@"type"]);
    srcType = [parent objectForKey:@"type"];
    
    if([srcType isEqualTo:@"headers"]) { startIndex = [headersArray indexOfObject: selectedItem];}
    if([srcType isEqualTo:@"resources"]) { startIndex = [resourcesArray indexOfObject: selectedItem];}
    if([srcType isEqualTo:@"globals"]) { startIndex = [globalsArray indexOfObject: selectedItem];}
    if([srcType isEqualTo:@"includes"]) {startIndex = [includesArray indexOfObject: selectedItem];}
    if([srcType isEqualTo:@"main"]) { startIndex = [mainArray indexOfObject: selectedItem];}
    NSLog(@"startIndex = %ld", (long)startIndex);
    NSLog(@"++++++++++++");
    
    return NSDragOperationEvery;
}

// NSPasteboard    *pboard = [info draggingPasteboard];
// NSArray *myTypes = [NSArray arrayWithObjects:MyItemsPboardType, nil];
// if([pboard availableTypeFromArray:myTypes] != nil){
// add items here and delete the old one
// <NSDraggingInfo>

- (BOOL) outlineView:(NSOutlineView *)outlineView acceptDrop:(id)info item:(id)item childIndex:(NSInteger)childIndex
{
    // **** Outside draggingSource **** //
    if([info draggingSource] != outlineView) {
        
        NSLog(@"#############");
        NSLog(@"outside drag.");
        NSLog(@"item = %@", item);
        NSLog(@"fileName = %@", [item objectForKey:@"fileName"]);
        NSLog(@"parent = %@", [outlineView parentForItem:item]);
        NSLog(@"childIndex = %ld",childIndex);
        
        NSArray *draggedFilenames = [[info draggingPasteboard] propertyListForType:NSFilenamesPboardType];
        NSString *fileStr = [[draggedFilenames objectAtIndex:0] lastPathComponent];
        NSString *destType = [item objectForKey:@"type"];
        
        NSLog(@"fileStr = %@", fileStr);
        NSLog(@"destType = %@", destType);
        NSLog(@"#############");
        
        if ([destType isEqualTo:@"headers"]) { [headersArray insertObject: fileStr atIndex:childIndex]; }
        if ([destType isEqualTo:@"resources"]) { [resourcesArray insertObject: fileStr atIndex:childIndex]; }
        if ([destType isEqualTo:@"globals"]) { [globalsArray insertObject: fileStr atIndex:childIndex]; }
        if ([destType isEqualTo:@"includes"]) { [includesArray insertObject: fileStr atIndex:childIndex]; }
        if ([destType isEqualTo:@"main"]) { [mainArray insertObject: fileStr atIndex:childIndex]; }
        
        [outlineView reloadData];
    }
    
    // **** Local Reorder of OutlineView **** //
    if([info draggingSource] == outlineView) {
        
        if(childIndex > -1) {
            NSArray *draggedFilenames = [[info draggingPasteboard] propertyListForType:NSFilenamesPboardType];
            NSString *fileStr = [[draggedFilenames objectAtIndex:0] lastPathComponent];
            NSString *destType = [item objectForKey:@"type"];
            
            NSLog(@"==========");
            NSLog(@"Local drag.");
            NSLog(@"child index = %ld", (long)childIndex);
            NSLog(@"local drag type = %@", [item objectForKey:@"type"]);
            NSLog(@"local drag fileName = %@", [item objectForKey:@"fileName"]);
            NSLog(@"fileStr = %@", fileStr);
            NSLog(@"destType = %@", destType);
            NSLog(@"==========");
            
            if ([destType isEqualTo:@"headers"]) {[headersArray insertObject: fileStr atIndex:childIndex]; }
            if ([destType isEqualTo:@"resources"]) { [resourcesArray insertObject: fileStr atIndex:childIndex]; }
            if ([destType isEqualTo:@"globals"]) { [globalsArray insertObject: fileStr atIndex:childIndex]; }
            if ([destType isEqualTo:@"includes"]) { [includesArray insertObject: fileStr atIndex:childIndex]; }
            if ([destType isEqualTo:@"main"]) { [mainArray insertObject: fileStr atIndex:childIndex]; }
            
            //   if (startIndex < childIndex) {
            //    [files removeObjectAtIndex:startIndex];
            //   } else {
            //    [files removeObjectAtIndex:startIndex+1];
            //  }
            
            if ([srcType isEqualTo: destType]) {
                if (startIndex < childIndex) {
                    if([srcType isEqualTo:@"headers"]) { [headersArray removeObjectAtIndex:startIndex];}
                    if([srcType isEqualTo:@"resources"]) { [resourcesArray removeObjectAtIndex:startIndex];}
                    if([srcType isEqualTo:@"globals"]) { [globalsArray removeObjectAtIndex:startIndex];}
                    if([srcType isEqualTo:@"includes"]) { [includesArray removeObjectAtIndex:startIndex];}
                    if([srcType isEqualTo:@"main"]) { [mainArray removeObjectAtIndex:startIndex];}
                } else {
                    if([srcType isEqualTo:@"headers"]) { [headersArray removeObjectAtIndex:startIndex +1];}
                    if([srcType isEqualTo:@"resources"]) { [resourcesArray removeObjectAtIndex:startIndex +1];}
                    if([srcType isEqualTo:@"globals"]) { [globalsArray removeObjectAtIndex:startIndex +1];}
                    if([srcType isEqualTo:@"includes"]) { [includesArray removeObjectAtIndex:startIndex +1];}
                    if([srcType isEqualTo:@"main"]) { [mainArray removeObjectAtIndex:startIndex +1];}
                }
            } else {
                if([srcType isEqualTo:@"headers"]) { [headersArray removeObjectAtIndex:startIndex];}
                if([srcType isEqualTo:@"resources"]) { [resourcesArray removeObjectAtIndex:startIndex];}
                if([srcType isEqualTo:@"globals"]) { [globalsArray removeObjectAtIndex:startIndex];}
                if([srcType isEqualTo:@"includes"]) { [includesArray removeObjectAtIndex:startIndex];}
                if([srcType isEqualTo:@"main"]) { [mainArray removeObjectAtIndex:startIndex];}
            }
        }
        [outlineView reloadData];
    } 
    return YES;
}

@end
