//-------------------------------------------------------
//                       aboutdlg.cpp
//-------------------------------------------------------

#define Uses_TEvent

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	#include <tv.h>
#else
	#include <tvision\tv.h>
#endif

#if !defined( __ABOUTDLG_H )
#include "aboutdlg.h"
#endif

TAboutDialog::TAboutDialog() :
       TDialog(TRect(15, 2, 65, 22), "About OSPlus Text Editor"),
       TWindowInit(TAboutDialog::initFrame)

{
 TView *control;
 options |= ofCenterX | ofCenterY;

 control = new TStaticText(TRect(2, 1, 48, 7), "OSPlus Text Editor\n"
     "Open Source\n"
     "\n"
     "Version 2.1, Build 20011224\n"
     "\n"
     "Copyright (c) Owen Rudge 2000-2001");
 insert(control);

 control = new TStaticText(TRect(2, 8, 48, 10), "This program is licensed und"
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

// From here to end of file may be removed if TAboutDialog will not be streamed.

TStreamableClass RAboutDialog( TAboutDialog::name,
                        TAboutDialog::build,
                        __DELTA(TAboutDialog)
                      );

__link(RAboutDialog)
__link(RButton)
__link(RStaticText)
