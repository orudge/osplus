//-------------------------------------------------------
//                       about.cpp
//-------------------------------------------------------

#define Uses_TEvent

#include <tvision\tv.h>

#if !defined( __ABOUT_H )
#include "about.h"
#endif

TAboutDialog::TAboutDialog() :
       TDialog(TRect(15, 0, 65, 22), "About OSPlus Disk Imager"),
       TWindowInit(TAboutDialog::initFrame)

{
 TView *control;
 options |= ofCenterX | ofCenterY;

 control = new TStaticText(TRect(2, 1, 48, 11), "OSPlus Disk Imager\n"
     "Open Source\n"
     "\n"
     "Version 1.3, Build 20010902\n"
     "\n"
     "Copyright (c) Owen Rudge 2000-2001\n"
     "Copyright (c) Hans Lerman 1995\n"
     "Copyright (c) Mark Becker 1991\n"
     "\n"
     "Contains code from RAWRITE 1.2 and RAWREAD");
 insert(control);

 control = new TStaticText(TRect(2, 12, 48, 14), "This program is licensed und"
     "er the GNU General Public License.");
 insert(control);

 control = new TStaticText(TRect(2, 15, 48, 18), "Please see the included docu"
     "ments for information on obtaining the source code and contact informat"
     "ion.");
 insert(control);

 control = new TButton(TRect(21, 19, 31, 21), "O~K~", cmOK, bfDefault);
 insert(control);

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
}

void *TAboutDialog::read( ipstream& is )
{
 TDialog::read( is );
 return this;
}

TStreamable *TAboutDialog::build()
{
    return new TAboutDialog( streamableInit );
}
