
/*
 General.m
 
 Brian 20160616
 */

#include "FBtoC.h"
#include "Runtime.h"
#include "StringUtils.h"

// InString(), LeftString(), RightString() and MidString() are copied from FBtoC's source ( String & Char Utilities )
/*
 fn LeftString, fn RightString, fn MidString and fn InString are CFString replacements for
 FB's LEFT, RIGHT, MID and INSTR.
 20141120 Brian
 
 left$ & right$
 If numChars is less than or equal to zero, an empty string is returned.
 
 mid$
 if startPos is less than 1, it is set to 1 (0 for our CF functions)
 if numChars is less than or equal to zero, it is set to the length of the string.
 
 instr
 if startPos is less than 1, it is set to 1 (0 for our CF functions)
 */


//
// N.B.  Returning a ZERO position is a valid value and different from FB's INSTR which returns zero for not found. CFStrings start at position zero.
//       By comparison, pascal strings start at position one ( with position zero the length byte ).
//       Use of this function to replace FB's INSTR should be checked to make sure the logic does not rely on a zero return for not found.
//       This INSTR replacement returns NSNotFound for not found. NSNotFound can be different values depending on where it runs
//       Brian 20151011

SInt64 InString(  SInt32 startPos,  CFStringRef strToSearch, CFStringRef strToSearchFor ) {
    
    SInt64 returnPos = NSNotFound;
    
    if ( startPos < 0 ) startPos = 0;
    NSInteger hayStackLen = [(NSString*)strToSearch length];
    NSInteger needleLen = [(NSString*)strToSearchFor length];
    NSRange myRange = NSMakeRange( startPos, hayStackLen - startPos );
    
    if ( startPos < hayStackLen && hayStackLen && needleLen ) {
        NSRange inRange = [(NSString *)strToSearch rangeOfString:(NSString *)strToSearchFor options:NSLiteralSearch range:myRange];
        returnPos = inRange.location;
    }
    return returnPos;
}

CFStringRef RightString( CFStringRef inString, CFIndex howManyChars ) {
    
    CFStringRef s = CFSTR("");
    
    if ( howManyChars > 0 ) {
        NSInteger length = [(NSString*)inString length];
        if ( howManyChars > length ) howManyChars = length;
        s = (CFStringRef)[(NSString *)inString substringFromIndex:length - howManyChars];
    }
    return s;
}

CFStringRef LeftString( CFStringRef inString, CFIndex toIndex ) {
    
    CFStringRef s = CFSTR("");
    
    if ( toIndex > 0 ) {
        NSInteger length = [(NSString *)inString length];
        if ( toIndex > length ) toIndex = length;
        s = (CFStringRef)[(NSString *)inString substringToIndex:toIndex];
    }
    return s;
}

CFStringRef MidString( CFStringRef inString , CFIndex startPos, CFIndex howManyChars ) {
    CFStringRef s = CFSTR("");
    NSInteger length = [(NSString*)inString length];
    if ( startPos < 0 ) startPos = 0;
    if ( howManyChars < 0 ) howManyChars = length - startPos;
    if ( startPos < length ) {
        if ( startPos + howManyChars > length ) howManyChars = length - startPos;
        s = (CFStringRef)[(NSString *)inString substringWithRange: NSMakeRange( startPos, howManyChars )];
    }
    return s;
}


// Note: used by FBtoC source but FBtoC still generates calls to old PSHex() function
// Replacement for FB's hex$ - Brian 20160617
CFStringRef HexString( SInt32 num ) {
    
    NSMutableString* hexStr = [NSMutableString stringWithCapacity:0];
    [hexStr appendFormat:@"%X",(int)num];
    UInt8 padLength = gPSDefStrSize * 2; // honor the user defstr setting
    
    // Size returned string based on user defstr setting
    if ( num < 0 ) {
        [hexStr setString:(NSString *)RightString( (CFStringRef)hexStr, padLength )];
    } else {
        
        short length = [hexStr length];
        if ( length > padLength ) { // truncate to padLength
            [hexStr setString:(NSString *)RightString( (CFStringRef)hexStr, padLength )];
        } else {  // pre-pend leading zeros
            for ( ; length < padLength ; length++) {
                [hexStr insertString:@"0" atIndex:0];
            }
        }
    }
    return (CFStringRef)hexStr;
}
