#include "FBtoC.h"
#include "Runtime.h"


#define kMaxStringLen        255

//#define kMaxStringStackLevel 32
enum { kMaxStringStackLevel = 64 };

Str255			gFBStrStk[kMaxStringStackLevel];		// string stack
SInt16	        gFBStk;									// index to string stack (for volatile see <http://www.gnu.org/software/autoconf/manual/html_node/Volatile-Objects.html>)
Str255			gReturnedString;						// return value from functions: myStringFn$, PSleft, PSinkey, PSstr...
/*
 Stash for string arguments passed to Toolbox functions.
 Single-level, i.e. doesn't allow nested calls.
 */
Str255		gFBTStk[16];		// toolbox parameter stack;  16 is max #parameters allowed by FBtoC
SInt8		gFBTStkP;


UInt8		gPSDefStrSize = sizeof( SInt32 );		// default defstr long
UInt8		gPSDefTabStops = 16;					// default def tab value
Boolean		gFBLeadingSpace = true;
UInt16		gFBPrintPos;							// see PrintStatement
char		gFBUsingSymbols[4] = ".,;$";			// default def using chars
UInt32		gFBNextData;							// support for data/read/restore

#define kMaxGosubLevel  1024
#if USE_SJLJ_FOR_GOSUB_RETURN
jmp_buf gReturnEnv[1024];
#else
UInt16		gReturnLabelNum[kMaxGosubLevel];
#endif
UInt16		gReturnLevel;

#define     kMaxSelectLevel 10
long        gSelectL[kMaxSelectLevel];
double      gSelectD[kMaxSelectLevel];
Str255      gSelectS[kMaxSelectLevel];

OSStatus	gFBSysError;   // for syserror   18-Nov-2007 BW

volatile FBBoolean  gFBSndBusy; // see Subs Sound.incl

StringPtr STACK_PUSH() {
    if ( gFBStk >= kMaxStringStackLevel ) {
        CFStringRef format = CFSTR("Pascal String stack index ( gFBStk ) exceeds string stack size of %d. FBtoC will terminate" );
        CFStringRef msg    = CFStringCreateWithFormat( kCFAllocatorDefault, 0, format, kMaxStringStackLevel );
        CFStringRef info   = CFSTR("If 'Source File Combining'( in Build Settings ) is turned OFF, try setting to Medium or Maximum");
        FBFullStop( msg, info, ZTRUE );
        CFRelease( msg );
    }
    ++gFBStk;
    return gFBStrStk[gFBStk];
}

///////////////////////////////////////////////////////////
//                       prototypes                      //
///////////////////////////////////////////////////////////

void DebugPrintStackLevel();
StringPtr PSstr( const double n );
void	FBKillDynamicArray( const int dynArrayNum );
void	FBGrowDynamicArray( const int dynArrayNum, const int index );
void FBSwap( void* p1, void* p2, const int bytes );
StringPtr PScstr( Str255 s );


///////////////////////////////////////////////////////////
//              stringlist localizablity                 //
///////////////////////////////////////////////////////////

StringPtr PSltrl( char * inCString, char * commentForGenstrings __attribute__ ((__unused__))  ) {
    /*
     One piece of the localization jigsaw.
     PSltrl() is structured like CFCopyLocalizedString(), so that
     it works with genstrings. For example:
     $ genstrings -macRoman -s PSltrl *.c
     Unlike CFCopyLocalizedString(), it takes a C string key, and
     returns the corresponding (possibly localized) value as a Pascal string.
     */
    static Str255	sReturnedString;
    CFStringRef		key, value;
    
    key = CFStringCreateWithCString( kCFAllocatorDefault, inCString, kCFStringEncodingMacRoman );
    value = CFBundleCopyLocalizedString( CFBundleGetMainBundle(), key, key, NULL );
    CFRelease( key );
    CFStringGetPascalString( value, sReturnedString, 256, kCFStringEncodingMacRoman );
    CFRelease( value );
    return sReturnedString;
}


StringPtr PSltrr( char * inCString, char * commentForGenstrings __attribute__ ((__unused__))  ) {
    static Str255	sReturnedString;
    CFStringRef		key, value;
    
    key = CFStringCreateWithCString( kCFAllocatorDefault, inCString, kCFStringEncodingMacRoman );
    value = CFBundleCopyLocalizedString( CFBundleGetMainBundle(), key, key, CFSTR( "Runtime") );
    CFRelease( key );
    CFStringGetPascalString( value, sReturnedString, 256, kCFStringEncodingMacRoman );
    CFRelease( value );
    return sReturnedString;
}

///////////////////////////////////////////////////////////
//         used if _debugPrintStringStackLevel           //
///////////////////////////////////////////////////////////
void DebugPrintStackLevel()
{
    printf( "gFBStk = %d\n", gFBStk );
}



///////////////////////////////////////////////////////////
//                 dynamic array support                 //
///////////////////////////////////////////////////////////

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#define PAGE_MASK 4095
#endif


DynArrayInfo gFBDynArrayInfo[kMaxNumDynArrays];


void FBKillDynamicArray( const int dynArrayNum ) __attribute__ ((noinline));
void FBKillDynamicArray( const int dynArrayNum ) {
    if ( gFBDynArrayInfo[dynArrayNum].base ) free( gFBDynArrayInfo[dynArrayNum].base );
    gFBDynArrayInfo[dynArrayNum].base = NULL;
    gFBDynArrayInfo[dynArrayNum].maxIndex = 0;
    gFBDynArrayInfo[dynArrayNum].lastElem = -1;
}

//#if kNSFBFullStop

