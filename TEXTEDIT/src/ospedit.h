/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2001. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/

#define OSP_MAJOR_VERSION          2
#define OSP_MINOR_VERSION          0
#define OSP_VERSION_STR            "2.00"

#define OSP_TXT_MAJOR_VERSION      2
#define OSP_TXT_MINOR_VERSION      1
#define OSP_TXT_VERSION_STR        "2.1"

#ifndef RES_INFO_ONLY

extern char __tv_ver[50];
extern char __alleg_ver[50];
extern char __os[50];
extern char __os_ver[50];

#define OSP_TV_VER     __tv_ver
#define OSP_ALLEG_VER  __alleg_ver
#define OSP_OS         __os
#define OSP_OS_VER     __os_ver

#if defined(__MSDOS__) && defined(__BORLANDC__)
	#define BUILD_VERSION     "Borland C++ version (DOS)"
	#define OSP_COMPILER      "Borland C++"
	#define OSP_COMPILER_VER  __bc_compiler_ver
   #define SOUND_SUPPORT     1

	extern char __bc_compiler_ver[50];
#elif defined(__DJGPP__)
	#ifdef DJGPP_NO_SOUND_SUPPORT
		#define BUILD_VERSION   "DJGPP version (DOS), no sound"
      #define SOUND_SUPPORT   0
	#else
		#define BUILD_VERSION   "DJGPP version (DOS), with sound"
		#define SOUND_SUPPORT     1
	#endif

	#define OSP_COMPILER      "DJGPP"
	#define OSP_COMPILER_VER  __dj_compiler_ver

   extern char __dj_compiler_ver[50];
#elif defined(__WIN32__)
	#define BUILD_VERSION     "Borland C++ version (Win32)"
	#define OSP_COMPILER      "Borland C++"
	#define OSP_COMPILER_VER  __bc_compiler_ver
	#define SOUND_SUPPORT     1

	extern char __bc_compiler_ver[50];
#elif defined(__LINUX__)
	#ifdef LINUX_NO_SOUND_SUPPORT
		#define BUILD_VERSION   "Linux version, no sound"
      #define SOUND_SUPPORT   0
	#else
		#define BUILD_VERSION   "Linux version, with sound"
		#define SOUND_SUPPORT   1
	#endif

	#define OSP_COMPILER      "gcc"
	#define OSP_COMPILER_VER  "Unknown (how do you find it?)"
#else
	#define BUILD_VERSION     "Build unknown"
	#define OSP_COMPILER      "Unknown"
	#define OSP_COMPILER_VER  "Unknown"
#endif

#ifndef VERSION_INFORMATION_ONLY

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;
class fpstream;

const int
#if defined(__BORLANDC__) && !defined(__WIN32__) 
  cmChangeDrct     = 102,
  cmShowClip       = 105,
#endif
  cmCalcCmd        = 104,
  cmAbout          = 106,
  cmAboutApp       = 107,
  cmSelectWAV      = 108,
  cmPlayWAV        = 109,
  cmStopWAV        = 110,
  cmSelectMID      = 111,
  cmPlayMID        = 112,
  cmStopMID        = 113,
  cmSaveDesktop    = 114,
  cmRestoreDesktop = 115,
  cmVerInfo        = 116;

class TEditorApp : public TApplication
{

public:

	 TEditorApp();

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );
    virtual void outOfMemory();

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

    void selectWAV();
    void playWAV();
    void stopWAV();

    void selectMID();
    void playMID();
	 void stopMID();

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