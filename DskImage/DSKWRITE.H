/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.3                                   */
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

class TDiskWriter : public TApplication
{

public:
	 int OKClicked;

	 TDiskWriter();
	 ~TDiskWriter();
	 static TStatusLine *initStatusLine( TRect r );
	 static TMenuBar *initMenuBar( TRect r );
	 virtual void handleEvent( TEvent& event);
	 void newDialog();
	 void SetOKClicked(int clicked);
	 void dosShell();
    void doRead();

	 void aboutBox();
    void aboutProg();
};