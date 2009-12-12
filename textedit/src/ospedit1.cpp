/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Main source file                                          */
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
 * 17/12/2000: Made compatible with DJGPP  (orudge)
 * 18/12/2000: Fixed link problem (orudge)
 * 19/12/2000: Added support for Allegro and sound (orudge)
 * 19/12/2000: Added new About dialog (orudge)
 * 20/12/2000: Added Win32 version - uses Windows WINMM.DLL (orudge)
 * 24/12/2000: Added Linux version (orudge)
 * 14/01/2001: Updated copyright for OSPlus Utility Kit 1.2 (orudge)
 * 04/02/2001: Added MIDI player (orudge)
 * 19/05/2001: Added new About OSPlus dialog instead of old one (orudge)
 * 19/05/2001: Tried to add support to save and restore workspace (orudge)
 * 02/10/2001: Updated includes to reflect Win32 port (orudge)
 * 12/10/2001: Added improved converter error support (orudge)
 * 12/10/2001: Added Version Info dialog and version info detection (orudge)
 * 12/10/2001: Made Win32 port work (orudge)
 * 13/10/2001: Attempted to add new conversion routines (orudge)
 * 23/12/2001: Did more work on converter routines (orudge)
 * 04/01/2002: Improved Borland C++ version detection (orudge)
 * 05/01/2002: Cleaned up indentation (orudge)
 * 25/05/2002: Got ready for 2.1 release (orudge)
 * 13/10/2002: Added MSVC support (orudge)
 * 05/02/2004: Added support for 0x0551 in BCC versions (orudge)
 * 27/12/2004: Added support for MSVC 8 (orudge)
 * 27/12/2004: GCC 3.x compatibility updates, converter updates (orudge)
 * 28/12/2004: Added new converter info dialog (orudge)
 * 31/12/2004: Added GCC ver for DJGPP (orudge)
 * 28/03/2005: Fixed deletion bug (orudge)
 */

//#define DJGPP_NO_SOUND_SUPPORT

#ifdef __LINUX__
	#define DJGPP_NO_SOUND_SUPPORT
#endif

#define Uses_MsgBox
#define Uses_TParamText
#define Uses_TApplication
#define Uses_TEditWindow
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TEditor
#define Uses_TFileEditor
#define Uses_TFileDialog
#define Uses_TChDirDialog
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton

#ifdef SAVE_RESTORE_DESKTOP
	#define Uses_fpstream
#endif

#include "sound.h"

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	#define Uses_TCalculator

	#include <tv.h>

	#ifndef __WIN32__
		#include <unistd.h>
	#endif

	#if (__GNUC__ >= 3) || (_MSC_VER >= 1300)
		#include <sstream>
	#else
		#include <strstream.h>
	#endif

	#if (defined(__LINUX__) && !defined(LINUX_NO_SOUND_SUPPORT)) || (defined(__DJGPP__) && !defined(DJGPP_NO_SOUND_SUPPORT))
		#define USE_CONSOLE
		#include <allegro.h>
	#endif
#else
	#include <tvision\tv.h>
	#include <strstrea.h>

	#include "calc.h"

	#ifndef __WIN32__
		#define NO_MIDI_MUSIC
	#endif
#endif

#if !defined(ALLEGRO_H) && !defined(__WIN32__) && !defined(__LINUX__)
	#define REAL_DOS // real-mode DOS
	#include "realdos.h"
#endif

#if defined(__MSDOS__) && !defined(ALLEGRO_H)
	#include "realdos.h"
#endif

#if defined(__LINUX__) && !defined(ALLEGRO_H)
	extern "C" {
		extern char *strlwr(char *str);
		extern char *strupr(char *str);
	}
#endif

#ifdef __WIN32__
	#include <windows.h>
#endif

#include "ospedit.h"
#include "aboutdlg.h"
#include "aboutosp.h"
#include "verinfo.h"
#include "cnvinfo.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if (__GNUC__ >= 3) || (_MSC_VER >= 1300)
	#include <iomanip>
#else
	#include <iomanip.h>
#endif

#include <limits.h>
#include <stdio.h>

#if defined(__MSDOS__) || defined(__DJGPP__)
	#include <dos.h>
#endif

#ifndef __LINUX__
	#include <process.h>
	#include <io.h>
	#include <conio.h>
