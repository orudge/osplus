/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.3                                   */
/*                                               */
/* Copyright (c) Owen Rudge 2000-2001            */
/* Disk Writing Routines: Written by Mark Becker */
/* Disk Reading Routines: (c) Hans Lermen 1995   */
/*************************************************/

#define Uses_TEvent
#define Uses_TFileDialog
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_MsgBox

#include <tvision\tv.h>
#include <string.h>

#if !defined( __READ_H )
#include "read.h"
#endif

#define cmBrowse   43

TInputLine *read_fn;
TInputLine *read_drive;

extern char ReadFN[150];
extern char ReadDrive[3];
extern int ReadOKPressed;

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

 read_fn = new TInputLine(TRect(14, 4, 43, 5), 126);
 insert(read_fn);

 control = new TStaticText(TRect(2, 6, 13, 7), "Drive:");
 insert(control);

 read_drive = new TInputLine(TRect(14, 6, 19, 7), 3);
 insert(read_drive);

 control = new TButton(TRect(14, 8, 24, 10), "~R~ead", cmOK, bfDefault);
 insert(control);

 control = new TButton(TRect(25, 8, 35, 10), "~C~ancel", cmCancel, bfNormal);
 insert(control);

 control = new TButton(TRect(31, 5, 43, 7), "~B~rowse...", cmBrowse, bfNormal);
 insert(control);

 selectNext(False);
}

void TReadDialog::handleEvent( TEvent& event)
{
	if (event.message.command == cmBrowse)
	{
		char fileName[200];
		strcpy(fileName, "");

		if( execDialog( new TFileDialog( "*.*", "Select File",
			"~N~ame", fdOKButton, 100 ), fileName) != cmCancel )
		{
			read_fn->setData(fileName); //strcpy(read_fn->data, fileName);
		}
	}
	else
		TDialog::handleEvent(event);
}

Boolean TReadDialog::valid(ushort command)
{
	Boolean rslt = TDialog::valid(command);
	char fileName[200];

	if (rslt && (command == cmOK))
	{
		ReadOKPressed = 1;
		strcpy(ReadFN, read_fn->data);
		strcpy(ReadDrive, read_drive->data);
	}
	return rslt;
}

const char * const TReadDialog::name = "TReadDialog";

void TReadDialog::write( opstream& os )
{
 TDialog::write( os );
 os << BrowseRead;
}

void *TReadDialog::read( ipstream& is )
{
 TDialog::read( is );
 is >> BrowseRead;
 return this;
}

TStreamable *TReadDialog::build()
{
    return new TReadDialog( streamableInit );
}

ushort TReadDialog::execDialog( TDialog *d, void *data )
{
	TView *p = TProgram::application->validView( d );

	if(p == 0)
		return cmCancel;
	else
	{
		if( data != 0 )
			p->setData( data );
		ushort result = TProgram::deskTop->execView( p );
		if( result != cmCancel && data != 0 )
			p->getData( data );
		TObject::destroy( p );
		return result;
	}
}
