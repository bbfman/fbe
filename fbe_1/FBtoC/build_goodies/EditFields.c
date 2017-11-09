#include "FBtoC.h"
#include "Runtime.h"
#if !__LP64__

///////////////////////////////////////////////////
//               edit field stuff                //
///////////////////////////////////////////////////
/*
 These functions currently only work with static and edit text controls 
 */

const UInt32		kFBEFFilter	= 'Fltr';
Str1				gFBTEKey;
FBOnXxxxxVector		gFBOnEditVector;

// field types
enum
{
	kEFFramedNoCR		= 1,
	kEFFramed			= 2,
	kEFNoFramedNoCR		= 3,
	kEFNoFramed			= 4,
	kEFStatNoFramed		= 7
};


// text types
enum
{
	kEFPascalType		= 0,
	kEFZTXTHandleType	= 1,
	kEFTEXTResIDType	= 2,
	kEFContainerType	= 3,
    kEFCFStringType     = 4
};


// justification
enum
{
	kEFLeftJust		= 1,
	kEFCenterJust	= 2,
	kEFRightJust	= 3
};






void FBEditDollarStatement( long id, long selStart, long selEnd, void *txtPtr, long font, long size, long style, long mode, void* red, long green, long blue, long textType )
{
	/* 
	 edit$( efID ) = string | &ZTXTHandle | %TEXTResID | #container [, [font][, [size][, [style][, [mode][, red | @rgb[, green, blue]]]]]]
	 edit$( efID, lineNumber ) = string | &ZTXTHandle | %TEXTResID | #container [, [font][, [size][, [style][, [mode][, red | @rgb[, green, blue]]]]]]
	 edit$( efID, -1 ) = string | &ZTXTHandle | %TEXTResID | #container [, [font][, [size][, [style][, [mode][, red | @rgb[, green, blue]]]]]]
	 edit$( efID, selStart, selEnd ) = string | &ZTXTHandle | %TEXTResID | #container [, [font][, [size][, [style][, [mode][, red | @rgb[, green, blue]]]]]]
	 
	 Bugs:
	 - multistyled fields not supported
	 */
	ControlFontStyleRec				cfs;
	ControlEditTextSelectionRec		sel;
	CFMutableStringRef				contentStr;
	CFStringRef						string = 0, tempStr;
	Handle							h;
	CFIndex							lineBeginIndex, lineEndIndex, contentsEndIndex, currentLine, contentLength;
	Str255							temp;
	
	HIViewRef view = FBButtonAmpersandFunction( id );
	if ( view )
	{
		if ( txtPtr )
		{
			switch ( textType )
			{
				case kEFPascalType:
					gFBStk--;
					string = CFStringCreateWithPascalString( NULL, txtPtr, kCFStringEncodingMacRoman );
					break;
					
				case kEFZTXTHandleType:
					string = CFStringCreateWithBytes( NULL, *(UInt8**)txtPtr, GetHandleSize( txtPtr ), kCFStringEncodingMacRoman, false );
					break;
					
				case kEFTEXTResIDType:
					h = GetResource( 'TEXT', (int)txtPtr );
					if ( h )
					{
						string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), kCFStringEncodingMacRoman,  false );
						ReleaseResource( h );
					}
					break;
                ////////////////////////////////////////////////////////////////////////////
                // new 'pointer' ( compared to prior 'Handle' ) container support for Edit$.
				// 'long' cast suspect but used for 32-bit Carbon edit fields
                // Brian 20170905
                ////////////////////////////////////////////////////////////////////////////
                case kEFContainerType: {
                    void *p = (void*)*(long*)txtPtr;
                    CFIndex ptrSize = (CFIndex)CtnrRequestedSize( p );
                    if ( p ) string = CFStringCreateWithBytes( NULL, p, ptrSize, kCFStringEncodingMacRoman,  false );
                    //h = *(Handle*)txtPtr;
                    //if ( h ) string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), kCFStringEncodingMacRoman,  false );
					break;
                }
			}
		}
		
		
		if ( string )
		{
			//gFBStk--;								// bug #243 - Edit$ crashes 
			if ( selStart == kFBParamMissing && selEnd == kFBParamMissing )		// all
			{
				ControlSetText( view, string );
			}
			else
			{
				if ( selStart == -1 )											// current selection
				{
					GetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel, NULL );
					selStart = sel.selStart;
					selEnd = sel.selEnd;
				}
				if ( selEnd == kFBParamMissing )								// lineNumber
				{
					tempStr = FBControlCopyText( view );
					if ( tempStr )
					{
						CFIndex stringLength = CFStringGetLength( tempStr );
						CFRange range = CFRangeMake( 0, 0 );
						currentLine = 0;
						while ( range.location < stringLength && currentLine != selStart )
						{
							CFStringGetLineBounds( tempStr, range, &lineBeginIndex, &lineEndIndex, &contentsEndIndex);
							currentLine++;
							range.location = lineEndIndex;
						}
						contentStr = CFStringCreateMutableCopy( NULL, 0, tempStr );
						if ( contentStr )
						{
							contentLength = CFStringGetLength( contentStr );
							if ( selStart >= contentLength ) selStart = contentLength;
							//if ( selEnd >= contentLength ) selEnd = contentLength;	/* #469 */
							//if ( selEnd < selStart ) FBSwap( &selStart, &selEnd, 4 );	/* #469 */
							
							if ( currentLine == selStart )
							{
								CFStringReplace( contentStr, CFRangeMake( lineBeginIndex, contentsEndIndex-lineBeginIndex ), string );
							}
							else
							{
								CFStringAppend( contentStr, string );
								lineBeginIndex = stringLength;
							}
							ControlSetText( view, contentStr );
							sel.selStart = lineBeginIndex;
							sel.selEnd = lineBeginIndex + CFStringGetLength( string );
							SetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel );
							CFRelease( contentStr );
						}
						CFRelease( tempStr );
					}
				}
				else															// selStart, selEnd
				{
					tempStr = FBControlCopyText( view );
					if ( tempStr )
					{
						contentStr = CFStringCreateMutableCopy( NULL, 0, tempStr );
						if ( contentStr )
						{	
							contentLength = CFStringGetLength( contentStr );
							if ( selStart >= contentLength ) selStart = contentLength;
							if ( selEnd >= contentLength ) selEnd = contentLength;
							if ( selEnd < selStart ) FBSwap( &selStart, &selEnd, 4 );
							CFStringReplace( contentStr, CFRangeMake( selStart, selEnd-selStart ), string );
							ControlSetText( view, contentStr );
							sel.selStart = selStart;
							sel.selEnd = selStart + CFStringGetLength( string );
							SetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel );
							CFRelease( contentStr );
						}
						CFRelease( tempStr );
					}
				}
			}
			CFRelease( string );
		}
		
		// font, size, style, mode, red, green, blue
		if ( font != kFBParamMissing || size != kFBParamMissing || style != kFBParamMissing || mode != kFBParamMissing || (long)red != kFBParamMissing )
		{
			if ( GetControlData( view, kControlEditTextPart, kControlEditTextStyleTag, sizeof(ControlFontStyleRec), &cfs, NULL ) == noErr )
			{
				// font
				if ( font != kFBParamMissing )
				{
					cfs.flags = cfs.flags | kControlUseFontMask;
					cfs.font = font;
				}
				
				// size
				if ( size != kFBParamMissing )
				{
					cfs.flags = cfs.flags | kControlUseSizeMask;
					cfs.size = size;
				}
				
				// style
				if ( style != kFBParamMissing )
				{
					cfs.flags = cfs.flags | kControlUseFaceMask;
					cfs.style = style;
				}
				
				// mode
				if ( mode != kFBParamMissing )
				{
					cfs.flags = cfs.flags | kControlUseModeMask;
					cfs.mode = mode;
				}
				
				// rgb
				if ( (long)red != kFBParamMissing )
				{
					cfs.flags = cfs.flags | kControlUseForeColorMask;
					if ( green == kFBParamMissing )//( red <= -1 || red > 65535 )
					{
						cfs.foreColor = *(RGBColor*)red;
					}
					else
					{
						cfs.foreColor.red = (long)red;
						cfs.foreColor.green = green;
						cfs.foreColor.blue = blue;
					}
				}
				
				SetControlData( view, kControlEditTextPart, kControlEditTextStyleTag, sizeof( ControlFontStyleRec ), &cfs );
			}
		}
	}
	else
	{
		PSstrcpy( temp, "\pInvalid id in edit$ statement. Either there is no output window, or the window does not contain an edit field with id " );
		PSstrcat( temp, PSstr( id ) );
		gFBTStkP = 0;
		RuntimeErrMsg( PSstrcpy( gFBTStk[++gFBTStkP], temp ) );
	}
}