#endif

#ifndef MAXPATH
	#ifdef PATH_MAX
		#define MAXPATH PATH_MAX
	#else
		#define MAXPATH  200
	#endif
#endif

#include "convert.h"

typedef int BOOL;

#ifndef FALSE
	#define FALSE False
	#define TRUE  True
#endif

BOOL IsInitialFile = False;          // was file specified on command line?
char initialfile[200];               // file name on cmd line

extern "C" char WAVName[MAXPATH];    // file name of WAV
extern "C" BOOL WAVLoaded;           // WAV loaded?

extern "C" char MIDName[MAXPATH];    // file name of MID
extern "C" BOOL MIDLoaded;           // MID loaded?

extern "C" BOOL SoundEnabled;

TEditWindow *clipWindow;

#ifdef __BORLANDC__
	char __bc_compiler_ver[50];
#elif defined(__DJGPP__)
	char __dj_compiler_ver[50];
#elif defined(__MSVC__)
	char __msvc_compiler_ver[50];
#elif defined(__GNUC__)
	char __gcc_ver[50];
#endif

char __tv_ver[50];
char __alleg_ver[50];
char __os[50];
char __os_ver[50];

class QuickMessage: public TWindow
{
	TParamText *currentDir;
public:
	QuickMessage( const char *drive ):
				 TWindow( TRect( 25,10,55,16 ), "Please Wait", 0 ),
				 TWindowInit( TWindow::initFrame )
	{
		insert( new TStaticText( TRect( 2,2,28,3 ), "Converting File..." ) );

		flags = 0; // no move, close, grow or zoom
		options |= ofCentered;
		palette = wpGrayWindow;
	}

	virtual void handleEvent(TEvent &event)
	{
		TWindow::handleEvent(event);
	}

	void setCurrentDir(char *newDir)
	{
		currentDir->setText(newDir);
	}
};

TEditWindow *TEditorApp::openEditor(const char *fileName, Boolean visible)
{
	TRect r = deskTop->getExtent();
	TView *p = validView(new TEditWindow(r, fileName, wnNoNumber));

	if (!visible)
		p->hide();

	deskTop->insert( p );
	return (TEditWindow *)p;
}

TEditorApp::TEditorApp() :
	 TProgInit(TEditorApp::initStatusLine,
					TEditorApp::initMenuBar,
					TEditorApp::initDeskTop
				 ),
	 TApplication()
{
	char szBuf[200], errortxt[200];

	TCommandSet ts;
	ts.enableCmd(cmSave);
	ts.enableCmd(cmSaveAs);
	ts.enableCmd(cmCut);
	ts.enableCmd(cmCopy);
	ts.enableCmd(cmPaste);
	ts.enableCmd(cmClear);
	ts.enableCmd(cmUndo);
	ts.enableCmd(cmFind);
	ts.enableCmd(cmReplace);
	ts.enableCmd(cmSearchAgain);

	disableCommands(ts);

	TEditor::editorDialog = doEditDialog;
	clipWindow = openEditor(0, False);

	if (clipWindow != 0)
	{
		TEditor::clipboard = clipWindow->editor;
		TEditor::clipboard->canUndo = False;
	}

	if (IsInitialFile == True)
	{
		int ret;
		QuickMessage *qm = new QuickMessage("");
		TProgram::deskTop->insert(qm);

		ret = convert_text_file(initialfile, szBuf, errortxt, 0);

		if ((!(ret & ERROR_CNV_OK)) && (ret != -2))
		{
			messageBox(mfError | mfOKButton, errortxt);
		}
		
		TProgram::deskTop->remove( qm );
		destroy(qm);

		openEditor(szBuf, True);

		if ((ret != -2) && (ret & CONVERT_DELETE_FILE))
			unlink(szBuf);
	}
}

void TEditorApp::fileOpen()
{
	char fileName[MAXPATH];
	char szBuf[200], errortxt[200];

	strcpy( fileName, "*" );

	if (execDialog(new TFileDialog("*", "Open File",
				"~N~ame", fdOpenButton, 100), fileName) != cmCancel)
	{
		int ret;
		QuickMessage *qm = new QuickMessage("");
		TProgram::deskTop->insert(qm);

		ret = convert_text_file(fileName, szBuf, errortxt, 0);

		if ((!(ret & ERROR_CNV_OK)) && (ret != -2))
		{
			messageBox(mfError | mfOKButton, errortxt);
		}
		
		TProgram::deskTop->remove( qm );
		destroy(qm);

		openEditor(szBuf, True);

		if ((ret != -2) && (ret & CONVERT_DELETE_FILE))
			unlink(szBuf);
	}
}

