// prefix header, with public (user-code-reachable) struct and typedef declarations

// some of these functions are toolbox referenced in CFIndex.incl within build_goodies




#include "FBtoC.h"
#include "Runtime.h"

/*
 *  FBtoCIndex.c
 *
 */


///////////////////////////////////////////////////
//                  index$ stuff                 //
///////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------

#define  kFBIndexMaxArray 9

CFMutableArrayRef	gFBIndex[kFBIndexMaxArray + 1];


static CFMutableArrayRef CFIndexGetArray_priv( CFIndex indexID ) {
	if ( indexID < 0 || indexID > kFBIndexMaxArray ) indexID = 0;
	return gFBIndex[indexID];
}


static void CFIndexSetArray_priv( CFIndex indexID, CFMutableArrayRef array ) {
	if ( indexID < 0 || indexID > kFBIndexMaxArray ) indexID = 0;
	gFBIndex[indexID] = array;
}


static void CFIndexReleaseArray_priv( CFIndex indexID ) {
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( array ) {
		CFRelease( array );
		CFIndexSetArray_priv( indexID, 0 );
	}
}


//------------------------------------------------------------------------------
// User
//------------------------------------------------------------------------------

CFMutableArrayRef CFIndexGetArrayRef( CFIndex indexID ) {
	return CFIndexGetArray_priv( indexID );
}


CFIndex CFIndexGetCount( CFIndex indexID ) {
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( array ) return CFArrayGetCount( array );
	return 0;
}


void CFIndexSet( CFIndex indexID, CFIndex element, Str255 pStr ) {
	CFStringRef	string;
	
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( !array ) {
		array = CFArrayCreateMutable( NULL, 0, &kCFTypeArrayCallBacks );
		CFIndexSetArray_priv( indexID, array );
	}
	if ( array ) {
		CFIndex count = CFArrayGetCount( array );
		while ( count < element ) {
			string = CFStringCreateWithPascalString( NULL, "\p", kCFStringEncodingMacRoman );
			if ( string ) {
				CFArrayAppendValue( array, string );
				CFRelease( string );
			}
			count++;
		}
		string = CFStringCreateWithPascalString( NULL, pStr, kCFStringEncodingMacRoman );
		if ( string ) {
			CFArraySetValueAtIndex( array, element, string );
			CFRelease( string );
		}
	}
}


/*
 index$( element [, indexID] ) = string
 */
void FBIndexSet( CFIndex element, CFIndex indexID, Str255 pStr ) {
	CFIndexSet( indexID, element, pStr );
}


StringPtr CFIndexGet( CFIndex indexID, CFIndex element ) {
	gReturnedString[0] = 0;
	
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( array ) {
		if ( element < CFArrayGetCount( array ) ) {
			CFStringGetPascalString( CFArrayGetValueAtIndex( array, element ), gReturnedString, 256, kCFStringEncodingMacRoman );
		}
	}
	return gReturnedString;
}


/*
 string = index$( element [, indexID] )
 */
StringPtr FBIndexGet( CFIndex element, CFIndex indexID ) {
	return CFIndexGet( indexID, element );
}


void CFIndexInsert( CFIndex indexID, CFIndex element, Str255 pStr ) {
	CFStringRef	string;
		
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( !array ) {
		array = CFArrayCreateMutable( NULL, 0, &kCFTypeArrayCallBacks );
		CFIndexSetArray_priv( indexID, array );
	}
	if ( array ) {
		CFIndex count = CFArrayGetCount( array );
		if ( element <= count ) {
			string = CFStringCreateWithPascalString( NULL, pStr, kCFStringEncodingMacRoman );
			if ( string ) {
				CFArrayInsertValueAtIndex( array, element, string );
				CFRelease( string );
			}
		} else {
			CFIndexSet( indexID, element, pStr );
		}
	}
}


/*
 index$ I( element [, indexID] ) = string
 */
void FBIndexI( CFIndex element, CFIndex indexID, Str255 pStr ) {
	CFIndexInsert( indexID, element, pStr );
}


void CFIndexDelete( CFIndex indexID, CFIndex element ) {
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( array ) {
		if ( element < CFArrayGetCount( array ) ) CFArrayRemoveValueAtIndex( array, element );
	}
}


/*
 index$ D( element [, indexID] )
 */
void FBIndexD( CFIndex element, CFIndex indexID ) {
	CFIndexDelete( indexID, element );
}


CFIndex CFIndexFind( CFIndex indexID, CFIndex startElement, Str255 pStr ) {
	CFRange		range;
	CFIndex		element = -1;
		
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( array ) {
		CFIndex count = CFArrayGetCount( array );
		if ( startElement < count ) {
			CFStringRef stringToFind = CFStringCreateWithPascalString( NULL, pStr, kCFStringEncodingMacRoman );
			if ( stringToFind ){
				CFIndex i = startElement;
				do {
					range = CFStringFind( CFArrayGetValueAtIndex( array, i ), stringToFind, 0 );
					i++;
				}
				while ( ( range.location == kCFNotFound ) && ( i < count ) );
				if ( range.location != kCFNotFound ) element = i - 1;
				CFRelease( stringToFind );
			}
		}
	}
	return element;
}


/*
 foundElement = indexf( stringToFind [, startElement [,indexID]] )
 */
CFIndex FBIndexF( Str255 pStr, CFIndex startElement, CFIndex indexID ) {
	gFBStk--; // keep stack happy
	return CFIndexFind( indexID, startElement, pStr );
}


/*
 clear -1
 clear index$ [indexID]
 */
