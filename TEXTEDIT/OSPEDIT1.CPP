/**********************************************************/
/* OSPlus Utility Kit 1.1                                 */
/* Copyright (c) Owen Rudge 2000. All Rights Reserved.    */
/**********************************************************/
/* OSPlus Text Editor - Standalone                        */
/* OSPEDIT.EXE                                            */
/**********************************************************/
/* Main source file                                       */
/**********************************************************/

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
 */

//#define DJGPP_NO_SOUND_SUPPORT

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

#ifdef __DJGPP__
   #define Uses_TCalculator

   #include <tv.h>
   #include <unistd.h>
   #include <strstream.h>

   #ifndef DJGPP_NO_SOUND_SUPPORT
      #include <allegro.h>

      SAMPLE *waveSample;
   #endif
#else
   #include <tvision\tv.h>
   #include <strstrea.h>

   #include "calc.h"
#endif

#ifdef __WIN32__
   #include <windows.h>
#endif

#include "ospedit.h"
#include "aboutdlg.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iomanip.h>
#include <limits.h>
#include <stdio.h>
#include <process.h>
#include <io.h>
#include <conio.h>

#include "gemsnd.h"

#ifndef MAXPATH
   #ifdef PATH_MAX
      #define MAXPATH PATH_MAX
   #else
      #define MAXPATH  200
   #endif
#endif

typedef int BOOL;

BOOL IsInitialFile = False;     // was file specified on command line?
char initialfile[200];          // file name on cmd line

char WAVName[MAXPATH];          // file name of WAV
BOOL WAVLoaded = FALSE;         // WAV loaded?

BOOL SoundEnabled = FALSE;

TEditWindow *clipWindow;

class QuickMessage: public TWindow
{
	 TParamText *currentDir;
public:
	 QuickMessage( const char *drive ):
		  TWindow( TRect( 25,10,55,16 ), "Please Wait", 0 ),
		  TWindowInit( TWindow::initFrame ) {

	 insert( new TStaticText( TRect( 2,2,28,3 ), "Converting File..." ) );

	 flags = 0; // no move, close, grow or zoom
	 options |= ofCentered;
	 palette = wpGrayWindow;
  }
  virtual void handleEvent( TEvent &event ) {
	 TWindow::handleEvent( event );
  }
  void setCurrentDir( char *newDir ) {
	 currentDir->setText( newDir );
  }
};

TEditWindow *TEditorApp::openEditor( const char *fileName, Boolean visible )
{
	 TRect r = deskTop->getExtent();
	 TView *p = validView( new TEditWindow( r, fileName, wnNoNumber ) );
	 if( !visible )
		  p->hide();
	 deskTop->insert( p );
	 return (TEditWindow *)p;
}

TEditorApp::TEditorApp() :
	 TProgInit( TEditorApp::initStatusLine,
					TEditorApp::initMenuBar,
					TEditorApp::initDeskTop
				 ),
	 TApplication()
{
	 char szBuf[200];

	 TCommandSet ts;
	 ts.enableCmd( cmSave );
	 ts.enableCmd( cmSaveAs );
	 ts.enableCmd( cmCut );
	 ts.enableCmd( cmCopy );
	 ts.enableCmd( cmPaste );
	 ts.enableCmd( cmClear );
	 ts.enableCmd( cmUndo );
	 ts.enableCmd( cmFind );
	 ts.enableCmd( cmReplace );
	 ts.enableCmd( cmSearchAgain );

	 disableCommands( ts );

	 TEditor::editorDialog = doEditDialog;
	 clipWindow = openEditor( 0, False );
	 if( clipWindow != 0 )
		  {
		  TEditor::clipboard = clipWindow->editor;
		  TEditor::clipboard->canUndo = False;
		  }

	 if (IsInitialFile == True)
	 {
		  if (strstr(strupr(initialfile), ".RTF") != NULL)
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
			  spawnl(P_WAIT, "TXTRTF.CNV", "TXTRTF.CNV", initialfile, szBuf, NULL);

			  TProgram::deskTop->remove( qm );
			  destroy(qm);

			  openEditor(szBuf, True);
			  unlink(szBuf);
		  }
		  else if (strstr(strupr(initialfile), ".WRI") != NULL)
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
			  spawnl(P_WAIT, "TXTWRITE.CNV", "TXTWRITE.CNV", initialfile, szBuf, NULL);

			  TProgram::deskTop->remove( qm );
			  destroy(qm);

			  openEditor(szBuf, True);
			  unlink(szBuf);
		  }
		  else
			  openEditor(initialfile, True);
	 }
}

void TEditorApp::fileOpen()
{
	 char fileName[MAXPATH];
	 char szBuf[144];

	 strcpy( fileName, "*.*" );

	 if( execDialog( new TFileDialog( "*.*", "Open File",
				"~N~ame", fdOpenButton, 100 ), fileName) != cmCancel )
	 {
		  if (strstr(strupr(fileName), ".RTF") != NULL)
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
			  spawnl(P_WAIT, "TXTRTF.CNV", "TXTRTF.CNV", fileName, szBuf, NULL);

			  TProgram::deskTop->remove( qm );
			  destroy(qm);

			  openEditor(szBuf, True);
			  unlink(szBuf);
		  }
		  else if (strstr(strupr(fileName), ".WRI") != NULL)
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
			  spawnl(P_WAIT, "TXTWRITE.CNV", "TXTWRITE.CNV", fileName, szBuf, NULL);

			  TProgram::deskTop->remove( qm );
			  destroy(qm);

			  openEditor(szBuf, True);
			  unlink(szBuf);
		  }
		  else
			  openEditor(fileName, True);
	 }
}

