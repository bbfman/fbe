/*
 Prefix header for the translated product of FBtoC
 */
#ifndef FBTOC_H
#define FBTOC_H

// see AssertMacros.h      To activate, put in CFLAGS: -DDEBUG=1 
#ifdef DEBUG
#define DEBUG_ASSERT_COMPONENT_NAME_STRING "FBtoC" 
#endif

#define TO_STACK gFBStrStk[gFBStk]
//#define STACK_PUSH gFBStrStk[++gFBStk]   // replaced by STACK_PUSH(). See General.c Brian 20160515

#define USE_SJLJ_FOR_GOSUB_RETURN 1
#if USE_SJLJ_FOR_GOSUB_RETURN // see _useSJLJForGosubReturn in FBtoC source
#define GOSUB if ( !setjmp( gReturnEnv[++gReturnLevel] ) ) goto
#define RETURN_FROM_GOSUB longjmp( gReturnEnv[gReturnLevel--], 1 ) 
#endif


#define NO_SUFFIX_ON_ARRAY_NAMES 1 // matching _noSuffixOnArrayNames in FBtoC source


#if DECARBONATE
#if !__LP64__
#error DECARBONATE requires -m64 [-DDECARBONATE -m64]
#endif

#include <strings.h>
#include <CoreServices/CoreServices.h> /* mainly for MacTypes.h> */
#else
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#endif /* DECARBONATE */

#include <termios.h>
#include <unistd.h>
#include <util.h>

////////////////////////////////////////////////////////////
// public (user-and runtime-code-reachable) declarations: //
//         struct, typedef, enum, #define                 //
////////////////////////////////////////////////////////////

typedef struct termios termios; // so we don't need to preface termios vars with 'struct'


#if __LP64__
// resurrect some things inconveniently missing from 64-bit Carbon
typedef unsigned char *                 StringPtr;
static inline void BlockZero( void *destPtr, Size byteCount ) { if ( byteCount > 0 ) bzero( destPtr, byteCount ); }
static inline void BlockMoveData( const void *srcPtr, void *destPtr, Size byteCount ) { if ( byteCount > 0 ) memmove( destPtr, srcPtr, byteCount ); }
static inline void BlockMove( const void *srcPtr, void *destPtr, Size byteCount ) { if ( byteCount > 0 ) memmove( destPtr, srcPtr, byteCount ); }
#endif  /* __LP64__ */


typedef signed char  FBBoolean;
#define ZTRUE -1

// custom-sized strings [those already in MacTypes.h are commented out] 
typedef unsigned char Str1[2];
typedef unsigned char Str3[4];
typedef unsigned char Str5[6];
typedef unsigned char Str7[8];
typedef unsigned char Str9[10];
typedef unsigned char Str11[12];
typedef unsigned char Str13[14];

#if DECARBONATE
typedef unsigned char Str15[16];
#endif

typedef unsigned char Str17[18];
typedef unsigned char Str19[20];
typedef unsigned char Str21[22];
typedef unsigned char Str23[24];
typedef unsigned char Str25[26];

#if DECARBONATE
typedef unsigned char Str27[28];
#endif

typedef unsigned char Str29[30];

#if DECARBONATE
typedef unsigned char Str31[32];
#endif

typedef unsigned char Str33[34];
typedef unsigned char Str35[36];
typedef unsigned char Str37[38];
typedef unsigned char Str39[40];
typedef unsigned char Str41[42];
typedef unsigned char Str43[44];
typedef unsigned char Str45[46];
typedef unsigned char Str47[48];
typedef unsigned char Str49[50];
typedef unsigned char Str51[52];
typedef unsigned char Str53[54];
typedef unsigned char Str55[56];
typedef unsigned char Str57[58];
typedef unsigned char Str59[60];
typedef unsigned char Str61[62];

#if DECARBONATE
typedef unsigned char Str63[64];
#endif

