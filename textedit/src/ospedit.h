/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2009. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/

/* Revision History:
 *
 * 05/01/2002: Cleaned up indentation
 * 13/10/2002: Added MSVC support
 * 12/12/2009: Tidy up intendation again
 */

#ifndef __OSPEDIT_H__

#define __OSPEDIT_H__

#include "ospver.h"

#ifndef PATH_MAX
	#define PATH_MAX  255
#endif

#ifndef RES_INFO_ONLY

extern char __tv_ver[50];
extern char __os[50];
extern char __os_ver[50];

#define OSP_TV_VER     __tv_ver
#define OSP_OS         __os
#define OSP_OS_VER     __os_ver

#if defined(__MSDOS__) && defined(__BORLANDC__)
	#define BUILD_VERSION     "Borland C++ version (DOS)"
	#define OSP_COMPILER      "Borland C++"
	#define OSP_COMPILER_VER  __bc_compiler_ver

	extern char __bc_compiler_ver[50];
#elif defined(__DJGPP__)
	#define BUILD_VERSION   "DJGPP version (DOS)"

	#define OSP_COMPILER          "DJGPP"
	#define OSP_COMPILER_VER      __dj_compiler_ver

	extern char __dj_compiler_ver[50];
#elif defined(__WIN32__)
	#ifdef __BORLANDC__
		#define BUILD_VERSION     "Borland C++ version (Win32)"
		#define OSP_COMPILER      "Borland C++"
		#define OSP_COMPILER_VER  __bc_compiler_ver

		extern char __bc_compiler_ver[50];
	#elif defined(__MINGW32__)
		#ifdef _WIN64
			#define BUILD_VERSION     "MinGW version (Win64)"
		#else
			#define BUILD_VERSION     "MinGW version (Win32)"
		#endif

		#define OSP_COMPILER	  "MinGW"
		#define OSP_COMPILER_VER  __gcc_ver

		extern char __gcc_ver[50];
	#else
		#ifdef _WIN64
			#define BUILD_VERSION     "MSVC version (Win64)"
		#else
			#define BUILD_VERSION     "MSVC version (Win32)"
		#endif

		#define OSP_COMPILER	  "Microsoft Visual C++"
		#define OSP_COMPILER_VER  __msvc_compiler_ver

		#define __MSVC__  1
		extern char __msvc_compiler_ver[50];
	#endif
#elif defined(__APPLE__)
	#include <stdint.h>

	#if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
		#define BUILD_VERSION	"Mac OS X version (64-bit)"
	#else
		#define BUILD_VERSION	"Mac OS X version (32-bit)"
	#endif

	#ifdef __llvm__
		#define OSP_COMPILER	"LLVM"
	#else
		#define OSP_COMPILER	"GNU GCC"
	#endif
	
	#define OSP_COMPILER_VER	__gcc_ver

	extern char __gcc_ver[50];
#elif defined(__LINUX__)
	#define __STDC_LIMIT_MACROS
	#include <stdint.h>

	#if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
		#define BUILD_VERSION	"Linux version (64-bit)"
	#else
		#define BUILD_VERSION	"Linux version (32-bit)"
	#endif

	#define OSP_COMPILER      "GNU GCC"
	#define OSP_COMPILER_VER  __gcc_ver

	extern char __gcc_ver[50];
#else
	#define BUILD_VERSION     "Build unknown"
	#define OSP_COMPILER      "Unknown"
	#define OSP_COMPILER_VER  "Unknown"
#endif

#ifndef VERSION_INFORMATION_ONLY

// TODO: This should probably not be included app-wide - need to look and figure out the best approach here

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

/*
#define Uses_TApplication
#define Uses_TEditWindow
#define Uses_TMenuBar
#define Uses_TStatusLine
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TScrollBar
#define Uses_TListBox

#include <tvision/tv.h>*/

class fpstream;

const int
#if !(defined(_SET_TVISION) || defined(__DJGPP__))
  cmChangeDrct     = 102,
  cmShowClip       = 105,
#endif
  cmCalcCmd	       = 104,
  cmAbout          = 106,
  cmAboutApp       = 107,
  cmSelectWAV      = 108,
  cmPlayWAV	       = 109,
  cmStopWAV	       = 110,
  cmSelectMID      = 111,
  cmPlayMID	       = 112,
  cmStopMID	       = 113,
  cmSaveDesktop    = 114,
  cmRestoreDesktop = 115,
  cmVerInfo	       = 116,
  cmCnvInfo	       = 117;

class TEditorApp : public TApplication
{

public:

   TEditorApp();

   virtual void handleEvent( TEvent& event );
   static TMenuBar *initMenuBar( TRect );
   static TStatusLine *initStatusLine( TRect );
   virtual void outOfMemory();

   void OpenFile(char *filename);

private:

   TEditWindow *openEditor( const char *fileName, Boolean visible );
   void fileOpen();
   void fileNew();
   void changeDir();
   void dosShell();
   void showClip();
   void tile();
   void cascade();
   void calculator();
   void aboutBox();
   void aboutProg();
   void verInfo();
   void cnvInfo();

   void retrieveDesktop();
   void saveDesktop();
   void loadDesktop(fpstream &r);
   void storeDesktop(fpstream &r);
};

extern TEditWindow *clipWindow;

ushort execDialog( TDialog *d, void *data );
TDialog *createFindDialog();
TDialog *createReplaceDialog();
ushort doEditDialog( int dialog, ... );

#ifdef SAVE_RESTORE_DESKTOP
   static void writeView(TView *p, void *strm);
   static void closeView(TView *p, void *p1);
#endif

#endif // VERSION_INFORMATION_ONLY

#endif // RES_INFO_ONLY

#endif // __OSPEDIT_H__