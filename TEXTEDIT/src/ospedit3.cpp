/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2004. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Dialogs and menus                                         */
/*************************************************************/

/* This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/* Revision History:
 *
 *  19/05/2001:   Added Save/Restore Desktop
 *  24/12/2001:   Hid sound-related menu items if SOUND_SUPPORT = 0
 *  05/01/2002:   Cleaned up indentation, etc
 *  28/12/2004:   Added new converter info dialog (orudge)
 */

#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TRect
#define Uses_TSubMenu
#define Uses_TKeys
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TPoint
#define Uses_TEditor
#define Uses_MsgBox
#define Uses_TFileDialog
#define Uses_TDeskTop

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
   #include <tv.h>

	#if __GNUC__ >= 3
		#include <strstream>        // yes, it's depreciated, I know
		using std::strstreambuf;
		using std::istrstream;
		using std::ostrstream;
		using std::strstream;
	#else
		#include <strstream.h>
	#endif
#else
   #include <tvision\tv.h>
   #include <strstrea.h>
#endif

#include "ospedit.h"

#include <stdarg.h>

#include <iomanip.h>

TMenuBar *TEditorApp::initMenuBar(TRect r) // Another messy function...
{
   TSubMenu& sub0 = *new TSubMenu("~ð~", kbAltSpace) +
      *new TMenuItem("~A~bout OSPlus...", cmAbout, kbNoKey) +
      newLine() +
#ifdef __LINUX__
      *new TMenuItem("S~h~ell", cmDosShell, kbAltD, hcNoContext, "Alt-D") +
#else
      *new TMenuItem("~D~OS Shell", cmDosShell, kbAltD, hcNoContext, "Alt-D") +
#endif
      *new TMenuItem("~C~alculator", cmCalcCmd, kbNoKey)
#if SOUND_SUPPORT == 0
      ;
#else
      +
      newLine() +
      *new TMenuItem("~S~elect WAV...", cmSelectWAV, kbNoKey) +
      *new TMenuItem("~P~lay WAV",	cmPlayWAV, kbNoKey) +
      *new TMenuItem("S~t~op WAV", cmStopWAV, kbNoKey)
#if defined(__MSDOS__) && defined(__BORLANDC__)
  #ifndef SAVE_RESTORE_DESKTOP
      ;
  #else
      +
  #endif
#else
      + newLine() +
      *new TMenuItem("S~e~lect MID...", cmSelectMID, kbNoKey) +
      *new TMenuItem("Pla~y~ MID", cmPlayMID, kbNoKey) +
      *new TMenuItem("Stop M~I~D", cmStopMID, kbNoKey)
#ifdef SAVE_RESTORE_DESKTOP
      + newLine() +
      *new TMenuItem("Sa~v~e Desktop", cmSaveDesktop, kbNoKey) +
      *new TMenuItem("Res~t~ore Desktop", cmRestoreDesktop, kbNoKey);
#else
      ;
  #endif
#endif
#endif // SOUND_SUPPORT == 0

   TSubMenu& sub1 = *new TSubMenu("~F~ile", kbAltF) +
      *new TMenuItem("~N~ew", cmNew, kbNoKey) +
      *new TMenuItem("~O~pen...", cmOpen, kbF3) +
      *new TMenuItem("~C~lose", cmClose, kbAltF3, hcNoContext, "Alt-F3") +
      newLine() +
      *new TMenuItem("~S~ave", cmSave, kbF2, hcNoContext, "F2") +
      *new TMenuItem("S~a~ve As...", cmSaveAs, kbNoKey) +
      newLine() +
      *new TMenuItem("C~h~ange Directory...", cmChangeDrct, kbNoKey) +
      newLine() +
      *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext, "Alt-X");

   TSubMenu& sub2 = *new TSubMenu("~E~dit", kbAltE) +
      *new TMenuItem("~U~ndo", cmUndo, kbNoKey) +
      newLine() +
      *new TMenuItem("Cu~t~", cmCut, kbShiftDel, hcNoContext, "Shift-Del") +
      *new TMenuItem("~C~opy", cmCopy, kbCtrlIns, hcNoContext, "Ctrl-Ins") +
      *new TMenuItem("~P~aste", cmPaste, kbShiftIns,	hcNoContext, "Shift-Ins") +
      *new TMenuItem("~S~how Clipboard", cmShowClip, kbNoKey) +
      newLine() +
      *new TMenuItem("~C~lear", cmClear, kbCtrlDel, hcNoContext, "Ctrl-Del");

   TSubMenu& sub3 = *new TSubMenu("~S~earch", kbAltS) +
      *new TMenuItem("~F~ind...", cmFind, kbNoKey) +
      *new TMenuItem("~R~eplace...", cmReplace, kbNoKey) +
      *new TMenuItem("Find ~N~ext", cmSearchAgain, kbNoKey);

   TSubMenu& sub4 = *new TSubMenu( "~W~indows", kbAltW) +
      *new TMenuItem("~S~ize/move",cmResize, kbCtrlF5, hcNoContext, "Ctrl-F5") +
	*new TMenuItem("~Z~oom", cmZoom, kbF5, hcNoContext, "F5") +
	*new TMenuItem("~T~ile", cmTile, kbNoKey) +
	*new TMenuItem("C~a~scade", cmCascade, kbNoKey) +
	*new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6") +
	*new TMenuItem("~P~revious", cmPrev, kbShiftF6, hcNoContext, "Shift-F6") +
      *new TMenuItem("~C~lose", cmClose, kbAltF3, hcNoContext, "Alt-F3");

   TSubMenu& sub5 = *new TSubMenu("~H~elp", kbAltH) +
      *new TMenuItem("~V~ersion Information...", cmVerInfo, kbNoKey) +
	*new TMenuItem("~C~onverter Information...", cmCnvInfo, kbNoKey) +
	newLine() + 
      *new TMenuItem("~A~bout Text Editor...", cmAboutApp,	kbAltA);

   r.b.y = r.a.y+1;
   return new TMenuBar(r, sub0 + sub1 + sub2 + sub3 + sub4 + sub5);
}