void FBGrowDynamicArray( const int dynArrayNum, const int index ) __attribute__ ((noinline));
void FBGrowDynamicArray( const int dynArrayNum, const int index ) {
    /*
     Allocate or enlarge a dynamic array.
     Called from FBDynArrayReference() if required.
     */
    if ( index < 0 || index == INT_MAX ) { // common mistakes in FB programming
        CFStringRef s1 = (CFStringRef)NSLocalizedStringFromTable(@"Bad index ",@"Runtime",@"FBGrowDynamicArray");
        CFStringRef s2 = (CFStringRef)NSLocalizedStringFromTable(@" for dynamic array (ref ",@"Runtime",@"FBGrowDynamicArray");
        CFStringRef msgString = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@%d%@%d)"),s1,index,s2,dynArrayNum );
        FBFullStop( msgString, CFSTR(""), true );
        CFRelease( msgString );
        
        /*
         char *buf;
         asprintf( &buf, "Bad index (%d) while growing dynamic array (ref %d)\n", index, dynArrayNum );
         FBFatalRuntimeError( buf );
         */
    }
    
    // make the new size a multiple of the page size (typically 4096 bytes)
    int		newMaxIndex = (((index + 1)*gFBDynArrayInfo[dynArrayNum].elemSize + PAGE_SIZE) & ~PAGE_MASK)/gFBDynArrayInfo[dynArrayNum].elemSize;
    int     newSize		= newMaxIndex*gFBDynArrayInfo[dynArrayNum].elemSize;
    
    if ( gFBDynArrayInfo[dynArrayNum].base == NULL ) {
        gFBDynArrayInfo[dynArrayNum].base = calloc( 1, newSize ); // allocate from scratch
    } else {
        gFBDynArrayInfo[dynArrayNum].base = realloc( gFBDynArrayInfo[dynArrayNum].base, newSize ); // enlarge
        int		oldSize = gFBDynArrayInfo[dynArrayNum].maxIndex*gFBDynArrayInfo[dynArrayNum].elemSize;
        bzero( gFBDynArrayInfo[dynArrayNum].base + oldSize, newSize - oldSize );
    }
    gFBDynArrayInfo[dynArrayNum].maxIndex = newMaxIndex;
}



void FBWriteDynamic( int fileID, int dynArrayNum ){
    int		itemSize, nextIndex, size;
    void *	base;
    
    itemSize  = gFBDynArrayInfo[dynArrayNum].elemSize;
    base      = gFBDynArrayInfo[dynArrayNum].base;
    nextIndex = gFBDynArrayInfo[dynArrayNum].lastElem + 1;
    size      = nextIndex*itemSize;
    if ( base ) {
        FBWriteSwapLong( size, fileID );
        FBWriteFile( fileID, base, size );
    } else {
        FBWriteFile( fileID, &base, 4 );
    }
}


void FBReadDynamic( int fileID, int dynArrayNum ) {
    int		itemSize, itemCount, size;
    void *	base;
    
    itemSize = gFBDynArrayInfo[dynArrayNum].elemSize;
    base     = gFBDynArrayInfo[dynArrayNum].base;
    if ( base ) { free( base ); }
    size     = FBReadSwapLong( fileID );
    base     = (void*)( malloc( size ) );
    FBReadFile( fileID, base, false, size );
    if ( size % itemSize ) {
        //FBStopMsg( PSstrcpy( TO_STACK, PSltrr ( "read dynamic: array element size does not match target", "FBReadDynamic" ) ), 0 );
        CFStringRef e1 = CFStringCreateCopy( NULL, CFSTR( "READ DYNAMIC: array element size does not match target" ) );
        CFStringRef e2 = CFStringCreateCopy( NULL, CFSTR( "Was the file created with WRITE DYNAMIC?" ) );
        FBFullStop( e1, e2, false );
        CFRelease( e1 );
        CFRelease( e2 );
        free( base );
    } else {
        itemCount = size/itemSize;
        gFBDynArrayInfo[dynArrayNum].base = base;
        gFBDynArrayInfo[dynArrayNum].maxIndex = itemCount;
        gFBDynArrayInfo[dynArrayNum].lastElem = itemCount - 1;
    }
}




///////////////////////////////////////////////////////////
//                         swap                          //
///////////////////////////////////////////////////////////

void FBSwap( void* p1, void* p2, const int bytes ) __attribute__ ((noinline));
void FBSwap( void* p1, void* p2, const int bytes ) {
    UInt8	swap1;
    UInt16	swap2;
    UInt32	swap4;
    UInt64	swap8;
    char	buffer[256];
    void*	tmp;
    
    switch ( bytes ) {
        case 1  : swap1 = *(UInt8*)p1;  *(UInt8*)p1 =  *(UInt8*)p2;  *(UInt8*)p2  = swap1; break;
        case 2  : swap2 = *(UInt16*)p1; *(UInt16*)p1 = *(UInt16*)p2; *(UInt16*)p2 = swap2; break;
        case 4  : swap4 = *(UInt32*)p1; *(UInt32*)p1 = *(UInt32*)p2; *(UInt32*)p2 = swap4; break;
        case 8  : swap8 = *(UInt64*)p1; *(UInt64*)p1 = *(UInt64*)p2; *(UInt64*)p2 = swap8; break;
        default :
            if ( bytes <= 256) 	{ // faster than malloc and will cover most cases
                memmove( &buffer, p1, bytes );  // Brian 01-August-2014
                memmove( p1, p2, bytes );  // Brian 01-August-2014
                memmove( p2, &buffer, bytes );  // Brian 01-August-2014
            } else {
                tmp = malloc( bytes );
                memmove( tmp, p1, bytes );  // Brian 01-August-2014
                memmove( p1, p2, bytes );  // Brian 01-August-2014
                memmove( p2, tmp, bytes );  // Brian 01-August-2014
                free( tmp );
            }
    }
}



///////////////////////////////////////////////////////////
//               C string <-> Pascal string              //
///////////////////////////////////////////////////////////

StringPtr PScstr( Str255 s ) __attribute__ ((noinline));
StringPtr PScstr( Str255 s ) {
    //Convert Pascal to C string, in place
    UInt8	len, k = 0;
    
    len = s[0];
    while ( k < len && k < kMaxStringLen ) {
        s[k] = s[k + 1];
        if( s[k] == 0 )  s[k] = 1;
        k++;
    }
    s[k] = 0; // 0-terminate
    return s;
}



StringPtr CSpstr( char * s ) __attribute__ ((noinline));
StringPtr CSpstr( char * s ) {
    //Convert C to Pascal string, in place
    UInt8	len = strlen( s );
    UInt8	k = 0;
    
    if ( len >= kMaxStringLen ) len = kMaxStringLen;
    k = len;
    while ( k > 0 ) {
        s[k] = s[k - 1];
        k--;
    }
    s[0] = len;
    return (StringPtr)s;
}


///////////////////////////////////////////////////////////////
//   functions that return a StringPtr to Pascal FB string   //
///////////////////////////////////////////////////////////////

StringPtr PSstrcpy ( StringPtr dest, ConstStr255Param source ) {
    memmove( dest, &source[0], source[0] + 1 );  // Brian 01-August-2014
    return dest;
}


