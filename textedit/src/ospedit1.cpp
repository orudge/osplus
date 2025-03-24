/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2012. All Rights Reserved.  */
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
 * 12/12/2009: Add support for detecting Windows Vista/7, 64-bit (orudge)
 * 
 * As of 2011 this is no longer being updated; please refer to the Subversion log instead.
 */

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

#ifdef __APPLE__
	#define Boolean __apple_Boolean
	#include <CoreServices/CoreServices.h>
	#undef Boolean
#endif

#ifdef SAVE_RESTORE_DESKTOP
	#define Uses_fpstream
#endif

#if defined(__DJGPP__) || defined(__SET_TVISION__)
	#define Uses_TCalculator
#endif

#include "inc_tv.h"

#if !(defined(__BORLANDC__) && defined(__REALDOS__))
	#ifndef __WIN32__
		#include <unistd.h>
	#endif

	#if (__GNUC__ >= 3) || (_MSC_VER >= 1300)
		#include <sstream>
	#else
		#include <strstream.h>
	#endif
#else
	#include <strstrea.h>
#endif

#if defined(__REALDOS__) || defined(__DJGPP__)
	#include "realdos.h"
#endif

#if defined(__LINUX__) && defined(__SET_TVISION__)
	extern "C" {
		extern char *strlwr(char *str);
		extern char *strupr(char *str);
	}
#endif

#ifdef __WIN32__
	#include <windows.h>
#endif

#if defined(__DJGPP__)
	#include <sys/exceptn.h>
#endif

#include "ospedit.h"
#include "aboutdlg.h"
#include "aboutosp.h"
#include "verinfo.h"
#include "cnvinfo.h"
#include "calc.h"

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

#include "convert.h"

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

#ifdef __REALDOS__
	typedef char bool;

	#define false	0
	#define true	1
#endif

extern "C" {
	extern bool fold_file (char const *filename, char const *output, size_t width);
}

void TEditorApp::OpenFile(char *filename)
{
	QuickMessage *qm = new QuickMessage("");
	char szBuf[PATH_MAX], errortxt[200];
	int ret;

	TProgram::deskTop->insert(qm);

	ret = convert_text_file(filename, szBuf, errortxt, 0);

	if ((!(ret & ERROR_CNV_OK)) && (ret != -2))
	{
		messageBox(mfError | mfOKButton, errortxt);
	}
	
	TProgram::deskTop->remove( qm );
	destroy(qm);

#if 0 /* TODO */
	fold_file (szBuf, output, 78);
#endif

	openEditor(szBuf, True);

	if ((ret != -2) && (ret & CONVERT_DELETE_FILE))
		unlink(szBuf);
}

TEditorApp::TEditorApp() :
	 TProgInit(TEditorApp::initStatusLine,
					TEditorApp::initMenuBar,
					TEditorApp::initDeskTop
				 ),
	 TApplication()
{
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
}

#ifdef __REALDOS__
	#define ALL_FILES		"*.*"
#else
	#define ALL_FILES		"*"
#endif

