// prefix header, with public (user-code-reachable) struct and typedef declarations
#include "FBtoC.h"
#include "Runtime.h"
#include <malloc/malloc.h>
/*
 FB runtime functions from Subs Containers.incl, adapted and edited to work (as part of Demo.bas) for container operations in C.
 Then FBtoC's translation was used to to populate this file, with editing where necessary to prevent gcc warnings.
 
 July/August/September 2017
 All container functions rewritten to use pointers instead of Handles.
 Idea is to make container.c 64-bit by elminating deprecated Handle calls such as Get/SetHandleSize
 Calling parameters for the container functions in this file stay the same.
 FBtoC source changed to translate a 'container' ( or var$$ ) to '*void'
 Brian
 */

void             *gFBTempContainer;
void             *gFBTemp2Container; // 20081020 #390 fix; used only for cntr vs string comparison




void * AddStringToCtnr( void **ctnrVarPtr, Str255 inPStr ) {
    
    
    void              *newContainer, *containerFromCaller   ;
    Str255             pStr;
    
    if ( ctnrVarPtr != &gFBTempContainer ) FBClearTempContainer();
    
    containerFromCaller =  *ctnrVarPtr;
    
    PSstrcpy( pStr, inPStr );
    short pascalStringSize = pStr[0];
    if ( pascalStringSize ) {
        if ( containerFromCaller ) {
#if __LP64__
            SInt64 priorContainerSize  = CtnrRequestedSize( *ctnrVarPtr );
#else
            UInt32 priorContainerSize  = CtnrRequestedSize( *ctnrVarPtr );
#endif
            newContainer          = FBNewContainer( priorContainerSize + pascalStringSize );
            memmove( newContainer, containerFromCaller, priorContainerSize );
            memmove( newContainer + priorContainerSize, &pStr[1], pascalStringSize );
            CtnrFree( *ctnrVarPtr );                                              // free input ptr because it will be replaced with alloc_realloc_Ptr
        } else {
            newContainer = FBNewContainer( pascalStringSize );
            memmove( newContainer, &pStr[1], pascalStringSize );
        }
        *ctnrVarPtr = newContainer;
    }
    
    // const char *newStr = *ctnrVarPtr;
    return *ctnrVarPtr;  //  in some cases the caller ignores this value since caller's container is updated
    
}

// Appends V2 container to V1 container and returns V1
//
// Brian 20170803 - This function 'returns' ctnrV1Ptr and updates the caller's variable.
// Probably done to allow different FBtoC translations to work.
// Other container.c functions have this same behavior


void * AddCtnrs( void **ctnrV1Ptr, void **ctnrV2Ptr ) {
    void             *tempPtr;
#if __LP64__
    SInt64            sizeP1, sizeP2;
#else
    UInt32            sizeP1, sizeP2;
#endif
    if ( *ctnrV2Ptr ) {
        sizeP2 = CtnrRequestedSize( *ctnrV2Ptr );
        if ( *ctnrV1Ptr == 0 ) {
            sizeP1 = 0;
        } else {
            sizeP1 = CtnrRequestedSize( *ctnrV1Ptr );
        }
        tempPtr = FBNewContainer( sizeP1 + sizeP2 );
        memmove( tempPtr ,*ctnrV1Ptr, sizeP1 );
        if ( sizeP1 ) CtnrFree( *ctnrV1Ptr );
        memmove( tempPtr + sizeP1, *ctnrV2Ptr, sizeP2 );
        *ctnrV1Ptr = tempPtr;
    }
    return ctnrV1Ptr;
}

long Asc_ctnr( void **ctnrVarPtr ) {
    long  theASCIIChar = 0;
    // the two 'ifs' can't be combined in one test( with && )  because CtnrRequestedSize could crash if *ctnrVarPtr is nil.
    if ( *ctnrVarPtr )
        if ( CtnrRequestedSize( *ctnrVarPtr ) )
            theASCIIChar = *(unsigned char*)(*ctnrVarPtr);
    return theASCIIChar;
}

//
// concatenates a container contents ( up to 255 characters ) to a pascal string
//
// dim as container gC
// dim as Str255    pStr
// pStr += gC
// generates CatCtnrString( &gC, pStr,  256 );
//
StringPtr CatCtnrString( void **srcCtnrVar, StringPtr pStrPtr, long lstring ) {
    Str255            temp;
    GetCtnrString( srcCtnrVar, (void*)&temp, lstring );   // get container contents in 'temp' string
    PSstrcpy( TO_STACK, pStrPtr ); // copy the incoming pascal string to the stack
    PSstrcat( TO_STACK, temp );    // copy temp to the stack and make sure resultant concatentation is less 256 characters
    PSstrcpy( pStrPtr, TO_STACK ); // copy the stack back into the pascal string to be returned to caller.
    return pStrPtr;
}