StringPtr PSstrcat( StringPtr dest, ConstStr255Param appendStr) {
    SInt16            destSize    = *(unsigned char*)dest;
    SInt16            appendSize  = appendStr[0];
    StringPtr         origDestPtr = dest;
    
    if ( destSize + appendSize > 255 ){ appendSize = 255 - destSize; }
    *(unsigned char*)dest = destSize + appendSize;
    dest += destSize + 1;
    memmove( dest, &appendStr[1], appendSize );  // Brian 01-August-2014
    return (origDestPtr );
}


// size-limited form of PSstrcpy()
StringPtr PSstrcpyn( StringPtr dest, ConstStr255Param source, const int destSize ) __attribute__ ((noinline));
StringPtr PSstrcpyn( StringPtr dest, ConstStr255Param source, const int destSize ) {
    unsigned char   charsToCopy = *source;
    unsigned char * origDestAddr = dest;
    
    if ( charsToCopy > destSize - 1 ) { charsToCopy = destSize - 1; }
    *dest = charsToCopy;
    while ( charsToCopy ) {
        *(++dest) = *(++source);
        charsToCopy--;
    }
    return origDestAddr;
}


// size-limited form of PLstrcat
StringPtr PSstrcatn( StringPtr dest, ConstStr255Param source, const int destSize ) __attribute__ ((noinline));
StringPtr PSstrcatn( StringPtr dest, ConstStr255Param source, const int destSize ) {
    unsigned char   charsToCopy;
    unsigned char * origDestAddr = dest;
    
    charsToCopy = *source;
    if ( charsToCopy > destSize - *dest - 1 ) { charsToCopy = destSize - *dest - 1; }
    dest += *dest;					// so we append at end of existing dest string
    *origDestAddr += charsToCopy;	// length of result
    while ( charsToCopy ) {
        *(++dest) = *(++source);
        charsToCopy--;
    }
    return origDestAddr;
}


void PSdeftab( const int tabstops ) {
    //def tab [=] value
    if ( tabstops < 1 ) {
        gPSDefTabStops = 1;
    } else if ( tabstops > 255 ) {
        gPSDefTabStops = 255;
    } else {
        gPSDefTabStops = tabstops;
    }
}


void FBDefUsing( const SInt32 formatchars) {
    //def using [=] long
    //Bugs: - Possible endian issues
    int				j;
    UInt8	*address;
    
    address = (UInt8*)&formatchars ;
    for ( j = 0; j < 4; j++ ) {
        gFBUsingSymbols[j] = *(address++);
    }
}