typedef unsigned char Str65[66];
typedef unsigned char Str67[68];
typedef unsigned char Str69[70];
typedef unsigned char Str71[72];
typedef unsigned char Str73[74];
typedef unsigned char Str75[76];
typedef unsigned char Str77[78];
typedef unsigned char Str79[80];
typedef unsigned char Str81[82];
typedef unsigned char Str83[84];
typedef unsigned char Str85[86];
typedef unsigned char Str87[88];
typedef unsigned char Str89[90];
typedef unsigned char Str91[92];
typedef unsigned char Str93[94];
typedef unsigned char Str95[96];
typedef unsigned char Str97[98];
typedef unsigned char Str99[100];
typedef unsigned char Str101[102];
typedef unsigned char Str103[104];
typedef unsigned char Str105[106];
typedef unsigned char Str107[108];
typedef unsigned char Str109[110];
typedef unsigned char Str111[112];
typedef unsigned char Str113[114];
typedef unsigned char Str115[116];
typedef unsigned char Str117[118];
typedef unsigned char Str119[120];
typedef unsigned char Str121[122];
typedef unsigned char Str123[124];
typedef unsigned char Str125[126];
typedef unsigned char Str127[128];
typedef unsigned char Str129[130];
typedef unsigned char Str131[132];
typedef unsigned char Str133[134];
typedef unsigned char Str135[136];
typedef unsigned char Str137[138];
typedef unsigned char Str139[140];
typedef unsigned char Str141[142];
typedef unsigned char Str143[144];
typedef unsigned char Str145[146];
typedef unsigned char Str147[148];
typedef unsigned char Str149[150];
typedef unsigned char Str151[152];
typedef unsigned char Str153[154];
typedef unsigned char Str155[156];
typedef unsigned char Str157[158];
typedef unsigned char Str159[160];
typedef unsigned char Str161[162];
typedef unsigned char Str163[164];
typedef unsigned char Str165[166];
typedef unsigned char Str167[168];
typedef unsigned char Str169[170];
typedef unsigned char Str171[172];
typedef unsigned char Str173[174];
typedef unsigned char Str175[176];
typedef unsigned char Str177[178];
typedef unsigned char Str179[180];
typedef unsigned char Str181[182];
typedef unsigned char Str183[184];
typedef unsigned char Str185[186];
typedef unsigned char Str187[188];
typedef unsigned char Str189[190];
typedef unsigned char Str191[192];
typedef unsigned char Str193[194];
typedef unsigned char Str195[196];
typedef unsigned char Str197[198];
typedef unsigned char Str199[200];
typedef unsigned char Str201[202];
typedef unsigned char Str203[204];
typedef unsigned char Str205[206];
typedef unsigned char Str207[208];
typedef unsigned char Str209[210];
typedef unsigned char Str211[212];
typedef unsigned char Str213[214];
typedef unsigned char Str215[216];
typedef unsigned char Str217[218];
typedef unsigned char Str219[220];
typedef unsigned char Str221[222];
typedef unsigned char Str223[224];
typedef unsigned char Str225[226];
typedef unsigned char Str227[228];
typedef unsigned char Str229[230];
typedef unsigned char Str231[232];
typedef unsigned char Str233[234];
typedef unsigned char Str235[236];
typedef unsigned char Str237[238];
typedef unsigned char Str239[240];
typedef unsigned char Str241[242];
typedef unsigned char Str243[244];
typedef unsigned char Str245[246];
typedef unsigned char Str247[248];
typedef unsigned char Str249[250];
typedef unsigned char Str251[252];
typedef unsigned char Str253[254];

#if DECARBONATE
typedef unsigned char Str255[256];
#endif


// dynamic array support
#define kMaxNumDynArrays 64

typedef struct DynArrayInfo
{
	int		elemSize;
	int		maxIndex;
	int		lastElem; // for autoXRefCurr emulation
	void *	base;
} DynArrayInfo;




// system function expressions
enum {
	//	kFBAplVol		= 1,
	kFBAplRes		= 4,
	kFBMemAvail		= 5,
	kFBScrnWidth	= 6,
	kFBScrnHeight	= 7,
	kFBSysVers		= 8,
	kFBAplActive	= 9,
	kFBMaxColors	= 10,
	kFBCrntDepth	= 11,
	kFBMachType		= 13,
	kFBCPUSpeed		= 15,
	kFBClockSpeed	= 16,
	kFBLastCursType	= 17,
	kFBAplVRefNum	= 18,
	kFBAplParID		= 19
};

#if !DECARBONATE

