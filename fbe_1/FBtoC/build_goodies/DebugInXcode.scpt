-- DebugInXcode.scpt
-- phase 1: open our special host project and set its active executable to the app under test
	set executableName to item 3 of argv -- <appname>
	end tell
-- phase 2: debug
	ignoring application responses
		tell application "Xcode"
			tell project "DebuggerHost" to debug
		end tell
	end ignoring