void CFIndexClear( CFIndex indexID ) {
	CFIndex	i;

	if ( indexID < 0 ) {
		for ( i = 0; i <= kFBIndexMaxArray; i++ ) CFIndexReleaseArray_priv( i );
	} else {
		CFIndexReleaseArray_priv( indexID );
	}
}


CFComparisonResult CFIndexCompareFunction( const void *val1, const void *val2, void *context  __attribute__ ((__unused__)) ) {
	return CFStringCompare( val1, val2, kCFCompareCaseInsensitive );
}


void CFIndexSort( CFIndex indexID ){
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( array ) CFArraySortValues( array, CFRangeMake( 0, CFArrayGetCount( array ) ), CFIndexCompareFunction, 0 );	
}


void CFIndexAppend( CFIndex indexID, Str255 pStr ) {
	CFMutableArrayRef array = CFIndexGetArray_priv( indexID );
	if ( !array ) {
		array = CFArrayCreateMutable( NULL, 0, &kCFTypeArrayCallBacks );
		CFIndexSetArray_priv( indexID, array );
	}
	if ( array ) {
		CFStringRef string = CFStringCreateWithPascalString( NULL, pStr, kCFStringEncodingMacRoman );
		if ( string ){
			CFArrayAppendValue( array, string );
			CFRelease( string );
		}	
	}
}

Boolean CFIndexWrite( CFIndex indexID, CFURLRef url ) {
    CFMutableArrayRef	array;
    CFDataRef			dataRef;
    Boolean				result = false;
    
    array = CFIndexGetArray_priv( indexID );
    if ( array ) {
        dataRef = CFPropertyListCreateXMLData( NULL, array );
        if ( dataRef ) {
            if ( url ) {
                result = CFURLWriteDataAndPropertiesToResource( url, dataRef, NULL, NULL );
                CFRelease( url );
            }
            CFRelease( dataRef );
        }
    }
    return result;
} 


Boolean CFIndexWriteAll( CFURLRef url ) {

	CFMutableArrayRef   array, tempArray;
	CFDataRef           dataRef;
    //CFURLRef            url;
	CFIndex             index;
	Boolean             result = false;
	
	tempArray = CFArrayCreateMutable( NULL, 0, &kCFTypeArrayCallBacks );
	if ( tempArray ) {
		for ( index = 0; index <= kFBIndexMaxArray; index++ ) {
			array = CFIndexGetArray_priv( index );
			if ( array ) {
				CFArrayAppendValue( tempArray, array );
			} else {
				array = CFArrayCreateMutable( NULL, 0, &kCFTypeArrayCallBacks );
				if ( array ) {
					CFArrayAppendValue( tempArray, array );
					CFRelease( array );
				}
			}
		}
		
		dataRef = CFPropertyListCreateXMLData( NULL, tempArray );
		if ( dataRef ) {
			if ( url ) {
				result = CFURLWriteDataAndPropertiesToResource( url, dataRef, 0, NULL );
				CFRelease( url );
			}
			CFRelease( dataRef );
		}
		
		CFRelease( tempArray );
	}
	return result;
}


Boolean CFIndexRead( CFIndex indexID,  CFURLRef url ) {
	CFMutableArrayRef	array;
	CFDataRef			dataRef;
	Boolean				result = false;
	
	array = CFIndexGetArray_priv( indexID );
	if ( array == 0 ) {
		if ( url ) {
			if ( CFURLCreateDataAndPropertiesFromResource( NULL, url, (void*)&dataRef, NULL, NULL, NULL ) ) {
				if ( dataRef ) {
					array = (CFMutableArrayRef)CFPropertyListCreateFromXMLData( NULL, dataRef, 0, NULL );
					if ( array ) {
						CFIndexSetArray_priv( indexID, array );
						result = true;
					}
					CFRelease( dataRef );
				}
			}
			CFRelease( url );
		}
	}
	return result;
} 


Boolean CFIndexReadAll( CFURLRef url ) {

	CFMutableArrayRef     array, tempArray;
	CFDataRef             dataRef;
	CFIndex               index, count;
	Boolean               result = false;
	
	if ( url ) {
		if ( CFURLCreateDataAndPropertiesFromResource( NULL, url, &dataRef, NULL, 0, NULL ) ) {
			if ( dataRef ) {
				tempArray = (CFMutableArrayRef)CFPropertyListCreateFromXMLData( NULL, dataRef, 0, NULL );
				if ( tempArray ) {
					count = CFArrayGetCount( tempArray );
					if ( count == ( kFBIndexMaxArray + 1 ) ) {
						for ( index = 0; index < count; index++ ) {
							array = (CFMutableArrayRef)CFArrayGetValueAtIndex( tempArray, index );
							if ( array ) {
								CFIndexReleaseArray_priv( index );
								
								//array = CFArrayCreateMutableCopy( 0, 0, array );
								CFRetain( array );
								
								CFIndexSetArray_priv( index, array );
							}
						}
						result = true;
					}
					CFRelease( tempArray );
				}
				CFRelease( dataRef );
			}
		}
		CFRelease( url );
	}
	return result;
}



/*
 info = mem( expr ) 
 */
CFIndex FBMem( long expr ) {
	if ( expr < 0 ) {
		return( 0 ); // rp:20100226 
//		switch ( expr ) 
//		{
//			case -1:															// _maxAvail
//				return MaxMem(0);
//			case -2:															// _freeBytes
//				return FreeMem();
//		}
	} else {
		CFIndex	indexID = ( expr % 10 );
		expr -= indexID;
		if ( expr == 10 ) return CFIndexGetCount( indexID );				// _numElem
	}
	return 0;
}


