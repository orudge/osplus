/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2012. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* About dialog for OSPlus                                   */
/*************************************************************/

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

//-------------------------------------------------------
//                       aboutosp.cpp
//-------------------------------------------------------

#define Uses_TEvent

#include "inc_tv.h"
#include "aboutosp.h"

TAboutOSPlus::TAboutOSPlus() :
       TDialog(TRect(15, 1, 65, 21), "About OSPlus"),
       TWindowInit(TAboutOSPlus::initFrame)

{
 TView *control;
 options |= ofCenterX | ofCenterY;

 control = new TStaticText(TRect(2, 1, 48, 7), "OSPlus\n"
     "Open Source version\n"
     "\n"
     "Version 2.0\n"
     "\n"
     "Copyright (c) Owen Rudge 2000-2025");
 insert(control);

 control = new TStaticText(TRect(2, 8, 48, 10), "This program is licenced und"
     "er the GNU General Public License.");
 insert(control);

 control = new TStaticText(TRect(2, 11, 48, 14), "Please see the included docu"
     "ments for information on obtaining the source code and contact informat"
     "ion.");
 insert(control);

 control = new TButton(TRect(20, 17, 30, 19), "O~K~", cmOK, bfDefault);
 insert(control);

 control = new TStaticText(TRect(2, 15, 48, 16), "Web site: http://www.osplus."
     "co.uk/");
 insert(control);

 selectNext(False);
}

void TAboutOSPlus::handleEvent( TEvent& event)
{
    TDialog::handleEvent(event);
}

Boolean TAboutOSPlus::valid(ushort command)
{
   Boolean rslt = TDialog::valid(command);
   if (rslt && (command == cmOK))
     {
     }
   return rslt;
}

const char * const TAboutOSPlus::name = "TAboutOSPlus";

void TAboutOSPlus::write( opstream& os )
{
 TDialog::write( os );
}

void *TAboutOSPlus::read( ipstream& is )
{
 TDialog::read( is );
 return this;
}

TStreamable *TAboutOSPlus::build()
{
	 return new TAboutOSPlus( streamableInit );
}
