@echo off
if "%1"=="djgpp" goto djgpp
if "%1"=="mingw" goto mingw
if "%1"=="undefined" goto undefined

echo Usage: configure target
echo.
echo You did not specify a target to configure OSPlus Text Editor for.
echo.
echo Possible targets:
echo.
echo    djgpp
echo    mingw
echo.
echo To build the MSVC, Borland C++ (Win32) or Borland C++ (DOS) versions
echo please read the appropriate file in docs/build.
echo.
echo All options are case-sensitive.

goto realend

:djgpp
echo COMPILER=djgpp > makefile.cfg
goto end

:mingw
echo COMPILER=mingw > makefile.cfg
echo NOTE: You may need to use the MSYS or Cygwin make instead of the mingw
echo NOTE: tool in order for the `rhtv-config` lines in the makefile to work:
echo NOTE: if you do not want to do this, edit the Makefile to manually
echo NOTE: specify Turbo Vision paths and libraries.
goto end

:undefined
echo COMPILER=undefined> makefile.cfg

goto end

:end
echo Configuration complete. Now run `make'.

:realend