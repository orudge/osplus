@ECHO OFF
ECHO OSPlus Text Editor Build Process
ECHO.
ECHO DJMAKE must be set to your DJGPP\BIN\MAKE.EXE path.
ECHO BCMAKE must be set to your BC4*\BIN\MAKE.EXE path.
ECHO.
ECHO DJMAKE: %DJMAKE%
ECHO BCMAKE: %BCMAKE%
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
ECHO.
ECHO Win32 support has been dropped until the new version of Turbo Vision
ECHO compatible with Win32 comes out.
ECHO.
ECHO Press any key to continue, or CTRL-C to abort.
PAUSE > NUL

ECHO Building Borland C++ version...
ECHO.
CALL BUILDBC.BAT NOWARNING
ECHO.
ECHO Cleaning up...
ECHO.
CALL CLEANUP.BAT

ECHO Building DJGPP version...
ECHO.
CALL BUILDDJ.BAT NOWARNING
ECHO.
ECHO Cleaning up...
ECHO.
CALL CLEANUP.BAT

ECHO Building DJGPP version without Allegro...
ECHO.
CALL BUILDDNA.BAT NOWARNING
ECHO.
ECHO Cleaning up...
ECHO.
CALL CLEANUP.BAT