// files$ Nav dialog types, matching enum in Subs Files.incl 
enum {
	//	kFBSave					= 0,
	//	kFBOpen					= 1,
	//	kFBFOpenPreview			= 3,
	//	kFBFFolder				= 4,
	kFBFSSpecOpen			= 6,
	kFBFSSpecOpenPreview	= 7,
	kFBFSSpecSave			= 8,
	kFBFSSpecFolder			= 9,
	kFBFSRefOpen			= 10,
	kFBFSRefSave			= 11,
	kFBFSRefFolder			= 12,
	kFBCFURLRefOpen			= 13,
	kFBCFURLRefSave			= 14,
	kFBCFURLRefFolder		= 15,
};


// NavDialog types
enum {
	kNavDialogGetFile		= 0,
	kNavDialogPutFile		= 1,
	kNavDialogChooseFile	= 2,
	kNavDialogChooseFolder	= 3,
	kNavDialogChooseVolume	= 4,
	kNavDialogChooseObject	= 5
};

// NavDialogAsk types			bw:20081205
enum {
	kNavDialogAskSaveChanges,
	kNavDialogAskDiscardChanges,
	kNavDialogAskReviewDocuments
};

// NavDialog options
enum {
	kNavDialogSheet				= ( 1 << 8 ),
	kNavDialogMultiple			= ( 1 << 9 ),
	kNavDialogInvisible			= ( 1 << 10 ),
	kNavDialogSupportPackages	= ( 1 << 11 ),
	kNavDialogOpenPackages		= ( 1 << 12 ),
	kNavDialogPreserveFileExt	= ( 1 << 13 )
};



/*
 FB dialog event types  {bw:20080429}
 */
enum {
	kFBBtnClick			= 1,
	kFBEFClick			= 2,
	kFBWndClick			= 3,
	kFBWndClose			= 4,
	kFBWndRefresh		= 5,
	kFBEFReturn			= 6,
	kFBEFTab			= 7,
	kFBWndZoomIn		= 8,
	kFBWndZoomOut		= 9,
	kFBEFShiftTab		= 10,
	kFBEFClear			= 11,
	kFBEFLeftArrow		= 12,
	kFBInToolbarBtn		= 13,
	kFBEFUpArrow		= 14,
	kFBEFDownArrow		= 15,
	kFBEvKey			= 16,
	kFBEvDiskInsert		= 17,
	kFBWndActivate		= 18,
	kFBMFEvent			= 19,
	kFBCursEvent		= 20,
	kFBCursOverBtn		= 21,
	kFBCursOver			= 21,
	kFBPreview			= 22,
	kFBUserDialog		= 23,
    
    //	kFBCntxtMenuClick	= 24,  wrong until 20120218; see Rntm Appearance.incl
    //	kFBEvCmdKey			= 24,
    //	kFBPFClick			= 25,
    //	kFBEFSelected		= 26,
    //	kFBCursOverEF		= 27,
    //	kFBCursOverPF		= 28,
    //	kFBCursOverNothing	= 29,
    //	kFBWndResized		= 30,
    //	kFBFBQuitEvent		= 31,
    //	kFBToolbarClick		= 33
    
    kFBEvCmdKey			= 24,
	kFBCursOverNothing	= 29,
	kFBWndResized		= 30,
    kFBCntxtMenuClick	= 31,
	kFBToolbarClick		= 33
};


/*
 FB dialog preview event IDs {bw:20080429}
 */
enum {
	kFBPremenuClick		= 1,
	kFBPreWndGrow		= 2,
	kFBWndMoved			= 3,
	kFBWndSized			= 4,
	kFBEFChanged		= 5,
	kFBPreEFClick		= 6,
	kFBPreWndZoomIn		= 7,
	kFBPreWndZoomOut	= 8,
	kFBWndDocWillMove	= 9
};


/*
 FB dialog MF event IDs {bw:20080429}
 */
enum {
	kFBMFResume		= 1,
	kFBMFSuspend	= 2,
	kFBMFClipboard	= 3,
	kFBMFMouse		= 4
};

#endif // !DECARBONATE



#if __LP64__

typedef struct FBEventThing // cocoaui 20170814 bw
{
    long theEvent;
} FBEventThing;

#else

typedef struct FBEventThing 
{
	UInt32		eventTicks; // 8 bytes before theEvent (whose address is given by FB's event function), so we can write 'poke long event - 8, ticksToWait'
	UInt32		dummy;
        
#if !DECARBONATE
	EventRecord	theEvent;
#endif
    
} FBEventThing;  

#endif  //__LP64__


typedef void (*FBOnXxxxxVector)();




