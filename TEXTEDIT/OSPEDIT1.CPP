/************************************************************/
/* OSPlus Utility Kit 1.2                                   */
/* Copyright (c) Owen Rudge 2000-2001. All Rights Reserved. */
/************************************************************/
/* OSPlus Text Editor - Standalone                          */
/* OSPEDIT.EXE                                              */
/************************************************************/
/* Main source file                                         */
/************************************************************/

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

#include "sound.h"

#if defined(__DJGPP__) || defined(__LINUX__)
   #define Uses_TCalculator

   #include <tv.h>
   #include <unistd.h>
   #include <strstream.h>

   #if defined(__LINUX__) && !defined(LINUX_NO_SOUND_SUPPORT)
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

#if !defined(ALLEGRO_H) && !defined(__WIN32__)
   #define REAL_DOS // real-mode DOS
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

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iomanip.h>
#include <limits.h>
#include <stdio.h>

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

typedef int BOOL;

#ifndef FALSE
  #define FALSE False
  #define TRUE  True
#endif

BOOL IsInitialFile = False;     // was file specified on command line?
char initialfile[200];          // file name on cmd line

extern char WAVName[MAXPATH];          // file name of WAV
extern BOOL WAVLoaded;         // WAV loaded?

extern char MIDName[MAXPATH];          // file name of MID
extern BOOL MIDLoaded;         // MID loaded?

extern BOOL SoundEnabled;

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
	 char szBuf[200], tmp[200];

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
#ifdef __LINUX__
                  strcpy(tmp, initialfile);
		  if (strstr(strupr(tmp), ".RTF") != NULL)
#else
		  if (strstr(strupr(initialfile), ".RTF") != NULL)
#endif
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

#ifdef __LINUX__
			  sprintf(tmp, "./txtrtf.cnv \"%s\" \"%s\"", initialfile, szBuf);
			  system(tmp);
#else
			  tmpnam(szBuf);
			  spawnl(P_WAIT, "TXTRTF.CNV", "TXTRTF.CNV", initialfile, szBuf, NULL);
#endif

			  TProgram::deskTop->remove( qm );
			  destroy(qm);

			  openEditor(szBuf, True);
			  unlink(szBuf);
		  }
#ifdef __LINUX__
                  else if (strstr(strupr(tmp), ".WRI") != NULL)
#else
		  else if (strstr(strupr(initialfile), ".WRI") != NULL)
#endif
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
#ifdef __LINUX__
			  sprintf(tmp, "./txtwrite.cnv \"%s\" \"%s\"", initialfile, szBuf);
			  system(tmp);
#else
			  spawnl(P_WAIT, "TXTWRITE.CNV", "TXTWRITE.CNV", initialfile, szBuf, NULL);
#endif

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
	 char szBuf[144], tmp[MAXPATH];

	 strcpy( fileName, "*.*" );

	 if( execDialog( new TFileDialog( "*.*", "Open File",
				"~N~ame", fdOpenButton, 100 ), fileName) != cmCancel )
	 {
#ifdef __LINUX__
                  strcpy(tmp, fileName);
                  if (strstr(strupr(tmp), ".RTF") != NULL)
#else
		  if (strstr(strupr(fileName), ".RTF") != NULL)
#endif
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
#ifdef __LINUX__
			  sprintf(tmp, "./txtrtf.cnv \"%s\" \"%s\"", fileName, szBuf);
			  system(tmp);
#else
			  spawnl(P_WAIT, "TXTRTF.CNV", "TXTRTF.CNV", fileName, szBuf, NULL);
#endif

			  TProgram::deskTop->remove( qm );
			  destroy(qm);

			  openEditor(szBuf, True);
			  unlink(szBuf);
		  }
#ifdef __LINUX__
                  else if (strstr(strupr(tmp), ".WRI") != NULL)
#else
		  else if (strstr(strupr(fileName), ".WRI") != NULL)
#endif
		  {
			  QuickMessage *qm = new QuickMessage( "" );
			  TProgram::deskTop->insert( qm );

			  tmpnam(szBuf);
#ifdef __LINUX__
			  sprintf(tmp, "./txtwrite.cnv \"%s\" \"%s\"", fileName, szBuf);
			  system(tmp);
#else
			  spawnl(P_WAIT, "TXTWRITE.CNV", "TXTWRITE.CNV", fileName, szBuf, NULL);
#endif

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

#ifdef __LINUX__
 	system("clear");
        cout << "Type exit to return to OSPlus Text Editor\n";
	system(getenv("SHELL"));
#else
	system("cls");
        cout << "Type EXIT to return to OSPlus Text Editor\n";
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

				case cmSelectMID:
					 selectMID();
					 break;

				case cmPlayMID:
					 playMID();
					 break;

				case cmStopMID:
					 stopMID();
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
                  "\003Version 1.2\n\n"
                  "\003Copyright (c) Owen Rudge 2000-2001\n"       // The \003 centers the line.
		  "\003All Rights Reserved\n"
		  )
		);

	 aboutBox->insert(
		new TButton(TRect(14, 10, 26, 12), " OK", cmOK, bfDefault)
		);

	 aboutBox->options |= ofCentered;

#if defined(__DJGPP__) || defined(__LINUX__)
    deskTop->execView(aboutBox);
#else
	 executeDialog(aboutBox);
#endif
}

void TEditorApp::aboutProg()
{
	 TDialog *aboutBox = new TAboutDialog();

	 aboutBox->options |= ofCentered;

#if defined(__DJGPP__) || defined(__LINUX__)
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

#if (defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)) && !defined(ALLEGRO_H)
   messageBox("Sound is not enabled with this version.", cmOK);
#else
   strcpy( WAVName, "*.WAV" );

   if( execDialog( new TFileDialog( "*.WAV", "Select WAV File",
			"~N~ame", fdOpenButton, 100 ), WAVName) != cmCancel )
      snd_LoadWAV();
#endif
leave: ;
}

void TEditorApp::playWAV()
{
#if (defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)) && !defined(ALLEGRO_H)
   messageBox("Sound is not enabled with this version.", cmOK);
#else
   snd_PlayWAV();
#endif
}

void TEditorApp::stopWAV()
{
#if (defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)) && !defined(ALLEGRO_H)
   messageBox("Sound is not enabled with this version.", cmOK);
#else
   snd_StopWAV();
#endif
}

void TEditorApp::selectMID()
{
   if (SoundEnabled == FALSE) goto leave;

#ifdef NO_MIDI_MUSIC
   messageBox("MIDI music is not enabled with this version.", cmOK);
#else
   strcpy( MIDName, "*.MID" );

   if( execDialog( new TFileDialog( "*.MID", "Select MIDI File",
			"~N~ame", fdOpenButton, 100 ), MIDName) != cmCancel )
      snd_LoadMID();
#endif
leave: ;
}

void TEditorApp::playMID()
{
#ifdef NO_MIDI_MUSIC
   messageBox("MIDI music is not enabled with this version.", cmOK);
#else
   snd_PlayMID();
#endif
}

void TEditorApp::stopMID()
{
#ifdef NO_MIDI_MUSIC
   messageBox("MIDI music is not enabled with this version.", cmOK);
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

	 snd_Init();

	 TEditorApp editorApp;
	 editorApp.run();

       snd_Exit();
	 return 0;
#if defined(__LINUX__) && defined(ALLEGRO_H)
} END_OF_MAIN();
#else
}
#endif