void TEditorApp::fileOpen()
{
	char fileName[PATH_MAX];

	strcpy( fileName, ALL_FILES );

	if (execDialog(new TFileDialog(ALL_FILES, "Open File",
				"~N~ame", fdOpenButton, 100), fileName) != cmCancel)
	{
		OpenFile(fileName);
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
	#if defined(__WIN32__)
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

#ifdef __REALDOS__
	executeDialog(aboutBox);
#else
	deskTop->execView(aboutBox);
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

#ifdef __REALDOS__
	executeDialog(aboutBox);
#else
	deskTop->execView(aboutBox);
#endif
}

void TEditorApp::verInfo()
{
	TVerInfoDlg *verInfoBox = new TVerInfoDlg();

	verInfoBox->options |= ofCentered;

#ifdef __REALDOS__
	executeDialog(verInfoBox);
#else
	deskTop->execView(verInfoBox);
#endif
}

void TEditorApp::cnvInfo()
{
	TCnvInfoDialog *cnvInfoBox = new TCnvInfoDialog();

	cnvInfoBox->options |= ofCentered;

#ifdef __REALDOS__
	executeDialog(cnvInfoBox);
#else
	deskTop->execView(cnvInfoBox);
#endif
}

void TEditorApp::calculator()
{
	TCalculator* calc = (TCalculator*)validView(new TCalculator);

	if (calc != 0)
		deskTop->insert(calc);
}

#if __WIN32__
static void identify_windows_version(DWORD platform, DWORD major, DWORD minor, LPSTR csdVersion)
{
	if (platform & VER_PLATFORM_WIN32_WINDOWS)
	{
		if (major == 4 && minor == 0)
			strcpy(__os, "Windows 95");
		else if (major == 4 && minor == 10)
			strcpy(__os, "Windows 98");
		else if (major == 4 && (minor == 90 || minor == 9)) // not 100% sure which
			strcpy(__os, "Windows Me");						// - W98 was a bit odd
		else
			strcpy(__os, "Windows 9x");
	}
	else if (platform & VER_PLATFORM_WIN32_NT)
	{
		if (major == 5 && minor == 0)
			strcpy(__os, "Windows 2000");
		else if (major == 5 && minor == 1)
			strcpy(__os, "Windows XP");
		else if (major == 5 && minor == 2)
			strcpy(__os, "Windows Server 2003/XP 64-bit");
		else if (major == 6 && minor == 0)
			strcpy(__os, "Windows Vista");
		else if (major == 6 && minor == 1)
			strcpy(__os, "Windows 7");
		else if (major == 6 && minor == 2)
			strcpy(__os, "Windows 8");
		else if (major == 6 && minor == 3)
			strcpy(__os, "Windows 8.1");
		else if (major == 10 && minor == 0)
			strcpy(__os, "Windows 10");
		else if (major == 11 && minor == 0)
			strcpy(__os, "Windows 11");
		else
			strcpy(__os, "Windows NT"); // dunno what MS will invent in the future  ;)
	}

	if (csdVersion != NULL && strlen(csdVersion) > 0)
		sprintf(__os_ver, "%ld.%ld (%s)", major, minor, csdVersion);
	else
		sprintf(__os_ver, "%ld.%ld", major, minor);
}
#endif

int main(int argc, char *argv[])
{
	// Register converters
	get_app_path(argc, argv);
	register_converter_file_type("rtf", "txtrtf.cnv", NULL, "Rich Text Format");
	register_converter_file_type("wri", "txtwrite.cnv", NULL, "Windows Write");
	register_microsoft_converters();
	register_ini_converters();

#ifdef __WIN32__
	SetConsoleTitle("OSPlus Text Editor");
#endif

#if defined(__DJGPP__)
	__djgpp_set_ctrl_c(0);
#endif

#if defined(__REALDOS__) || defined(__DJGPP__)
	detect_os();
#endif

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

#if defined(_SET_TVISION) || defined(__DJGPP__)
	strcpy(__tv_ver, TV_VERSION);
#elif defined(__BORLANDC__) && defined(__MSDOS__)
	strcpy(__tv_ver, "2.0");
#else
	strcpy(__tv_ver, "2.0 (magiblot)");
#endif

#if defined(__GNUC__) && !defined(__DJGPP__)
	sprintf(__gcc_ver, "%d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

#if defined(__MSDOS__)	 // OS running on
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
#ifndef __REALDOS__ // BCC isn't liking these for some reason :/
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
		strcpy(__os, "Linux/Unix");
	#elif defined(__WIN32__)
		NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW) = NULL;
		HMODULE ntdll = GetModuleHandleA("ntdll");

		if (ntdll != NULL)
			*(FARPROC*)&RtlGetVersion = GetProcAddress(ntdll, "RtlGetVersion");

		if (RtlGetVersion != NULL)
		{
			OSVERSIONINFOEXW verInfo;

			verInfo.dwOSVersionInfoSize = sizeof(verInfo);
			RtlGetVersion(&verInfo);

			char* csd_version = NULL;

			if (lstrlenW(verInfo.szCSDVersion) > 0)
			{
				int size_needed = WideCharToMultiByte(CP_ACP, 0, verInfo.szCSDVersion, lstrlenW(verInfo.szCSDVersion), NULL, 0, NULL, NULL);
				csd_version = (char *) malloc(size_needed);
				WideCharToMultiByte(CP_ACP, 0, verInfo.szCSDVersion, lstrlenW(verInfo.szCSDVersion), csd_version, size_needed, NULL, NULL);
			}

			identify_windows_version(verInfo.dwPlatformId, verInfo.dwMajorVersion, verInfo.dwMinorVersion, csd_version);
			free(csd_version);
		}
		else
		{
			OSVERSIONINFO verInfo;

			verInfo.dwOSVersionInfoSize = sizeof(verInfo);
			GetVersionExA(&verInfo);

			identify_windows_version(verInfo.dwPlatformId, verInfo.dwMajorVersion, verInfo.dwMinorVersion, verInfo.szCSDVersion);
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
	// Handled in identify_windows_version
#elif defined(__APPLE__)
	SInt32 major, minor, revision;

	Gestalt(gestaltSystemVersionMajor, &major);
	Gestalt(gestaltSystemVersionMinor, &minor);
	Gestalt(gestaltSystemVersionBugFix, &revision);

	sprintf(__os_ver, "%d.%d.%d", major, minor, revision);

	if (major < 16)
		strcpy(__os, "Mac OS X");
	else
		strcpy(__os, "macOS");
#else
	sprintf(__os_ver, "Unknown");
#endif

#ifdef __MSVC__
	#if (_MSC_VER >= 1930)
		sprintf(__msvc_compiler_ver, "17+ (%d)", _MSC_VER);
	#elif (_MSC_VER >= 1920 && _MSC_VER < 1930)
		sprintf(__msvc_compiler_ver, "16 (%d)", _MSC_VER);
	#elif (_MSC_VER >= 1910 && _MSC_VER < 1920)
		sprintf(__msvc_compiler_ver, "15 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1900)
		sprintf(__msvc_compiler_ver, "14.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1800)
		sprintf(__msvc_compiler_ver, "12.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1700)
		sprintf(__msvc_compiler_ver, "11.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1600)
		sprintf(__msvc_compiler_ver, "10.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1500)
		sprintf(__msvc_compiler_ver, "9.0 (%d)", _MSC_VER);
	#elif (_MSC_VER == 1400)
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

#ifdef _WIN64
	strcat(__msvc_compiler_ver, " (x64)");
#else
	strcat(__msvc_compiler_ver, " (x86)");
#endif

#endif

	TEditorApp editorApp;

	if (argc >= 2) // command line parameters?
		editorApp.OpenFile(argv[1]);

	editorApp.run();

	return 0;
}

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