ControlKeyFilterResult FBAnyKey( ControlRef  c, SInt16* keyCode  __attribute__ ((__unused__)), SInt16* charCode, EventModifiers* modifiers )
{
	OSStatus		err;
	FBOnXxxxxVector customFilter;
	
	switch ( *charCode )
	{
		case kLeftArrowCharCode:
		case kRightArrowCharCode:
		case kUpArrowCharCode:
		case kDownArrowCharCode:
		case kBackspaceCharCode:
		case kDeleteCharCode:
			return kControlKeyFilterPassKey;
			break;
	}
	
	gFBTEKey[0] = 1;
	gFBTEKey[1] = *charCode;
	if ( *modifiers & cmdKeyBit ) return kControlKeyFilterBlockKey;			// command key     FBtoC #476
	err = GetControlProperty( c, kFBtoCAppSignature, kFBEFFilter, sizeof(void*), NULL, &customFilter);
	if ( err == noErr )
	{
		(*customFilter)();
		return kControlKeyFilterBlockKey;	
	}
	if ( gFBOnEditVector )
	{
		(*gFBOnEditVector)();
		return kControlKeyFilterBlockKey;	
	}
	
	return kControlKeyFilterPassKey;
} 


void FBEditFieldClose( long id )
{
	HIViewRef view = FBButtonAmpersandFunction( id );
	if ( view ) DisposeControl( view );
}


