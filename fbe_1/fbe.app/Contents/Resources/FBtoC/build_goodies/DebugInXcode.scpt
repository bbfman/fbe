-- DebugInXcode.scpt-- FBtoC sends 3 arguments (items 1-3 below)on run argv
-- phase 1: open our special host project and set its active executable to the app under test	set pathToDebugProject to item 1 of argv -- /path/to/<your FB source>/build_temp/DebuggerHost.xcodeproj	set pathToAppUnderTest to item 2 of argv -- /path/to/<your FB source>/<appname>.app
	set executableName to item 3 of argv -- <appname>	tell application id "com.apple.Xcode" -- should launch Xcode 3  bug#590 [was tell application "Xcode"]		activate		open POSIX file pathToDebugProject as Unicode text		tell project "DebuggerHost"			set e to executables whose name is executableName			if length of e is 0 then				set e to make new executable with properties {name:executableName, path:pathToAppUnderTest}			else				set e to first item of e			end if			set active executable to e		end tell
	end tell
-- phase 2: debug
	ignoring application responses
		tell application "Xcode"
			tell project "DebuggerHost" to debug
		end tell
	end ignoringend run