#ifndef RUNTIME_H
#define RUNTIME_H

// Dev flags
#define kNSFBFullStop 1 // 20160408
#define kSmartTabsRadioGroup 1  // 20160515


/* temp_includes.c */
//
// gFBStrStk size increased ( to 127 )to bypass bug that causes crash when memmove() overwrites another var's contents. Brian 20160514
//
extern Str255 gFBStrStk[64];
extern SInt16 gFBStk;
extern Str255 gReturnedString;
extern Str255 gFBTStk[16];
extern SInt8 gFBTStkP;
extern UInt8 gPSDefStrSize;
extern UInt8 gPSDefTabStops;
extern Boolean gFBLeadingSpace;
extern UInt16 gFBPrintPos;
extern char gFBUsingSymbols[4];
extern UInt32 gFBNextData;

#if USE_SJLJ_FOR_GOSUB_RETURN // see _useSJLJForGosubReturn in FBtoC source
extern jmp_buf gReturnEnv[1024];
#else
extern UInt16 gReturnLabelNum[1024];
#endif

extern UInt16 gReturnLevel;
extern long gSelectL[10];
extern double gSelectD[10];
extern Str255 gSelectS[10];
extern OSStatus gFBSysError;
extern volatile FBBoolean gFBSndBusy;
extern DynArrayInfo gFBDynArrayInfo[64];
extern void *gFBTempContainer;
extern void *gFBTemp2Container;
#if !__LP64__
extern FBFilesDollarInfo gFBFilesInfo;
#endif  /* !__LP64__ */

extern OSStatus gFBFileError;
extern OSType gFBSaveType;
extern OSType gFBSaveCreator;
//extern FSRef gFBDefaultFolderRef;
//extern FSSpec gFBDefaultFolderSpec;
extern Str255 gFBLineBuf;
extern UInt32 gFBLineDev;
extern UInt32 gFBLinePos;
extern SInt64 gFBLastFilePos;
extern char gFBFileTabMode;
extern UInt16 gFBTabWidth;

#if NO_SUFFIX_ON_ARRAY_NAMES
extern FBInfoForOpenFile gFBOpenFiles[kMaxFBFiles + kFBMaxSerialFiles];
#else
extern FBInfoForOpenFile gFBOpenFiles_A[kMaxFBFiles + kFBMaxSerialFiles];
#endif

extern FBOnXxxxxVector gFBOnErrorVector;
extern CFMutableArrayRef gFBIndex[9 + 1];
extern FBBoolean gFBQuit;

extern FBOnXxxxxVector gFBOnAppEventVector; // cocoaui 20170704 bw

extern FBOnXxxxxVector gFBOnEventVector;
extern FBOnXxxxxVector gFBOnDialogVector;
extern FBOnXxxxxVector gFBOnMouseVector;
extern FBOnXxxxxVector gFBOnMenuVector;
extern FBOnXxxxxVector gFBOnTimerVector;
extern FBOnXxxxxVector gFBOnFinderInfoVector;
extern FBEventThing gFBEventThing;


#if 1// cocoaui 20170704 bw

extern FBDialogEventInfo gFBDialogEventInfo;

void WindowSetTitle( SInt32 wndNum, CFStringRef title );// 20170923 bw - moved here so it can be shared by Carbon and CocoaUI windows

#if !DECARBONATE

extern FBMenuEventInfo gFBMenuEventInfo;

extern UInt32 gFBMouseClickCount;
extern Point gFBLastMousePt;
extern short gFBAplActive;
extern long gFBLastCursor;
extern long gFBLastCursType;
extern long gFBLastCrsrWnd;
extern LongDateRec gFBControlLongDate;
extern SInt64 gFBControlSeconds;
extern Str255 gFBControlText$;
extern CGrafPtr gFBBadPort;
extern const UInt32 kFBEFClassTag;
extern const OSType kFBWindowCategoryTag;

#endif// !DECARBONATE

#else// cocoaui 20170704 bw