TStatusLine *TEditorApp::initStatusLine(TRect r)
{
   r.a.y = r.b.y-1;
   return new TStatusLine( r,
   *new TStatusDef(0, 0xFFFF) +
      *new TStatusItem("~F2~ Save", kbF2,	cmSave) +
      *new TStatusItem("~F3~ Open", kbF3, cmOpen) +
      *new TStatusItem("~Alt-F3~ Close",	kbAltF3, cmClose) +
      *new TStatusItem("~F5~ Zoom", kbF5, cmZoom) +
      *new TStatusItem("~F6~ Next", kbF6, cmNext) +
      *new TStatusItem("~F10~ Menu", kbF10, cmMenu) +
      *new TStatusItem(0, kbCtrlF5, cmResize)
   );
}

void TEditorApp::outOfMemory()
{
   messageBox("Not enough memory for this operation.", mfError | mfOKButton);
}

typedef char *_charPtr;
typedef TPoint *PPoint;

#pragma warn -rvl

ushort doEditDialog(int dialog, ...)
{
   va_list arg;

   char buf[80];
   ostrstream os(buf, sizeof(buf));

   switch (dialog)
   {
      case edOutOfMemory:
         return messageBox("Not enough memory for this operation", mfError | mfOKButton);

	case edReadError:
         va_start(arg, dialog);
         os << "Error reading file " << va_arg(arg, _charPtr) << "." << ends;
         va_end(arg);

         return messageBox(buf, mfError | mfOKButton);

	case edWriteError:
         va_start( arg, dialog );
         os << "Error writing file " << va_arg(arg,_charPtr ) << "." << ends;
         va_end(arg);

         return messageBox(buf, mfError | mfOKButton);

      case edCreateError:
         va_start( arg, dialog );
         os << "Error creating file " << va_arg(arg, _charPtr) << "." << ends;
         va_end(arg);

         return messageBox(buf, mfError | mfOKButton);

      case edSaveModify:
         va_start(arg, dialog);
         os << va_arg(arg, _charPtr) << " has been modified. Save?" << ends;
         va_end(arg);

         return messageBox(buf, mfInformation | mfYesNoCancel);

	case edSaveUntitled:
	    return messageBox("Save untitled file?", mfInformation | mfYesNoCancel);

      case edSaveAs:
         va_start( arg, dialog );
         return execDialog(new TFileDialog("*.*", "Save file as", "~N~ame", fdOKButton, 101),
               va_arg(arg, _charPtr));

      case edFind:
         va_start(arg, dialog);
         return execDialog(createFindDialog(), va_arg(arg, _charPtr));

      case edSearchFailed:
         return messageBox("Search string not found.", mfError | mfOKButton);

      case edReplace:
         va_start( arg, dialog );
         return execDialog(createReplaceDialog(), va_arg(arg, _charPtr));

      case edReplacePrompt:
         //  Avoid placing the dialog on the same line as the cursor
         TRect r(0, 1, 40, 8);
         r.move((TProgram::deskTop->size.x-r.b.x)/2, 0);
         TPoint t = TProgram::deskTop->makeGlobal(r.b);
         t.y++;
         va_start(arg, dialog);
         TPoint *pt = va_arg(arg, PPoint);

         if (pt->y <= t.y)
            r.move(0, TProgram::deskTop->size.y - r.b.y - 2);

         va_end(arg);
	   return messageBoxRect(r, "Replace this occurence?", mfYesNoCancel | mfInformation);
   }
}

#pragma warn .rvl