// special free() wrapper to pass the actual allocated pointer ( which starts 8 bytes prior to what the FBer sees )
// since ptr is cast to size_t* which is eight bytes, ptr -1 effectively backs up 8 bytes.
void CtnrFree(void * ptr) {
    free( (size_t*)ptr - 1);
}

// the requested allocation size is stored in the first eight bytes of the pointer. This is often different from what malloc() allocates.

#if __LP64__
SInt64 CtnrRequestedSize(void * ptr) {
    return ((size_t*)ptr)[-1];
}
#else
UInt32 CtnrRequestedSize(void * ptr) {
    return ((size_t*)ptr)[-1];
}
#endif

//
//
//
void * CtnrStoreString( void **ctnrVarPtr, Str255 inPStr ) {
    
    void              *newContainer, *containerFromCaller   ;
    Str255             pStr;
    
    containerFromCaller =  *ctnrVarPtr;
    
    PSstrcpy( pStr, inPStr );
    short pascalStringSize = pStr[0];
    if ( containerFromCaller ) CtnrFree( containerFromCaller );
    if( pascalStringSize ) {
        newContainer = FBNewContainer( pascalStringSize );
        memmove( newContainer, &pStr[1], pascalStringSize );
        *ctnrVarPtr = newContainer;
    } else {
        *ctnrVarPtr = NULL;
    }
    //return *ctnrVarPtr;  //  in some cases the caller ignores this value since caller's container is updated
    return ctnrVarPtr;  //  in some cases the caller ignores this value since caller's container is updated
    
}

void FBClearTempContainer() {
    //if ( gFBTempContainer ) DisposeHandle( gFBTempContainer );
    if ( gFBTempContainer )  CtnrFree( gFBTempContainer );
    gFBTempContainer = NULL;
}

void FBClearTemp2Container() {
    if ( gFBTemp2Container )  CtnrFree( gFBTemp2Container );
    //DisposeHandle( gFBTemp2Container );
    gFBTemp2Container = NULL;
}

//
// Same logic as handle-based compare except adapted to pointers - 24-August-2017
//
SInt64 FBCompareContainers( void **container1, void **container2 ) {
    void              *charPtr1, *charPtr2, *endPtr1;
    SInt64             chkPos = ( container1 != container2 ), lenPtr1, lenPtr2;
    if ( chkPos ) {
        if ( *container1 ) {
            if ( *container2 ) {
                lenPtr1   = CtnrRequestedSize( *container1 );
                lenPtr2   = CtnrRequestedSize( *container2 );
                charPtr1  = *container1;
                charPtr2  = *container2;
                if ( lenPtr2 < lenPtr1 ) {
                    endPtr1 = ( charPtr1 + lenPtr2 );
                } else {
                    endPtr1 = ( charPtr1 + lenPtr1 );
                }
                while ( charPtr1 < endPtr1 ) {
                    if ( *(unsigned char*)charPtr1 != *(unsigned char*)charPtr2 ) break;
                    charPtr1++;
                    charPtr2++;
                } // end 'while'
                
                chkPos = ( charPtr1 - *container1 ) + 1;
                if ( charPtr1 == endPtr1 ) {
                    if ( lenPtr1 <= lenPtr2 ) {
                        if ( lenPtr1 < lenPtr2 )
                            chkPos = -chkPos;
                        else
                            chkPos = 0;
                    }
                } else {
                    if ( *(unsigned char*)charPtr1 < *(unsigned char*)charPtr2 ) chkPos = -chkPos;
                }
            }
        } else {
            chkPos = -1;
        }
    }
    return chkPos;
}


Boolean FBCmpCtnr( void **ctnr1, void **ctnr2 ) {
    Boolean res = (FBCompareContainers( ctnr1, ctnr2 ) != 0);
    FBClearTemp2Container();
    return res;
}