#if !DECARBONATE
extern FBMenuEventInfo gFBMenuEventInfo;
extern FBDialogEventInfo gFBDialogEventInfo;
extern UInt32 gFBMouseClickCount;
extern Point gFBLastMousePt;
extern short gFBAplActive;
extern long gFBLastCursor;
extern long gFBLastCursType;
extern long gFBLastCrsrWnd;
extern LongDateRec gFBControlLongDate;
extern SInt64 gFBControlSeconds;
extern Str255 gFBControlText$;
extern CGrafPtr gFBBadPort;
extern const UInt32 kFBEFClassTag;
extern const OSType kFBWindowCategoryTag;
#endif /* !DECARBONATE */

#endif// cocoaui 20170704 bw

extern unsigned char gFBFloatMaxDigits;
extern long gFBWidth;
extern long gFBWidthLprint;
extern long gFBRoute;
extern const Boolean gNumberLeadingSpace;

#if NO_SUFFIX_ON_ARRAY_NAMES
extern Handle gFBBuffer[6];
#else
extern Handle gFBBuffer_A[6];
#endif

#if !DECARBONATE
extern short gFBRatioX;
extern short gFBRatioY;
extern short gGrafLastX;
extern short gGrafLastY;
extern short gFBPagefn;
extern short gFBStdFont;
extern short gFBStdSize;
extern PMPrintSession gFBPrintSession;
extern PMPrintSettings gFBPrintSettings;
extern PMPageFormat gFBPageFormat;
extern long gFBFirstPrintedPage;
extern long gFBLastPrintedPage;
extern Handle gFBPRHandle;
extern CGrafPtr gFBPRPort;
extern Rect gFBPrintRect;
extern FBBoolean gFBPRCancel;
extern PicHandle gFBTempPictureH;
extern short gTextFont;
extern short gTextSize;
extern short gTextFace;
extern short gTextMode;
#endif /* !DECARBONATE */

extern Boolean gThreadsUsed;
extern short gFBInThread;
extern short gFBAbortThreads;
extern long gFBThreadList[32];
extern long gFBThreadFlags[32];
extern long gFBThreadProc[32];
extern CFMutableArrayRef sFinderInfoArray;
extern const UInt32 kFBEFFilter;
extern Str1 gFBTEKey;
extern FBOnXxxxxVector gFBOnEditVector;
/* temp_includes.c */
StringPtr PSltrl(char *inCString, char *commentForGenstrings);
StringPtr PSltrr(char *inCString, char *commentForGenstrings);
void DebugPrintStackLevel(void);
void FBKillDynamicArray(const int dynArrayNum);
void FBGrowDynamicArray(const int dynArrayNum, const int index);
void FBWriteDynamic(int fileRef, int dynArrayNum);
void FBReadDynamic(int fileRef, int dynArrayNum);
void FBSwap(void *p1, void *p2, const int bytes);
StringPtr PScstr(Str255 s);
StringPtr CSpstr(char *s);
void PSinput( StringPtr s );

StringPtr STACK_PUSH(); // replacement for STACK_PUSH macro Brian 20160515

StringPtr PSstrcpy(StringPtr dest, ConstStr255Param source);
StringPtr PSstrcpyn(StringPtr dest, ConstStr255Param source, const int destSize);
StringPtr PSstrcat( StringPtr dest, ConstStr255Param appendStr);
StringPtr PSstrcatn(StringPtr dest, ConstStr255Param source, const int destSize);
void PSdeftab(const int tabstops);
void FBDefUsing(const SInt32 formatchars);
StringPtr PSusing(Str255 format, const double n);
StringPtr PSleft(ConstStr255Param s, SInt16 n);
StringPtr PSright(ConstStr255Param s, SInt16 n);
StringPtr PSmid(ConstStr255Param s, SInt16 p, SInt16 n);
StringPtr PSucase(ConstStr255Param s);
StringPtr PSstring(SInt16 n, UInt8 theChar);
StringPtr PSstringstr(SInt16 n, ConstStr255Param s);
StringPtr PSstrampersand(Handle h, int index);
StringPtr PSstrhash(const int resID, const int index);
StringPtr PSinkey( void );
StringPtr PSpstr(StringPtr p);
CFStringRef DateFormatCreateStringWithAbsoluteTime( CFStringRef formatStr, CFAbsoluteTime at );
StringPtr PSdatePStime( ConstStr255Param s );
SInt16 STRLeadingSpace(void);
void PSdefstr(const UInt8 length);
StringPtr PSstr(const double n);
StringPtr PShex(SInt32 num);
StringPtr PSoct( const unsigned long n );
StringPtr PSbin(const SInt32 n);
StringPtr PSuns( const SInt32 n );
StringPtr PSmki(UInt32 n);
SInt8 FBPopCmpStr(Str255 s1, Str255 s2);
int FBcvi(ConstStr255Param s);
UInt8 PSlen(ConstStr255Param s);
UInt8 PSasc(ConstStr255Param s);
UInt8 PSinstr(int startPos, Str255 haystack, Str255 needle);
SInt64 PSvalint(Str255 s);
double PSval(Str255 s);
long MidStmt(void *strvPtr, long position, long length, Str255 s_p);
double frac(const double x);
double sgn(const double value);
SInt32 sgni(const SInt32 value);
SInt32 maybe(void);
long rnd(long expr);
int FBmax(const SInt32 n1, const SInt32 n2);
void FBDelay(const Duration t);
void FBGetProcessInfo(long index, StringPtr s, ProcessSerialNumber *psnPtr);
SInt32 FBSystemFunction( const SInt32 expr );
SInt32 OSVersion( void );
void OSMajorMinor( short *major, short *minor );
void HideGrowBox( WindowRef w );

