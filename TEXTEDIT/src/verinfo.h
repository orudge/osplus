/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Version Information window                                */
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

#if !defined( __VERINFO_H )
#define __VERINFO_H

#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TButton
#define Uses_TStaticText

#if defined(__DJGPP__) || defined(__LINUX__) || defined(__WIN32__)
	#include <tv.h>
#else
	#include <tvision\tv.h>
#endif



class TVerInfoDlg : public TDialog
{

public:

    TVerInfoDlg( );
    TVerInfoDlg( StreamableInit ) :
           TDialog (streamableInit),
           TWindowInit(TVerInfoDlg::initFrame) {};
    virtual void handleEvent( TEvent& );
    virtual Boolean valid( ushort );

    TStaticText *Compiler;
    TStaticText *BuildDate;
    TStaticText *CompilerVer;
    TStaticText *BuildTime;
    TStaticText *TVVer;
    TStaticText *AllegroVer;
    TStaticText *OS;
    TStaticText *OSVer;


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

inline ipstream& operator >> ( ipstream& is, TVerInfoDlg& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TVerInfoDlg*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TVerInfoDlg& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TVerInfoDlg* cl )
    { return os << (TStreamable *)cl; }


#endif  // __VERINFO_H