void TEditorApp::fileNew()
{
	openEditor(0, True);
}

void TEditorApp::changeDir()
{
	execDialog(new TChDirDialog(cdNormal, 0), 0);
}

void TEditorApp::dosShell()
{
	suspend();

#ifdef __LINUX__
	system("clear");

	puts("Type `exit' to return to OSPlus Text Editor\n");
	system(getenv("SHELL"));
#else
	#if defined(__MSVC__) || defined(__MINGW32__)
		system("cls");
	#else
		clrscr();
	#endif

	puts("Type EXIT to return to OSPlus Text Editor\n");
	system(getenv("COMSPEC"));
#endif

	resume();
	redraw();
}

void TEditorApp::showClip()
{
	clipWindow->select();
	clipWindow->show();
}

void TEditorApp::tile()
{
	deskTop->tile(deskTop->getExtent());
}

void TEditorApp::cascade()
{
	deskTop->cascade(deskTop->getExtent());
}

void TEditorApp::handleEvent(TEvent& event)
{
	TApplication::handleEvent(event);

	if (event.what != evCommand)
		return;
	else
		switch (event.message.command)
		{
			case cmOpen:
				fileOpen();
				break;

			case cmNew:
				fileNew();
				break;

			case cmChangeDrct:
				changeDir();
				break;

			case cmDosShell:
				dosShell();
				break;

			case cmShowClip:
				showClip();
				break;

			case cmTile:
				tile();
				break;

			case cmCascade:
				cascade();
				break;

			case cmAbout:
				aboutBox();
				break;

			case cmAboutApp:
				aboutProg();
				break;

			case cmVerInfo:
				verInfo();
				break;

			case cmCnvInfo:
				cnvInfo();
				break;

			case cmCalcCmd:
				calculator();
				break;

			case cmSelectWAV:
				selectWAV();
				break;

			case cmPlayWAV:
				playWAV();
				break;

			case cmStopWAV:
				stopWAV();
				break;

			case cmSelectMID:
				selectMID();
				break;

			case cmPlayMID:
				playMID();
				break;

			case cmStopMID:
				stopMID();
				break;

#ifdef SAVE_RESTORE_DESKTOP
			case cmSaveDesktop:
				saveDesktop();
				break;

			case cmRestoreDesktop:
				retrieveDesktop();
				break;
#endif

			default:
				return;
	}

	clearEvent(event);
}

void TEditorApp::aboutBox(void)
{
	TDialog *aboutBox = new TAboutOSPlus();

	aboutBox->options |= ofCentered;

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	deskTop->execView(aboutBox);
#else
	executeDialog(aboutBox);
#endif
}

void TEditorApp::aboutProg()
{
	TAboutDialog *aboutBox = new TAboutDialog();
	char bldtxt[50];

	sprintf(bldtxt, "%c%s", 3, BUILD_VERSION);

	aboutBox->options |= ofCentered;
	aboutBox->build_version = new TStaticText(TRect(2, 15, 49, 16), bldtxt);
	aboutBox->insert(aboutBox->build_version);

#if defined(__DJGPP__) || defined(__LINUX__)  || defined(__WIN32__)
	deskTop->execView(aboutBox);
#else
	executeDialog(aboutBox);
#endif
}

void TEditorApp::verInfo()
{
	TVerInfoDlg *verInfoBox = new TVerInfoDlg();

	verInfoBox->options |= ofCentered;

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	deskTop->execView(verInfoBox);
#else
	executeDialog(verInfoBox);
#endif
}

void TEditorApp::cnvInfo()
{
	TCnvInfoDialog *cnvInfoBox = new TCnvInfoDialog();

	cnvInfoBox->options |= ofCentered;

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	deskTop->execView(cnvInfoBox);
#else
	executeDialog(cnvInfoBox);
#endif
}

void TEditorApp::calculator()
{
	TCalculator *calc = (TCalculator *) validView(new TCalculator);

	if (calc != 0)
		deskTop->insert(calc);
}