#if !DECARBONATE
void FBRun(Str255 path, short vRefNum, long dirID);
void FBKillPicture(PicHandle *picHPtr);
void FBKillField(Handle h);
#endif /* !DECARBONATE */

void FBRunPath( CFStringRef path ); // 20151002 bw
void FBRunURL( CFURLRef url );      // 20151002 bw

long FBUsrFnIndicator(long indicator, long expr);
void FBPstrDollarStatement(void *address, void *s);
////////////////////////////////////////////////////////
//   Container declares from Container.c              //
////////////////////////////////////////////////////////
StringPtr GetCtnrString(void **srcCtnrVar, StringPtr s, short lstring);
StringPtr CatCtnrString(void **srcCtnrVar, StringPtr sPtr, long lstring);
void FBCtnrCopy(void **srcCtnrVar, void **destCtnrVar);
void * FBCtnrCopy3( void **destCtnrVar, void **srcCtnrVar );

//void * BuildContainer(void *ctnrVarPtr, SInt64 size);
void *CtnrStoreString(void **ctnrVarPtr, Str255 t$_p);
void CtnrFree(void * ptr);

long Asc_ctnr( void **ctnrVarPtr);
void PrintContainer( Handle *ctnrVarPtr );
void FBClearTempContainer(void);
void FBClearTemp2Container(void);


void *AddStringToCtnr(void **ctnrVarPtr, Str255 t$_p);
void *AddCtnrs( void **ctnrV1Ptr, void **ctnrV2Ptr);

SInt64 FBCompareHandles( void **ctnr1, void **ctnr2 );
SInt64 FBCompareContainers( void **ctnr1, void **ctnr2 );
Boolean FBCmpCtnr(void **ctnr1, void **ctnr2);
void *UCase_Ctnr(void **ctnrVarPtr);

#if __LP64__
void * FBNewContainer(SInt64 length);
SInt64 CtnrRequestedSize(void * ptr);
void *Left_Ctnr( void **ctnrVarPtr, SInt64 length);
SInt64 Len_Ctnr( void **ctnrPtr);
void *Mid_Ctnr( void **ctnrVarPtr, SInt64 startPos, SInt64 length);
void *Right_Ctnr( void **ctnrVarPtr, SInt64 length);
void *StringNum_Ctnr(SInt64 length, long c);
void *StringStr_Ctnr(SInt64 length, Str255 t$_p);

#else

void * FBNewContainer(UInt32 length);
UInt32 CtnrRequestedSize(void * ptr);
void *Left_Ctnr( void **ctnrVarPtr, long length);
UInt32 Len_Ctnr( void **ctnrPtr);
void *Mid_Ctnr( void **ctnrVarPtr, long startPos, long length);
void *StringNum_Ctnr(UInt32 length, long c);
void *Right_Ctnr( void **ctnrVarPtr, long length);
void *StringStr_Ctnr(UInt32 length, Str255 t$_p);
#endif