StringPtr PSusing ( Str255 format, const double n ) {
    /*
     using formatstring; number
     
     Bugs:
     - Preserved FB's mysterious decision to do using "-$###.##";12.4 as $-12.40 instead of -$12.40
     - dvalue = 123456.789: print using "-#-#,##,##.##"; dvalue is  1-2,34,56.79 here but -1-2,34,56.79 in FB: latter "fixed" instead of obeyed
     - Differs from FB in output where DEF USING has been used, but Robin says this is right and FB is wrong.
     E.g. with DEF USING _"^!;&"
     Format						FB			C			Comment
     USING "$***.##"; 12.34		$*12^34		&*12^34		FB is not honouring user-set currency symbol with * format
     USING "$###";12345			$???		&???		FB ignores localisation settings when errors
     USING "+##.#";1234.5		+??.?		+??^?		Ditto
     */
    gFBStk--;
    Str255	result;
    short	fdecimal = 0, ndecimal, npos, fpos, fdigits = 0, ndigits = 0, decimalplaces = 0, isign = 0, idollar = 0;
    short	fmtlen = format[0], chars, j, xpos, fexp, nexp = 0, fexchars = 0, nexchars = 0;
    Boolean	negative = false, scientific = false;
    double	num, rounded, multiplier, divisor = 1.0, power;
    char	fillchar = ' ', xchar = 0, formatchar;
    char	buffer[256];
    char	localcurrency = gFBUsingSymbols[3], thousandschar = gFBUsingSymbols[1];
    char	decimalchar = gFBUsingSymbols[0];
    
    if ( copysign( 1.0, n ) < 0.0 ) {
        negative = true;
        num = -n;
    } else {
        num = n;
    }
    
    // -------------------------------------------------------------------------
    // Find the decimal point and the number of integer digits & decimal places:
    
    for ( j = 1; j <= fmtlen; j++ ) {
        formatchar = format[j];
        if ( formatchar == '.' ){
            if ( fdecimal == 0 ) fdecimal = j;
        } else if ( formatchar == '#' || formatchar == '*' ) {
            if ( fdecimal > 0 ) decimalplaces++; else fdigits++;
        }
    }
    if ( fdecimal == 0 ) fdecimal = fmtlen + 1;
    
    // ----------------------------------------------------
    // Sort out the interaction between $, +/- and # vs * formats:
    // FB does +$ and -$ same as $- and $+, e.g. $-123.45 (a poor decision?) - but only for '#' not '*' format strings.
    // It also shifts $ and sign to abut digits, again for '#' but not '*'
    
    // First, find out if it's a # or * format, and get position of $/sign special chars, & swap latter if required.
    // We assume some degree of sense and just accept the first # or * found:
    
    for ( j = 1; j <= fmtlen; j++ ) {
        switch( format[j] ) {
            case '#':
                j = fmtlen;
                break;
            case '*':
                fillchar = '*';
                j = fmtlen;
                break;
            case '$':
                if ( idollar == 0 ) idollar = j;
                break;
            case '+':
                if ( isign == 0 ) isign = j;
                break;
            case '-':
                if ( isign == 0 ) isign = j;
                break;
        }
    }
    
    //---------------------
    //Now do the formatting:
    
    if ( format[fmtlen] == '^' ) {
        scientific = true;
        if (num > 0.0 ) {
            power = log10( num );
            power = floor( power );
            divisor = pow( 10.0, power);
            num = num/divisor;
        }
        fexp = fmtlen;
        while ( format[fexp-1] == '^' ) fexp--;
        fexchars = fmtlen - fexp + 1;
    } else {
        fexp = fmtlen + 1;
    }
    
    // If n's precision exceeds number of formatted decimals we'll get wrong answer unless we round to desired precision first:
    
    multiplier = pow( 10.0, decimalplaces );
    rounded = rint( num * multiplier );
    num = rounded / multiplier;
    
    // Though rounded, we still have to force minimum decimals in sprintf to ensure we have all the digits requested:
    if ( !scientific ) {
        chars = sprintf( buffer, "%.*f", decimalplaces, num );
        nexp = chars + 1;
    } else {
        num = num * divisor;
        chars = sprintf( buffer, "%.*E" , decimalplaces, num );
        for ( j = 0; j < chars; j++ ) {
            if ( buffer[j] == 'E' ) {
                nexp = j;
                break;
            }
        }
        nexchars = chars - nexp;
    }
    
    // Find the decimal point in preliminary formatted number:
    if ( decimalplaces == 0  ) ndecimal = chars; else ndecimal = chars + 1;
    for ( j = 0; j < chars; j++ ) {
        if ( buffer[j] == '.' ) {
            ndecimal = j;
            break;
        } else {
            ndigits++;
        }
    }
    
    // Adjust formatting according to exceptions:
    if ( ndigits > fdigits )	{										// if overflow, don't suppress - if +ve (this is what FB does{
        isign = 0;
    } else if ( fillchar == ' ' && !scientific ) {
        if ( idollar == 2 && isign == 1 ) {								// fix ordering: FB always does dollar then +/- with # formats
            isign = 2;
            idollar = 1;
            format[isign] = format[idollar];
            format[idollar] = '$';
        } else if ( isign == 0 && negative ) {						// FB shows sign if it can
            isign = idollar + 1;
            format[isign] = '-';
        }
    } else {														// *** or scientific format
        if ( isign == 0 && negative ) format[idollar + 1] = '?';		// another FB idiosyncracy
    }
    
    // Now fill the integer part of the format string, backwards from decimal:
    npos	= ndecimal - 1;
    fpos	= fdecimal - 1;
    while( fpos > 0 ) {
        formatchar = format[fpos];
        if ( formatchar == '#' || formatchar == '*' )	{ // digit placement
            if ( ndigits <= fdigits ) {
                if ( npos >= 0 ) result[fpos] = buffer[npos]; else result[fpos] = fillchar;
            } else {
                result[fpos] = '?';
            }
            npos--;
        } else if ( formatchar == ',' ) {
            if ( npos >= 0 ) result[fpos] = thousandschar; else result[fpos] = fillchar;
        } else if ( fpos == isign || fpos == idollar ) {
            switch ( formatchar ) {
                case '+':
                    if ( negative ) xchar = '-'; else xchar = '+';
                    break;
                case '-':
                    if ( !negative ) {
                        xchar = ' ';
                    } else if ( npos >= 0 ) {	// possible if result is -ve but no - in original format string
                        xchar = '?';
                        npos--;
                    } else {
                        xchar = '-';
                    }
                    break;
                case '$':
                    xchar = localcurrency;
            }
            if ( fillchar == ' ' )	{	// move to abut numerals if there's space before them
                xpos = fpos;
                while ( result[xpos + 1] == ' ' ) xpos++;
                if ( xpos > fpos){
                    result[xpos] = xchar;
                    xchar = ' ';
                }
            }
            result[fpos] = xchar;
        } else {					// put in whatever the user wants
            result[fpos] = format[fpos];
        }
        
        fpos--;
    }					// end of digit insertion ( left of decimal)
    
    
    if ( fdecimal <= fmtlen ) {	// show decimals
        result[fdecimal] = decimalchar;		// internationalise
        fpos = fdecimal + 1;
        npos = ndecimal + 1;
        
        while ( fpos <= fmtlen ) {
            formatchar = format[fpos];
            if ( formatchar == '#' || formatchar == '*' ) {
                if ( ndigits > fdigits ){
                    result[fpos] = '?';
                } else if ( npos < nexp ) {
                    result[fpos] = buffer[npos];
                    npos++;
                } else {
                    result[fpos] = '0';
                }
            } else if ( fpos >= fexp ) {
                if ( fpos <= fexp + 1 || fexchars == nexchars ) {
                    result[fpos] = buffer[npos];
                    npos++;
                } else if ( fexchars > nexchars ) {			// exponent needs leading zeroes
                    result[fpos] = '0';
                    nexchars++;
                } else {								// fexchars < nexchars: strip any leading zeroes from latter if can
                    while ( buffer[npos] == '0' && nexchars > fexchars ) {
                        npos++;
                        nexchars--;
                    }
                    result[fpos] = buffer[npos];
                    npos++;
                }
            }
            else if ( formatchar == '.' ) {			// FB replaces extra "decimals" with DEF USING setting too
                result[fpos] = decimalchar;
            } else {
                result[fpos] = formatchar;
            }
            fpos++;
        }
    }
    result[0] = fmtlen;
    
    return PSstrcpy( gReturnedString, result );
}


StringPtr PSleft( ConstStr255Param s, SInt16 n ) __attribute__ ((noinline));
StringPtr PSleft( ConstStr255Param s, SInt16 n ) {
    // left$( s, n )
    gFBStk--;
    if ( n < 0 ) n = 0;
    if ( n > s[0] ) n = s[0];
    memmove( &gReturnedString[1], &s[1], n );  // Brian 01-August-2014( replaced BlockMoveData )
    gReturnedString[0] = n;
    return gReturnedString;
}


StringPtr PSright( ConstStr255Param s, SInt16 n ) __attribute__ ((noinline));
StringPtr PSright( ConstStr255Param s, SInt16 n ) {
    //right$( s, n )
    gFBStk--;
    SInt16 len = s[0];
    if ( n < 0 ) n = 0;
    if ( n > len ) n = len;
    //BlockMoveData( &s[1 + len - n], &gReturnedString[1], n );
    memmove( &gReturnedString[1], &s[1 + len - n], n );  // Brian 01-August-2014
    gReturnedString[0] = n;
    return gReturnedString;
}

// Now honors a numChars value equal to zero instead of returning the
// entire string-- Brian 20151113
//
StringPtr PSmid( ConstStr255Param string, SInt16 startPos, SInt16 numChars ) __attribute__ ((noinline));
StringPtr PSmid( ConstStr255Param string, SInt16 startPos, SInt16 numChars ) {
    
    //mid$( s, p, n )  (string, startpos, number)
    
    gFBStk--;
    SInt16 len = string[0];
    if ( numChars < 0 ) numChars = 0;
    if ( numChars > len ) numChars = len;                   // if numChars > len string use len of string
    if ( startPos < 1 ) {
        startPos = 1;										// treat startPos < 1 as 1
    } else if ( startPos > len || numChars == 0 ) {			// startPos is > length of string OR numChars equal zero - return null string
        gReturnedString[0] = 0;
        return gReturnedString;
    } else if ( len + 1 - startPos < numChars ) {  		    // if startPos + numChars is > len of string go from startPos to end of string
        numChars = len - startPos + 1;
    }
    //BlockMoveData( &string[startPos], &gReturnedString[1], numChars );
    memmove( &gReturnedString[1], &string[startPos], numChars );  // Brian 01-August-2014
    gReturnedString[0] = numChars;
    return gReturnedString;
}

