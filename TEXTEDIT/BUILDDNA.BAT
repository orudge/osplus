@ECHO OFF
IF "%1"=="NOWARNING" GOTO SKIPWARNING
ECHO OSPlus Text Editor Build Process - DJGPP without Allegro
ECHO.
ECHO DJMAKE must be set to your DJGPP\BIN\MAKE.EXE path. It is currently set
ECHO to %DJMAKE%
ECHO.
ECHO If this path is not correct, please press CONTROL+C to return to
ECHO the command prompt, and correct them using SET.
ECHO.
ECHO This batch file will build the DJGPP version of Text Editor with Allegro.
ECHO Please ensure that DJGPP and Turbo Vision are both correctly installed.
ECHO.
ECHO Press any key to continue, or CTRL-C to abort.
PAUSE > NUL

:SKIPWARNING
%DJMAKE% -f djgpp.mak
CALL BUILDCNV.BAT djgpp
