//-------------------------------------------------------
//                       aboutosp.cpp
//-------------------------------------------------------

#define Uses_TEvent

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	#include <tv.h>
#else
	#include <tvision\tv.h>
#endif

#if !defined( __ABOUTOSP_H )
#include "aboutosp.h"
#endif

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
     "Copyright (c) Owen Rudge 2000-2004 ");
 insert(control);

 control = new TStaticText(TRect(2, 8, 48, 10), "This program is licensed und"
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
