/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Converter window (header)                                 */
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
//
//   cnvinfo.h: Header file for cnvinfo.cpp
//
//-------------------------------------------------------

#if !defined( __CNVINFO_H )
#define __CNVINFO_H

#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TButton
#define Uses_TStaticText
#define Uses_TParamText
#define Uses_TListBox
#define Uses_TScrollBar

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
   #include <tv.h>
#else
   #include <tvision\tv.h>
#endif

struct ConverterDataRec  {
  TListBoxRec ConverterData;   //TListBox
  };


class TCnvInfoDialog : public TDialog
{

public:

    TCnvInfoDialog( );
    TCnvInfoDialog( StreamableInit ) :
           TDialog (streamableInit),
           TWindowInit(TCnvInfoDialog::initFrame) {};
    virtual void handleEvent( TEvent& );
    virtual Boolean valid( ushort );

    TButton *CloseButton;
    TListBox *ConverterList;
    TParamText *CnvInfoLbl;


private:

    virtual const char *streamableName() const
        { return name; }

protected:

    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TCnvInfoDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCnvInfoDialog*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TCnvInfoDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCnvInfoDialog* cl )
    { return os << (TStreamable *)cl; }


#endif  // __CNVINFO_H

