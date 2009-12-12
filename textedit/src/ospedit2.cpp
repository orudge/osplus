/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Dialog routines, including Find and Replace               */
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

/* Revision History:
 *
 * 05/01/2002: Cleaned up indentation, etc (orudge)
 */

#define Uses_TDialog
#define Uses_TDeskTop
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TObject
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_THistory
#define Uses_TRect
#define Uses_TCheckBoxes
#define Uses_TButton
#define Uses_TButton
#define Uses_MsgBox
#define Uses_TSItem
#define Uses_TEditor

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
   #include <tv.h>
#else
   #include <tvision\tv.h>
   #include <strstrea.h>
#endif

#include "ospedit.h"

#include <stdlib.h>
#include <stdarg.h>

#if __GNUC__ >= 3
   #include <iomanip>
#else
   #include <iomanip.h>
#endif

ushort execDialog( TDialog *d, void *data )
{
   TView *p = TProgram::application->validView(d);
   if (p == 0)
      return cmCancel;
   else
   {
      if(data != 0)
         p->setData(data);

      ushort result = TProgram::deskTop->execView(p);

      if (result != cmCancel && data != 0)
         p->getData(data);

      TObject::destroy(p);
      return result;
   }
}

TDialog *createFindDialog()
{
   TDialog *d = new TDialog( TRect( 0, 0, 38, 12 ), "Find" );

   d->options |= ofCentered;

   TInputLine *control = new TInputLine(TRect(3, 3, 32, 4), 80 );
   d->insert(control);
   d->insert(new TLabel(TRect(2, 2, 15, 3), "~T~ext to find", control));
   d->insert(new THistory(TRect(32, 3, 35, 4), control, 10));

   d->insert(new TCheckBoxes(TRect(3, 5, 35, 7),
       new TSItem("~C~ase sensitive",
       new TSItem("~W~hole words only", 0))));

   d->insert(
       new TButton(TRect(14, 9, 24, 11), "O~K~", cmOK, bfDefault));
   d->insert(
       new TButton(TRect(26, 9, 36, 11), "Cancel", cmCancel, bfNormal));

   d->selectNext( False );
   return d;
}

TDialog *createReplaceDialog()
{
   TDialog *d = new TDialog( TRect( 0, 0, 40, 16 ), "Replace" );

   d->options |= ofCentered;

   TInputLine *control = new TInputLine(TRect(3, 3, 34, 4), 80);
   d->insert(control);
   d->insert(new TLabel(TRect(2, 2, 15, 3), "~T~ext to find", control));
   d->insert(new THistory(TRect(34, 3, 37, 4), control, 10));

   control = new TInputLine(TRect( 3, 6, 34, 7), 80);
   d->insert(control);
   d->insert(new TLabel(TRect(2, 5, 12, 6), "~N~ew text", control));
   d->insert(new THistory(TRect(34, 6, 37, 7), control, 11));

   d->insert(new TCheckBoxes(TRect(3, 8, 37, 12),
       new TSItem("~C~ase sensitive",
       new TSItem("~W~hole words only",
       new TSItem("~P~rompt on replace",
       new TSItem("~R~eplace all", 0))))));

   d->insert(new TButton(TRect(17, 13, 27, 15), "O~K~", cmOK, bfDefault));
   d->insert(new TButton(TRect(28, 13, 38, 15), "Cancel", cmCancel, bfNormal));

   d->selectNext(False);

   return d;
}