#if 1   // kCFAppWndBtn and kCFEditField

void FBEditField( long id, void *txtPtr, Rect *rectPtr, long type, long class, void *filterProc, long textType )
/*
 edit field efID, [string | ZTXTHandle | TEXTResID | container][, [rect][, [type][, [class][, filterProc]]]]
 edit field 0
 
 Bugs:
 - not all field types supported
 - control based equivalent of _framedNoCR inserts space when CR pressed
 - multistyled fields not supported
 - noFocus not supported
 - copyOnly not supported
 */
{
    ControlFontStyleRec		cfs;
    static ControlKeyFilterUPP	sFilterUPP;
    CFStringRef				string = NULL;
    HIViewRef				view;
    Handle					h = NULL;
    SInt16					just;
    Boolean					noFocus, copyOnly, flag;
    //    Boolean                 disable;
    
    WindowRef w = FBGetCurOutputWindow();
    
    if ( id == 0 ) {																// remove all focus
        ClearKeyboardFocus( w );
    } else {
        //		disable = ( id < 0 );
        id = abs( id );
        view = FBButtonAmpersandFunction( id );
        if ( view )	{															// already exists
            view = FBAppearanceButton( id, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, 0, rectPtr, 0, false );
        } else {
            // options
            noFocus = ( ( type & 0x100 ) != 0 );
            copyOnly = ( ( type & 0x800 ) != 0 );
            
            // type
            type = ( type & 0xFF );
            if ( type == 0 ) type = kEFFramed;
            switch ( type ) {
                case kEFFramedNoCR:
                case kEFFramed:
                case kEFNoFramedNoCR:
                case kEFNoFramed:
                    view = FBAppearanceButton( id, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, 0, rectPtr, kControlEditUnicodeTextProc, false );
                    
                    if ( type == kEFFramedNoCR || type == kEFNoFramedNoCR ) {	// no CR
                        flag = true;
                        SetControlData( view, kControlEditTextPart, kControlEditTextSingleLineTag, sizeof(Boolean), &flag );
                    }
                    
                    if ( noFocus ) {
                        
                    }
                    
                    if ( copyOnly ) {												// read only
                        
                    }
                    break;
                    
                default:														// kEFStatNoFramed
                    view = FBAppearanceButton( id, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, 0, rectPtr, kControlStaticTextProc, false );
                    break;
            }
            
            // font, size, style
            cfs.flags = kControlUseFontMask | kControlUseSizeMask | kControlUseFaceMask;
            cfs.font = gTextFont;
            cfs.size = gTextSize;
            cfs.style = gTextFace;
            SetControlFontStyle( view, &cfs );
        }
        
        // just & class
        if ( class != kFBParamMissing ) {
            // just
            just = ( class & 0x3 );
            if ( just ) {
                GetControlData( view, kControlEditTextPart, kControlEditTextStyleTag, sizeof(ControlFontStyleRec), &cfs, NULL );
                switch ( class & 0x3 ) {
                    case kEFCenterJust:
                        just = teCenter;
                        break;
                    case kEFRightJust:
                        just = teFlushRight;
                        break;
                    default:
                        just = teFlushDefault;
                        break;
                }
                cfs.flags = cfs.flags | kControlUseJustMask;
                cfs.just = just;
                SetControlFontStyle( view, &cfs );
            }
            
            // class
            SetControlProperty( view, 0, kFBEFClassTag, sizeof(long), &class );
        }
        
        // keyFilter
        if ( ! sFilterUPP ) sFilterUPP = NewControlKeyFilterUPP( &FBAnyKey );
        SetControlData( view, kControlEditTextPart, kControlKeyFilterTag, sizeof( ControlKeyFilterUPP ), &sFilterUPP );
        if ( filterProc ) SetControlProperty( view, kFBtoCAppSignature, kFBEFFilter, sizeof( void* ), &filterProc );
        
        // text
        if ( txtPtr )
        {
            switch ( textType )
            {
                case kEFCFStringType:
                    string = CFStringCreateCopy( kCFAllocatorDefault, txtPtr );
                    break;
                    
                case kEFPascalType:
                    gFBStk--;
                    string = CFStringCreateWithPascalString( NULL, txtPtr, kCFStringEncodingMacRoman );
                    break;
                    
                case kEFZTXTHandleType:
                    if ( txtPtr ) string = CFStringCreateWithBytes( NULL, *(UInt8**)txtPtr, GetHandleSize( txtPtr ), kCFStringEncodingMacRoman, false );
                    break;
                    
                case kEFTEXTResIDType:
                    h = GetResource( 'TEXT', (int)txtPtr );
                    if ( h ) {
                        string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), kCFStringEncodingMacRoman, false );
                        ReleaseResource( h );
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////
                    // new 'pointer' ( compared to prior 'Handle' ) container support for Edit$.
                    // 'long' cast suspect but used for 32-bit Carbon edit fields
                    // Brian 20170905
                    ////////////////////////////////////////////////////////////////////////////    
                case kEFContainerType: {
                    void *p = (void*)*(long*)txtPtr;
                    CFIndex ptrSize = (CFIndex)CtnrRequestedSize( p );
                    if ( p ) string = CFStringCreateWithBytes( NULL, p, ptrSize, kCFStringEncodingMacRoman,  false );
                    break;
                    //h = *(Handle*)txtPtr;
                    //if ( h ) string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), kCFStringEncodingMacRoman,  false );
                    //h = *(Handle*)txtPtr;
                    //if ( h ) string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), //kCFStringEncodingMacRoman, false );
                    //break;
                }
            }
            if ( string ) {
                ControlSetText( view, string );
                CFRelease( string );
            }
        }
        SetKeyboardFocus( w, view, kControlEditTextPart );
    }
}