void MidStmt_Ctnr(void **ctnrVarPtr, SInt64 position_L, SInt64 length, Str255 st$_p);
void MidStmt_Ctnr_Ctnr( void **ctnrVarPtr, SInt64 position_L, SInt64 length, void **srcCtnrVar );
SInt64 Instr_Ctnr(long position_L, void **BigctnrVarPtr, void **SearchctnrVarPtr);
SInt64 Instr_Ctnr_PStr(long position_L, void **BigctnrVarPtr, Str255 st$_p);
//void CtnrStoreResID(long userID, Handle *destCtnrVar);
////////////////////////////////////////////////////////
//   End of container declares from Container.c       //
////////////////////////////////////////////////////////

#if !__LP64__
#if !DECARBONATE
void NavDialog_SetFilterFn(void *filterFn);
long NavDialog_GetItemCount(NavReplyRecord *reply);
void NavDialog_GetItemFSSpec(NavReplyRecord *reply, long index, FBBoolean noNameVariant, FSSpec *outSpec);
void NavDialog_GetItemFSRef(NavReplyRecord *reply, long index, FSRef *outRef);
void NavDialog_CopyItemName(NavReplyRecord *reply, long index, CFStringRef *name);
void NavDialog_GetItemNameAsPascalString(NavReplyRecord *reply, long index, Str255 *name);
void NavDialog_CopyItemCFURLRef(NavReplyRecord *reply, long index, CFURLRef *outUrlRef);
void NavDialog_GetSaveFileNameAsPascalString(NavReplyRecord *reply, Str255 *s);
CFStringRef NavDialog_CopySaveFileName(NavReplyRecord *reply);
void NavDialog_AddUTIPascalString(ConstStr255Param uti);
void NavDialog_AddUTICFString(CFStringRef uti);
void NavDialog_ChangeOptionFlags(NavDialogOptionFlags flagsToSet, NavDialogOptionFlags flagsToClear);
void NavDialog_SetPreserveSaveFileExtension(void);
void NavDialog_SetSaveFileName(CFStringRef string);
void NavDialog_SetLocation(Point pt);
void NavDialog_SetClientName(CFStringRef string);
void NavDialog_SetWindowTitle(CFStringRef string);
void NavDialog_SetActionButtonLabel(CFStringRef string);
void NavDialog_SetCancelButtonLabel(CFStringRef string);
void NavDialog_SetMessage(CFStringRef string);
void NavDialog_SetPreferenceKey(UInt32 key);
void NavDialog_SetModality(WindowModality modality);
void NavDialog_SetParentWindow(WindowRef w);
void NavDialog_AddPopupExtension(CFStringRef string);
void NavDialog_SetPopupExtension(CFArrayRef array);
Boolean NavDialog_UTIConformsTo(AEDesc *theItem, CFStringRef inUTI);
void NavDialog(UInt32 type, ConstStr255Param prompt, ConstStr255Param typeListOrDefaultSaveName, void *callbackFunction, void *userData);
StringPtr FBFiles(UInt32 dialogType, Str255 typeList, Str255 prompt, void *fs);
void NavDialogAsk(UInt32 type, UInt32 actionDocCount, void *callbackFunction, void *userData);
//void FBInitDefaultFolderRef(void);
#endif // !DECARBONATE
#endif  /* !__LP64__ */

