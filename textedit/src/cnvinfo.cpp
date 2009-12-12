/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Converter Information window                              */
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
 * 28/12/2004: Added new converter info dialog (orudge)
 */

#define Uses_TEvent
#define Uses_TCollection
#define Uses_TNSCollection
#define Uses_TStringCollection
#define Uses_TApplication
#define Uses_TParamText
#define Uses_MsgBox
#define Uses_stdio
#define Uses_stdarg
#define Uses_string
#define Uses_snprintf

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	#include <tv.h>
#else
	#include <tvision\tv.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef _USERENTRY
   #define _USERENTRY
#endif

#if !defined( __CNVINFO_H )
#include "cnvinfo.h"
#endif

#include "convert.h"

typedef struct {
   char title2[210];
   char title[210];
   char fn[200];
   char ext[50];
   char params[200];
} CONVERTER;

static char tmpbuf[512];

static CONVERTER *converter_list = NULL;
int num_converters = 0;

int _USERENTRY sort_function(const void *a, const void *b);

// For sorting the list
int _USERENTRY sort_function(const void *a, const void *b)
{
	return(strcmp( (char *)((CONVERTER *)a)->title, (char *)((CONVERTER *)b)->title));
}

TCnvInfoDialog::TCnvInfoDialog() :
       TDialog(TRect(10, 3, 70, 20), "Converter Information"),
       TWindowInit(TCnvInfoDialog::initFrame)

{
	TView *control;
	options |= ofCenterX | ofCenterY;

	control = new TStaticText(TRect(3, 2, 58, 5), "The following converters are "
	     "registered with OSPlus Text Editor:");
	insert(control);

	CloseButton = new TButton(TRect(25, 14, 35, 16), "~C~lose", cmOK, bfDefault);
	insert(CloseButton);

	control = new TScrollBar(TRect(57, 5, 58, 9));
	insert(control);

	int i;
	TStringCollection *l = new TStringCollection(100, 1);

	num_converters = get_number_of_converters();
	if (num_converters > 0)
	{
		if (converter_list == NULL)
		{
			converter_list = (CONVERTER *) malloc(sizeof(CONVERTER)*num_converters);

			if (!converter_list)
			{
				messageBox(mfError | mfOKButton, "Unable to allocate buffer %ld bytes long for converter list.", sizeof(CONVERTER)*num_converters);
				return;
			}
		}
		else
			realloc(converter_list, sizeof(CONVERTER)*num_converters);
     
 		for (i = 0; i < num_converters; i++)
			get_converter_info(i, converter_list[i].fn, converter_list[i].ext, converter_list[i].params, converter_list[i].title);

		qsort((void *)converter_list, num_converters, sizeof(converter_list[0]), sort_function);

 		for (i = 0; i < num_converters; i++)
		{
			sprintf(converter_list[i].title2, "%02d: %s", i+1, converter_list[i].title);
			l->insert(converter_list[i].title2);
		}
	}

	CnvInfoLbl = new TParamText(TRect(3, 10, 58, 14));

	if ((CnvInfoLbl) && (converter_list))
	{
		sprintf(tmpbuf, "Filename:   %s\nExtension:  %s\nParameters: %s", converter_list[0].fn, converter_list[0].ext, converter_list[0].params);
		CnvInfoLbl->setText(tmpbuf);
	}

	ConverterList = new TListBox(TRect(3, 5, 57, 9), 1, (TScrollBar*)control);
	insert(ConverterList);

	ConverterList->newList(l);

	insert(CnvInfoLbl);
	selectNext(False);
}

void TCnvInfoDialog::handleEvent( TEvent& event)
{
	TDialog::handleEvent(event);

	int i = ConverterList->focused;
	static int last_focused = -1;

	if (last_focused != i+1)
	{
		last_focused = i+1;

		if ((i >= 0) && (i <= num_converters))
		{
			if ((CnvInfoLbl) && (converter_list))
			{
				sprintf(tmpbuf, "Filename:   %s\nExtension:  %s\nParameters: %s", converter_list[i].fn, converter_list[i].ext, converter_list[i].params);
				CnvInfoLbl->setText(tmpbuf);
			}
		}
	}
}

Boolean TCnvInfoDialog::valid(ushort command)
{
   Boolean rslt = TDialog::valid(command);
   if (rslt && (command == cmOK))
     {
     }
   return rslt;
}

const char * const TCnvInfoDialog::name = "TCnvInfoDialog";

void TCnvInfoDialog::write( opstream& os )
{
 TDialog::write( os );
 os << CloseButton << ConverterList << CnvInfoLbl;
}

void *TCnvInfoDialog::read( ipstream& is )
{
 TDialog::read( is );
 is >> CloseButton >> ConverterList; // >> CnvInfo;
 return this;
}

TStreamable *TCnvInfoDialog::build()
{
    return new TCnvInfoDialog( streamableInit );
}
