/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.0                                   */
/*                                               */
/* Copyright (c) Owen Rudge 2000                 */
/* Disk Imaging Routines: Written by Mark Becker */
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

#define Uses_TApplication
#define Uses_MsgBox
#define Uses_TEvent

#include <tvision\tv.h>
#include <string.h>

#include "dskwrite.h"

#if !defined( __WRITE_H )
#include "write.h"
#endif

extern int OKPressed;
extern char fname[128];
extern char gdrive[3];

TInputLine *fn;
TInputLine *drive;

TWriteDialog::TWriteDialog() :
		 TDialog(TRect(17, 6, 62, 17), "Write Image to Disk"),
		 TWindowInit(TWriteDialog::initFrame)

{
 TView *control;

 options |= ofCenterX | ofCenterY;

 control = new TStaticText(TRect(2, 2, 42, 3), "Please choose the image file "
     "and drive.");
 insert(control);

 control = new TStaticText(TRect(2, 4, 13, 5), "Image File:");
 insert(control);

 fn = new TInputLine(TRect(14, 4, 43, 5), 126);
 insert(fn);

 control = new TStaticText(TRect(2, 6, 13, 7), "Drive:");
 insert(control);

 drive = new TInputLine(TRect(14, 6, 19, 7), 3);
 insert(drive);

 control = new TButton(TRect(14, 8, 24, 10), "~W~rite", cmOK, bfDefault);
 insert(control);

 control = new TButton(TRect(25, 8, 35, 10), "~C~ancel", cmOK, bfNormal);
 insert(control);

 selectNext(False);
}

void TWriteDialog::handleEvent( TEvent& event)
{
    TDialog::handleEvent(event);
}

Boolean TWriteDialog::valid(ushort command)
{
	Boolean rslt = TDialog::valid(command);
	if (rslt && (command == cmOK))
	{
		OKPressed = 1;
		strcpy(fname, fn->data);
		strcpy(gdrive, drive->data);
	}

	return rslt;
}

const char * const TWriteDialog::name = "TWriteDialog";

void TWriteDialog::write( opstream& os )
{
 TDialog::write( os );
}

void *TWriteDialog::read( ipstream& is )
{
 TDialog::read( is );
 return this;
}

TStreamable *TWriteDialog::build()
{
	 return new TWriteDialog( streamableInit );
}
