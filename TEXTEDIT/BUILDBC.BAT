@ECHO OFF
IF "%1"=="NOWARNING" GOTO SKIPWARNING
ECHO OSPlus Text Editor Build Process - Borland C++
ECHO.
ECHO BCMAKE must be set to your BC4*\BIN\MAKE.EXE path. It is currently set
ECHO to %BCMAKE%
ECHO.
ECHO If this path is not correct, please press CONTROL+C to return to
ECHO the command prompt, and correct them using SET.
ECHO.
ECHO This batch file will the Borland C++ version of Text Editor. Please
ECHO ensure that Borland C++ 4 or higher is installed.
ECHO.
ECHO Press any key to continue, or CTRL-C to abort.
PAUSE > NUL

:SKIPWARNING
%BCMAKE% -f REALDOS.MAK
