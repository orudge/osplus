/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.1                                   */
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

/* RAREAD Information
============================================================================
	LOADLIN v1.5 (C) 1995 Hans Lermen (lermen@elserv.ffm.fgan.de)
------------------------------------------------------------------------------

  NOTE: disk read code has some code stolen from RAWRITE.EXE 1.2
		  originally written by Mark Becker,
		  so, this part is (C) Mark Becker.
		  He does not mention any copyright restrictions in the
		  source, and as this source is distributed round over the
		  world on allmost any Linux FTP-site, I took it and used
		  it for my purpose.
		  However, I don't know if Mark Becker did put it under GPL.
============================================================================
*/
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
#include <stdlib.h>
#include <alloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <setjmp.h>
#include <dir.h>
#include <conio.h>

#include <bios.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>

#define space64k 0x10000

static void *DMAbuffer=0;

void RawReadMain(char *fn, char *drive);

class QuickMessage: public TWindow
{
	 TParamText *msg1;
	 TParamText *msg2;

public:
	 QuickMessage():
		  TWindow( TRect( 25,10,65,16 ), "Disk Imager", 0 ),
		  TWindowInit( TWindow::initFrame ) {

	 flags = 0; // no move, close, grow or zoom
	 options |= ofCentered;
	 palette = wpGrayWindow;
	 char temp[30];
	 sprintf(temp,"Please wait...");
	 msg1 = new TParamText( TRect( 2,2,38,3 ));
	 msg2 = new TParamText( TRect( 2,3,38,4 ));

	 insert(msg1);
	 insert(msg2);
  }
  virtual void handleEvent( TEvent &event ) {
	 TWindow::handleEvent( event );
  }
  void setProgress( char *__format, ...) {
	 msg1->setText(__format);
  }
  void setProgress2( char *__format, ...) {
	 msg2->setText(__format);
  }
};


#define MAXDMASIZE (18*2*512)
static char *__buf=0;

static void *malloc_DMAbuffer(unsigned short size)
{
  unsigned long l, boundary;
  void **p;
  char *p_;
  if (!__buf) __buf=(char *)malloc(MAXDMASIZE);
  p=(void **)__buf;
  l=((long)FP_SEG(p) << 4) + FP_OFF(p);
  boundary= (l+space64k-1) & (~(space64k-1));
  if ((l+size) > boundary) l=boundary;
  p_= (char *)(void *) (((l & ~15)<<12)+(l&15));
  return p_;
}

static void free_DMAbuffer(void **pp)
{
}


static done;
static void *oldint1e=0;

static cancel()
{
  if (done) return done;
  done=kbhit();
}


void free_dpt()
{
  if (oldint1e) {
    setvect(0x1e,(void interrupt (*) (...))oldint1e);
    oldint1e=0;
  }
}

void set_dpt()
{
  static unsigned char our_dpt[] ={
    0xdf,0x02,0x25,0x02,0x12,0x1b,0xff,0x54,0xf6,0x0f,0x08
  };
  if (oldint1e) free_dpt();
  oldint1e=(void *)getvect(0x1e);
  setvect(0x1e,(void interrupt (*) (...))&our_dpt);
}

#define FALSE	0
#define TRUE	(!FALSE)

#define SECTORSIZE	512

#define	RESET	0
#define	LAST	1
#define	READ	2
#define	WRITE	3
#define	VERIFY	4
#define	FORMAT	5

#define SECPERCALL -1 /* or 3 or 1 */
#define MAXRETRY 10

QuickMessage *qm;

extern void msg(char (*s));

/*
 Identify the error code with a real error message.
*/
static void Error(int status, int quit)
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
  if (quit) exit(1);
}

struct bootsector {
  char org1f1[0x1F1-0];
  unsigned char setup_sects;  /* no. of sectors of setupcode */
  unsigned short ro_flag;     /* =0: root file system should be mounted read-write
                                <>0: root file system should be mounted readonly */
  unsigned short kernel_size; /* size of kernel-part in the image-file
                                 (in 16 byte units, rounded up) */
  unsigned short swap_dev;    /* swap device */

  unsigned short ram_disk;    /* 01F8 size of ram-disk (in 1Kb units ) or ZERO */
  short vga_mode;             /* 01FA VGA-Mode */
  unsigned short root_dev;    /* 01FC Root Device (high=Major, low=minor) */
  unsigned short bootmagic;   /* 01FE Bootsector magic (0AA55h) */
} bootsec;

long image_filesize;
int  option_image;

