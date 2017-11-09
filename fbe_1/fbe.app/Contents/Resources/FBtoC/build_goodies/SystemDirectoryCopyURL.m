
#include "SystemDirectoryCopyURL.h"
#include "Util_FileDirectory.h"

CFURLRef SystemDirectoryCopyURL( UInt32 whichDirectory, SInt32 domain )
{
    CFURLRef oURL = NULL;
    BOOL frameworksDir = NO;
    
    // bw 20141016
    if ( domain == kUserDomain && whichDirectory == kPreferencesFolderType ) {
        return (CFURLRef)[[NSURL fileURLWithPath:[@"~/Library/Preferences" stringByExpandingTildeInPath]] retain];
    }
    
    // bw 20141106
    if ( domain == kUserDomain && whichDirectory == kTemporaryFolderType ) {
        return (CFURLRef)[[NSURL fileURLWithPath:NSTemporaryDirectory()] retain];
    }
    
    switch ( domain ) {
        case kUserDomain:
            domain = NSUserDomainMask;
            break;
        case kLocalDomain:
            domain = NSLocalDomainMask;
            break;
        case kNetworkDomain:
            domain = NSNetworkDomainMask;
            break;
        case kSystemDomain:
            domain = NSSystemDomainMask;
            break;
        default:
            break;
    }
    
    switch ( whichDirectory ) {
            
            // NSApplicationDirectory;  // ????
            // NSDemoApplicationDirectory;  // ????
            
        case kDeveloperApplicationsFolderType:
            whichDirectory = NSDeveloperApplicationDirectory;
            break;
            
            // NSAdminApplicationDirectory;  // ????
            
        case kDomainLibraryFolderType:
            whichDirectory = NSLibraryDirectory;
            break;
        case kDeveloperFolderType:
            whichDirectory = NSDeveloperDirectory;
            break;              // bw 20141015
        case kUsersFolderType:
            whichDirectory = NSUserDirectory;
            break;
        case kDocumentationFolderType:
            whichDirectory = NSDocumentationDirectory;
            break;
        case kDocumentsFolderType:
            whichDirectory = NSDocumentDirectory;
            break;
        case kCoreServicesFolderType:
            whichDirectory = NSCoreServiceDirectory;
            break;
        case kAutosaveInformationFolderType:
            whichDirectory = NSAutosavedInformationDirectory;
            break;
        case kDesktopFolderType:
            whichDirectory = NSDesktopDirectory;
            break;
        case kCachedDataFolderType:
            whichDirectory = NSCachesDirectory;
            break;
        case kApplicationSupportFolderType:
            whichDirectory = NSApplicationSupportDirectory;
            break;
        case kDownloadsFolderType:
            whichDirectory = NSDownloadsDirectory;
            break;
        case kInputMethodsFolderType:
            whichDirectory = NSInputMethodsDirectory;
            break;
        case kMovieDocumentsFolderType:
            whichDirectory = NSMoviesDirectory;
            break;
        case kMusicDocumentsFolderType:
            whichDirectory = NSMusicDirectory;
            break;
        case kPictureDocumentsFolderType:
            whichDirectory = NSPicturesDirectory;
            break;
        case kPrinterDescriptionFolderType:
            whichDirectory = NSPrinterDescriptionDirectory;
            break;
        case kSharedUserDataFolderType:
            whichDirectory = NSSharedPublicDirectory;
            break;
        case kPreferencePanesFolderType:
            whichDirectory = NSPreferencePanesDirectory;
            break;
            
            // NSItemReplacementDirectory;  // ????
            
        case kApplicationsFolderType:
            whichDirectory = NSAllApplicationsDirectory;
            break;
        case kSharedLibrariesFolderType:
            whichDirectory = NSAllLibrariesDirectory;
            break;
            
        case kFrameworksFolderType:
            whichDirectory = NSLibraryDirectory;
            frameworksDir = YES;
            break;
            
        default:
            break;
    }
    
    // URLsForDirectory:inDomains: is 10.6 or later.
    NSArray *urls = [[NSFileManager defaultManager] URLsForDirectory:whichDirectory inDomains:domain];
    if( [urls count] > 0 ) {
        oURL =(CFURLRef)[[urls objectAtIndex:0] retain];
        if ( frameworksDir ) {
            NSURL *fwURL = [[(NSURL *)oURL URLByAppendingPathComponent:@"Frameworks"] retain];
            CFRelease( oURL );
            oURL = (CFURLRef)fwURL;
        }
    }
    return oURL;
}

