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

class QuickMessage: public TWindow
{
	 TParamText *msg1;
	 TParamText *msg2;
	 TParamText *msg3;

	 TProgressBar *pbar;

public:
	 QuickMessage();

	 void setProgress( char *__format, ...);
	 void setProgress2( char *__format, ...);
	 void setProgress3( char *__format, ...);

	 void SetProgressBar(int);

private:
	 void QuickMessage::handleEvent(TEvent &event);
};
