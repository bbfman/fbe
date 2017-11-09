
#include "FBtoC.h"
#include "Runtime.h"

#ifndef ARRAYINDICES_H_GUARD
#define ARRAYINDICES_H_GUARD

// static inline functions

///////////////////////////////////////////////////////////
//                 array bounds checking                 //
///////////////////////////////////////////////////////////

/*
 All non-dynamic array references come here if bounds checking is enabled.
 */


/*
 'unsigned' here ensures that negative indices (in the FB source) are illegal.
*/

/*
static inline int ChkBounds( const unsigned int index, const unsigned int maxIndex, const int lineNum, ConstStr255Param fileName )
{
    if ( index > maxIndex ) FBReportRangeError( index, lineNum, maxIndex, fileName );
    return index;
}
*/

/*
  Brian & Bernie 7-Apr-2016 - when FBtoC generates calls to ChkBounds(), it now passes a CFStringRef for the last ( fileName ) parameter
 
*/


#if kNSFBFullStop

static inline int ChkBounds( const unsigned int index, const unsigned int maxIndex, const int lineNum, CFStringRef fileName ) {
    if ( index > maxIndex ) {
        FBReportRangeError( index, lineNum, maxIndex, fileName );
    }
    return index;
}

#else

static inline int ChkBounds( const unsigned int index, const unsigned int maxIndex, const int lineNum, CFStringRef fileName ) {
    if ( index > maxIndex ) {
        Str255 s;
        CFStringGetPascalString( fileName, (void*)&s, 256, kCFStringEncodingMacRoman );
        FBReportRangeError( index, lineNum, maxIndex, s );
    }
    return index;
}

#endif


///////////////////////////////////////////////////////////
//                 dynamic array support                 //
///////////////////////////////////////////////////////////

/*
 All dynamic array references come here.
 Signed/unsigned casts ensure that 'negative' indices are illegal.
 */
static inline void * FBDynamicArray( const int dynArrayNum, const unsigned int index )
{
	if ( index >= (unsigned)gFBDynArrayInfo[dynArrayNum].maxIndex ) FBGrowDynamicArray( dynArrayNum, index ); // grow only if required
    if ( (signed)index > (signed)gFBDynArrayInfo[dynArrayNum].lastElem ) gFBDynArrayInfo[dynArrayNum].lastElem = index;
return gFBDynArrayInfo[dynArrayNum].base + index * gFBDynArrayInfo[dynArrayNum].elemSize; // pointer to the element referenced
}


static inline void * FBDynamicArrayRecord( const int dynArrayNum, const unsigned int index, const unsigned int fieldOffset )
{
	if ( index >= (unsigned)gFBDynArrayInfo[dynArrayNum].maxIndex ) FBGrowDynamicArray( dynArrayNum, index ); // grow only if required
    if ( (signed)index > (signed)gFBDynArrayInfo[dynArrayNum].lastElem ) gFBDynArrayInfo[dynArrayNum].lastElem = index;
	return gFBDynArrayInfo[dynArrayNum].base + index * gFBDynArrayInfo[dynArrayNum].elemSize + fieldOffset; // pointer to the element referenced
}

#endif //ARRAYINDICES_H_GUARD