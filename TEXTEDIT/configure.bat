@echo off
if "%1"=="djgpp" goto djgpp
if "%1"=="mingw" goto mingw
if "%1"=="msvc" goto msvc
if "%1"=="undefined" goto undefined

echo Usage: configure target [--with-alleg]
echo.
echo You did not specify a target to configure OSPlus Text Editor for.
echo.
echo Possible targets:
echo.
echo    djgpp
echo    mingw
echo    msvc
echo.
echo --with-alleg is only applicable on DJGPP, if you have the Allegro
echo games library installed.
echo.
echo All options are case-sensitive.

goto realend

:djgpp
echo COMPILER=djgpp > makefile.cfg
if "%2"=="--with-alleg" goto alleg
echo WITH_ALLEG=0 >> makefile.cfg
goto end

:mingw
echo COMPILER=mingw > makefile.cfg
echo WITH_ALLEG=0 >> makefile.cfg
echo NOTE: You may need to use Cygwin's make instead of mingw's in order for
echo NOTE: the `rhtv-config` lines in the makefile to work: if you do not want
echo NOTE: to do this, edit the Makefile so a similar system to the djgpp
echo NOTE: section is used.
goto end

:undefined
echo COMPILER=undefined> makefile.cfg
echo WITH_ALLEG=0 >> makefile.cfg

goto end

:alleg
echo WITH_ALLEG=1 >> makefile.cfg

:end
echo Configuration complete. Now run `make'.

:realend