typedef struct FBAppEventInfo { // cocoaui 20170704 bw
    long appEventKind;
    long appEventID;
} FBAppEventInfo;



#if 1// cocoaui 20170704 bw

typedef struct FBDialogEventInfo
{
    long			fbEventKind;
    long			fbEventID;
    long			fbWndNum; // rp 20120207
} FBDialogEventInfo;

/*
 FB window types {bw:20080429}
 */
enum {
    kFBDoc				= 1,
    kFBDialogFrame		= 2,
    kFBDialogPlain		= 3,
    kFBDialogShadow		= 4,
    kFBDocNoGrow		= 5,
    kFBDialogMovable	= 6,
    kFBDocZoom			= 9,
    kFBDocRound			= 17,
    kFBWDEFBaseID		= 129
};


/*
 FB palette window variations {bw:20080429}
 */
enum {
    kFBWDEFSideDrag	= 2,
    kFBWDEFHasZoom	= 8
};


// FBWindowFunction expressions
enum {
    kFBActiveWnd			= 0,
    kFBOutputWnd			= 1,
    kFBWidth				= 2,
    kFBHeight				= 3,
    kFBPenH					= 4,
    kFBPenV					= 5,
    kFBToLeft				= 10,
    kFBToTop				= 11,
    kFBToRight				= 12,
    kFBToBottom				= 13,
    kFBWndRef				= 14,
    kFBWndPointer			= kFBWndRef,
    kFBEfNum				= 15,
    kFBSelStart				= 16,
    kFBSelEnd				= 17,
    kFBEfClass				= 24,
    kFBOutputWCategory		= 25,
    kFBActiveWCategory		= 26,
    kFBActiveDoc			= 27,
    kFBActivePlt			= 28,
    kFBWndPort				= 31,
    kFBStructureTop			= 1000,
    kFBStructureLeft		= 1001,
    kFBStructureWidth		= 1002,
    kFBStructureHeight		= 1003,
    kFBContentTop			= 1004,
    kFBContentLeft			= 1005,
    kFBContentWidth			= 1006,
    kFBContentHeight		= 1007,
    kFBMacWClass			= 1008,
    kFBMacWAttributes		= 1009,
    kFBFloatingPtr			= 1012
};

#else// cocoaui 20170704 bw

#if !DECARBONATE
typedef struct FBDialogEventInfo 
{
	long			fbEventKind;
	long			fbEventID;
	long			fbWndNum; // rp 20120207
} FBDialogEventInfo;



/*
 FB window types {bw:20080429}
 */
enum {
	kFBDoc				= 1,
	kFBDialogFrame		= 2,
	kFBDialogPlain		= 3,
	kFBDialogShadow		= 4,
	kFBDocNoGrow		= 5,
	kFBDialogMovable	= 6,
	kFBDocZoom			= 9,
	kFBDocRound			= 17,
	kFBWDEFBaseID		= 129
};


/*
 FB palette window variations {bw:20080429}
 */
enum {
	kFBWDEFSideDrag	= 2,
	kFBWDEFHasZoom	= 8
};


// FBWindowFunction expressions
enum {
	kFBActiveWnd			= 0,
	kFBOutputWnd			= 1,
	kFBWidth				= 2,
	kFBHeight				= 3,
	kFBPenH					= 4,
	kFBPenV					= 5,
	kFBToLeft				= 10,
	kFBToTop				= 11,
	kFBToRight				= 12,
	kFBToBottom				= 13,
	kFBWndRef				= 14,
	kFBWndPointer			= kFBWndRef,
	kFBEfNum				= 15,
	kFBSelStart				= 16,
	kFBSelEnd				= 17,
	kFBEfClass				= 24,
	kFBOutputWCategory		= 25,
	kFBActiveWCategory		= 26,
	kFBActiveDoc			= 27,
	kFBActivePlt			= 28,
	kFBWndPort				= 31,
	kFBStructureTop			= 1000,
	kFBStructureLeft		= 1001,
	kFBStructureWidth		= 1002,
	kFBStructureHeight		= 1003,
	kFBContentTop			= 1004,
	kFBContentLeft			= 1005,
	kFBContentWidth			= 1006,
	kFBContentHeight		= 1007,
	kFBMacWClass			= 1008,
	kFBMacWAttributes		= 1009,
	kFBFloatingPtr			= 1012
};

#endif /* !DECARBONATE */