#else

void FBEditField( long id, void * txtPtr, Rect * rectPtr, long type, long class, void * filterProc, long textType )
/*
 edit field efID, [string | ZTXTHandle | TEXTResID | container][, [rect][, [type][, [class][, filterProc]]]]
 edit field 0
 
 Bugs:
 - not all field types supported
 - control based equivalent of _framedNoCR inserts space when CR pressed
 - multistyled fields not supported
 - noFocus not supported
 - copyOnly not supported
 */
{
    ControlFontStyleRec		cfs;
    static ControlKeyFilterUPP	sFilterUPP;
    CFStringRef				string = NULL;
    HIViewRef				view;
    Handle					h = NULL;
    SInt16					just;
    Boolean					noFocus, copyOnly, flag;
    //    Boolean                 disable;
    
    WindowRef w = FBGetCurOutputWindow();
    
    if ( id == 0 )																// remove all focus
    {
        ClearKeyboardFocus( w );
    }
    else
    {
        //		disable = ( id < 0 );
        id = abs( id );
        view = FBButtonAmpersandFunction( id );
        if ( view )																// already exists
        {
            view = FBAppearanceButton( id, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, 0, rectPtr, 0 );
        }
        else
        {
            // options
            noFocus = ( ( type & 0x100 ) != 0 );
            copyOnly = ( ( type & 0x800 ) != 0 );
            
            // type
            type = ( type & 0xFF );
            if ( type == 0 ) type = kEFFramed;
            switch ( type )
            {
                case kEFFramedNoCR:
                case kEFFramed:
                case kEFNoFramedNoCR:
                case kEFNoFramed:
                    view = FBAppearanceButton( id, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, 0, rectPtr, kControlEditUnicodeTextProc );
                    
                    if ( type == kEFFramedNoCR || type == kEFNoFramedNoCR )	// no CR
                    {
                        flag = true;
                        SetControlData( view, kControlEditTextPart, kControlEditTextSingleLineTag, sizeof(Boolean), &flag );
                    }
                    
                    if ( noFocus )
                    {
                        
                    }
                    
                    if ( copyOnly )												// read only
                    {
                        
                    }
                    break;
                    
                default:														// kEFStatNoFramed
                    view = FBAppearanceButton( id, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, 0, rectPtr, kControlStaticTextProc );
                    break;
            }
            
            // font, size, style
            cfs.flags = kControlUseFontMask | kControlUseSizeMask | kControlUseFaceMask;
            cfs.font = gTextFont;
            cfs.size = gTextSize;
            cfs.style = gTextFace;
            SetControlFontStyle( view, &cfs );
        }
        
        // just & class
        if ( class != kFBParamMissing )
        {
            // just
            just = ( class & 0x3 );
            if ( just )
            {
                GetControlData( view, kControlEditTextPart, kControlEditTextStyleTag, sizeof(ControlFontStyleRec), &cfs, NULL );
                switch ( class & 0x3 )
                {
                    case kEFCenterJust:
                        just = teCenter;
                        break;
                    case kEFRightJust:
                        just = teFlushRight;
                        break;
                    default:
                        just = teFlushDefault;
                        break;
                }
                cfs.flags = cfs.flags | kControlUseJustMask;
                cfs.just = just;
                SetControlFontStyle( view, &cfs );
            }
            
            // class
            SetControlProperty( view, 0, kFBEFClassTag, sizeof(long), &class );
        }
        
        // keyFilter
        if ( ! sFilterUPP ) sFilterUPP = NewControlKeyFilterUPP( &FBAnyKey );
        SetControlData( view, kControlEditTextPart, kControlKeyFilterTag, sizeof( ControlKeyFilterUPP ), &sFilterUPP );
        if ( filterProc ) SetControlProperty( view, kFBtoCAppSignature, kFBEFFilter, sizeof( void* ), &filterProc );
        
        // text
        if ( txtPtr )
        {
            switch ( textType )
            {
                case kEFPascalType:
                    gFBStk--;
                    string = CFStringCreateWithPascalString( NULL, txtPtr, kCFStringEncodingMacRoman );
                    break;
                    
                case kEFZTXTHandleType:
                    if ( txtPtr ) string = CFStringCreateWithBytes( NULL, *(UInt8**)txtPtr, GetHandleSize( txtPtr ), kCFStringEncodingMacRoman, false );
                    break;
                    
                case kEFTEXTResIDType:
                    h = GetResource( 'TEXT', (int)txtPtr );
                    if ( h )
                    {
                        string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), kCFStringEncodingMacRoman, false );
                        ReleaseResource( h );
                    }
                    break;
                    
                case kEFContainerType:
                    h = *(Handle*)txtPtr;
                    if ( h ) string = CFStringCreateWithBytes( NULL, *(UInt8**)h, GetHandleSize( h ), kCFStringEncodingMacRoman, false );
                    break;
            }
            if ( string )
            {
                ControlSetText( view, string );
                CFRelease( string );
            }
        }
        SetKeyboardFocus( w, view, kControlEditTextPart );
    }
}