OSStatus FBerror(void);
void FBFileErrorAlert(int fileID, OSStatus err);
void FBCheckFileError(int fileID, OSStatus err);
void FBOpenUNIX(unsigned fileID, Str255 command);
void FBOpenOutput(const int fileID, void *fileNamePtr, void *fs);
void FBOpenRandom(const int fileID, void *fileNamePtr, void *fs, SInt64 recLen, Boolean exclusive);
void FBOpenAppend(const int fileID, void *fileNamePtr, void *fs, SInt64 recLen);
void FBOpenInput(const int fileID, void *fileNamePtr, void *fs, SInt64 recLen, Boolean isResourceOpen);
void FBClose(int fileID);
long FBeof(int fileID);
#if __LP64__
SInt64 FBlof(int fileID, SInt64 recLen);
#else
UInt32 FBlof(int fileID, UInt32 recLen);
#endif
void FBAppend(const int fileID);
void FBRecord(const int fileID, SInt64 recNum, SInt64 posInRec);
SInt64 FBrec(int fileID);
SInt64 FBloc(int fileID);
void FBLineInputHash(int fileID, StringPtr s);
//ByteCount FBReadFile(const int fileID, CFMutableStringRef mutStr, Boolean isInputHashString, ByteCount numBytesRequested);
ByteCount FBReadFile(const int fileID, void *dest, Boolean isInputHashString, ByteCount numBytesRequested);
StringPtr FBInkeyFromFile(const int fileID);
short FBReadSwapShort(const unsigned fileID);
long FBReadSwapLong(const unsigned fileID);
float FBReadSwapSingle(const unsigned fileID);
double FBReadSwapDouble(const unsigned fileID);
//void FBReadField(const int fileID, Handle *outHandle);
void FBParseInput(const short varType, void *vPtr);
void FBToCInputFile(const int fileID, void *target, short varType);
void FBReadString( const int fileID, void * dest, ByteCount numBytesRequested );
void FBWriteFile(const int fileID, void *src, ByteCount numBytes);
//void FBWriteFile(const int fileID, CFStringRef s, ByteCount numBytes);
void FBWriteString( const int fileID, char * src, ByteCount varLength );
void FBWriteSwapShort(short s, const unsigned fileID);
void FBWriteSwapLong(long l, const unsigned fileID);
void FBWriteSwapSingle(float f, const unsigned fileID);
void FBWriteSwapDouble(double d, const unsigned fileID);
//void FBWriteField(int fileID, Handle inHandle);
void FBFilePrintChar(char theChar);
SInt64 FBFileGetPos(void);
void FBFilePrintTab(void);
void FBFilePrintCR(void);
void FBFilePrintString(Str255 s_p);
//StringPtr FBFilesLastType(void);
void FBSetOpenType(Str15 opentype$_p);

//  32-bit utilities renamed '_OLD' while replacement assume the previous name without the '_OLD' Brian 20170214
#if !__LP64__
void FBRename_OLD(Str255 oldName, Str255 newName, short vRefNum, long dirID);
void FBKill_OLD(Str255 path, short vRefNum, long parID);
#endif  /* !__LP64__ */

void FBRename(CFURLRef oldName, CFURLRef newName );
void FBKill( CFURLRef fileName );



OSStatus GetFileFSRef(FSRef *parRef, ConstStr255Param name, FSRef *fileRef);
void CopyNameFromFSRef(FSRef *ref, CFStringRef *string);
//void GetNameAsPascalStringFromFSRef(FSRef *ref, Str255 *name);
void CopyNameFromCFURLRef(CFURLRef url, CFStringRef *string);
void GetNameAsPascalStringFromCFURLRef(CFURLRef url, Str255 *name);
void FBPutPreferences(Str255 name, void *prefsPtr, CFIndex size);
void FBGetPreferences(Str255 name, void *prefsPtr);
void FBKillPreferences(Str255 name);
CFMutableArrayRef CFIndexGetArrayRef(CFIndex indexID);
CFIndex CFIndexGetCount(CFIndex indexID);
void CFIndexSet(CFIndex indexID, CFIndex element, Str255 pStr);
void FBIndexSet(CFIndex element, CFIndex indexID, Str255 pStr);
StringPtr CFIndexGet(CFIndex indexID, CFIndex element);
StringPtr FBIndexGet(CFIndex element, CFIndex indexID);
void CFIndexInsert(CFIndex indexID, CFIndex element, Str255 pStr);
void FBIndexI(CFIndex element, CFIndex indexID, Str255 pStr);
void CFIndexDelete(CFIndex indexID, CFIndex element);
void FBIndexD(CFIndex element, CFIndex indexID);
CFIndex CFIndexFind(CFIndex indexID, CFIndex startElement, Str255 pStr);
CFIndex FBIndexF(Str255 pStr, CFIndex startElement, CFIndex indexID);
void CFIndexClear(CFIndex indexID);
CFComparisonResult CFIndexCompareFunction(const void *val1, const void *val2, void *context);
void CFIndexSort(CFIndex indexID);
void CFIndexAppend(CFIndex indexID, Str255 pStr);

// Change CFIndexRead/Write to use CFURLs instead of FSSpecs. Brian 20170928
//
//CFURLRef CFIndexCreateCFURLFromFSSpec(FSSpec *spec);
Boolean CFIndexWrite(CFIndex indexID, CFURLRef url);
Boolean CFIndexWriteAll(CFURLRef url);
Boolean CFIndexRead(CFIndex indexID, CFURLRef url);
Boolean CFIndexReadAll(CFURLRef url);
// END Change CFIndexRead/Write to use CFURLs instead of FSSpecs. 

