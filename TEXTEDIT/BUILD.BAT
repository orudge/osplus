@ECHO OFF
ECHO OSPlus Utility Kit - Editor Build Process
ECHO.
ECHO DJMAKE must be set to your DJGPP\BIN\MAKE.EXE path.
ECHO Current value: %DJMAKE%
ECHO.
ECHO If this is not your DJGPP MAKE utility, please SET DJMAKE to the path.
ECHO If you do not have DJGPP, or don't want to build the DJGPP version,
ECHO type "BUILD BCC". If you do not want to build the Borland C++ version,
ECHO type "BUILD DJGPP". If you do not want to build the Win32 version,
ECHO type "BUILD NOWIN32". The DJGPP and BCC versions will be built.
ECHO.
ECHO If you do not want Allegro support, edit TEXTEDIT.MAK. There are
ECHO instructions in the file on how to do this.
ECHO.
ECHO Press any key to continue, or CTRL-C to abort.
PAUSE > NUL

REM *** Build Borland C++ version ***
IF "%1" == "DJGPP" GOTO DJGPP
MAKE

REM *** Build Win32 version ***
:WIN32
IF "%1" == "NOWIN32" GOTO DJGPP
MAKE -f OSPEDT32.MAK

REM *** Build DJGPP version ***
:DJGPP
IF "%1" == "BCC" GOTO UPX
%DJMAKE% -f TEXTEDIT.MAK

REM *** Compress them, and the converters ***
:UPX
UPX OSPEDIT.EXE TEXTEDIT.EXE OSPEDT32.EXE *.CNV
