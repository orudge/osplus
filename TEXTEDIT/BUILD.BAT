@ECHO OFF
ECHO OSPlus Text Editor Build Process
ECHO.
ECHO DJMAKE must be set to your DJGPP\BIN\MAKE.EXE path.
ECHO BCMAKE must be set to your BC4*\BIN\MAKE.EXE path.
ECHO CBMAKE must be set to your C++Builder (BCC32 v5.4 or greater)
ECHO                                                MAKE.EXE path.
ECHO.
ECHO DJMAKE: %DJMAKE%
ECHO BCMAKE: %BCMAKE%
ECHO CBMAKE: %CBMAKE%
ECHO.
ECHO If these paths are not correct, please press CONTROL+C to return to
ECHO the command prompt, and correct them using SET.
ECHO.
ECHO This batch file will build all versions of Text Editor except Linux.
ECHO These are:
ECHO.
ECHO * DOS (Borland C++)
ECHO * DOS (DJGPP, with Allegro)
ECHO * DOS (DJGPP, without Allegro)
ECHO * Win32 (Borland C++)
ECHO.
ECHO Press any key to continue, or CTRL-C to abort.
PAUSE > NUL

ECHO Building Borland C++ version (to BIN\REALDOS)...
ECHO.
CALL BUILDBC.BAT NOWARNING
ECHO.
ECHO Building DJGPP version (to BIN\DJGPPSND)...
ECHO.
CALL BUILDDJ.BAT NOWARNING
ECHO.
ECHO Building DJGPP version without Allegro (to BIN\DJGPP)...
ECHO.
CALL BUILDDNA.BAT NOWARNING
ECHO.
ECHO Building Win32 version (to BIN\WIN32)...
CALL BUILDWIN.BAT NOWARNING
ECHO.
ECHO OSPlus Text Editor has been built.
