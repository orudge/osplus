//-------------------------------------------------------
//
//   verinfo.h: Header file for verinfo.cpp
//
//-------------------------------------------------------

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