#endif




void FBEditText( long id, long font, long size, long style, long mode, RGBColor *foreRGB, RGBColor *backRGB )
/*
 edit text [efID],[font][, [size][, [style][, [mode][, [foreRGB][, backRGB]]]]]
 
 Bugs:
 - edit text not yet implemented
 */
{
	// to stop unused parm warning
//	id += 0;
//	font += 0;
//	size += 0;
//	style += 0;
//	mode += 0;
//	foreRGB += 0;
//	backRGB += 0;
}



static void OurCFStringGetPascalString( CFStringRef inString, void * outStr ) __attribute__ ((noinline));
static void OurCFStringGetPascalString( CFStringRef inString, void * outStr )
{
	/*
	 CFStringGetPascalString() stupidly returns a null Str255 if CFString length > 255
	 but OurCFStringGetPascalString() returns a Str255 of length 255 // #458 rp:20090723
	 */
	// #458 Brian:20090725
	CFIndex			length = CFStringGetLength( inString );
	if ( length > 255 ) length = 255;
	CFStringRef		tempStr = CFStringCreateWithSubstring( kCFAllocatorDefault, inString, CFRangeMake( 0, length ) );
	CFStringGetPascalString( tempStr, outStr, 256, kCFStringEncodingMacRoman );
	CFRelease( tempStr);	
}