//#if NO_SUFFIX_ON_ARRAY_NAMES
extern SInt8 gFBUpperChar[256];
//#else
//extern SInt8 gFBUpperChar_A[256];
//#endif

StringPtr PSucase( ConstStr255Param  s ) {
    // ucase$( s )
    
    unsigned char * outAddr;
    unsigned char   len;
    
    gFBStk--;
    outAddr = gReturnedString;
    len = *s;
    *outAddr = len;
    while ( len ) {
        //#if NO_SUFFIX_ON_ARRAY_NAMES
        *(++outAddr) = gFBUpperChar[*(++s)];
        //#else
        //*(++outAddr) = gFBUpperChar_A[*(++s)];
        //#endif
        len--;
    }
    return gReturnedString;
}


StringPtr PSstring( SInt16 n, UInt8 theChar ) {
    // string$( n, theChar )    also used for chr$ and space$
    
    if ( n < 0  || n > kMaxStringLen )  n = 0;
    gReturnedString[0] = n;
    while ( n ) {
        gReturnedString[n--] = theChar;
    }
    return gReturnedString;
}

StringPtr PSstringstr( SInt16 n, ConstStr255Param s ) {
    //	 string$( n, theStr$ )
    
    gFBStk--;
    if ( n < 0  || n > kMaxStringLen )  n = 0;
    gReturnedString[0] = n;
    while ( n ) {
        gReturnedString[n--] = s[1];
    }
    return gReturnedString;
}


StringPtr PSstrampersand( Handle h, int index ) {
    
    //str&( h, index )
    
    int offset = 2, nElements = 0;
    
    gReturnedString[0] = 0;
    if ( h ) {
        nElements =  *(short*)(*h);
        //		nElements = CFSwapInt32BigToHost( nElements );// :rdp:20101213
        if ( index <= nElements) {
            while ( index-- > 1 ) {
                offset += 1 + *(unsigned char*)(*h + offset);
            }
            PSstrcpy ( gReturnedString, (ConstStr255Param)(*h + offset) );
        }
    }
    return gReturnedString;
}


StringPtr PSstrhash( const int resID, const int index  ) {
    //str#( resID, index )
    return PSstrampersand( GetResource( 'STR#', resID ), index );
}


#include <stdio.h>
#include <termios.h>
#include <unistd.h>

StringPtr PSinkey( void ) {
    
    //inkey$  for command line tool. This must be non-blocking under all circumstances, hence the ridiculous-looking code
    struct termios	oldterm, newterm;
    int				theChar;
    
    tcgetattr( STDIN_FILENO, &oldterm );
    newterm = oldterm;
    newterm.c_lflag &= ~ICANON; // raw mode
    newterm.c_cc[VMIN]  = 0;     // non-blocking
    newterm.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSANOW, &newterm ); // non-blocking (tty)
    
    int		oldflags = fcntl( STDIN_FILENO, F_GETFL, 0 );
    if ( oldflags != -1 ) fcntl( STDIN_FILENO, F_SETFL, oldflags | O_NONBLOCK ); // non-blocking (pipe, for which tcsetattr doesn't do it)
    
    theChar = getchar();
    
    if ( oldflags != -1 ) fcntl( STDIN_FILENO, F_SETFL, oldflags ); // restore
    tcsetattr( STDIN_FILENO, TCSANOW, &oldterm ); // restore
    if ( theChar == EOF ) {
        clearerr( stdin );
        gReturnedString[0] = 0;
    } else {
        gReturnedString[0] = 1;
        gReturnedString[1] = theChar;
    }
    return gReturnedString;
}


void PSinput( StringPtr s ) {
    //input  (if executable is run as Terminal command)
    s[0] = 0;
    if ( NULL != fgets( (char*)s, 256, stdin ) ) {
        CSpstr( (char*)s );
        if ( s[0] && s[s[0]] == 10 ) s[0]--; // strip lf
    }
}


StringPtr PSpstr( StringPtr p ) {
    //pstr$( p )
    return PSstrcpy( gReturnedString, p );
}


CFStringRef DateFormatCreateStringWithAbsoluteTime( CFStringRef formatStr, CFAbsoluteTime at ) {
    /*
     was CFStringRef GetFormattedDateTimeAsString( CFAbsoluteTime  at, CFStringRef formatStr )
     Replacement for PSdate
     */
    CFLocaleRef locale = CFLocaleCopyCurrent();
    CFDateFormatterRef formatter = CFDateFormatterCreate( kCFAllocatorDefault, locale, kCFDateFormatterShortStyle, kCFDateFormatterNoStyle );
    CFRelease( locale );
    CFDateFormatterSetFormat( formatter, formatStr );
    CFStringRef outStr = CFDateFormatterCreateStringWithAbsoluteTime( kCFAllocatorDefault, formatter, at );
    CFRelease( formatter );
    return outStr;
}


StringPtr PSdatePStime( ConstStr255Param inPString) {
    /*
     Replacement for PStime
     */
    Str255	outPString = "\p";
    
    CFStringRef formatStr = CFStringCreateWithPascalString( kCFAllocatorDefault, inPString, kCFStringEncodingMacRoman );
    gFBStk--;
    CFStringRef string = DateFormatCreateStringWithAbsoluteTime( formatStr, CFAbsoluteTimeGetCurrent() );
    CFRelease( formatStr );
    
    CFStringGetPascalString( string, outPString, 256, kCFStringEncodingMacRoman );
    CFRelease( string );
    return PSstrcpy( gReturnedString, outPString );
}



///////////////////////////////////////////////////
//          Number to String Conversions         //
///////////////////////////////////////////////////

SInt16 STRLeadingSpace() {
    /*
     Return user's FB preference re leading space in STR$() of positive numbers
     Bugs: there's no way to change gFBLeadingSpace
     */
    return gFBLeadingSpace;
}



void PSdefstr( const UInt8 length ) {
    //defstr long | word | byte <-->  4 | 2 | 1
    gPSDefStrSize = length;
}


StringPtr PSstr( const double howMany ) {
    //str$( number )
    return PSstrcpy( gReturnedString, FBFloat2String( howMany, FIXEDDECIMAL, gFBFloatMaxDigits ) );
}