void TEditorApp::selectWAV()
{
	if (SoundEnabled == FALSE) goto leave;

#if (defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)) && !defined(ALLEGRO_H)
	messageBox("Sound is not enabled with this version.", mfOKButton);
#else
	strcpy( WAVName, "*.wav" );

	if (execDialog(new TFileDialog("*.wav", "Select WAV File",
			"~N~ame", fdOpenButton, 100), WAVName) != cmCancel)
		snd_LoadWAV();
#endif
leave: ;
}

void TEditorApp::playWAV()
{
#if (defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)) && !defined(ALLEGRO_H)
	messageBox("Sound is not enabled with this version.", mfOKButton);
#else
	snd_PlayWAV();
#endif
}

void TEditorApp::stopWAV()
{
#if (defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)) && !defined(ALLEGRO_H)
	messageBox("Sound is not enabled with this version.", mfOKButton);
#else
	snd_StopWAV();
#endif
}

void TEditorApp::selectMID()
{
	if (SoundEnabled == FALSE) goto leave;

#ifdef NO_MIDI_MUSIC
	messageBox("MIDI music is not enabled with this version.", mfOKButton);
#else
	strcpy( MIDName, "*.mid" );

	if (execDialog(new TFileDialog("*.mid", "Select MIDI File",
			"~N~ame", fdOpenButton, 100), MIDName) != cmCancel)
		snd_LoadMID();
#endif
leave: ;
}

void TEditorApp::playMID()
{
#ifdef NO_MIDI_MUSIC
	messageBox("MIDI music is not enabled with this version.", mfOKButton);
#else
	snd_PlayMID();
#endif
}

void TEditorApp::stopMID()
{
#ifdef NO_MIDI_MUSIC
	messageBox("MIDI music is not enabled with this version.", mfOKButton);
#else
	snd_StopMID();
#endif
}