CFIndex FBMem(long expr);
OSErr FBAEHandleQuit(AppleEvent *theAppleEvent, AppleEvent *reply, long myAEFlags);
void FBInitAEEvents(void);
void FBHandleEvents(void);
//MenuRef GetAboutMenuRef(void);
void InstallFBAppCommandProcessHandler(void);

#if !__LP64__
#if !DECARBONATE
long FBMenuFunction(long val);

#if 1   // kCFAppWndBtn
void FBAppleMenu( void *titlePtr, Boolean cfTitle );
#else
void FBAppleMenu(Str255 title);
#endif

void FBEditMenu(MenuID menuID);
void FBMenuPreferences(MenuID menuID, MenuItemIndex itemID);

#if 1   // _kFBMenuCF
void FBMenu(MenuID menuID, MenuItemIndex itemID, long state, void *titlePtr, MenuCommand commandID, Boolean cfType  );

void MenuSetTitle( MenuID menuID, CFStringRef title );
CFStringRef MenuCopyTitle( MenuID menuID );
void MenuItemSetText( MenuID menuID, MenuItemIndex item, CFStringRef string );
CFStringRef MenuItemCopyText( MenuID menuID, MenuItemIndex item );

#else
void FBMenu( MenuID menuID, MenuItemIndex itemID, long state, Str255 titlePtr,  MenuCommand commandID );
#endif

void FBWindowSetID(WindowRef w, long id);
long FBGetWndNumber(WindowRef w);
WindowRef FBWindowFindByID(long id);
long FBSetBestWindow(void);
void FBMakeSureItsAWindow(void);
WindowRef FBGetActiveWindow(void);
long FBWindowFunction(long expr);
void FBWindowClose(long id);
void FBWindowOutput(long id);

#if kSmartTabsRadioGroup
HIViewRef RadioGroupCreate( WindowRef w, ControlSize size, SInt32 rows, SInt32 cols, CFArrayRef titles, Rect *rectPtr );
HIViewRef RadioGroupViewViewAtIndex( HIViewRef groupView, CFIndex index );
HIViewRef RadioGroupViewAtIndex( SInt32 groupID, CFIndex index );
void RadioGroupViewSetTitleAtIndex( HIViewRef groupView, CFStringRef title, CFIndex index );
void RadioGroupSetTitleAtIndex( SInt32 groupID, CFStringRef title, CFIndex index );

void TabsViewSetTab( HIViewRef tabsView, SInt32 tabIndex );
OSStatus TabsViewHitHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void *userData );
HIViewRef TabsViewCreate( WindowRef w, SInt32 value, SInt32 max, CFArrayRef titleArray, Rect *rectPtr, long type );
void TabsViewAddViewToTab( HIViewRef tabsView, HIViewRef view, SInt32 tabIndex );
void TabsAddViewToTab( SInt32 tabsBtnNum, HIViewRef view, SInt32 tabIndex );
void TabsAddButtonToTab( SInt32 tabsBtnNum, long btnNum, SInt32 tabIndex );
#endif

#if 1   // kCFAppWndBtn

    WindowRef FBAppearanceWindow( long id, void *titlePtr, Rect *rectPtr, WindowClass wc, SInt32 attributes, long fbAttributes __attribute__ ((__unused__)), Boolean cfTitle );

#if 1   // new FBWindow
void FBWindow( long inID, CFStringRef inTitle, CGRect inRect, WindowClass inClass,  int inAttributes[] );
#else
    WindowRef FBWindow( long id, void *titlePtr, Rect *rectPtr, long type, long class __attribute__ ((__unused__)), Boolean cfTitle  );
#endif  // new FBWindow

#else

    WindowRef FBAppearanceWindow(long id, void *titlePtr, Rect *rectPtr, WindowClass wc, SInt32 attributes, long fbAttributes);
    WindowRef FBWindow(long id, void *titlePtr, Rect *rectPtr, long type, long class);

#endif

void FBGetWindow(long wndNum, WindowRef *w);
WindowRef FBGetCurOutputWindow(void);
void FBInstallTimer(EventTime interval, long (*onTimerFn)(void));
void InstallFBMouseHandler(WindowRef w);


