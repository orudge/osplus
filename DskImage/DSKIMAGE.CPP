/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.4                                   */
/*                                               */
/* Copyright (c) Owen Rudge 2000-2001            */
/* Disk Writing Routines: Written by Mark Becker */
/* Disk Reading Routines: (c) Hans Lermen 1995   */
/*************************************************/

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

/* OSPlus Disk Imager History can be found in CHANGES.TXT */

/* RAWRITE History
 *	---------------
 *
 * 1.0	-	Initial release
 *
 * 1.1	-	Beta test (fixing bugs)				4/5/91
 *		      Some BIOS's don't like full-track writes.
 *
 * 1.101	-	Last beta release.				4/8/91
 *		      Fixed BIOS full-track write by only
 *		      writing 3 sectors at a time.
 *
 * 1.2	-	Final code and documentation clean-ups.		4/9/91
 */

#define DSKIMAGE_VERSION   "1.4"

#include <alloc.h>
#include <bios.h>
#include <ctype.h>
#include <dir.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define Uses_TObject
#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TWindow
#define Uses_TFrame
#define Uses_TDialog
#define Uses_TButton
#define Uses_TSItem
#define Uses_TLabel
#define Uses_TInputLine
#define Uses_TParamText
#define Uses_MsgBox

#include <tvision\tv.h>

#include "tprogbar.h"
#include "dskwrite.h"
#include "write.h"
#include "about.h"
#include "read.h"
#include "aboutosp.h"

extern "C" {
	 void OS_ChangeWindowTitle();
	 void OS_RemoveWindowTitle();
}

int OKPressed;
char fname[128];
char gdrive[3];

int ReadOKPressed;
char ReadFN[128];
char ReadDrive[3];

const int cmWrite    = 200;
const int cmAbout    = 201;
const int cmAboutApp = 202;
const int cmRead     = 203;

struct DialogData
{
	 char FileData[126];
    char DriveData[3];
};

void DoWrite();

DialogData *demoDialogData;

#define FALSE	0
#define TRUE	(!FALSE)

#define SECTORSIZE	512

#define	RESET	0
#define	LAST	1
#define	READ	2
#define	WRITE	3
#define	VERIFY	4
#define	FORMAT	5

int	done;

extern void RawReadMain(char *fn, char *drive);

#include "quickmsg.h"

/*
 Catch ^C and ^Break.
*/
int	handler(void)
{
  done = TRUE;
  return(0);
}
void msg(char (*s))
{
	messageBox(s, mfInformation | mfOKButton); //fprintf(stderr, "%s\n", s);
//	_exit(1);
}
/*
 Identify the error code with a real error message.
*/
#ifdef USE_EXIT
void Error(int (status))
#else
int Error(int (status))
#endif
{
  switch (status) {
	 case 0x00:	msg("Operation Successful");				break;
	 case 0x01:	msg("Bad command");					break;
	 case 0x02:	msg("Address mark not found");				break;
	 case 0x03:	msg("Attempt to write on write-protected disk");	break;
	 case 0x04:	msg("Sector not found");				break;
	 case 0x05:	msg("Reset failed (hard disk)");			break;
	 case 0x06:	msg("Disk changed since last operation");		break;
	 case 0x07:	msg("Drive parameter activity failed");			break;
	 case 0x08:	msg("DMA overrun");					break;
	 case 0x09:	msg("Attempt to DMA across 64K boundary");		break;
	 case 0x0A:	msg("Bad sector detected");				break;
	 case 0x0B:	msg("Bad track detected");				break;
	 case 0x0C:	msg("Unsupported track");				break;
	 case 0x10:	msg("Bad CRC/ECC on disk read");			break;
	 case 0x11:	msg("CRC/ECC corrected data error");			break;
	 case 0x20:	msg("Controller has failed");				break;
	 case 0x40:	msg("Seek operation failed");				break;
    case 0x80:	msg("Attachment failed to respond");			break;
    case 0xAA:	msg("Drive not ready (hard disk only");			break;
    case 0xBB:	msg("Undefined error occurred (hard disk only)");	break;
    case 0xCC:	msg("Write fault occurred");				break;
	 case 0xE0:	msg("Status error");					break;
	 case 0xFF:	msg("Sense operation failed");				break;
  }
#ifdef USE_EXIT
  _exit(1);
#else
  return(EXIT_FAILURE);
#endif
}

