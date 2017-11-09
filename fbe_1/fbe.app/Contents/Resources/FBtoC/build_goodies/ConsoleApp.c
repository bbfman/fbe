#include "FBtoC.h"
#include "Runtime.h"

/*
 *  ConsoleApp.c
 *
 * included only from FBtoCConsole.incl
 */


static int	       gFBConsolePrintPos;

extern HIWindowRef sConsoleWindow;
extern HIViewRef   sConsoleHITextView;
extern HIViewRef   sConsoleScrollView;
extern TXNObject   sConsoleTXNObject;


void CA_SendStringToConsole( Str255  s$, Boolean  newLine )
{
	static unsigned long   lastTicks;
	unsigned long          nowTicks;
	
	TXNSetData( sConsoleTXNObject, kTXNTextData, &s$[1], s$[0], kTXNEndOffset, kTXNEndOffset );
	gFBConsolePrintPos += s$[0];
	if ( newLine)
	{
		char  cr = 13;
		TXNSetData( sConsoleTXNObject,  kTXNTextData, &cr, 1, kTXNEndOffset, kTXNEndOffset );
		HIScrollViewNavigate( sConsoleScrollView, 2 );
		gFBConsolePrintPos = 0;
	}
	nowTicks = TickCount();
	if ( nowTicks > lastTicks )
	{
		HIWindowFlush( sConsoleWindow ); // force redraw even without event handling
		lastTicks = nowTicks + 2; // limit to 30/s
	}
} 


///////////////////////////////////////////////////
//         print to console app                  //
//      called from translated code              //
///////////////////////////////////////////////////

long			gFBRoute = 0;
#define			kFBBufferCount	6
#define			kToBuffer		1000

void ConsoleAppPrintCR()
{
	if ( gFBRoute == 0 )		// _toScreen
	{
		CA_SendStringToConsole( "\p", true );
	}
	else if ( gFBRoute == 128 ) // _toPrinter
	{
		// show runtime error
	}
	else if ( gFBRoute >= kToBuffer )
	{
		FBAddCharacterToBuffer( 13 );
	}
	else // gFBRoute 1 to 127 and 129 to 255 represent files
	{
		FBFilePrintCR();
	}
} 

void ConsoleAppPrintString( Str255  string$ )
{	
	if ( gFBRoute == 0 )		// _toScreen
		CA_SendStringToConsole( string$, false );
	else if ( gFBRoute == 128	) // _toPrinter
	{
		// show runtime error
	}
	else if ( gFBRoute >= kToBuffer ) // _toBuffer
	{
		FBAddStringToBuffer( PSstrcpy( STACK_PUSH(), string$ ) );
	}
	else							// fileID
	{
		FBFilePrintString( PSstrcpy( STACK_PUSH(), string$ ) );
	}
	gFBStk--;
} 



void ConsoleAppPrintLong( const SInt64  theNumber )
{
	Str255             string;
	
	string[0] = sprintf( (char*)&string + 1, "%lld", theNumber);
	FBAddLeadingSpace( (char*)&string );
	ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), string ) );
} 



void ConsoleAppPrintFloat( const double  theNumber )
{
	ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), FBFloat2String( theNumber, FIXEDDECIMAL, gFBFloatMaxDigits ) ) );
} 



void ConsoleAppPrintSpc( long spc )
{
	do
	{
		CA_SendStringToConsole( "\p ", false );
		spc--;
	}
	while ( spc > 0 ); 
} 

void ConsoleAppPrintTab( long tabs )
{
	if ( tabs < 1 ) { tabs = 1; }
	if ( tabs > gFBConsolePrintPos ) { ConsoleAppPrintSpc( tabs - gFBConsolePrintPos ); }
} 

void ConsoleAppPrintCommaTabPosition( void )
{
	ConsoleAppPrintSpc( gPSDefTabStops - gFBConsolePrintPos % gPSDefTabStops );
}