#endif// cocoaui 20170704 bw


enum {
	kFBtoCAppSignature = 'FB>C',
	kFBWindowIDProperty = 'wNum',
	kFBWindowHScrlProperty = 'hScl',
	kFBWindowVScrlProperty = 'vScl',
	kFBWindowStandardHandlerRequestedProperty = 'SHnd'
};

enum { kFBParamMissing = -1999999999 };


#if !DECARBONATE

/*
 FB button types {bw:20080429}
 */
enum {
	kFBPush		= 1,
	kFBCheckbox	= 2,
	kFBRadio	= 3,
	kFBShadow	= 4
};


// FBButtonFunction selectors
enum {
	kFBGetCtlRawValue				= 1,
	kFBGetCtlMinimum				= 2,
	kFBGetCtlMaximum				= 3,
	kFBGetCtlPage					= 4,
	kFBGetRootControl				= 5,
	kFBCountSubcontrols				= 6,
	kFBGetIndexedSubcontrol			= 7,
	kFBGetSupercontrol				= 8,
	kFBGetControlDate				= 9,
	kFBGetControlTime				= 10,
	kFBGetControlTEHandle			= 11,
	kFBGetBevelControlMenuHandle	= 12,
	kFBGetBevelControlMenuVal		= 13,
	kFBGetBevelControlLastMenu		= 14,
	kFBGetControlMenuHandle			= 15,
	kFBGetControlMenuID				= 16
};


// FBButton states
enum {
	kFBButtonStateGrayBtn	= 0,
	kFBButtonStateActiveBtn	= 1,
	kFBButtonStateMarkedBtn	= 2
};


/*
 FB scroll buttons {bw:20080429}
 */
enum {
	kFBScrollOther	= 0,
	kFBScrollVert	= 1,
	kFBScrollHorz	= 2
};


/*
 FB menu states {bw:20080429}
 */
enum {
	kFBDisable	= 0,
	kFBEnable	= 1,
	kFBChecked	= 2	
};


enum {
	kControlRoundButtonProc = (31 << 4)
};



typedef struct FBMenuEventInfo 
{
	MenuID			menu;							// last selected menu id
	MenuItemIndex	item;							// last selected menu item
	MenuID			quitMenu;						// FB quit menu ID
	MenuItemIndex	quitItem;						// FB quit item ID
	MenuID			prefsMenu;						// menu param specified in FB's 'menu preferences' statement
	MenuItemIndex	prefsItem;						// item param specified in ............ " ..............
	MenuItemIndex	lastAppleItem;					// used to remove previous apple menu items if we issue a subsequent apple menu statement (FB behaviour)
} FBMenuEventInfo;


typedef struct MenuItemInfo 
{
	CFMutableStringRef		title;
	long					state;
	UniChar					cmdKey;
	UniChar					markChar;
	CFIndex					item;
} MenuItemInfo;

#else	// 20170914 bw       // use this struct when -DDECARBONATE is specified in build settings

typedef struct FBMenuEventInfo	// CocoaUI only uses the menu and item fields
{
    SInt32    menu;                            // last selected menu id
    SInt32    item;                            // last selected menu item
} FBMenuEventInfo;

#endif /* !DECARBONATE */


enum {
	kMaxFBFiles = 256,
	kFBMaxSerialFiles = 12
};


/*
 See also OSX SerialIO.incl, where this declaration is shadowed
 
 */
typedef struct  __attribute__ ((__packed__))  FBInfoForOpenFile 
{
	// all: open "UNIX", disk files, serial 
	Boolean			isOpen;
	FBBoolean		eof;
	// open "UNIX" only
	FILE *			pipeUNIX;       // see FBOpenUNIX()
	// disk files only
    UInt64			savedOffset;    // was: SInt16 forkRefNum; Brian 20170114
    CFURLRef        url;            // was: FSRef  fileRef;    Brian 20170114
	SInt64			recordSize, linePos;
	// serial only  see FBSerialOpen (in OSX SerialIO.incl)
	//struct termios	originalOptions; removed rp 20080209
	void *			buff;
	ByteCount		buffSize, bytesInBuff;
	SInt32			fileDescriptor;
} FBInfoForOpenFile;


#if !__LP64__

typedef struct FBFilesDollarInfo 
{
	NavTypeListHandle	typeListH;
	OSType				lastType;
} FBFilesDollarInfo;

