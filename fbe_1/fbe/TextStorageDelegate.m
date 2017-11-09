//
//  TextStorageDelegate.m
//  fbe
//
//  Created by steve on 11/6/17.
//  Copyright © 2017 vsquared. All rights reserved.
//

#import "TextStorageDelegate.h"

@implementation TextStorageDelegate
@synthesize window;

-(void)dealloc {
    [window release];
    [super dealloc];
}

-(void)textStorageDidProcessEditing:(NSNotification *)notification
{
    NSTextStorage *textStorage = [notification object];
    NSString *text = [textStorage string];
    NSRange range = NSMakeRange(0, text.length);
    NSColor *green = [NSColor colorWithCalibratedRed: 0.0 green: 0.4 blue: 0.0 alpha: 1.0];
    NSColor *magenta = [NSColor colorWithCalibratedRed: 0.62 green: 0.30 blue: 1.0 alpha: 1.0];
    
    [textStorage removeAttribute:NSForegroundColorAttributeName range:range];
    
    // ====== Includes ========= //
    NSString *includes = [NSString stringWithFormat:@"%@|%@|%@", @"include", @"library", @"resources"];
    NSRegularExpression *rexIncludes = [NSRegularExpression regularExpressionWithPattern:includes options:kNilOptions error:nil];
    [rexIncludes enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop) {
        NSRange subStringRange = [result rangeAtIndex:0];
        [textStorage addAttribute:NSForegroundColorAttributeName value:green range:subStringRange];
        [textStorage addAttribute:NSFontAttributeName value:[NSFont fontWithName:@"Menlo Bold" size:12] range:subStringRange];
    }];
    
    // ====== Class ========== //
    NSString *class = [NSString stringWithFormat:@"\\b%@\\b|%@|%@|%@|%@|%@|%@|%@|%@|%@", @"main", @"@interface", @"@implementation", @"@end",@"select",@"end select",@"case",@"begin",@"end enum",@"enum"];
    NSRegularExpression *rexClass = [NSRegularExpression regularExpressionWithPattern:class options:kNilOptions error:nil];
    [rexClass enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop) {
        NSRange subStringRange = [result rangeAtIndex:0];
        [textStorage addAttribute:NSForegroundColorAttributeName value:[NSColor blueColor] range:subStringRange];
        // [textStorage addAttribute:NSStrokeWidthAttributeName value:[NSNumber numberWithFloat: 3.0] range:subStringRange];
        [textStorage addAttribute:NSFontAttributeName value:[NSFont fontWithName:@"Menlo Bold" size:12] range:subStringRange];
    }];
    
    // ====== Types ========== //
    // @"\\b%@\\b",
    NSString *types = [NSString stringWithFormat:@"%@|\\b%@\\b|%@|%@|%@|%@|%@|%@|%@|%@|\\b%@\\b|\\b%@\\b|\\b%@\\b",  @"void", @"long", @"long if",@"unsigned",@"double",@"signed", @"char", @"short",@"CGFloat",@"float",@"id",@"int",@"end if"];
    NSRegularExpression *rexTypes = [NSRegularExpression regularExpressionWithPattern:types options:NSRegularExpressionCaseInsensitive error:nil];
    [rexTypes enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop) {
        NSRange subStringRange = [result rangeAtIndex:0];
        [textStorage addAttribute:NSForegroundColorAttributeName value:magenta range:subStringRange];
    }];
    
    // ====== Controls ======= //
    NSString *controls = [NSString stringWithFormat:@"%@|%@|%@|%@|%@|%@|%@|%@", @"textfield", @"cocoa window", @"cocoa button", @"popupbutton", @"combobox", @"cocoa box",@"imageview",@"colorwell"];
    NSRegularExpression *rexControls = [NSRegularExpression regularExpressionWithPattern:controls options:kNilOptions error:nil];
    [rexControls enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop) {
        NSRange subStringRange = [result rangeAtIndex:0];
        [textStorage addAttribute:NSForegroundColorAttributeName value:[NSColor blueColor] range:subStringRange];
    }];
    
    // ====== Keywords ======= //
    NSString *keywords = [NSString stringWithFormat:@"%@|%@|%@|%@|%@|%@|%@|%@|%@|%@|%@|%@|%@",@"BeginCCode",@"BeginCFunction", @"BeginCDeclaration", @"EndC", @"return", @"local fn",@"fn",@"end fn",@"dim as",@"handleevents",@"on dialog",@"on menu",@"HandleEvents"];
    NSRegularExpression *rexKeywords = [NSRegularExpression regularExpressionWithPattern:keywords options:kNilOptions error:nil];
    [rexKeywords enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop) {
        NSRange subStringRange = [result rangeAtIndex:0];
        [textStorage addAttribute:NSForegroundColorAttributeName value:[NSColor redColor] range:subStringRange];
        [textStorage addAttribute:NSFontAttributeName value:[NSFont fontWithName:@"Menlo Bold" size:12] range:subStringRange];
    }];
    
    // ====== String With Double Quotes ========== //
    NSString *doubleQuotes = @"\"(.*?)\"";
    NSString *quotes = [NSString stringWithFormat:@"%@", doubleQuotes];
    NSRegularExpressionOptions doubleQuoteOptions = NSRegularExpressionDotMatchesLineSeparators;
    NSRegularExpression *rexDoubleQuotes = [NSRegularExpression regularExpressionWithPattern:quotes options:doubleQuoteOptions error:nil];
    [rexDoubleQuotes enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop)
     {
         NSRange subStringRange = [result rangeAtIndex:0];
         [textStorage addAttribute:NSForegroundColorAttributeName value:[NSColor redColor] range:subStringRange];
     }];
    
    // ====== Comments========== //
    NSString *blockComments = @"/[*](.*?)[*]/";
    NSString *lineComments = @"//[^\"\\n\\r]*(?:\"[^\"\\n\\r]*\"[^\"\\n\\r]*)*[\\r\\n]";
    NSString *comments = [NSString stringWithFormat:@"%@|%@", blockComments, lineComments];
    NSRegularExpressionOptions regexOptions = NSRegularExpressionDotMatchesLineSeparators;
    NSRegularExpression *rex5 = [NSRegularExpression regularExpressionWithPattern:comments options:regexOptions error:nil];
    [rex5 enumerateMatchesInString:text options:kNilOptions range:range usingBlock:^(NSTextCheckingResult *result, NSMatchingFlags flags, BOOL *stop) {
        NSRange subStringRange = [result rangeAtIndex:0];
        [textStorage addAttribute:NSForegroundColorAttributeName value:[NSColor orangeColor] range:subStringRange];
    }];
}
@end