int main(int argc, char *argv[])
{
	if (argc >= 2) // command line parameters?
	{
		strcpy(initialfile, argv[1]);
		IsInitialFile = True;
	}

	// Register converters
	register_converter_file_type("rtf", "txtrtf.cnv", NULL, "Rich Text Format");
	register_converter_file_type("wri", "txtwrite.cnv", NULL, "Windows Write");
	register_microsoft_converters();
	register_ini_converters();

	snd_Init();

	// Detect compiler
#ifdef __BORLANDC__  // Not the best way of doing things, but it works
	int cmajor, cminor;

	#pragma warn -ccc // Turn off annoying warnings
	#pragma warn -rch

#if (__BORLANDC__ < 0x0200)
	cmajor = 1;
	cminor = 0;
#elif (__BORLANDC__ == 0x0200)
	cmajor = 2;
	cminor = 0;
#elif (__BORLANDC__ == 0x0400)
	cmajor = 3;
	cminor = 0;
#elif (__BORLANDC__ == 0x0410)	/* __BCPLUSPLUS__ = 0x0310 */
	cminor = 3;
	cminor = 1;
#elif (__BORLANDC__ == 0x0452)	/* __BCPLUSPLUS__ = 0x0320 */
	cmajor = 4;
	cminor = 0;
#elif (__BORLANDC__ == 0x0460)	/* __BCPLUSPLUS__ = 0x0340 */
	cmajor = 4;
	cminor = 5;
#elif (__BORLANDC__ == 0x0500)	/* __BCPLUSPLUS__ = 0x0340 */
	cmajor = 5;
	cminor = 0;
#elif (__BORLANDC__ == 0x0520)	/* __BCPLUSPLUS__ = 0x0520 */
	cmajor = 5;
	cminor = 2; // C++Builder 1
#elif (__BORLANDC__ == 0x0530)	/* __BCPLUSPLUS__ = 0x0530 */
	cmajor = 5;
	cminor = 3; // C++Builder 3
#elif (__BORLANDC__ == 0x0540)	/* __BCPLUSPLUS__ = 0x0540 */
	cmajor = 5;
	cminor = 4; // C++Builder 4
#elif (__BORLANDC__ == 0x0550)	/* __BCPLUSPLUS__ = 0x0550 */
	cmajor = 5;
	cminor = 5; // C++Builder 5
#elif (__BORLANDC__ == 0x0551)	/* __BCPLUSPLUS__ = 0x0551 */
	cmajor = 5;
	cminor = 5; // BCC 5.5 (command line tools)
#else
	cmajor = -1;
	cminor = -1;
#endif

	#pragma warn .ccc
	#pragma warn .rch

	if (cmajor == -1)
		sprintf(__bc_compiler_ver, "__BORLANDC__ = 0x%X", __BORLANDC__);
	else
		sprintf(__bc_compiler_ver, "%d.%d (0x%X)", cmajor, cminor, __BORLANDC__);
#elif defined(__DJGPP__)
	sprintf(__dj_compiler_ver, "%d.%d (GCC %d.%d.%d)", __DJGPP__, __DJGPP_MINOR__, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

#if defined(__BORLANDC__) && defined(__MSDOS__)  // Turbo Vision version
	strcpy(__tv_ver, "2.0 or higher");
#elif defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	strcpy(__tv_ver, TV_VERSION);
#endif

#if defined(__GNUC__) && !defined(__DJGPP__)
	sprintf(__gcc_ver, "%d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

#ifdef ALLEGRO_H	// Allegro version
	#ifdef ALLEGRO_VERSION_STR
		strcpy(__alleg_ver, ALLEGRO_VERSION_STR);
	#else
		#ifdef ALLEGRO_VERSION && ALLEGRO_SUB_VERSION
			sprintf(__alleg_ver, "%d.%d", ALLEGRO_VERSION, ALLEGRO_SUB_VERSION);
		#else
			strcpy(__alleg_ver, "Unknown");
		#endif
	#endif
#else
	strcpy(__alleg_ver, "Not applicable");
#endif

#if defined(ALLEGRO_H) || defined(__MSDOS__)	 // OS running on
	char *s;

	switch (os_type)
	{
		case OSTYPE_UNKNOWN:     s = "DOS";                    break;
		case OSTYPE_WIN3:        s = "Windows 3.x";            break;
		case OSTYPE_WIN95:       s = "Windows 95";             break;
		case OSTYPE_WIN98:       s = "Windows 98";             break;
		case OSTYPE_WINME:       s = "Windows ME";             break;
		case OSTYPE_WINNT:       s = "Windows NT";             break;
		case OSTYPE_WIN2000:     s = "Windows 2000";           break;
		case OSTYPE_WINXP:       s = "Windows XP";             break;
		case OSTYPE_OS2:         s = "OS/2";                   break;
		case OSTYPE_WARP:        s = "OS/2 Warp 3";            break;
		case OSTYPE_DOSEMU:      s = "Linux DOSEMU";           break;
		case OSTYPE_OPENDOS:     s = "Caldera OpenDOS";        break;
		case OSTYPE_LINUX:       s = "Linux";                  break;
		case OSTYPE_FREEBSD:     s = "FreeBSD";                break;
		case OSTYPE_UNIX:        s = "Unix";                   break;
#ifndef REAL_DOS // BCC isn't liking these for some reason :/
		case OSTYPE_BEOS:        s = "BeOS";                   break;
		case OSTYPE_QNX:         s = "QNX";                    break;
		case OSTYPE_MACOS:       s = "MacOS";                  break;
#endif		
		default:                 s = "Unknown";                break;
	}

	strcpy(__os, s);
#else // Got to do it the hard way
	#ifdef __MSDOS__
		strcpy(__os, "DOS");
	#elif defined(__LINUX__)
		strcpy(__os, "Linux (presumably)");
	#elif defined(__WIN32__)
		OSVERSIONINFO VerInfo;

		VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&VerInfo);

		if (VerInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS)
		{
			if (VerInfo.dwMajorVersion == 4 && VerInfo.dwMinorVersion == 0)
				strcpy(__os, "Windows 95");
			else if (VerInfo.dwMajorVersion == 4 && VerInfo.dwMinorVersion == 10)
				strcpy(__os, "Windows 98");
			else if (VerInfo.dwMajorVersion == 4 && (VerInfo.dwMinorVersion == 90 || VerInfo.dwMinorVersion == 9)) // not 100% sure which
				strcpy(__os, "Windows ME");																								 // - W98 was a bit odd
			else
				strcpy(__os, "Windows 9x");
		}
		else if (VerInfo.dwPlatformId & VER_PLATFORM_WIN32_NT)
		{
			if (VerInfo.dwMajorVersion == 5 && VerInfo.dwMinorVersion == 0)
				strcpy(__os, "Windows 2000");
			else if (VerInfo.dwMajorVersion == 5 && VerInfo.dwMinorVersion == 1)
				strcpy(__os, "Windows XP");
			else if (VerInfo.dwMajorVersion == 5 && VerInfo.dwMinorVersion == 2)
				strcpy(__os, "Windows Server 2003/XP 64-bit");
			else
				strcpy(__os, "Windows NT"); // dunno what MS will invent in the future  ;)
		}
	#endif
#endif

#ifdef __MSDOS__ // OS version
	int dmajor, dminor;
	union REGS regs;

	regs.x.ax = 0x3000;
	int86(0x21, &regs, &regs);

	dmajor = regs.h.al;
	dminor = regs.h.ah;

	sprintf(__os_ver, "DOS %d.%d", dmajor, dminor);
#elif defined(__WIN32__)
	sprintf(__os_ver, "%ld.%ld (%s)", VerInfo.dwMajorVersion, VerInfo.dwMinorVersion, VerInfo.szCSDVersion);
#else
	sprintf(__os_ver, "Unknown");
#endif

#ifdef __MSVC__
	#if (_MSC_VER == 1400)
		sprintf(__msvc_compiler_ver, "8.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1310)
		sprintf(__msvc_compiler_ver, "7.1 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1300)
		sprintf(__msvc_compiler_ver, "7.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1200)
		sprintf(__msvc_compiler_ver, "6.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1100)
		sprintf(__msvc_compiler_ver, "5.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1020)
		sprintf(__msvc_compiler_ver, "4.2 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1000)
		sprintf(__msvc_compiler_ver, "4.0 (%d)", _MSC_VER);
	#else
		sprintf(__msvc_compiler_ver, "%d", _MSC_VER);
	#endif
#endif

	TEditorApp editorApp;
	editorApp.run();

	snd_Exit();
	return 0;
#if defined(__LINUX__) && defined(ALLEGRO_H)
} END_OF_MAIN();
#else
}
#endif

#ifdef SAVE_RESTORE_DESKTOP
//
// retrieveDesktop() function (restores the previously stored Desktop)
//

void TEditorApp::retrieveDesktop()
{
	FILE *fp;

	if ((fp = fopen("OSPEDIT.DST", "r")) == NULL)
		messageBox("Cannot find desktop file OSPEDIT.DST.", mfOKButton | mfError);
	else
	{
		fclose(fp);
		fpstream *f = new fpstream("OSPEDIT.DST", ios::in|ios::bin);

		if (!f)
			messageBox("Cannot open desktop file", mfOKButton | mfError);
		else
		{
			TEditorApp::loadDesktop(*f);
			if (!f)
				messageBox("Error reading desktop file", mfOKButton | mfError);
		}

		delete f;
	}
}

//
// saveDesktop() function (saves the DeskTop by calling storeDesktop function)
//

void TEditorApp::saveDesktop()
{
	fpstream *f = new fpstream("OSPEDIT.DST", ios::out|ios::bin);

	if (f)
	{
		TEditorApp::storeDesktop(*f);

		if (!f)
		{
			messageBox("Could not create OSPEDIT.DST.", mfOKButton | mfError);
			delete f;
			::remove("OSPEDIT.DST");
			return;
		}
	}

	delete f;
	messageBox("The desktop has been saved.", mfOKButton | mfInformation);
}

void TEditorApp::loadDesktop(fpstream &s)
{
	TView *p;

	if (deskTop->valid(cmClose))
	{
		deskTop->forEach(::closeView, 0); //View, 0);  // Clear the desktop
		do {
			s >> p;
			deskTop->insertBefore(validView(p), deskTop->last);
		} while (p != 0);
	}
}

//
// storeDesktop() function (stores the Desktop in a resource file)
//

void TEditorApp::storeDesktop(fpstream& s)
{
	deskTop->forEach(::writeView, &s);
	s << 0;
}

static void writeView(TView *p, void *strm)
{
	fpstream *s = (fpstream *) strm;
	if (p != TProgram::deskTop->last)
		*s << p;
}

//
// closeView() function
//

static void closeView(TView *p, void *p1)
{
	message(p, evCommand, cmClose, p1);
}
#endif