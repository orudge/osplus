OSPlus Text Editor - Open Source Version
Version 2.1

Copyright (c) Owen Rudge 2000-2002
==================================

Thanks for downloading the source code to the OSPlus Text Editor! This has
evolved a fair bit since it was closed source, and now includes features such
as sound support and file format conversion.

There are a number of different builds for the Text Editor, which are:

- Borland C++ 4.x (real mode DOS)
- DJGPP (protected mode DOS)
- DJGPP without Allegro (protected mode DOS, no sound)
- Linux
- Linux without Allegro (no sound)
- Win32 (Borland C++Builder)
- Win32 (Microsoft Visual C++)

There has recently been some directory reorganisation and the new system is a
lot cleaner and more logical. You will find documentation on building the Text
Editor in the docs/build directory.

To build the Borland C++ (real-mode DOS), DJGPP and Win32 versions at the
same time, type the following at the command prompt:

   SET DJMAKE=d:\djgpp_path\BIN\MAKE.EXE
   SET BCMAKE=d:\bc_path\BIN\MAKE.EXE
   SET CBMAKE=d:\cb_path\BIN\MAKE.EXE
   SET VCMAKE=d:\vc_path\BIN\NMAKE.EXE
   BUILD

where "d:\djgpp_path" is the path to your DJGPP directory, "d:\bc_path" is the path
to your Borland C++ 4.x directory, "d:\cb_path" is the path to your Borland
C++Builder directory and "d:\vc_path" is the path to your Visual C++ directory.
Please note that your C++Builder BIN directory should be _before_ the Borland C++
4.x directory in the PATH.

DIREECTORY LAYOUT
=================

The following is a list of most of the files provided in this distribution:

BUILD.BAT               - Batch file to build all versions of the Text Editor except Linux
BUILDBC.BAT             - Batch file to build the real-mode DOS version of the Text Editor
BUILDCNV.BAT            - Batch file to build the DJGPP versions of the converters
BUILDDJ.BAT             - Batch file to build DJGPP version of Text Editor
BUILDDNA.BAT            - Batch file to build DJGPP version of Text Editor without Allegro
buildlnx                - The build script for Linux
BUILDVC.BAT             - Batch file to build Visual C++ version of Text Editor
BUILDWIN.BAT            - Batch file to build Borland C++Builder version of Text Editor
CLEANUP.BAT             - Batch file to clean up after a full build
COMPRESS.BAT            - Batch file to UPX compress all executables (except Linux)
djgpp.gdt               - RHIDE desktop file for DJGPP (without sound) version
djgpp.gpr               - RHIDE project file for DJGPP (without sound) version
djgpp.mak               - Makefile for DJGPP (without sound) version
djgppsnd.gdt            - RHIDE desktop file for DJGPP (with sound) version
djgppsnd.gpr            - RHIDE project file for DJGPP (with sound) version
djgppsnd.mak            - Makefile for DJGPP (with sound) version
ospedit.dsw             - Borland C++ IDE desktop file
ospedit.ide             - Borland C++ IDE project file
readme.txt              - Readme file (you're reading it now!)
realdos.mak             - Real-mode DOS (Borland C++) makefile
w32msvc.mak             - Win32 (MSVC) makefile
win32.mak               - Win32 (Borland C++) makefile

bin/                    - Binaries
docs/                   - Documentation
obj/                    - Object files

src/about.dlg           - About Text Editor dialog (requires Dialog Designer)
src/aboutdlg.cpp        - About Text Editor dialog source code
src/aboutdlg.h          - About Text Editor source header
src/aboutosp.cpp        - About OSPlus dialog source code
src/aboutosp.dlg        - About OSPlus dialog (for DLGDSN)
src/aboutosp.h          - About OSPlus dialog source code
src/calc.cpp            - Calculator accessory for BC (DOS) version
src/calc.h              - Calculator header for BC (DOS) version
src/convert.c           - New file conversion routines - INCOMPLETE!!!
src/convert.h           - Header for new file conversion routines
src/djgpp.c             - DJGPP/Linux (Allegro)-specific sound routines
src/gemsnd.c            - GEM Sound Driver bindings (BC version)
src/gemsnd.h            - GEM Sound Driver bindings header
src/libc.c              - libc emulation functions for Linux without Allegro
src/ospedit.h           - OSPlus Text Editor header
src/ospedit.rc          - Resource file for Win32 version
src/ospedit1.cpp        - Main source code
src/ospedit2.cpp        - Main source code
src/ospedit3.cpp        - Main source code
src/ospver.h            - OSPlus Text Editor version information header
src/realdos.c           - Borland C++-specific sound-related routines
src/sound.c             - High-level sound routines
src/sound.h             - High-level sound routines (header)
src/verinfo.cpp         - Version information dialog source code
src/verinfo.dlg         - Version information dialog
src/verinfo.h           - Version information dialog - header file
src/win32.c             - Win32-specific sound routines

src/convert/msconv.c    - Microsoft converter layer
src/convert/msconv.rc   - Microsoft converter layer resource script for Win32
src/convert/rtfactn.c   - RTF to TXT converter
src/convert/rtfdecl.h   - RTF to TXT converter
src/convert/rtferror.h  - RTF to TXT converter - error codes
src/convert/rtfreadr.c  - RTF to TXT converter
src/convert/rtftype.h   - RTF to TXT converter
src/convert/txtrtf.rc   - Resource file for Win32 version of txtrtf
src/convert/txtwrite.c  - Write to TXT converter (buggy)
src/convert/txtwrite.rc - Resource file for Win32 version of txtwrite
src/convert/write.h     - Write converter error codes

The Microsoft converter layer is currently not used from the Text Editor, but it
does work. As an example, rename bin/win32/msconv.cnv to msconv.exe, and type
the following at the command prompt:

   msconv "C:\Program Files\Common Files\Microsoft Shared\TextConv\mswrd832.cnv" 
      "C:\My Documents\<whatever>.doc" output.rtf

(it should be all on one line). Assuming you have the mswrd832.cnv converter installed
(you can choose a different one), <whatever>.doc should be converted to RTF format, and
output.rtf should be generated.

CONTACT INFORMATION
===================

Web site:         http://www.osplus.co.uk/
Open Source site: http://osplus.sourceforge.net/

If you have any questions or comments, please e-mail me at osplus@orudge.freeuk.com or
owen@osplus.co.uk. I've probably forgotton something important from the Readme, so if
you notice anything missing, e-mail me! ;-)

Happy hacking!

Owen Rudge, 14th October 2002
http://www.owenrudge.co.uk/

MSN Messenger: orudge@freeuk.com
ICQ: 125455765