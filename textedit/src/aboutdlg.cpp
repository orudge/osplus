/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2012. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* About dialog for Text Editor                              */
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
//                       aboutdlg.cpp
//-------------------------------------------------------

/* Revision History:
 *
 * 05/01/2002: Cleaned up indentation (orudge)
 * 27/12/2004: Got version number from ospver.h (orudge)
 */

#define Uses_TEvent

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
   #include <tv.h>
#else
   #include <tvision\tv.h>
#endif

#include <stdio.h>

#if !defined( __ABOUTDLG_H )
   #include "aboutdlg.h"
#endif

#include "ospver.h"

TAboutDialog::TAboutDialog() :
       TDialog(TRect(15, 2, 65, 22), "About OSPlus Text Editor"),
       TWindowInit(TAboutDialog::initFrame)

{
   TView *control;
   char tmpbuf[200];

   options |= ofCenterX | ofCenterY;

   sprintf(tmpbuf, "OSPlus Text Editor\n"
       "Open Source\n"
       "\n"
       "Version %s, Build %s\n"
       "\n"
       "Copyright (c) Owen Rudge 2000-2012", OSP_TXT_VERSION_STR, OSP_TXT_BUILD_DATE);

   control = new TStaticText(TRect(2, 1, 48, 7), tmpbuf);
   insert(control);

   control = new TStaticText(TRect(2, 8, 48, 10), "This program is licenced und"
       "er the GNU General Public License.");
   insert(control);

   control = new TStaticText(TRect(2, 11, 48, 14), "Please see the included docu"
       "ments for information on obtaining the source code and contact informat"
       "ion.");
   insert(control);

   control = new TButton(TRect(21, 17, 31, 19), "O~K~", cmOK, bfDefault);
   insert(control);

   build_version = new TStaticText(TRect(2, 15, 49, 16), "Version unknown");
   insert(build_version);

   selectNext(False);
}

void TAboutDialog::handleEvent( TEvent& event)
{
   TDialog::handleEvent(event);
}

Boolean TAboutDialog::valid(ushort command)
{
   Boolean rslt = TDialog::valid(command);
   if (rslt && (command == cmOK))
   {
   }
   return rslt;
}

const char * const TAboutDialog::name = "TAboutDialog";

void TAboutDialog::write( opstream& os )
{
   TDialog::write( os );
   os << build_version;
}

void *TAboutDialog::read( ipstream& is )
{
   TDialog::read( is );
   is >> build_version;
   return this;
}

TStreamable *TAboutDialog::build()
{
   return new TAboutDialog( streamableInit );
}