StringPtr PShex( SInt32 num ) {
    
    //hex$( number )
    
    SInt32		n;
    UInt8		i, c;
    
    c = gPSDefStrSize*2;
    gReturnedString[0] = c;
    for ( i = c; i >= 1; i-- ) {
        n = num & 0x0F;
        if ( n < 10 ){
            n += (unsigned char)'0';
        } else {
            n += (unsigned char)'A' - 10;
        }
        gReturnedString[i] = n;
        num = num >> 4;
    }
    return gReturnedString;
}


StringPtr PSoct( const unsigned long n ) {
    
    //oct$( number )
    
    Str63	s = "\p";
    SInt16	slen = 0, chars, first, digits, j;
    char	buffer [20];
    
    chars = sprintf( buffer, "%011lo", n );
    
    // # digits = 11, 6, 3 for defstr long, word, byte:
    digits = gPSDefStrSize*3;
    if ( gPSDefStrSize == 4 ) digits--;
    first = 11 - digits ;
    
    // Copy desired digits into result:
    for ( j = first; j < chars; j++ ) {
        slen++;
        s[slen] = buffer[j];
    }
    s[0] = slen;
    return PSstrcpy( gReturnedString, s );
}


StringPtr PSbin( const SInt32 n ) {
    
    //bin$( number )
    
    Str63	s = "\p";
    SInt16	slen = 0, bitpos;
    SInt32	num = n;
    
    slen = gPSDefStrSize * 8;
    bitpos = slen;
    
    while ( bitpos > 0 ){
        if ( (num % 2 )== 0 ) s[bitpos] = '0'; else s[bitpos] = '1';
        num  = num >> 1;
        bitpos--;
    }
    s[0] = slen;
    
    return PSstrcpy( gReturnedString, s );
}


StringPtr PSuns( const SInt32 n ) {
    
    // uns$( number )
    
    Str255	s = "\p";
    SInt16  slen = 0, j, spos, chars;
    char    buffer[256];
    long    num;
    
    // In FB, UNS$ has length 10, 5 or 3 for DEFSTR long, word or byte respectively:
    switch ( gPSDefStrSize ) {
        case 1:
            num = (n & 0xFF);
            slen = 3;
            break;
        case 2:
            num = (n & 0xFFFF);
            slen = 5;
            break;
        default:
            num = n;
            slen = 10;
    }
    
    chars = sprintf( buffer, "%010lu", num );
    
    spos = slen;
    
    // Copy required part into string:
    for ( j = chars - 1; j >= 0; j-- ) {
        s[spos] = buffer[j];
        spos--;
        if ( spos == 0 ) break;
    }
    s[0] = slen;
    return PSstrcpy( gReturnedString, s );
}



StringPtr PSmki( UInt32 n ) {
    // mki$( number )
    // Bug fix - Brian, 20100201. Wrong result when defstr word or defstr byte set.
    
    int   j;
    gReturnedString[0] = gPSDefStrSize;
    for ( j = gPSDefStrSize; j >= 1; j-- ) {
        gReturnedString[j] = n & 0xFF;
        n = n >> 8;
    }
    return gReturnedString;
}


///////////////////////////////////////////////////
//     arithmetic functions of Pascal strings    //
///////////////////////////////////////////////////


SInt8 FBPopCmpStr( Str255 s1, Str255 s2 ) {
    gFBStk -= 2;
    return strcmp( (char*)PScstr( s1 ), (char*)PScstr( s2 ) );
}

int FBcvi( ConstStr255Param s ) {
    // cvi( s )
    int value = 0, n = s[0], j;
    
    gFBStk--;
    if ( n > 4 || n == 3 || n == 0) return 0; // like FB  runtime cvi
                                              // n is 1, 2 or 4
    for ( j = 1; j <= n; j++ ) {
        value = value << 8;
        value += s[j];
    }
    return value;
}


UInt8 PSlen( ConstStr255Param s ) {
    //len( s )
    gFBStk--;
    return s[0];
}


UInt8 PSasc( ConstStr255Param s ) {
    // asc( s )
    gFBStk--;
    if ( s[0] )
        return s[1];
    else
        return 0;
}



UInt8 PSinstr( int startPos, Str255 haystack, Str255 needle ) {
    // thanks to Jay Reeve
    gFBStk -= 2;
    int              foundPos = 0, chrNum = needle[0];
    
    --startPos;
    if ( startPos < 0 ) startPos = 0;
    while ( startPos + chrNum <= haystack[0] )  {
        while ( needle[chrNum] == haystack[startPos + chrNum] )  {
            --chrNum;
            if ( chrNum == 0 ) return (UInt8)startPos + 1;
        }
        chrNum = needle[0];
        ++startPos;
    }
    return (UInt8)foundPos;
}

SInt64 PSvalint( Str255 s ) {
    /*
     val&( string )
     
     Works for these tests
     '--------------------
     print "All 0:"
     print val&( "0" ),  val&( " 0  " ), val&( "&X0" ),  val&( "-0x0" ), val&( "4294967296" )
     print "All 1:"
     print val&( " &x1" ), val&( "0"+"1" ), val&( "&O1" ), val&( " &1" ), val&( "0x1" )
     print "All -1:"
     print val&( "  -1" ), val&( "-&O1" ), val&( " -&1" ), val&( "4294967295" ), val&( "0xffffffff" )
     print val&( "-0x1" )
     
     print val&( "5e3" ) // should be 5
     '--------------------
     
     Bugs:
     - absurdly complicated, but maybe it needs to be
     */
    gFBStk--;
    SInt64	sign = 1;
    int	start = 1, base, pos, end;
    while ( ' ' == s[start] ) start++;						// leading spaces
    if ( '-' == s[start] ) {
        sign = -1;	// adjust sign
        start++;	// skip over
    }
    
    switch ( s[start] )	{									// check first char of string to see if hex, oct, bin
        case '&' :
            switch ( s[start + 1] ) {
                case 'H' :
                case 'h' :									// hex
                    pos = start + 2;	base = 16;
                    break;
                case 'o' :
                case 'O' :									// octal
                    pos = start + 2;	base = 8;
                    break;
                case 'X' :
                case 'x' :									// binary
                    pos = start + 2;	base = 2;
                    break;
                default  :									// hex only with leading ampersand
                    pos = start + 1;	base = 16;
                    break;
            }
            PScstr( s );                  // convert pascal string to C string in place
            return sign*strtoull( (char*)s + pos - 1, NULL, base );      // base 2, 8 or 16
        case '0' :											// hex or decimal
            if ( s[start + 1] == 'x' || s[start + 1] == 'X' )
            {
                PScstr( s );								// convert pascal string to C string in place
                return sign*strtoull( (char*)s + start - 1, NULL, 16 );
            }
            // else fall through
        default :								// decimal
            end = start;
            while ( isdigit( s[end] ) ) end++;	// stop at first non-digit
            gFBStk++;							// pretend we are pushing s for PSmid
            s = PSmid( s, start, end - start );		// just the digit string
            if ( s[1] >= '0' && s[1] <= '9' )
            {
                PScstr( s );													// convert pascal string to C string in place
                return sign*strtoull( (char*)s , NULL, 10 );
            }
            else
                return 0;
    }
}