void FBCtnrCopy( void **srcCtnrVar, void **destCtnrVar ) {
#if __LP64__
    SInt64 size;
#else
    UInt32 size;
#endif
    void *srcP  = *srcCtnrVar, *destP;
    if ( srcP != NULL ) {
        size = CtnrRequestedSize( srcP );
        if ( *destCtnrVar != *srcCtnrVar ) {
            if ( *destCtnrVar ) CtnrFree( *destCtnrVar );
            destP  = FBNewContainer( size );
            if ( srcP ) memcpy( destP, *srcCtnrVar, size );
            *destCtnrVar = destP;
        }
    } else {
        *destCtnrVar = *srcCtnrVar;
    }
}

void * FBCtnrCopy3( void **destCtnrVar, void **srcCtnrVar )
{
    FBCtnrCopy( srcCtnrVar, destCtnrVar );
    return destCtnrVar;
}

// add 8 bytes to the new pointer to save the requested size. malloc() often rounds up, so requested and allocated can be different.
// give the caller byte 1's address, so they only know about the user data and not the embedded size.
#if __LP64__
void * FBNewContainer( SInt64 requestedAllocationSize ) {
    SInt64 totalSize = sizeof(SInt64) + requestedAllocationSize;
    SInt64 *returnP = (SInt64 *)malloc( totalSize );
    *returnP = requestedAllocationSize;
    return &returnP[1];
}
#else    // 32-bit
void * FBNewContainer( UInt32 requestedAllocationSize ) {
    UInt32 totalSize = sizeof(UInt32) + requestedAllocationSize;
    UInt32 *returnP = (UInt32 *)malloc( totalSize );
    *returnP = requestedAllocationSize;
    return &returnP[1];
}
#endif

//
// Retrieves a pascal string of length ( pStrLength ) from the container and puts it in the pascal string
// Is the equivalent of FB:  pStr = gC
// Length generated is based on the pascal string type. e.g. Str15 generates a length of 15. Str255 generates 255 etc.
//
StringPtr GetCtnrString( void **srcCtnrVar, StringPtr pStr, short pStrLength ) {
    SInt64            containerSize;
    //void              *ctnrPtr;
    //ctnrPtr = *srcCtnrVar;
    
    if ( pStrLength > 255 ) pStrLength = 255;
    if ( *srcCtnrVar ) {
        containerSize = CtnrRequestedSize( *srcCtnrVar );
        if ( containerSize > pStrLength )  containerSize = pStrLength;
        memmove( pStr + 1, *srcCtnrVar, containerSize );
    } else {
        containerSize = 0;
    }
    *pStr = containerSize;
    
    return pStr;
}

SInt64 Instr_Ctnr( long inPosition, void **targetPtr, void **searchPtr ) {
    
    /*   Brian 20170725 - Potential ways to avoid Munger() use are:
     
     on input cast the two pointers ( currently both Handles ) to:
     const char *bigStr = *BigctnrVarPtr;
     const char *searchStr = *SearchctnrVarPtr;
     
     then use strstr() to locate a substring in a string:
     ( maybe strnstr() is better to be able to specify number of characters searched so we don't have to add C string null terminator to end of string '\0'
     
     char *ptr;
     ptr = strstr( bigStr, searchStr );
     
     
     */
    char       *offSetToStartSearch = *targetPtr;
    SInt64     foundPosition       = 0;
    SInt64     containerSize       = CtnrRequestedSize( *targetPtr );
    SInt64     searchContainerSize = CtnrRequestedSize( *searchPtr );
    
    if ( searchContainerSize && containerSize && ( searchContainerSize <= containerSize ) ) {
        if ( inPosition <= 0 ) inPosition  = 1;
        if ( inPosition > 0 ) {
            inPosition--;                                               // adjust for one-based position since find is zero-based.
            offSetToStartSearch += inPosition;
            char *ptrToFoundStr = strstr( offSetToStartSearch, *searchPtr );  // find searchStr within container
            if ( ptrToFoundStr ) {
                foundPosition = ptrToFoundStr - (char *)*targetPtr;     // found address minus beginnning address is found position
                foundPosition++;                                        // adjust for one-based position since find is zero-based.
            }
        }
    }
    
    return foundPosition;
    
}