void CallUserDefineOnAppEventFunction( long appEventKind, long appEventID ); // cocoaui 20170704 bw
long FBAppEventFunction( long val );// cocoaui 20170704 bw


void CallUserDefinedOnDialogFunction(long fbEventKind, long fbEventID, long fbWndNum);

void InstallFBAppDialogHandler(void);
void InstallFBWndDialogHandler(WindowRef w);
void FBDialogStatement(long id);

long FBDialogFunction(long val);
OSStatus SetFBDialogEventInfo(EventRef theEvent);
void ControlSetText(HIViewRef c, CFStringRef titleStr);
CFStringRef FBControlCopyText(HIViewRef c);
HIViewRef FBButtonAmpersandFunction(long btnNum);
void FBButtonClose(long btnNum);
long FBButtonFunction(long btnNum, long selector);
void MakeAllControlsIntoFBButtons(HIViewRef c);
void MakeIntoFBWindow(WindowRef w, SInt32 wNum);
void MakeIntoFBButton(HIViewRef view, SInt32 btnNum);

#if 1   // kCFAppWndBtn

    HIViewRef FBAppearanceButton( long id, SInt32 state, SInt32 value, SInt32 min, SInt32 max, void * titlePtr, Rect * rectPtr, long type, Boolean cfTitle );
    HIViewRef FBButton(long id, long state, void *titlePtr, Rect *rectPtr, long type, Boolean cfTitle );

//    void WindowSetTitle( SInt32 wndNum, CFStringRef title );
    CFStringRef WindowCopyTitle( SInt32 wndNum );

    void ButtonSetText( SInt32 btnNum, CFStringRef string );
    CFStringRef ButtonCopyText( SInt32 btnNum );

#else

    HIViewRef FBAppearanceButton(long id, SInt32 state, SInt32 value, SInt32 min, SInt32 max, void *titlePtr, Rect *rectPtr, long type);
    HIViewRef FBButton(long id, long state, void *titlePtr, Rect *rectPtr, long type);

#endif

FBBoolean FBCanGrow(WindowRef w);
void GetScrlRectFB(long scrlType, Rect *scrlRect, Rect *portRect, FBBoolean canGrow, WindowClass wc);
HIViewRef GetFBWindowScrlH(WindowRef w, long scrollType);
void FBResizedWindow(WindowRef w);
void FBScrollButton(long id, long current, long min, long max, long page, Rect *rectPtr, long type);
long FBPortWidth(void);
void FBDrawString(Str255 string$_p, short maxWidth);
void FBLimitDrawString(Str255 string_p$, short maxWidth);
//void FBAddLeadingSpace(char *sPtr);    // does not need to be limited to 32-bit( within the #if !__LP64__). Code in this is fine for 64-bit. Brian 20170507
void PrintLong(const SInt64 theNumber);
#endif /* !DECARBONATE */
#endif  /* !__LP64__ */


void FBAddLeadingSpace(char *sPtr);  // relocated here outside the #if !__LP64__  Brian 20170507
void PrintCR(void);
void FBPrintString(Str255 s);
StringPtr FBFloat2String(const double theNum, long style, long digits);
#if !__LP64__
void PrintFloat(const double d);
void PrintTab(long tabs);
void PrintSpc(long spc);
#endif  /* !__LP64__ */

void PrintContainerWnd(void **ctnrVarPtr);


