//-------------------------------------------------------
//                       verinfo.cpp
//-------------------------------------------------------

#define Uses_TEvent

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
   #include <tv.h>
#else
   #include <tvision\tv.h>
#endif

#include <stdio.h>

#if !defined( __VERINFO_H )
   #include "verinfo.h"
#endif

#define VERSION_INFORMATION_ONLY

#include "ospedit.h"

TVerInfoDlg::TVerInfoDlg() :
		 TDialog(TRect(15, 2, 65, 21), "Version Information"),
		 TWindowInit(TVerInfoDlg::initFrame)

{
 TView *control;
 char buf[30];

 options |= ofCenterX | ofCenterY;

 sprintf(buf, "OSPlus Text Editor %s", OSP_TXT_VERSION_STR);
 control = new TStaticText(TRect(2, 2, 25, 3), buf);
 insert(control);

 control = new TStaticText(TRect(2, 4, 24, 5), "Built with:");
 insert(control);

 Compiler = new TStaticText(TRect(25, 4, 48, 5), OSP_COMPILER);
 insert(Compiler);

 control = new TStaticText(TRect(2, 7, 24, 8), "Date:");
 insert(control);

 BuildDate = new TStaticText(TRect(25, 7, 48, 8), __DATE__);
 insert(BuildDate);

 control = new TStaticText(TRect(2, 8, 24, 9), "Time:");
 insert(control);

 control = new TStaticText(TRect(2, 5, 24, 6), "Version:");
 insert(control);

 CompilerVer = new TStaticText(TRect(25, 5, 48, 6), OSP_COMPILER_VER);
 insert(CompilerVer);

 BuildTime = new TStaticText(TRect(25, 8, 48, 9), __TIME__);
 insert(BuildTime);

 control = new TStaticText(TRect(2, 10, 24, 11), "Turbo Vision version:");
 insert(control);

 TVVer = new TStaticText(TRect(25, 10, 48, 11), OSP_TV_VER);
 insert(TVVer);

 control = new TStaticText(TRect(2, 11, 24, 12), "Allegro version:");
 insert(control);

 AllegroVer = new TStaticText(TRect(25, 11, 48, 12), OSP_ALLEG_VER);
 insert(AllegroVer);

 control = new TStaticText(TRect(2, 13, 24, 14), "Operating System:");
 insert(control);

 OS = new TStaticText(TRect(25, 13, 48, 14), OSP_OS);
 insert(OS);

 control = new TStaticText(TRect(2, 14, 24, 15), "OS Version:");
 insert(control);

 OSVer = new TStaticText(TRect(25, 14, 48, 15), OSP_OS_VER);
 insert(OSVer);

 control = new TButton(TRect(19, 16, 29, 18), "O~K~", cmOK, bfDefault);
 insert(control);

 selectNext(False);
}

void TVerInfoDlg::handleEvent( TEvent& event)
{
    TDialog::handleEvent(event);
}

Boolean TVerInfoDlg::valid(ushort command)
{
   Boolean rslt = TDialog::valid(command);
   if (rslt && (command == cmOK))
     {
     }
   return rslt;
}

const char * const TVerInfoDlg::name = "TVerInfoDlg";

void TVerInfoDlg::write( opstream& os )
{
 TDialog::write( os );
 os << Compiler << BuildDate << CompilerVer << BuildTime << TVVer << AllegroVer << OS << OSVer;
}

void *TVerInfoDlg::read( ipstream& is )
{
 TDialog::read( is );
 is >> Compiler >> BuildDate >> CompilerVer >> BuildTime >> TVVer >> AllegroVer >> OS >> OSVer;
 return this;
}

TStreamable *TVerInfoDlg::build()
{
    return new TVerInfoDlg( streamableInit );
}

// From here to end of file may be removed if TVerInfoDlg will not be streamed.

TStreamableClass RVerInfoDlg( TVerInfoDlg::name,
                        TVerInfoDlg::build,
                        __DELTA(TVerInfoDlg)
                      );

__link(RButton)
__link(RStaticText)
__link(RVerInfoDlg)