SInt64 Instr_Ctnr_PStr( long startPosition,void **containerPtr, Str255 inputPStr ) {
    
    /*   Brian 20170725 - Potential ways to avoid Munger() use are:
     
     on input cast the two pointers ( currently both Handles ) to:
     const char *bigStr = *BigctnrVarPtr;
     const char *searchStr = *SearchctnrVarPtr;
     
     then use strstr() to locate a substring in a string:
     ( maybe strnstr() is better to be able to specify number of characters searched so we don't have to add C string null terminator to end of string '\0'
     
     char *ptr = strstr( bigStr, searchStr );
     */
    
    //Str255 pStr;  PSstrcpy( pStr, inputPStr );
    gFBStk -= 1;                          // required because Instr_Ctnr_PStr()'s caller does a STACK_PUSH()
                                          // need to investigate more to determine if caller needs to do a STACK_PUSH(). Brian 20170726
    char       *searchStr, *offSetToStartSearch = *containerPtr;
    SInt64     foundPosition = 0, containerSize = CtnrRequestedSize( *containerPtr );
    short      sizePString   = inputPStr[0];//pStr[0];
    if ( startPosition <= 0 ) startPosition  = 1;
    if ( sizePString && containerSize && ( sizePString <= containerSize ) ) {
        searchStr = (char *)calloc( 1, sizePString );
        memmove(searchStr, inputPStr + 1, sizePString );
        if ( startPosition > 0 ) {
            startPosition--;                                       // FBer specifies one-based start while the search is zero-based, so adjust.
            offSetToStartSearch += startPosition;
            char *ptr = strstr( offSetToStartSearch, searchStr );  // find searchStr within container
            if ( ptr ) {
                foundPosition = ptr - (char *)*containerPtr;  // found address minus beginnning address is found position
                foundPosition++;     // FB user code expects one-based position and this find is zero-based.
            }
        }
        free( searchStr );
    }
    return foundPosition;
}


// note: Right_Ctnr() and Left_Ctnr() are identical except for the memmove()
// opportunity here to reduce code. Brian 20170726
/*
 Brian 20170915
 
 Left_Ctnr and Right_Ctnr both have a bug due to use of gFBTempContainer. When gFBTempContainer is an intermediate placeholder for data,
 its contents are overwritten.  Consider the following
 dim as container gC1, gC2, gC3
 gC1 = "futurebasic" : gC2 = "Anarctica"
 gC3 = right$$( gC1, 5 ) + left$$( gC2, 3 )
 
 Code first calls Right_Ctnr which returns "basic" in gTempContainer. That result is then copied(FBCtnrCopy3) to gTempContainer
 Then Left_Ctnr returns "Ana" in gTempContainer which overwrites its content ( which was "basic" ). Then "Ana" is appened to gTempContainer, so
 now gTempContainer contains "AnaAna". Lastly the gTempContainer is copied(FBCtnrCopy) into gC3. If the code worked correctly gC3  but inre
 should contain "basicAna"
 
 Workaround is to do it in two steps:   gc3  = right$$( gC1, 5 )
 gc3 += left$$( gC2, 3 )
 
 Bug was inherited from the Handle-based containers and remains in the pointer-based containers.
 
 
 */