StringPtr FBEditDollarFunction( long id, long selStart, long selEnd )
{
	/*
	 string$ = edit$(efID)
	 string$ = edit$(efID, lineNumber)
	 string$ = edit$(efID, -1)
	 string$ = edit$(efID, selStart, selEnd)
	 */
	Str255							theStr = "\p";
	ControlEditTextSelectionRec		sel;
	CFStringRef						tempStr;
	
	HIViewRef view = FBButtonAmpersandFunction( id );
	if ( view )
	{
		CFStringRef string = FBControlCopyText( view );
		if ( string )
		{
			if ( selStart == kFBParamMissing && selEnd == kFBParamMissing )
			{
				OurCFStringGetPascalString( string, &theStr ); 
			}
			else
			{
				if ( selEnd == kFBParamMissing )								// lineNumber or current selection
				{
					if ( selStart == -1 )										// current selection
					{
						GetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof( ControlEditTextSelectionRec ), &sel, NULL );
						tempStr = CFStringCreateWithSubstring( NULL, string, CFRangeMake( sel.selStart, sel.selEnd - sel.selStart ) );
						if ( tempStr )
						{
							OurCFStringGetPascalString( tempStr, &theStr ); 
							CFRelease( tempStr );
						}
					}
					else														// lineNumber
					{
						CFIndex stringLength = CFStringGetLength( string );
						CFRange range = CFRangeMake( 0, 0 );
						CFIndex currentLine = -1 /* #469 */, lineBeginIndex, lineEndIndex, contentsEndIndex;
						while ( range.location < stringLength && currentLine != selStart )
						{
							CFStringGetLineBounds( string, range, &lineBeginIndex, &lineEndIndex, &contentsEndIndex);
							currentLine++;
							range.location = lineEndIndex;
						}
						if ( currentLine == selStart )
						{
							tempStr = CFStringCreateWithSubstring( NULL, string, CFRangeMake( lineBeginIndex, contentsEndIndex - lineBeginIndex ) );
							if ( tempStr )
							{
								OurCFStringGetPascalString( tempStr, &theStr ); 
								CFRelease( tempStr );
							}
						}
					}
				}
				else															// selStart, selEnd
				{
					tempStr = CFStringCreateWithSubstring( NULL, string, CFRangeMake( selStart, selEnd - selStart ) );
					if ( tempStr )
					{
						OurCFStringGetPascalString( tempStr, &theStr ); 
						CFRelease( tempStr );
					}
				}
			}
			CFRelease( string );
		}
	}
	return PSstrcpy( gReturnedString, theStr );
}


