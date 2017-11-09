//
//  ProjectManagerPanel.h
//  fbe
//
//  Created by steve on 11/7/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DataSource.h"

@interface ProjectManagerPanel : NSPanel
{
  NSPanel *projMgrPanel;
  DataSource *dataSource;
}

- (void) buildProjMgrPanel;
- (void) showProjMgrPanel;
- (void) hideProjMgrPanel;

@end
