//-------------------------------------------------------
//
//   read.h: Header file for read.cpp
//
//-------------------------------------------------------

#if !defined( __READ_H )
#define __READ_H

#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TButton
#define Uses_TStaticText
#define Uses_TInputLine

#include <tvision\tv.h>


class TReadDialog : public TDialog
{

public:

    TReadDialog( );
    TReadDialog( StreamableInit ) :
           TDialog (streamableInit),
           TWindowInit(TReadDialog::initFrame) {};
    virtual void handleEvent( TEvent& );
    virtual Boolean valid( ushort );



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

inline ipstream& operator >> ( ipstream& is, TReadDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TReadDialog*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TReadDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TReadDialog* cl )
    { return os << (TStreamable *)cl; }


#endif  // __READ_H