/*  replaced by NewPointerFromCFString - Brian 20170905
static Handle NewHandleFromCFString( CFStringRef string ) __attribute__ ((noinline));
static Handle NewHandleFromCFString( CFStringRef string )
{
    CFIndex           length;
    Handle            h;
	
    length = CFStringGetLength( string );
    h = NewHandle( length );
    CFStringGetBytes( string, CFRangeMake( 0, length ), kCFStringEncodingMacRoman, 0, 0, (UInt8*)*h, length, 0 );
	return h;
}
*/
void * NewPointerFromCFString( CFStringRef string ) {
    CFIndex length = CFStringGetLength( string );
    void *p        = FBNewContainer( length );
    CFStringGetBytes( string, CFRangeMake( 0, length ), kCFStringEncodingMacRoman, 0, 0, p, length, 0 );
    return p;
}
//
// Now returns a pointer instead of a Handle for the new container support. Brian 20170905
//
//Handle * FBEditDollarCtnrFunction( long id, long selStart, long selEnd ) {
void * FBEditDollarCtnrFunction( long id, long selStart, long selEnd ) {
	/*
	 container$$ = edit$(efID)
	 container$$ = edit$(efID, lineNumber)
	 container$$ = edit$(efID, -1)
	 container$$ = edit$(efID, selStart, selEnd)
	 */
    //Handle          newHndl = NULL;
    void *newPointer = NULL;
    CFStringRef     tempStr;
    ControlEditTextSelectionRec  sel;
	HIViewRef view = FBButtonAmpersandFunction( id );
	FBClearTempContainer();
    if ( view ) {
        CFStringRef string = FBControlCopyText( view );
        if ( string ) {
            if ( selStart == kFBParamMissing && selEnd == kFBParamMissing ){
                //newHndl = NewHandleFromCFString( string );
                newPointer = NewPointerFromCFString( string );
            } else {
                if ( selEnd == kFBParamMissing ) {                               // lineNumber or current selection
                    if ( selStart == -1 ) {                                      // current selection
                        GetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof( ControlEditTextSelectionRec ), &sel, NULL );
                        tempStr = CFStringCreateWithSubstring( NULL, string, CFRangeMake( sel.selStart, sel.selEnd - sel.selStart ) );
                        if ( tempStr ){
                            //newHndl = NewHandleFromCFString( tempStr );
                            newPointer = NewPointerFromCFString( tempStr );
                            CFRelease( tempStr );
                        }
                    } else {                                                      // lineNumber
                        CFIndex stringLength = CFStringGetLength( string );
                        CFRange range = CFRangeMake( 0, 0 );
                        CFIndex currentLine = -1 /* #469 */, lineBeginIndex, lineEndIndex, contentsEndIndex;
                        while ( range.location < stringLength && currentLine != selStart ) {
                            CFStringGetLineBounds( string, range, &lineBeginIndex, &lineEndIndex, &contentsEndIndex);
                            currentLine++;
                            range.location = lineEndIndex;
                        }
                        if ( currentLine == selStart ){
                            tempStr = CFStringCreateWithSubstring( NULL, string, CFRangeMake( lineBeginIndex, contentsEndIndex - lineBeginIndex ) );
                            if ( tempStr ) {
                                //newHndl = NewHandleFromCFString( tempStr );
                                newPointer = NewPointerFromCFString( tempStr );
                                CFRelease( tempStr );
                            }
                        }
                    }
                } else {                                                          // selStart, selEnd
                    tempStr = CFStringCreateWithSubstring( NULL, string, CFRangeMake( selStart, selEnd - selStart ) );
                    if ( tempStr ) {
                        //newHndl = NewHandleFromCFString( tempStr );
                        newPointer = NewPointerFromCFString( tempStr );
                        CFRelease( tempStr );
                    }
                }
            }
            CFRelease( string );
        }
    }
    gFBTempContainer = newPointer;
    return &gFBTempContainer;
}




void FBTekeyDollarStatement( void *txtPtr )
{
	// tekey$ = string$
	ControlEditTextSelectionRec		sel;
	HIViewID						viewID;
	HIViewRef						view = 0;
	
	GetKeyboardFocus( FBGetCurOutputWindow(), &view );
	if ( view )
	{
		if ( GetControlID( view, &viewID ) == noErr )
		{
			// replace current selection
			FBEditDollarStatement( viewID.id, -1, kFBParamMissing, txtPtr, kFBParamMissing, kFBParamMissing, kFBParamMissing, kFBParamMissing, (void*)kFBParamMissing, kFBParamMissing, kFBParamMissing, kEFPascalType );
			
			// move ibeam to end of insertion
			GetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel, NULL );
			sel.selStart = sel.selEnd;
			SetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel );
		}
	}
}


void FBSetSelect( long selStart, long selEnd )
{
	/*
	 SetSelect selStart, selEnd
	 */
	ControlEditTextSelectionRec		sel = { selStart, selEnd };
	HIViewRef						view = 0;
	
	GetKeyboardFocus( FBGetCurOutputWindow(), &view );
	if ( view )
	{
		SetControlData( view, kControlEditTextPart, kControlEditTextSelectionTag, sizeof(ControlEditTextSelectionRec), &sel );
	}
}


void FBGetField( Handle *hndlPtr, long id )
{
	/*
	 get field hndl, efID
	 */
	Handle	h;
	
	*hndlPtr = 0;
	
	HIViewRef view = FBButtonAmpersandFunction( id );
	if ( view )
	{
		CFStringRef string = FBControlCopyText( view );
		if ( string )
		{
			CFIndex size = CFStringGetLength( string );
			h = NewHandle( size );
			if ( h )
			{
				CFStringGetBytes( string, CFRangeMake( 0, size ), kCFStringEncodingMacRoman, 0, false, (UInt8*)*h, size, NULL );
				*hndlPtr = h;
			}
			CFRelease( string );
		}
	}
}



#endif  /* !__LP64__ */