void TEditorApp::fileNew()
{
	 openEditor( 0, True );
}

void TEditorApp::changeDir()
{
    execDialog( new TChDirDialog( cdNormal, 0 ), 0 );
}

void TEditorApp::dosShell()
{
	 suspend();
    system("cls");
	 cout << "Type EXIT to return to OSPlus Text Editor";
	 system( getenv( "COMSPEC"));
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
    deskTop->tile( deskTop->getExtent() );
}

void TEditorApp::cascade()
{
    deskTop->cascade( deskTop->getExtent() );
}

void TEditorApp::handleEvent( TEvent& event )
{
    TApplication::handleEvent( event );
    if( event.what != evCommand )
        return;
    else
	  switch( event.message.command )
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

				default:
					 return ;
            }
	 clearEvent( event );
}

void TEditorApp::aboutBox( void ) {
	 TDialog *aboutBox = new TDialog(TRect(0, 0, 39, 13), "About OSPlus");

	 aboutBox->insert(
		new TStaticText(TRect(11, 2, 30, 10), // 9
		  "\003OSPlus Utility Kit\n"
		  "\003Version 1.1\n\n"
		  "\003Copyright (c) Owen Rudge 2000\n"       // The \003 centers the line.
		  "\003All Rights Reserved\n"
		  )
		);

	 aboutBox->insert(
		new TButton(TRect(14, 10, 26, 12), " OK", cmOK, bfDefault)
		);

	 aboutBox->options |= ofCentered;

#ifdef __DJGPP__
    deskTop->execView(aboutBox);
#else
	 executeDialog(aboutBox);
#endif
}

void TEditorApp::aboutProg()
{
	 TDialog *aboutBox = new TAboutDialog();

	 aboutBox->options |= ofCentered;

#ifdef __DJGPP__
	 deskTop->execView(aboutBox);
#else
	 executeDialog(aboutBox);
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

#ifdef DJGPP_NO_SOUND_SUPPORT
   messageBox("Sound is not enabled with the DJGPP version.", cmOK);
#else
	strcpy( WAVName, "*.WAV" );

	if( execDialog( new TFileDialog( "*.WAV", "Select WAV File",
				"~N~ame", fdOpenButton, 100 ), WAVName) != cmCancel )
	{
	    WAVLoaded = TRUE;
#ifdef __DJGPP__
       destroy_sample(waveSample);
       waveSample = load_sample(WAVName);
#endif
   }
#endif
leave: ;
}

void TEditorApp::playWAV()
{
#ifdef DJGPP_NO_SOUND_SUPPORT
   messageBox("Sound is not enabled with the DJGPP version.", cmOK);
#else
 	if (WAVLoaded == TRUE)
   {
#ifdef __DJGPP__
      play_sample(waveSample, 255, 128, 1000, FALSE);
#else
 #ifdef __WIN32__
		sndPlaySound(WAVName, SND_ASYNC | SND_NODEFAULT);
 #else
		if (isPlaying() == TRUE)  StopWAV();
		PlayIWAV(WAVName);
 #endif
#endif
    }
#endif
}

void TEditorApp::stopWAV()
{
#ifdef DJGPP_NO_SOUND_SUPPORT
   messageBox("Sound is not enabled with the DJGPP version.", cmOK);
#else
	if (WAVLoaded == TRUE)
#ifdef __DJGPP__
      stop_sample(waveSample);
#else
 #ifdef __WIN32__
		sndPlaySound(NULL, SND_SYNC | SND_NODEFAULT);
 #else
		if (isPlaying() == TRUE)  StopWAV();
 #endif
#endif
#endif
}

int main(int argc, char *argv[])
{
	 if (argc >= 2) // command line parameters?
	 {
		 strcpy(initialfile, argv[1]);
		 IsInitialFile = True;
	 }

#ifdef DJGPP_NO_SOUND_SUPPORT
    SoundEnabled = FALSE;
#else
#ifdef __DJGPP__
    allegro_init();

    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") == -1)
       SoundEnabled = FALSE;
    else
       SoundEnabled = TRUE;
#else
 #ifdef __WIN32__
	 if (waveInGetNumDevs() == 0)
		 SoundEnabled = FALSE;
	 else
		 SoundEnabled = TRUE;
 #else
	 if (getenv("OSPSOUND") != NULL) // only play sounds if OSPSOUND = TRUE
	 {                               // this is really just a 'safety net'
		 SpeakerOn();                 // as if Heinz's drivers are not loaded
		 SoundEnabled = TRUE;         // it causes a pretty bad crash.
	 }
 #endif
#endif
#endif

	 TEditorApp editorApp;
	 editorApp.run();

#ifndef __DJGPP__
 #ifdef __WIN32__
	 sndPlaySound(NULL, SND_NODEFAULT);
 #else
	 if (SoundEnabled == TRUE)
	 {
		 StopWAV();
		 SpeakerOff();
	 }
 #endif
#else
    #ifndef DJGPP_NO_SOUND_SUPPORT
       destroy_sample(waveSample);
    #endif
#endif

	 return 0;
}