#endif  /* !__LP64__ */


typedef struct  __attribute__ ((__packed__))     ldesRec // see case kControlListBoxProc in FBAppearanceButton()
    {
        short		versionNumber;
        short		numberOfRows;
        short		numberOfColumns;
        short		cellHeight;
        short		cellWidth;
        Boolean		hasVertScroll;
		char		filler1;
        Boolean		hasHorizScroll;
        char		filler2;
        short		LDEFresID;
        Boolean		hasSizeBox;
        char		reserved;
    } ldesRec;

#if !DECARBONATE


// FBFinderInfo vartype param
enum {
	kFBFinderInfoVarTypeFSSpec,
	kFBFinderInfoVarTypeFSRef,
	kFBFinderInfoVarTypeCFURL
};

// results of FinderInfo
enum {
	kFBFinderInfoOpen,
	kFBFinderInfoPrint,
	kFBFinderInfoErr	
};
#endif /*  !DECARBONATE */


// prototypes for data read/restore (see EmitFBData() in FBtoC source)
StringPtr FBReadData();
void FBRestore( short n );
/*
#if !DECARBONATE

// prototypes for Util_FileDirectory.c
OSStatus
FD_FSRefCreateCFURL( FSRef *ref, CFURLRef *url );
OSStatus
FD_FSRefGetFSSpec( FSRef *ref, FSSpec *spec );
OSStatus
FD_CFURLGetFSRef( CFURLRef url, FSRef *ref );
OSStatus
FD_CFURLGetFSSpec( CFURLRef url, FSSpec *spec );
OSStatus
FD_FSSpecGetFSRef( FSSpec *spec, FSRef *ref );
OSStatus
FD_FSSpecCreateCFURL( FSSpec *spec, CFURLRef *url );
Boolean
FD_ObjectExistsFSRef( CFStringRef name, FSRef *parRef );
Boolean
FD_ObjectExistsCFURL( CFURLRef url );
Boolean
FD_ObjectExistsFSSpec( FSSpec *spec );
OSStatus
FD_PathGetFSRef( CFStringRef path, FSRef *parRef, FSRef *ref );
OSStatus
FD_PathCreateCFURL( CFStringRef path, CFURLRef parUrl, CFURLRef *url );
OSStatus
FD_PathGetFSSpec( CFStringRef path, FSSpec *parSpec, FSSpec *spec );
OSStatus
FD_FSRefCreatePath( FSRef *ref, CFStringRef *path );
OSStatus
FD_CFURLCreatePath( CFURLRef url, CFStringRef *path );
OSStatus
FD_FSSpecCreatePath( FSSpec *spec, CFStringRef *path );
OSStatus
FD_CreateDirectoryWithFSRef( CFStringRef name, FSRef *parRef, FSRef *dirRef );
OSStatus
FD_CreateDirectoryWithCFURL( CFStringRef name, CFURLRef parUrl, CFURLRef *dirUrl );
OSStatus
FD_CreateDirectoryWithFSSpec( CFStringRef name, FSSpec *parSpec, FSSpec *dirSpec );
OSStatus
FD_ParentDirectoryGetFSRef( FSRef *ref, FSRef *parRef );
OSStatus
FD_ParentDirectoryCreateCFURL( CFURLRef url, CFURLRef *parUrl );
OSStatus
FD_ParentDirectoryGetFSSpec( FSSpec *spec, FSSpec *parSpec );
OSStatus
FD_ApplicationDirectoryGetFSRef( FSRef *appRef );
OSStatus
FD_ApplicationDirectoryCreateCFURL( CFURLRef *appUrl );
OSStatus
FD_ApplicationDirectoryGetFSSpec( FSSpec *appSpec );
OSStatus
FD_SpecialDirectoryGetFSRef( FSVolumeRefNum vRefNum, OSType folderType, FSRef *ref );
OSStatus
FD_SpecialDirectoryCreateCFURL( FSVolumeRefNum vRefNum, OSType folderType, CFURLRef *url );
OSStatus
FD_SpecialDirectoryGetFSSpec( FSVolumeRefNum vRefNum, OSType folderType, FSSpec *spec );
OSStatus
FD_RenameObjectFSRef( FSRef *ref, CFStringRef name, FSRef *outRef );
#endif //  !DECARBONATE 
*/

#endif  // FBTOC_H 
