/**********************************************************/
/* OSPlus Utility Kit 1.1                                 */
/* Copyright (c) Owen Rudge 2000. All Rights Reserved.    */
/**********************************************************/
/* OSPlus Text Editor - Standalone                        */
/* OSPEDIT.EXE                                            */
/**********************************************************/

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

const int
#ifdef __BORLANDC__
  cmChangeDrct = 102,
  cmShowClip   = 105,
#endif
  cmCalcCmd    = 104,
  cmAbout      = 106,
  cmAboutApp   = 107,
  cmSelectWAV  = 108,
  cmPlayWAV    = 109,
  cmStopWAV    = 110;

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
};

extern TEditWindow *clipWindow;

ushort execDialog( TDialog *d, void *data );
TDialog *createFindDialog();
TDialog *createReplaceDialog();
ushort doEditDialog( int dialog, ... );