void ConsoleAppPrintContainer( Handle* ctnrVarPtr )
{
	long              l;
	long              p;
	UInt8             c;
	Str255            t$;
	Handle            ctnrHandle;
	ctnrHandle = *ctnrVarPtr;
	if ( ctnrHandle )
	{
		l = GetHandleSize( ctnrHandle );
		p = 0;
		t$[0] = 0;
		while ( p < l )
		{
			if ( t$[0] == 255 )
			{
				ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), t$ ) );
				
				t$[0] = 0;
			} 
			c = *(UInt8*)(*ctnrHandle + p);
			p++;
			if ( c == 13 )
			{
				ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), t$ ) );
				ConsoleAppPrintCR();
				t$[0] = 0;
			}
			else
			{
				t$[++t$[0]] = c;
			} 
		} 
		ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), t$ ) );
	} 
}



// this should quit when app receives kAEQuitApplication
////static OSErr CA_HandleQuit( AppleEvent *theAppleEvent, AppleEvent *reply, long myAEFlags )
////{	
////	exit( 0 );
////	return noErr;
////}

static Str255 sTheString;

static OSStatus CAKeyHandler( EventHandlerCallRef nextHandler, EventRef inEvent, void* userData )
{
	UniChar   uniChar;
	
	GetEventParameter( inEvent, kEventParamTextInputSendText, typeUnicodeText, 0, sizeof( UniChar ), 0, &uniChar );
	if ( (uniChar == 8) && sTheString[0] ) //  backspace
	{
		sTheString[0]--;
		return eventNotHandledErr; // allow textView to process
	}
	
	if ( uniChar == 3 ) uniChar = 13; // enter -> cr
	
	if ( (uniChar < 32 || uniChar == 127) && uniChar != 13 )
	{
		SysBeep( 1 ); // ctrl char; beep and ignore
	}
	else
	{
		if ( sTheString[0] < 255 )
		{
			sTheString[0]++;
			sTheString[sTheString[0]] = (char)uniChar;
			return eventNotHandledErr;  // allow textView to process
		}
		else
		{
			SysBeep( 1 ); // too long
		}
	}
	return noErr; // textView will ignore
}


void FBParseInput( short varType, void * vPtr );
//// OSErr FBAEHandleQuit( AppleEvent *theAppleEvent, AppleEvent *reply, long myAEFlags );

void ConsoleAppInput( Str255  prompt, void *target, short varType )
{
    gFBLinePos = 1;
	EventRef			evnt;
	EventHandlerRef		evntHandler;
	EventTypeSpec		evntSpec = { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent };
	
    if ( prompt[0] ) { ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), prompt ) ); }
//// remove the usual kAEQuitApplication handler and install our own
//// AERemoveEventHandler( kCoreEventClass, kAEQuitApplication, (AEEventHandlerProcPtr)FBAEHandleQuit, true );
//// AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, (AEEventHandlerProcPtr)CA_HandleQuit, 0, true );
	InstallEventHandler( GetControlEventTarget( sConsoleHITextView ), &CAKeyHandler, 1, &evntSpec, 0, &evntHandler );
	sTheString[0] = 0;
	while ( sTheString[sTheString[0]] != 13  )
	{
		ReceiveNextEvent( 0, NULL, kEventDurationForever, true, &evnt );
		SendEventToEventTarget( evnt, GetEventDispatcherTarget() );
		ReleaseEvent( evnt );
		if ( gFBQuit ) exit( 0 ); // allow command-q quit during input
	}
//// remove our own and reinstall the usual kAEQuitApplication handler
//// AERemoveEventHandler( kCoreEventClass, kAEQuitApplication, (AEEventHandlerProcPtr)CA_HandleQuit, true );
//// AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, (AEEventHandlerProcPtr)FBAEHandleQuit, 0, true );
	RemoveEventHandler( evntHandler );
	sTheString[0]--; // remove trailing cr
	PSstrcpy( (StringPtr)&gFBLineBuf, sTheString );
	FBParseInput( varType, target );
	gFBStk--; // #598
}



///////////////////////////////////////////////////
//       used if _debugPrintStringStackLevel     //
///////////////////////////////////////////////////
void ConsoleAppDebugPrintStackLevel()
{
	ConsoleAppPrintString( PSstrcpy( STACK_PUSH(), "\pgFBStk = " ) );
	ConsoleAppPrintLong( gFBStk );
	ConsoleAppPrintCR();
}