#if __LP64__
void * Right_Ctnr( void **ctnrVarPtr, SInt64 requestedLen ) {
    SInt64            ctnrLen;
#else
    void * Right_Ctnr( void **ctnrVarPtr, long requestedLen ) {
        long            ctnrLen;
#endif
        
        void              *ctnrP = *ctnrVarPtr, *newContainer = nil;
        
        FBClearTempContainer();
        
        if ( ctnrP ) {
            ctnrLen = CtnrRequestedSize( ctnrP );
        } else{
            ctnrLen = 0;
        }
        
        if ( requestedLen > ctnrLen ) requestedLen = ctnrLen;
        if ( requestedLen < 0 )       requestedLen = 0;
        
        if ( requestedLen ) {
            newContainer = FBNewContainer( requestedLen );
            //BlockMoveData( (void*)*(ctnrH) + (ctnrLen - requestedLen), (void*)*(newHndl), length );
            memmove( newContainer, ctnrP + (ctnrLen - requestedLen ), requestedLen );
        }
        gFBTempContainer = newContainer;
        return &gFBTempContainer;
    }
    
    
    // Bug description above for Right_Ctnr also applies to Left_Ctnr  - Brian 20170915
    
#if __LP64__
    void * Left_Ctnr( void **ctnrVarPtr, SInt64 requestedLen ) {
        SInt64          ctnrLen;
#else
        void * Left_Ctnr( void **ctnrVarPtr, long requestedLen ) {
            long          ctnrLen;
#endif
            
            void *ctnrP = *ctnrVarPtr, *newContainer = nil;
            
            FBClearTempContainer();
            
            if ( ctnrP ) {
                ctnrLen = CtnrRequestedSize( ctnrP );
            } else{
                ctnrLen = 0;
            }
            
            if ( requestedLen > ctnrLen ) requestedLen = ctnrLen;
            if ( requestedLen < 0 )       requestedLen = 0;
            
            if ( requestedLen ) {
                newContainer = FBNewContainer( requestedLen );
                memmove( newContainer, ctnrP , requestedLen );
                //BlockMoveData( (void*)*(ctnrP), (void*)*(newHndl), length );
            }
            gFBTempContainer = newContainer;
            return &gFBTempContainer;
        }
        
        
#if __LP64__
        
        SInt64 Len_Ctnr( void **ctnrPtr ) {
            SInt64  theLength = 0;
#else
            UInt32 Len_Ctnr( void **ctnrPtr ) {
                UInt32  theLength = 0;
#endif
                
                if ( *ctnrPtr )  theLength = CtnrRequestedSize( *ctnrPtr );
                return theLength;
            }
            
#if __LP64__
            void * Mid_Ctnr( void **ctnrVarPtr, SInt64 startPos, SInt64 requestedLen ) {
                SInt64            ctnrLen;
#else
                void * Mid_Ctnr( void **ctnrVarPtr, long startPos, long requestedLen ) {
                    long            ctnrLen;
#endif
                    
                    void              *ctnrP, *newContainer = nil;
                    startPos--;
                    if ( startPos < 0 ) startPos = 0;
                    
                    ctnrP = *ctnrVarPtr;
                    
                    FBClearTempContainer();
                    
                    if ( ctnrP ) {
                        ctnrLen = CtnrRequestedSize( ctnrP );
                        if ( requestedLen < 0 ) requestedLen = ctnrLen;
                    } else {
                        ctnrLen = 0;
                    }
                    
                    if ( startPos > ctnrLen ) startPos = ctnrLen;
                    if ( requestedLen > ctnrLen - startPos ) requestedLen = ctnrLen - startPos;
                    
                    if ( requestedLen ) {
                        newContainer = FBNewContainer( requestedLen );
                        //BlockMoveData( *(ctnrP) + startPos, *newHndl, requestedLen );
                        memmove( newContainer, ctnrP + startPos, requestedLen );
                    }
                    gFBTempContainer = newContainer;
                    return &gFBTempContainer;
                }
                
                void MidStmt_Ctnr( void **inputCtnrPtrToPtr, SInt64 position, SInt64 length, Str255 inputPStr ) {
                    SInt64            i, pStringLength, destLength;
                    void             *ctnrP = *inputCtnrPtrToPtr;
                    
                    Str255 pStr;  PSstrcpy( pStr, inputPStr );
                    gFBStk -= 1;  // caller does stack push
                    
                    
                    pStringLength = pStr[0]; //Brian 20170713
                    if ( length <= 0 )    length = pStringLength;
                    if (position < 1 )    position = 1;
                    
                    if ( ctnrP ) {
                        destLength = CtnrRequestedSize( *inputCtnrPtrToPtr );
                        if ( length > destLength ) length = destLength;
                        if ( length > pStringLength ) length = pStringLength;
                        
                        if ( length > 0 ) {
                            position--;
                            i = 1;
                            do {
                                *(char*)( ctnrP + position + i - 1 ) = *(unsigned char*)((void*)&pStr + i);
                                i += 1;
                            } while ( i <= length );
                            
                            //while ( (1 >= 0 && i <= length) || (1 < 0 && i >= length) );
                        }
                    }
                }
                
                
                void MidStmt_Ctnr_Ctnr( void **destCntrPtrToPtr, SInt64 position, SInt64 length, void **srcCntrPtrToPtr ) {
                    SInt64              i, sourceLength, destLength;
                    void               *destP = *destCntrPtrToPtr, *sourceP = *srcCntrPtrToPtr;
                    
                    if ( sourceP ) {
                        sourceLength = CtnrRequestedSize( *srcCntrPtrToPtr );
                        if ( length < 0 )  length = sourceLength;
                        if ( position < 1 ) position = 1;
                        
                        if ( destP ) {
                            destLength = CtnrRequestedSize( *destCntrPtrToPtr );
                            if ( length > destLength )   length = destLength;
                            if ( length > sourceLength ) length = sourceLength;
                            if ( length > 0 )  {
                                position--;
                                i = 1; /* 'for' loop init */
                                do  {
                                    *(char*)( destP + position + i - 1 ) = *(unsigned char*)((void*)(sourceP) + i - 1);
                                    i += 1; /* step */
                                }
                                while ( i <= length );
                            }
                        }
                    }
                }
                
                
                // used for Unix tools only
                void PrintContainer( Handle* ctnrVarPtr )
                {
                    long              l;
                    long              p;
                    long              c;
                    Str255            t$;
                    Handle            ctnrHandle;
                    ctnrHandle = *ctnrVarPtr;
                    if ( ctnrHandle )
                    {
                        l = GetHandleSize( ctnrHandle );
                        p = 0;
                        PSstrcpy( t$, "\p" );
                        while ( -(p < l ) )
                        {
                            if ( t$[0] == 255 )
                            {
                                gFBPrintPos += printf( "%s", PScstr( PSstrcpy( TO_STACK, t$ ) ) );
                                PSstrcpy( t$, "\p" );
                            }
                            c = *(unsigned char*)(*ctnrHandle + p);
                            p++;
                            if ( c == 13 )
                            {
                                gFBPrintPos += printf( "%s", PScstr( PSstrcpy( TO_STACK, t$ ) ) );
                                printf( "\n" ); gFBPrintPos = 0;
                                PSstrcpy( t$, "\p" );
                            }
                            else
                            {
                                (PSstrcpy( TO_STACK, t$ ), PSstrcat( TO_STACK, PSstring( 1, c ) )); PSstrcpy( t$, TO_STACK );
                            }
                        } /* 'while' */
                        gFBPrintPos += printf( "%s", PScstr( PSstrcpy( TO_STACK, t$ ) ) );
                    }
                }
                
                //
                // Same as STRINGSTR_CTNR with ascii value instead of string as second parameter
                // gC1 = string$$( 500, 72 ) where 72 is an ascii value
                //
#if __LP64__
                void * StringNum_Ctnr( SInt64 length, long c ) {
                    SInt64            i = 0;
#else
                    void * StringNum_Ctnr( UInt32 length, long c ) {
                        UInt32            i = 0;
#endif
                        void             *newContainer = NULL;
                        FBClearTempContainer();
                        if ( length ) {
                            newContainer = FBNewContainer( length );
                            do {
                                *(char*)(newContainer + i ) = c;
                                i += 1;
                            }
                            while ( i <= length - 1);
                        }
                        gFBTempContainer = newContainer;
                        return &gFBTempContainer;
                    }
                    
                    
                    // dim as container     gC1
                    // gC1 = string$$( 500, "H" )
                    //
                    
#if __LP64__
                    void * StringStr_Ctnr( SInt64 length, Str255 t$_p ) {
                        SInt64   i = 0;
#else
                        void * StringStr_Ctnr( UInt32 length, Str255 t$_p ) {
                            UInt32   i = 0;
#endif
                            Str255 t$;  PSstrcpy( t$, t$_p );
                            gFBStk -= 1;
                            
                            void *newContainer = NULL;
                            FBClearTempContainer();
                            if ( length ) {
                                newContainer = FBNewContainer( length );
                                char c = *(unsigned char*)((void*)&t$ + 1);
                                do {
                                    *(char*)(newContainer + i) = c;
                                    i += 1;
                                }
                                while ( i <= length - 1);
                            }
                            gFBTempContainer = newContainer;
                            return &gFBTempContainer;
                        }
                        
                        void * UCase_Ctnr( void **ctnrVarPtr ) {
                            
#if __LP64__
                            SInt64           ctnrLen, endingAddress;
#else
                            long           ctnrLen, endingAddress;
#endif
                            
                            char *           cPtr;
                            void            *ctnrP = *ctnrVarPtr, *newContainer = nil;
                            
                            FBClearTempContainer();
                            
                            if ( ctnrP ){
                                ctnrLen = CtnrRequestedSize( ctnrP );
                            } else {
                                ctnrLen = 0;
                            }
                            if ( ctnrLen ) {
                                newContainer = FBNewContainer( ctnrLen );
                                
                                //BlockMoveData( (void*)*(long*)(ctnrP), cPtr, ctnrLen );
                                memmove( newContainer, ctnrP, ctnrLen );   // copy contents of input container to newly allocated container
                                cPtr = newContainer;                       // make our char pointer point to the first address in the new container
                                endingAddress = ctnrLen + (long)cPtr;     // add container length to beginning address in cPtr to get ending address
                                while ( ( (long)cPtr < endingAddress ) ) {  // while current address ( in cPtr ) is less than ending address
                                    *cPtr = toupper( *cPtr );
                                    cPtr++;
                                }
                            }
                            gFBTempContainer = newContainer;
                            return &gFBTempContainer;
                        }
