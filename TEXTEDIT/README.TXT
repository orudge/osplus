OSPLUS TEXT EDITOR SOURCE CODE NOTES
====================================

This code requires Borland C++ or DJGPP to compile. The Linux version
requires GCC. I believe it will work with Borland C++ 3.0 or higher,
but I have compiled and tested it with Borland C++ 4.51. Any version
of BC above 4.0 should work. See the /README.TXT for information on
where to obtain Turbo Vision. The DJGPP version requires DJGPP and
RHIDE. I used 1.4 (30th Sep 1997). It also requires Turbo Vision for
DJGPP. You can download it from the address mentioned in the
/README.TXT file.

If you want to include sound support in the DJGPP version, make sure that
DJGPP_NO_SOUND_SUPPORT is not defined. If you don't want sound support,
#define DJGPP_NO_SOUND_SUPPORT. Please remember to remove the ALLEG library
from the Libraries dialog. You need a copy of Allegro to compile with
sound support - download the "original" library from www.talula.demon.co.uk,
and the latest WIP from http://alleg.sourceforge.net/.

There is a bug where if you open an RTF file or DOS prompt, the sound may
stop playing and you have to restart the Editor. This is due to the system
switching into real mode to perform the conversion or the DOS prompt.

To compile the converters in DJGPP, run BUILDCNV.BAT.

Also, you may find it useful to compress all the resulting EXE files: without
Allegro support, this is about 1.5mb, with Allegro, this can be over 2mb!
(This is without the -s parameter, which strips debugging info.) I'd
recommend UPX. One last thing: *please* don't upload any uncompressed
copies of the Text Editor. (The real-mode, Borland C++, version is about
267kb uncompressed, or 97kb compressed).

The OSPlus Text Editor also runs under Win32. It requires the TV32.LIB,
which can be downloaded from Borland (see the \README.TXT file). On my
system, it isn't very good in Windows, as bits of the screen are left
behind, the interface is slow, it doesn't always paint properly and so
on. It works fine in real DOS mode with the Borland 32RTM extender.

I have used the Windows multimedia functions for WAV support in Win32
(WINMM.DLL - sndPlaySound and so on). If you try to run the program from
DOS, you'll recieve messages about "missing WINMM.DLL in imports" or
something similar. The Win32 version requires Borland C++.

This code also runs under Linux. Sound support is available too, via
Allegro again. Run the ./build script from the console to build four
programs - ospedit, the editor with sound support, ospedtns - the
Editor without sound support, txtwrite.cnv - the Write converter, and
txtrtf.cnv - the RTF converter.

I have encoded the binary files here to help prevent CVS messing them
up. Use UUDECODE, WinZip or something that can handle UUE files to decode
them.

OSPEDDSW.UUE is the file OSPEDIT.DSW, used by the Borland C++ 4.x IDE
OSPEDIDE.UUE is the file OSPEDIT.IDE, used by the Borland C++ 4.x IDE
OSP32DSW.UUE is the file OSPEDT32.DSW, used by the Borland C++ 4.x IDE
OSP32IDE.UUE is the file OSPEDT32.IDE, used by the Borland C++ 4.x IDE
TEXT_GDT.UUE is the file TEXTEDIT.GDT, which is used by RHIDE
TEXT_GPR.UUE is the file TEXTEDIT.GPR, which is used by RHIDE
ABOUTDLG.UUE is the file ABOUT.DLG, which is used by the Dialog Designer

Run DECODE to decode the files, and ENCODE to encode them again.

A makefile has been provided for Borland C++. You must modify BCPATH=
in the makefile to specify your Borland C++ path.

A makefile has been provided for DJGPP too. Please run BUILD.BAT to
create both versions of the Editor. To only create the Borland C++
version, type BUILD BCC. To only build the DJGPP version, type
BUILD DJGPP. To disable Allegro support in the DJGPP version, edit
TEXTEDIT.MAK. There are instructions inside the makefile for doing
this. Please note that unless you edit the batch file, the Borland
MAKE *must* be first in the path.

The sound support in the Borland C++ version uses Heinz Rath's sound
drivers. You can download them at www.geocities.com/heinz_rath/.

Happy hacking!

Owen Rudge, 26th December 2000