double PSval( Str255 s )
{
    /*
     val( expression )
     analyse string for base and convert into C (strtol or strtod)-understandable form if required:
     strtol/strtod don't care about leading spaces so leave them intact, but do care
     what lies between sign (if any) and start of real digits
     */
    gFBStk--;
    int		ichar = 1, base = 0, count = 0, strLen;
    char	thischar;
    
    strLen = s[0];
    while ( ichar <= strLen )
    {
        thischar = s[ichar];
        if ( base == 0 )
        {
            switch ( thischar )
            {
                case '&' :
                    switch ( s[ichar + 1])
                {
                    case 'H' :
                    case 'h' :									// hex
                        ichar += 2; base = 16;
                        break;
                    case 'o' :
                    case 'O' :									// octal
                        ichar += 2;	base = 8;
                        break;
                    case 'X' :
                    case 'x' :									// binary
                        ichar += 2;	base = 2;
                        break;
                    default  :									// hex only with leading ampersand
                        ichar++;	base = 16;
                        break;
                }
                    break;
                case '0' :											// hex or decimal
                    if ( s[ichar + 1] == 'x' || s[ichar + 1] == 'X' )
                    {
                        ichar += 2; base = 16;
                    } else {
                        base = 10;
                    }
                    break;
                default:
                    if ( (thischar >= '1' && thischar <= '9') || thischar == '.' ) base = 10;
            }
        }
        if ( ichar <= strLen )
        {
            count++;
            s[count] = s[ichar];
            ichar++;
        }
    }
    s[0] = count;
    PScstr( s );													// convert pascal string to C string in place
    if ( base == 10 ) {
        return strtod( (char*)s , NULL );
    } else {
        UInt64   longresult;
        double   dblresult;
        longresult = strtoull( (char*)s, NULL, base );
        dblresult  = longresult;
        if ( longresult > 0x7FFFFFFFFFFFFFFFULL ) dblresult = dblresult - 0xFFFFFFFFFFFFFFFFULL - 1; // ensure strings like "0x8000000000000000" give -ve result
        return dblresult;
    }
    
}

///////////////////////////////////////////////////
//                  mid$ statement               //
///////////////////////////////////////////////////

long MidStmt( void* strvPtr, long position, long length, Str255 s_p ) {
    Str255 s;  PSstrcpy( s, s_p );
    gFBStk -= 1;
    long              i, srcLen, dstLen;
    if ( position < 1 ) position = 1;
    
    srcLen = s[0];
    dstLen = *(unsigned char*)(strvPtr) - position + 1;
    if ( length > dstLen ) length = dstLen;
    if ( length > srcLen || length < 0 ) length = srcLen;
    strvPtr = (void*)( strvPtr + position );
    i = 1;
    do {
        *(unsigned char*)strvPtr = s[i];
        strvPtr++;
        i++;
    }
    while ( i <= length );
    return 0;
}


///////////////////////////////////////////////////
//               arithmetic functions            //
///////////////////////////////////////////////////

double frac( const double x ) {
    // frac
    return x - trunc( x );
}

double sgn( const double value ) {
    // sgn( expression )
    if ( value > 0.0 ) return 1.0;
    if ( value < 0.0 ) return -1.0;
    return 0.0;
}


SInt32 sgni( const SInt32 value ) {
    
    //sgni( expression )
    
    if ( value > 0 ) return 1;
    if ( value < 0 ) return -1;
    return 0;
}

SInt32 maybe() {
    // maybe
    return ( rand() > RAND_MAX/2 ) ? -1: 0;
}


long rnd( long expr ) {
    // rnd( expr )
    double	r;
    if ( expr == 0 ) return 1;
    r = (double)rand()/((double)(RAND_MAX) + 1.0);
    return (int)(r*expr) + 1;
}


int FBmax( const SInt32 n1, const SInt32 n2 ) {
    // max( n1, n2 )
    return ( n1 > n2 ) ? n1: n2;
}

// Maybe we should be using 'nanosleep()' here instead? Brian 20170702
// struct timespec required, remaining; // remaining can be NULL if the caller doesn't need the remaining time.
/* Where the timespec struc looks like:
 time_t  tv_sec    Seconds.
 long    tv_nsec   Nanoseconds.   1 sec = 1,000,000,000 nanoseconds.
 
 When you pass a timespec struct to nanosleep(), it sleeps for the seconds *plus* the nanoseconds specified
 
 
 struct timespec required, remaining;
 required.tv_sec  = t;
 required.tv_nsec = 0;
 nanosleep(&required , &remaining);
 */
void FBDelay( const Duration delayTime ) { // in delay ms   delay -µs   milliseconds
                                           // old code used deprecated MPLibraryIsLoaded() and MPDelayUntil 10.7. Brian 20170702
                                           // Deficiencies: code doesn't employ the 'remaining' option offered by nanosleep's second parameter.
    
    long nanoInMilli = 1000000;     // nanoseconds in a millisecond
    long milliInSec  = 1000;        // milliseconds in a second
    
    struct timespec required;
    required.tv_sec  = (long)delayTime / milliInSec;            // convert milliseconds to seconds
    required.tv_nsec = ( delayTime % milliInSec ) * nanoInMilli;  // convert remainder milliseconds to nanoseconds
    nanosleep(&required, NULL);                                   // sleep for seconds/nanoseconds. 'remaining' is unused and NULLed
}

///////////////////////////////////////////////////
//                    miscellaneous              //
///////////////////////////////////////////////////

#if !DECARBONATE

