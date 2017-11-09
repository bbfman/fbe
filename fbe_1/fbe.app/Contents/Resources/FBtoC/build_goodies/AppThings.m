/*
 AppThings.m
 
 Bernie Wylde 20160408
 */

#include "FBtoC.h"
#include "Runtime.h"

void FBFullStop( CFStringRef msg, CFStringRef info, FBBoolean fatal )
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:(NSString *)msg];
    [alert setInformativeText:(NSString *)info];
    if ( fatal ) {
        [alert setAlertStyle:NSCriticalAlertStyle];
        [alert addButtonWithTitle:NSLocalizedStringFromTable( @"Quit",@"Runtime",@"FBFullStop" )];
        [alert addButtonWithTitle:NSLocalizedStringFromTable( @"Crash",@"Runtime",@"FBFullStop" )];
    } else {
        [alert setAlertStyle:NSWarningAlertStyle];
        [alert addButtonWithTitle:NSLocalizedStringFromTable( @"Stop",@"Runtime",@"FBFullStop" )];
        [alert addButtonWithTitle:NSLocalizedStringFromTable( @"Continue",@"Runtime",@"FBFullStop" )];
        [alert addButtonWithTitle:NSLocalizedStringFromTable( @"Crash",@"Runtime",@"FBFullStop" )];
    }
    // unfortunately NSModalResponse is OS X 10.9+, so NSInteger used. See NSApplication.h: 'typedef NSInteger NSModalResponse NS_AVAILABLE_MAC(10_9);' // Brian 20160410
    NSInteger response = [alert runModal];
    if ( response == NSAlertFirstButtonReturn ) ExitToShell();
    if ( fatal == YES || response == NSAlertThirdButtonReturn ) abort();
    [alert release];
}

