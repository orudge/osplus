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

#include <stdio.h>

#define Uses_TWindow
#define Uses_TParamText
#define Uses_TRect
#define Uses_TEvent
#define Uses_TWindowInit

#include <tvision/tv.h>
#include "tprogbar.h"

#include "quickmsg.h"

QuickMessage::QuickMessage() :
	TWindow( TRect( 25,10,65,19 ), "Disk Imager", 0 ),
	TWindowInit( TWindow::initFrame )
{

	flags = 0; // no move, close, grow or zoom
	options |= ofCentered;
	palette = wpGrayWindow;
	char temp[30];
	sprintf(temp,"Please wait...");
	msg1 = new TParamText( TRect( 2,2,38,3 ));
	msg2 = new TParamText( TRect( 2,3,38,4 ));
	msg3 = new TParamText( TRect( 2,5,38,7 ));
	pbar = new TProgressBar(TRect(2,7,38,8), 100);

	insert(msg1);
	insert(msg2);
	insert(msg3);
   insert(pbar);
}

void QuickMessage::handleEvent(TEvent &event)
{
	TWindow::handleEvent( event );
}

void QuickMessage::setProgress(char *__format, ...)
{
	msg1->setText(__format);
}

void QuickMessage::setProgress2(char *__format, ...)
{
	msg2->setText(__format);
}

void QuickMessage::setProgress3(char *__format, ...)
{
	msg3->setText(__format);
}

void QuickMessage::SetProgressBar(int progress)
{
	pbar->update(progress);
}
