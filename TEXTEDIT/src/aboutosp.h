//-------------------------------------------------------
//
//   aboutosp.h: Header file for aboutosp.cpp
//
//-------------------------------------------------------

#if !defined( __ABOUTOSP_H )
#define __ABOUTOSP_H

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


class TAboutOSPlus : public TDialog
{

public:

    TAboutOSPlus( );
    TAboutOSPlus( StreamableInit ) :
           TDialog (streamableInit),
           TWindowInit(TAboutOSPlus::initFrame) {};
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

inline ipstream& operator >> ( ipstream& is, TAboutOSPlus& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TAboutOSPlus*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TAboutOSPlus& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TAboutOSPlus* cl )
    { return os << (TStreamable *)cl; }


#endif  // __ABOUTOSP_H