/*
 Identify what kind of diskette is installed in the specified drive.
 Return the number of sectors per track assumed as follows:
 9	-	360 K and 720 K 5.25".
15	-	1.2 M HD	5.25".
18	-	1.44 M		3.5".
*/
int nsects(int (drive))
{
  static	int	nsect[] = {18, 15, 9};

  char	*buffer;
  int	i, status;
/*
 Read sector 1, head 0, track 0 to get the BIOS running.
*/
  buffer = (char *)malloc(SECTORSIZE);
  biosdisk(RESET, drive, 0, 0, 0, 0, buffer);
  status = biosdisk(READ, drive, 0, 10, 1, 1, buffer);
  if (status == 0x06)			/* Door signal change?	*/
  status = biosdisk(READ, drive, 0, 0, 1, 1, buffer);

  for (i=0; i < sizeof(nsect)/sizeof(int); ++i) {
    biosdisk(RESET, drive, 0, 0, 0, 0, buffer);
    status = biosdisk(READ, drive, 0, 0, nsect[i], 1, buffer);
    if (status == 0x06)
      status = biosdisk(READ, drive, 0, 0, nsect[i], 1, buffer);
      if (status == 0x00) break;
    }
	 if (i == sizeof(nsect)/sizeof(int)) {
		msg("Can't figure out how many sectors/track for this diskette.");
		return(-100);
	 }
	 free(buffer);
	 return(nsect[i]);
}

int main(int argc, char *argv[])
{
  int i;

  if (argc > 1)
  {
	  for (i = 0; i < argc; i++)
	  {
		  if ((strstr(argv[i], "/?") != 0) ||
				(strstr(argv[i], "-?") != 0) ||
            (strstr(argv[i], "--help") != 0))
		  {
			  printf("OSPlus Disk Imager\n");
			  printf("Version " DSKIMAGE_VERSION "\n");
			  printf("\n");
			  printf("Copyright (c) Owen Rudge 2000-2001.\n");
			  printf("Disk Writing Routines: Written by Mark Becker\n");
			  printf("Disk Reading Routines: (c) Hans Lermen 1995\n");
			  printf("\n");
			  printf("Web site:    http://www.osplus.co.uk/\n");
			  printf("Development: http://sourceforge.net/projects/osplus/\n");
			  printf("\n");
			  printf("Built on %s at %s.\n", __DATE__, __TIME__);
			  printf("\n");
			  printf("Press any key to exit.\n");
			  getch();
           exit(0);
		  }
	  }
  }

  TDiskWriter dw;

  OS_ChangeWindowTitle();
  dw.run();
  OS_RemoveWindowTitle();

  return(0);
}	/* end main */

TDiskWriter::TDiskWriter() :
	 TProgInit( &TDiskWriter::initStatusLine,
					&TDiskWriter::initMenuBar,
					&TDiskWriter::initDeskTop
				 )
{
	 demoDialogData = new DialogData;
	 strcpy(demoDialogData->DriveData, "A:");
    strcpy(demoDialogData->FileData, "");
}

TDiskWriter::~TDiskWriter()
{

	delete demoDialogData;

}

void TDiskWriter::handleEvent(TEvent& event)
{
	 TApplication::handleEvent(event);
	 if( event.what == evCommand )
		  {
		  switch( event.message.command )
				{
				case cmWrite:
					 newDialog();
					 break;
				case cmRead:
					 doRead();
                break;
				case cmDosShell:
					 dosShell();
					 break;
				case cmAbout:         // About OSPlus
					 aboutBox();
					 break;
				case cmAboutApp:      // About Disk Writer
					 aboutProg();
					 break;

				default:
					 return;
				}
		  clearEvent( event );            // clear event after handling
		  }
}

TMenuBar *TDiskWriter::initMenuBar( TRect r )
{
	 r.b.y = r.a.y + 1;    // set bottom line 1 line below top line
	 return new TMenuBar( r,
			 *new TSubMenu( "~ð~", kbAltSpace ) +
				  *new TMenuItem( "~A~bout OSPlus...", cmAbout, kbAltA ) +
				  newLine() +
				  *new TMenuItem( "~D~OS Shell", cmDosShell, kbAltD, hcNoContext, "Alt-D") +
		  *new TSubMenu( "~F~ile", kbAltF )+
				*new TMenuItem( "~R~ead Image from Disk...", cmRead, kbF2, hcNoContext, "F2") +
				*new TMenuItem( "~W~rite Image to Disk...", cmWrite, kbF3, hcNoContext, "F3") +
            newLine() +
				*new TMenuItem( "E~x~it", cmQuit, kbAltX, hcNoContext, "Alt-X" ) +
		  *new TSubMenu( "~H~elp", kbAltH ) +
				*new TMenuItem( "~A~bout Disk Imager...", cmAboutApp, kbAltA)
			  );
}

TStatusLine *TDiskWriter::initStatusLine( TRect r )
{
    r.a.y = r.b.y - 1;     // move top to 1 line above bottom
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
		  // set range of help contexts
				*new TStatusItem( 0, kbF10, cmMenu ) +
            // define an item
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            // and another one
            *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )
            // and another one
        );
}

void TDiskWriter::SetOKClicked(int clicked)
{
	 OKClicked = clicked;
}

void TDiskWriter::newDialog()
{
	 TWriteDialog *d = new TWriteDialog();
	 d->setData(demoDialogData);

	 OKPressed = 0;

	 deskTop->execView( d );
	 destroy(d);

	 if (OKPressed == 1)
		 DoWrite();
}

