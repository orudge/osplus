OSPlus Text Editor - Open Source Version
Version 2.0

Copyright (c) Owen Rudge 2000-2001
========================================

Thanks for downloading the source code to the OSPlus Text Editor! This has
evolved a fair bit since it was closed source, and now includes features such
as sound support.

******************************** IMPORTANT: ********************************
While developing the Text Editor, I changed "TEXTEDIT.MAK" to "textedit.mak"
(or vice versa, I can't remember), and now there are two files in the CVS
repository and I can't delete the other one! The same goes for BUILD. So,
I've created a few files called !EXTEDIT.MAK and !UILD, and a file called
FIXCVS.BAT, which will put things right. * PLEASE RUN FIXCVS RIGHT AWAY!!! * 
****************************************************************************

There are a few different builds for the Text Editor, which are:

- Borland C++ 4.x (real mode DOS)
- DJGPP (protected mode DOS)
- DJGPP without Allegro (protected mode DOS, no sound)
- Linux
- Linux without Allegro (no sound)
- Win32 (this port is currently very unstable. It has been "scrapped" until
  the new version of Turbo Vision compatible with Win32 is available)

To build OSPlus Text Editor with Borland C++, you need:

- Borland C++ 4.0 or higher (may work with 3.x - untested)

- Borland PowerPack for DOS. Alternatively, download Turbo Vision from
  Borland's web site at www.borland.com.

- Dialog Designer, available from
  www.simtel.net/simtel.net/msdos/turbovis.html, to modify the About dialogs

- Heinz Rath's sound drivers available from www.geocities.com/heinz_rath/ if
  you want to be able to play WAV files from the Text Editor. These are not
  required for building.

Type SET BCPATH=C:\bc_directory\BIN\MAKE.EXE then BUILDBC to build this
version. It is recommended that you use the IDE to build this version, as the
executable produced is much smaller. (The Generate Makefile command must have
missed some switches or something).

To build the DJGPP version, you need:

- DJGPP 2.03 or better

- Turbo Vision, available from www.geocities.com/SiliconValley/Vista/6552/

- Allegro 3.12 or better, available from www.talula.demon.co.uk (latest WIP
  recommended, available from http://alleg.sourceforge.net/). If you don't
  want sound support, Allegro is not required. See below for more details.

- Dialog Designer (see above)

Type SET DJPATH=C:\djgpp_dir\BIN\MAKE.EXE then BUILDDJ to build this version.
To build the version without Allegro, set DJPATH as appropriate then type
BUILDDNA.

To build all these different versions at once, set BCPATH and DJPATH, then
type BUILD. The Borland C++ converters will have .CNV extensions, while the
DJGPP ones will have .EXE extensions.

To build the Linux version, you need:

- gcc (I don't know what version, but a new one should do fine)

- Turbo Vision, see the DJGPP version for details

- Allegro 3.9.36 WIP or better. Available from http://alleg.sourceforge.net/.
  Earlier WIPs should work, but 3.12 will not.

- A DOS emulator such as dosemu or a DOS machine to run Dialog Designer if
  you wish.

To build the Linux version, run the build script.

DIREECTORY LAYOUT
=================

The following is a list of files in this directory:

ABOUT.DLG    - About Text Editor dialog (requires Dialog Designer)
ABOUTDLG.CPP - About Text Editor dialog source code
ABOUTDLG.H   - About Text Editor source header
ABOUTOSP.CPP - About OSPlus dialog source code
ABOUTOSP.DLG - About OSPlus dialog (for DLGDSN)
ABOUTOSP.H   - About OSPlus dialog source code
AUTHORS.TXT  - People who have contributed to Text Editor
build        - The build script for Linux
BUILD.BAT    - Batch file to build all versions of DOS-based Text Editor
BUILDBC.BAT  - Borland C++ build file
BUILDCNV.BAT - Batch file to build the DJGPP versions of the converters
BUILDDJ.BAT  - Batch file to build DJGPP version of Text Editor
BUILDDNA.BAT - Batch file to build DJGPP version of Text Editor without Alleg.
CALC.CPP     - Calculator accessory for BC version
CALC.H       - Calculator header for BC version
CHANGES.TXT  - List of changes
CLEANUP.BAT  - Batch file to remove *.o, *.obj, *.bak, *.csm, BCCDOS.CFG
COMPRESS.BAT - Batch file to UPX compress all executables
DJGPP.C      - DJGPP/Linux (Allegro)-specific sound routines
GEMSND.C     - GEM Sound Driver bindings (BC version)
GEMSND.H     - GEM Sound Driver bindings header
LIBC.C       - libc emulation functions for Linux without Allegro
MAKEFILE     - Borland C++ makefile
OSPEDIT.DSW  - Borland C++ IDE desktop file
OSPEDIT.H    - OSPlus Editor header
OSPEDIT.IDE  - BC IDE project file
OSPEDIT1.CPP - Main source code
OSPEDIT2.CPP - Main source code
OSPEDIT3.CPP - Main source code
OSPEDT32.DEF - DEF file for Win32 version
README.TXT   - Readme file (you're reading it now!)
REALDOS.C    - Borland C++-specific sound-related routines
RTFACTN.C    - RTF > TXT converter
RTFDECL.H    - RTF > TXT converter
RTFREADR.C   - RTF > TXT converter
RTFTYPE.H    - RTF > TXT converter
SOUND.C      - High-level sound routines
SOUND.H      - High-level sound routines (header)
TEXTEDIT.GDT - RHIDE desktop file
TEXTEDIT.GPR - RHIDE project file
TEXTEDIT.MAK - DJGPP makefile
TEXTEDNS.GDT - RHIDE desktop file - no sound
TEXTEDNS.GPR - RHIDE project file - no sound
TEXTEDNS.MAK - DJGPP makefile, no sound
THANKS.TXT   - Thanks
TXTWRITE.C   - Write > TXT converter
WIN32.C      - Win32-specific sound routines

If you have any questions or comments, please e-mail me at
webmaster@osplus.co.uk or osplus@orudge.freeuk.com. I've
probably forgotton something important from the Readme, so
if you notice anything missing, e-mail me! :-)

Happy hacking!

Owen Rudge, 19th May 2001
http://www.osplus.co.uk/
http://www.owenrudge.com/
http://www.owenrudge.co.uk/
http://www.transportycoon.uk.tt/
http://www.savedgamemanager.uk.tt/