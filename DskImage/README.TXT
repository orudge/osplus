OSPLUS DISK IMAGER SOURCE CODE NOTES
====================================

This code requires Borland C++ to compile. I believe it will work
with Borland C++ 3.0 or higher, but I have compiled and tested it with
Borland C++ 4.51. Any version of BC above 4.0 should work. See the
/README.TXT for information on where to obtain Turbo Vision. The DJGPP
version requires DJGPP and RHIDE. I used 1.4 (30th Sep 1997). It also
requires Turbo Vision for DJGPP. You can download it from the address
mentioned in the /README.TXT file.

I have used the code in RAWRITE by Mark Becker. Thanks to him for
creating the code.

I have encoded the binary files here to help prevent CVS messing them
up. Use UUDECODE, WinZip or something that can handle UUE files to decode
them.

DSKIMDSW.UUE is the file DSKIMAGE.DSW, used by the Borland C++ 4.x IDE
DSKIMIDE.UUE is the file DSKIMAGE.IDE, used by the Borland C++ 4.x IDE
WRITEDLG.UUE is the file WRITE.DLG, used by DLGDSN, a Turbo Vision Dialog
             Designer. WRITE.CPP was produced by this.

Run DECODE to decode the files, and ENCODE to encode them again.

It is recommended you use the Borland C++ IDE for development, but a
MAKEFILE has been provided anyway. You must modify BCPATH= in the
makefile.

TO DO
=====

- Implement a more user-friendly environment - add Browse dialogs
- Incorporate RAREAD - disk reading option

Happy hacking!

Owen Rudge, 19th December 2000