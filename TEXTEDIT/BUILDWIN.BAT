@ECHO OFF
IF "%1"=="NOWARNING" GOTO SKIPWARNING
ECHO OSPlus Text Editor Build Process - Win32
ECHO.
ECHO CBMAKE must be set to your CBuilder\BIN\MAKE.EXE path.
ECHO It is currently set to %CBMAKE%. You will need version 5.4 of BCC32
ECHO to build the editor. (Earlier versions might work).
ECHO.
ECHO Turbo Vision 1.1.3 or higher must be built and the include files
ECHO should be copied to (CBPATH)\Include and (CBPATH)\Include\tv. RHTV.LIB
ECHO should be placed in (CBPATH)\Lib.
ECHO.
ECHO If CBPATH is not correct, please press CONTROL+C to return to
ECHO the command prompt, and correct them using SET.
ECHO.
ECHO Press any key to continue, or CTRL-C to abort.
PAUSE > NUL

:SKIPWARNING
%CBMAKE% -f WIN32.MAK
