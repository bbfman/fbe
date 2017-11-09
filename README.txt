Known problems:

1) File/OpenRecent not working correctly.  Files are added to menu, but are not selectable.  Trying to use it in a non-Document based app.

2) ProjectManager not hooked up.  Drag and Drop with local reorder should work.  Code is not optimal.

3) File Handling is rudimentary.  Needs to work like old editor.  When run btn is hit go ahead and compile if file has a name.  If file is not named then go to SaveAs and require user to name it before building/running.

4.) Line numbers linger after editor is cleared by hitting clear btn on toolbar.  Easily cleared by flicking magic mouse and causing text view to refresh.

5.) May be able to merge TextDelegate with AppDelegate

6.) May be able to scrap singleton Shared Variables (currently being retained in case it is needed later)

7.) Paste event into editor does not trigger -setDocumentEdited:

8.) ‘~’1 is not translated into a line.

Info:

Library/ApplicationSupport/FBE contains pref settings in a plist.
Templates/Controls folders contain CocoaUI code and are easily customizable.