char *FBCheckBuffer(long bytes2add);
void FBAddCharacterToBuffer(short c);
void FBAddStringToBuffer(Str255 string$_p);
#if !__LP64__
void DebugPrintStackLevelWnd(void);
long Csrlin(void);
#endif  /* !__LP64__ */
long Pos(long device);
#if !__LP64__
void FBLongColor(long blue, long green, long red, long backGround);
void FBColor(long theColor);
void FBPen(long PenX, long PenY, long PenVis, long ModeNum, long PatNum);
void FBSaveMaxPixel(long x, long y);
void FBPlot(long x, long y);
void FBPlotTo(long x, long y);
void FBBox(long l, long t, long r, long b, long fill);
void FBCircle(long h, long v, long Radius, long Start, long EndAngle, long Flags);
void PrintMovePixel(long x, long y);
void FBRatio(long x, long y);
void PrintMoveChar(long x, long y);
void FBFill(long x, long y);
void Cls(void);
long ClsLine(void);
long ClsPage(void);
void FBFlushWindowBuffer(long wNum);
void FBEndSession(void);
OSStatus FBBeginSession(void);
void FBFixUpPrhandle(void);
FBBoolean DoPageSetup(void);
FBBoolean FBPrintDialog(void);
void FBGetPRHandle(void);
Handle FBGetPRHandleRuntime(void);
void FBPRInit(void);
void FBPageSetUp(void);
void FBPrintDialogRuntime(void);
void ClearLPrint(void);
void CloseLPrint(void);
void FBSendToPrinter(void);
long DoPrintPicFB(PicHandle picH, Rect *r);
void PageLPrint(void);
long FBPrintPict(PicHandle ThePict);
FBBoolean PrError(void);
FBBoolean CheckForPRCancel(void);
#endif  /* !__LP64__ */
void FBRoute(long where);
void FBCheckForPageFeed(long isCr);
void FBText(long font, long size, long style, long xMode);

void FBStopMsg(ConstStr255Param msg_p, const int lineNumber);

#if kNSFBFullStop
void FBFullStop( CFStringRef maj, CFStringRef exp, FBBoolean fatal );
#else
void FBfullStop(StringPtr major_p, StringPtr explanation, FBBoolean fatal);
#endif

void RuntimeErrMsg(StringPtr msg);

#if kNSFBFullStop
void FBShutdown( void *msgPtr, Boolean cfType );
void FBReportRangeError(const int value, const int lineNumber, const int limit, CFStringRef fileName);
#else
void FBShutdown(ConstStr255Param msg_p);
void FBReportRangeError(const int value, const int lineNumber, const int limit, ConstStr255Param fileName);
#endif

// For the string functions in General.m Brian 20160706
SInt64 InString( SInt32 startPos, CFStringRef strToSearch, CFStringRef strToSearchFor );
CFStringRef RightString( CFStringRef inString, CFIndex howManyChars );
CFStringRef LeftString( CFStringRef inString, CFIndex toIndex );
CFStringRef MidString( CFStringRef inString , CFIndex startPos, CFIndex howManyChars );
CFStringRef HexString( SInt32 num );


#if !__LP64__
void FBPicture(Rect *rectPtr, PicHandle pictH);
void FBPictureOn(Rect *rectPtr);
void FBPictureOff(PicHandle *picHPtr);
long FBThreadStatus(unsigned long ticks);
void FBWaitForAllThreads(void);
void FBMasterThreader(long flags);
long FBThreadBegin(long LocalFNProc, long flags, long stackmin);
void FBCursor(long crsrID);
void FBCursors(long crsrID, long crsrType);
#endif  /* !__LP64__ */
StringPtr FBInkey(void);
void FBLineInput(Str255 prompt_p, void *vPtr);
void FBToCInput(Str255 prompt, void *target, short varType);
#if !__LP64__
long Mouse(long mArg);
void FBMaxWindow(long width, long height);
void FBMinWindow(long width, long height);
void ClearFinderInfo(void);
long FBFinderInfo(short *index, void *specRefUrl, OSType *type, short varType);
void InstallFBFinderInfoHandler(void);
void FBEditEquals(long margin);
void FBEditDollarStatement(long id, long selStart, long selEnd, void *txtPtr, long font, long size, long style, long mode, void *red, long green, long blue, long textType);
ControlKeyFilterResult FBAnyKey(ControlRef c, SInt16 *keyCode, SInt16 *charCode, EventModifiers *modifiers);
void FBEditFieldClose(long id);
void FBEditField(long id, void *txtPtr, Rect *rectPtr, long type, long class, void *filterProc, long textType);
void FBEditText(long id, long font, long size, long style, long mode, RGBColor *foreRGB, RGBColor *backRGB);
StringPtr FBEditDollarFunction(long id, long selStart, long selEnd);
void * FBEditDollarCtnrFunction( long id, long selStart, long selEnd );
void FBTekeyDollarStatement(void *txtPtr);
void FBSetSelect(long selStart, long selEnd);
void FBGetField(Handle *hndlPtr, long id);
#endif  /* !__LP64__ */

#endif /* RUNTIME_H */
