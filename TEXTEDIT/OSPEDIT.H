/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2001. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/

#if defined(__MSDOS__) && defined(__BORLANDC__)
   #define BUILD_VERSION   "Borland C++ version (DOS)"
#elif defined(__DJGPP__)
   #ifdef DJGPP_NO_SOUND_SUPPORT
      #define BUILD_VERSION   "DJGPP version (DOS), no sound"
   #else
      #define BUILD_VERSION   "DJGPP version (DOS), with sound"
   #endif
#elif defined(__WIN32__)
   #define BUILD_VERSION   "Borland C++ version (Win32)"
#elif defined(__LINUX__)
   #ifdef LINUX_NO_SOUND_SUPPORT
      #define BUILD_VERSION   "Linux version, no sound"
   #else
      #define BUILD_VERSION   "Linux version, with sound"
	#endif
#else
   #define BUILD_VERSION   "Build unknown"
#endif

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;
class fpstream;

const int
#ifdef __BORLANDC__
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
  cmRestoreDesktop = 115;

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