void TDiskWriter::doRead()
{
	 TReadDialog *d = new TReadDialog();
	 d->setData(demoDialogData);

	 ReadOKPressed = 0;

	 deskTop->execView( d );
	 destroy(d);

	 if (ReadOKPressed == 1)
		 RawReadMain(ReadFN, ReadDrive);
}

void DoWrite()
{
  char	*buffer, *pbuf, msg[200];
  int	 count, fdin, drive, head, track, status, spt, buflength, ns;
  long amountdone;
  double percentdone;

  ctrlbrk(handler);
  _fmode = O_BINARY;
  if ((fdin = open(fname, O_RDONLY)) <= 0) {
//	 perror(fname);
	 messageBox(mfInformation | mfOKButton, "%s\nFilename: %s", strerror(errno), fname);
//  exit(1);
    return;
  }

  drive = gdrive[0];
  drive = (islower(drive) ? toupper(drive) : drive) - 'A';
  messageBox(mfInformation	| mfOKButton, "Please insert a formatted diskette into drive %c: and click OK", drive + 'A');
//  while (bioskey(1) == 0) ;				/* Wait...	*/
//  if ((bioskey(0) & 0x7F) == 3) exit(1);		/* Check for ^C	*/
//  putchar('\n');
  done = FALSE;
/*
 * Determine number of sectors per track and allocate buffers.
 */

  QuickMessage *qm = new QuickMessage();
  TProgram::deskTop->insert( qm );

  spt = nsects(drive);

  if (spt == -100)
  {
	  TProgram::deskTop->remove( qm );
	  qm->destroy;

	  return;
  }

  buflength = spt * SECTORSIZE;
  buffer = (char *)malloc(buflength);

  sprintf(msg, "Sectors per track: %d", spt);
  qm->setProgress(msg);
/*
 * Start writing data to diskette until there is no more data to write.
 */
	head = track = 0;
	amountdone = 0;

	while ((count = read(fdin, buffer, buflength)) > 0 && !done) {
	  amountdone += buflength;

	  pbuf = buffer;
	  for (ns = 1; count > 0 && !done; ns+=3) {
		 sprintf(msg, "Track: %02d  Head: %2d Sector: %2d", track, head, ns);
		 qm->setProgress2(msg);

		 percentdone = (double)amountdone/(double)filelength(fdin);
		 sprintf(msg, "%ld bytes written out of %ld\nbytes", amountdone, filelength(fdin));
		 qm->setProgress3(msg);

		 qm->SetProgressBar((int)(percentdone*100));

		 status = biosdisk(WRITE, drive, head, track, ns, 3, pbuf);

		 if (status != 0)
		 {
			 Error(status);

			 TProgram::deskTop->remove( qm );
			 qm->destroy;
			 return;
		 }


		 count -= (3*SECTORSIZE);
		 pbuf  += (3*SECTORSIZE);
	  }
	  if ((head = (head + 1) & 1) == 0) ++track;
	}
	if (eof(fdin)) {
		TProgram::deskTop->remove( qm );
		qm->destroy;
	  biosdisk(2, drive, 0, 0, 1, 1, buffer);		/* Retract head	*/
	}
}

void TDiskWriter::dosShell()
{
	 suspend();
	 clrscr();
	 cout << "Type EXIT to return to Disk Imager";
	 system( getenv( "COMSPEC"));
	 resume();
	 redraw();
}

void TDiskWriter::aboutBox( void ) {
	 TDialog *aboutBox = new TAboutOSPlus; //Dialog(TRect(0, 0, 39, 13), "About Disk Writer");

/*	 TDialog *aboutBox = new TDialog(TRect(0, 0, 39, 13), "About OSPlus");

	 aboutBox->insert(
		new TStaticText(TRect(11, 2, 30, 10), // 9
		  "\003OSPlus\n"
		  "\003Version 1.1\n\n"
		  "\003Copyright (c) Owen Rudge 2000-2001\n"       // The \003 centers the line.
		  "\003All Rights Reserved\n"
		  )
		);

	 aboutBox->insert(
		new TButton(TRect(14, 10, 26, 12), " OK", cmOK, bfDefault)
		);*/

	 aboutBox->options |= ofCentered;

	 executeDialog(aboutBox);

}

void TDiskWriter::aboutProg( void ) {
	 TDialog *aboutProg = new TAboutDialog; //Dialog(TRect(0, 0, 39, 13), "About Disk Writer");

/*	 aboutProg->insert(
		new TStaticText(TRect(6, 2, 35, 10), // 9
		  "\003OSPlus Disk Writer\n"
		  "\003Version 1.0\n\n"       // These strings will be
		  "\003Copyright (c) Owen Rudge\n"       // The \003 centers the line.
		  "\0032000-2001. Based on RAWRITE by\n"
		  "\003Mark Becker\n"
		  )
		);

	 aboutProg->insert(
		new TButton(TRect(14, 10, 26, 12), " OK", cmOK, bfDefault)
		);

	 aboutProg->options |= ofCentered;
*/
	 executeDialog(aboutProg);
}

