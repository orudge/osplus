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

#if !defined( __WRITE_H )
#include "write.h"
#endif

extern char fname[150];
extern char gdrive[3];
extern int OKPressed;

TInputLine *write_fn;
TInputLine *write_drive;

TWriteDialog::TWriteDialog() :
		 TDialog(TRect(17, 6, 62, 17), "Write Image to Disk"),
		 TWindowInit(TWriteDialog::initFrame)

{
 TView *control;
 options |= ofCenterX | ofCenterY;

 control = new TStaticText(TRect(2, 2, 42, 3), "Please choose the image file "
	  "and drive.");
 insert(control);

 control = new TStaticText(TRect(2, 4, 13, 5), "Image File:");
 insert(control);

 write_fn = new TInputLine(TRect(14, 4, 43, 5), 126);
 insert(write_fn);

 control = new TStaticText(TRect(2, 6, 13, 7), "Drive:");
 insert(control);

 write_drive = new TInputLine(TRect(14, 6, 19, 7), 3);
 insert(write_drive);

 control = new TButton(TRect(14, 8, 24, 10), "~W~rite", cmOK, bfDefault);
 insert(control);

 control = new TButton(TRect(25, 8, 35, 10), "~C~ancel", cmCancel, bfNormal);
 insert(control);

 control = new TButton(TRect(31, 5, 43, 7), "~B~rowse...", cmBrowse, bfNormal);
 insert(control);

 selectNext(False);
}

void TWriteDialog::handleEvent( TEvent& event)
{
	if (event.message.command == cmBrowse)
	{
		char fileName[200];
		strcpy(fileName, "");

		if( execDialog( new TFileDialog( "*.*", "Select File",
			"~N~ame", fdOpenButton, 100 ), fileName) != cmCancel )
		{
			write_fn->setData(fileName); // strcpy(write_fn->text, fileName);
		}
	}
	else
		TDialog::handleEvent(event);
}

Boolean TWriteDialog::valid(ushort command)
{
	Boolean rslt = TDialog::valid(command);

	if (rslt && (command == cmOK))
	{
		OKPressed = 1;
		strcpy(fname, write_fn->data);
		strcpy(gdrive, write_drive->data);
	}
	return rslt;
}

const char * const TWriteDialog::name = "TWriteDialog";

void TWriteDialog::write( opstream& os )
{
 TDialog::write( os );
 os << BrowseWrite;
}

void *TWriteDialog::read( ipstream& is )
{
 TDialog::read( is );
 is >> BrowseWrite;
 return this;
}

TStreamable *TWriteDialog::build()
{
	 return new TWriteDialog( streamableInit );
}

ushort TWriteDialog::execDialog( TDialog *d, void *data )
{
	 TView *p = TProgram::application->validView( d );
    if( p == 0 )
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