/*
 Identify what kind of diskette is installed in the specified drive.
 Return the number of sectors per track assumed as follows:
 9	-	360 K and 720 K 5.25".
15	-	1.2 M HD	5.25".
18	-	1.44 M		3.5".
*/
static int nsects(int drive)
{
  static	int	nsect[] = {18, 15, 9};

  char	*buffer;
  int	i, status;
/*
 Read sector 1, head 0, track 0 to get the BIOS running.
*/
  DMAbuffer= malloc_DMAbuffer(512);
  buffer= (char *)DMAbuffer;
  biosdisk(RESET, drive, 0, 0, 0, 0, buffer);
  status = biosdisk(READ, drive, 0, 0, 1, 1, buffer);
  if (status == 0x06)			/* Door signal change?	*/
			 status = biosdisk(READ, drive, 0, 0, 1, 1, buffer);
  if (status) Error(status,1);
  memcpy(&bootsec, buffer, 512);

  for (i=0; i < sizeof(nsect)/sizeof(int); ++i) {
	 biosdisk(RESET, drive, 0, 0, 0, 0, buffer);
	 status = biosdisk(READ, drive, 0, 0, nsect[i], 1, buffer);
	 if (status == 0x06) status = biosdisk(READ, drive, 0, 0, nsect[i], 1, buffer);
	 if (status == 0x00) break;
  }
  if (i == sizeof(nsect)/sizeof(int))
	 msg("Can't figure out how many sectors/track for this diskette.");

  image_filesize = (long)nsect[i] * 512 * 2 * 80;

  char buf[100];

  sprintf(buf, "Reading %dKB from floppy to file...", image_filesize / 1024);
  qm->setProgress(buf);
  return(nsect[i]);
}

/*                    +------name of output file
                      |       +-- drive letter of floppy (e.g. 'A')
							 |       |            +-- format for printf like "Can't figure out how many sectors/track for this diskette."
							 |       |            |                                */
void disk_rawread(char	*fname, char drive,                  char *errio_ft, char *THS_ft, char *done_ft)
/*                                                                |           |            |
	 format for printf like "Disketten IO-Fehler, Status %2x\n" ---+           |            |
    format for printf like "Track: %02d  Head: %2d Sector: %2d\r" ------------+            |
    format for printf like "\nSchreiben auf Diskette beendet\n" ---------------------------+ */
{
  char	*buffer, *pbuf;
  int	 fdout, head, track, status, spt, buflength, ns;
  long int count;
  int ret,retry,retrycount,secpercall;
  if ((fdout = open(fname, O_RDWR | O_CREAT | O_TRUNC | O_BINARY)) == -1) {
	 messageBox(mfError | mfOKButton, "Can't create file %s.", fname);
	 return;
  }

  done = 0;
  drive = toupper(drive) -'A';

  qm = new QuickMessage();
  TProgram::deskTop->insert( qm );

/*
 * Determine number of sectors per track and allocate buffers.
 */
  set_dpt();
  spt = nsects(drive);
  free_dpt();

  char buf[100];
  sprintf(buf, "Reading %d sectors per track", spt);
  qm->setProgress(buf);

  buflength = spt * SECTORSIZE;
  free_DMAbuffer(&DMAbuffer);
  if (!spt) return;
  DMAbuffer= malloc_DMAbuffer(buflength);
  buffer = (char *)DMAbuffer;

  set_dpt();
  biosdisk(RESET, drive, 0, 0, 0, 0, buffer);

  if (SECPERCALL <0) secpercall=spt;
  else secpercall=SECPERCALL;
/*
 * Start reading data from diskette
 */
  head = track = 0;
  do {
    pbuf = buffer;
    count = buflength;
	 for (ns = 1; count > 0 && !cancel(); ns+=secpercall) {
		sprintf(buf, THS_ft, track, head, ns);
		qm->setProgress2(buf);
		retrycount=MAXRETRY;
		do {
		  status = biosdisk(READ, drive, head, track, ns, secpercall, pbuf);
		  switch (status) {
			 case 0x04:  /* Sector not found */
			 case 0x08:  /* DMA overrun */
			 case 0x10:  /* Bad CRC/ECC on disk read */
							 retry=1;
							 break;
			 case 0x11:  /* CRC/ECC corrected data error */
							 retry=0;
							 status=0;
							 break;
			 default:    retry=0;
		  }
		} while ( retry && (--retrycount >0) );

		if (status != 0) {
		  messageBox(mfError | mfOKButton, errio_ft,status);
		  free_DMAbuffer(&DMAbuffer);
		  close(fdout);
		  free_DMAbuffer(&DMAbuffer);
		  free_dpt();
		  return;
		}

		count -= (secpercall*SECTORSIZE);
		pbuf  += (secpercall*SECTORSIZE);
	 }
	 if ((head = (head + 1) & 1) == 0) ++track;

	 if (image_filesize < buflength) buflength = image_filesize;
	 if (buflength > 0) {
		count = write(fdout, buffer, buflength);
		if ((count == -1) || (count != buflength)) {
		  messageBox(mfError | mfOKButton, "Write error");
		  return;
		}
	 }
	 image_filesize -= buflength;
  } while ((image_filesize>0) && (!cancel()));
  messageBox(mfInformation | mfOKButton, done_ft);
  biosdisk(2, drive, 0, 0, 1, 1, buffer);		/* Retract head	*/
  free_DMAbuffer(&DMAbuffer);
  free_dpt();
  close(fdout);

  TProgram::deskTop->remove( qm );
  qm->destroy;
}

void RawReadMain(char *fn, char *drive)
{
  disk_rawread(fn,drive[0],
	 "Diskette I/O error, status %2x",
	 "Track: %02d  Head: %2d Sector: %2d",
	 "Finished reading the disk"
  );
}