#if !__LP64__
void FBGetProcessInfo( long index, StringPtr s, ProcessSerialNumber *psnPtr )
{
    /*
     FBGetProcessInfo
     */
    ProcessInfoRec		processInfo;
    ProcessSerialNumber	thisPSN;
    OSErr				err = noErr;
    
    processInfo.processInfoLength = sizeof( ProcessInfoRec );
    processInfo.processName       = s;
#if !__LP64__
    processInfo.processAppSpec    = NULL; // don't want FSSpec
#endif  /* !__LP64__ */
    if ( index < 0 ) {
        err = GetCurrentProcess( &thisPSN );
    } else {
        if ( index == 0 ) {
            err = GetFrontProcess( &thisPSN );
        } else {
            thisPSN.highLongOfPSN = kNoProcess;
            thisPSN.lowLongOfPSN = kNoProcess;
            while ( err == noErr && index > 0 ) {
                err = GetNextProcess( &thisPSN );
                index--;
            }
        }
    }
    
    if ( err == noErr ) {
        GetProcessInformation( &thisPSN, &processInfo );
        if ( psnPtr ){
            //if ( psnPtr ) BlockMoveData( &thisPSN, psnPtr, sizeof( ProcessSerialNumber ) );
            if ( psnPtr ) memmove( psnPtr, &thisPSN, sizeof( ProcessSerialNumber ) );  // Brian 01-August-2014
        }
    } else {
        s[0] = 0;
    }
}
#endif  /* !__LP64__ */
#endif /* !DECARBONATE */


// returns what the system sends.  Brian 05-May-2015
SInt32 OSVersion( void ) {
    NSArray *myarr = [[[NSProcessInfo processInfo] operatingSystemVersionString] componentsSeparatedByString:@" "];
    NSString  *cleanedString = [[myarr objectAtIndex:1] stringByReplacingOccurrencesOfString:@"." withString:@""];
    return [cleanedString integerValue];
}

void OSMajorMinor( short *major, short *minor ) {
    
    NSArray *myarr = [[[NSProcessInfo processInfo] operatingSystemVersionString] componentsSeparatedByString:@" "];
    NSArray *majorMinorBugFix = [[myarr objectAtIndex:1] componentsSeparatedByString:@"."];
    *major = [[majorMinorBugFix objectAtIndex:0]intValue];
    *minor = [[majorMinorBugFix objectAtIndex:1]intValue];
}


SInt32 FBSystemFunction( const SInt32 expr ) {
    /*
     FBSystemFunction
     
     Updated 20170708 for general removal of old code.
     Only kFBScrnWidth, kFBScrnHeight, kFBSysVers and kFBAplActive are supported.
     Unsupported selectors, even though shown in this code( now commented out ) as returning zero, would never get this far
     because the FBtoC translator now accepts only the four selectors noted above.
     Brian 20170708
     */
    
    CGRect				cgRect;
    SInt32				result = 0;
    
    switch ( expr ) {
            
        case kFBScrnWidth:
            cgRect = CGDisplayBounds( CGMainDisplayID() );
            result = cgRect.size.width;
            break;
        case kFBScrnHeight:
            cgRect = CGDisplayBounds( CGMainDisplayID() );
            result = cgRect.size.height;
            break;
            
        case kFBSysVers:
            //Gestalt( gestaltSystemVersion, &sysVers ); // Gestalt fails in OS X 10.10 and returns wrong value ( e.g. 1093 )
            //result = ((sysVers & 0xF000)>>12)*1000 + ((sysVers & 0x0F00)>>8)*100 + ((sysVers & 0x00F0)>>4)*10 + (sysVers & 0x000F);
            result = OSVersion();   // call new code since Gestalt fails - Brian 05-May-2015
            break;
            
        case kFBAplActive: {
            
            NSRunningApplication *currentApp = [NSRunningApplication currentApplication]; // replaces old calls Brian 20170708
            if ( [currentApp isActive])
                result = 1;        // is front-most app
            else
                result = -1;       // is not front-most app
            
            //GetFrontProcess( &psn1);
            // GetCurrentProcess( &psn2 );
            //SameProcess( &psn1, &psn2, &same );
            //if ( same ) result = 1; else result = -1;
            break;
             }
#if !__LP64__
        case kFBCrntDepth:
#if defined MAC_OS_X_VERSION_10_7 && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7
            result = 32;
#else
            if ( &CGDisplayBitsPerPixel == NULL )
                result = 32;
            else
                result = CGDisplayBitsPerPixel( CGMainDisplayID() );
#endif
            break;
#endif /* !__LP64__ */
       
            /*
             case kFBMaxColors:
             // result = 32;
             break;
             
             case kFBMachType:
             break;
             
             case kFBCPUSpeed:
             break;
             
             case kFBClockSpeed:
             break;
             
             case kFBLastCursType:														// _lastCursType
             break;
             
             case kFBAplVRefNum:														// _aplvRefNum
             break;
             
             case kFBAplParID:															// _aplparID
             break;
             
             case kFBMemAvail:
             //result = FreeMem();
             break;
             
             case kFBAplRes:
             break;
             */
            
    }
    return result;
}


void FBRunPath( CFStringRef pathToAppName ) {     // Brian 20170703
    [[NSWorkspace sharedWorkspace] launchApplication:(NSString *)pathToAppName];
}


void FBRunURL( CFURLRef url ) {          // Brian 20170703
    [[NSWorkspace sharedWorkspace] openURL:(NSURL *)url];
}

#if !__LP64__


void FBRun( Str255 path, short vRefNum, long dirID ) {
    FSSpec	spec;
    FSRef	ref;
    
    if ( FSMakeFSSpec( vRefNum, dirID, path, &spec ) == noErr ) {
        if ( FSpMakeFSRef( &spec, &ref ) == noErr ) LSOpenFSRef( &ref, NULL );
    }
}



void FBKillPicture( PicHandle *picHPtr ) {
    /*
     Kill picture myPict
     */
    if ( picHPtr && *picHPtr ) {
        KillPicture( *picHPtr );
        *picHPtr =  NULL;
    }
}

#endif  /* !__LP64__ */


/*
 void FBKillField( Handle h ) {
 // kill field h
 if ( h ) {
 if ( ( HGetState( h ) & 0x20 ) == 0 ) DisposeHandle( h );
 }
 }
 */


long FBUsrFnIndicator( long indicator, long expr ) {
    /*
     value = usr fnIndicator (expr)
     
     Bugs:
     - not all fnIndicators handled
     - _fileAddr returns Boolean
     */
    switch ( indicator ) {
        case 4:		// _fileAddr
                    //#if NO_SUFFIX_ON_ARRAY_NAMES
            return gFBOpenFiles[expr].isOpen;
            //#else
            //return gFBOpenFiles_A[expr].isOpen;
            //#endif
    }
    return 0;
}


void FBPstrDollarStatement( void* address, void* s ) {
    //pstr$( address ) = string
    *(long*)address = (long)(s);
}


