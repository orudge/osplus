//-------------------------------------------------------
//
//   aboutdlg.h: Header file for aboutdlg.cpp
//
//-------------------------------------------------------

#if !defined( __ABOUTDLG_H )
#define __ABOUTDLG_H

#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TButton
#define Uses_TStaticText

#ifdef __DJGPP__
   #include <tv.h>
#else
   #include <tvision\tv.h>
#endif

class TAboutDialog : public TDialog
{

public:

    TAboutDialog( );
    TAboutDialog( StreamableInit ) :
           TDialog (streamableInit),
           TWindowInit(TAboutDialog::initFrame) {};
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

inline ipstream& operator >> ( ipstream& is, TAboutDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TAboutDialog*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, TAboutDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TAboutDialog* cl )
    { return os << (TStreamable *)cl; }


#endif  // __ABOUTDLG_H

