//-------------------------------------------------------
//                       read.cpp
//-------------------------------------------------------

#define Uses_TEvent

#include <tvision\tv.h>
#include <string.h>

#if !defined( __READ_H )
#include "read.h"
#endif

extern char ReadFN[150];
extern char ReadDrive[3];
extern int ReadOKPressed;

TInputLine *fn;
TInputLine *drive;

TReadDialog::TReadDialog() :
		 TDialog(TRect(17, 6, 62, 17), "Read Image from Disk"),
		 TWindowInit(TReadDialog::initFrame)

{
 TView *control;
 options |= ofCenterX | ofCenterY;

 control = new TStaticText(TRect(2, 2, 42, 3), "Please choose the image file "
	  "and drive.");
 insert(control);

 control = new TStaticText(TRect(2, 4, 13, 5), "Image File:");
 insert(control);

 fn = new TInputLine(TRect(14, 4, 43, 5), 126);
 insert(fn);

 control = new TStaticText(TRect(2, 6, 13, 7), "Drive:");
 insert(control);

 drive = new TInputLine(TRect(14, 6, 19, 7), 3);
 insert(drive);

 control = new TButton(TRect(14, 8, 24, 10), "~R~ead", cmOK, bfDefault);
 insert(control);

 control = new TButton(TRect(25, 8, 35, 10), "~C~ancel", cmOK, bfNormal);
 insert(control);

 selectNext(False);
}

void TReadDialog::handleEvent( TEvent& event)
{
    TDialog::handleEvent(event);
}

Boolean TReadDialog::valid(ushort command)
{
   Boolean rslt = TDialog::valid(command);
	if (rslt && (command == cmOK))
	{
		ReadOKPressed = 1;
		strcpy(ReadFN, fn->data);
		strcpy(ReadDrive, drive->data);
	}
	return rslt;
}

const char * const TReadDialog::name = "TReadDialog";

void TReadDialog::write( opstream& os )
{
 TDialog::write( os );
}

void *TReadDialog::read( ipstream& is )
{
 TDialog::read( is );
 return this;
}

TStreamable *TReadDialog::build()
{
    return new TReadDialog( streamableInit );
}

// From here to end of file may be removed if TReadDialog will not be streamed.

TStreamableClass RReadDialog( TReadDialog::name,
                        TReadDialog::build,
                        __DELTA(TReadDialog)
                      );

__link(RButton)
__link(RInputLine)
__link(RReadDialog)
__link(RStaticText)

