/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.3                                   */
/*                                               */
/* Copyright (c) Owen Rudge 2000-2001            */
/* Disk Writing Routines: Written by Mark Becker */
/* Disk Reading Routines: (c) Hans Lermen 1995   */
/*************************************************/

#if !defined( __WRITE_H )
#define __WRITE_H

#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TButton
#define Uses_TStaticText
#define Uses_TInputLine

#include <tvision\tv.h>


struct dataRec  {
  char ImageFile[126];   //TInputLine
  char Drive[3];   //TInputLine
  };


class TWriteDialog : public TDialog
{

public:

    TWriteDialog( );
    TWriteDialog( StreamableInit ) :
           TDialog (streamableInit),
           TWindowInit(TWriteDialog::initFrame) {};
    virtual void handleEvent( TEvent& );
    virtual Boolean valid( ushort );

    TButton *BrowseWrite;

    ushort execDialog( TDialog *d, void *data );

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

inline ipstream& operator >> ( ipstream& is, TWriteDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TWriteDialog*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TWriteDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TWriteDialog* cl )
    { return os << (TStreamable *)cl; }

#define cmBrowse   20

#endif  // __WRITE_H

