# Makefile for OSPlus Text Editor for Win32 (MSVC)
# By Owen Rudge, 13/10/2002
# Based on the Borland C++ Win32 makefile

# This and the BC makefile could probably be merged
# but I'll leave that for the next version ;)

CC       = cl
LD       = link
RC       = rc
CXX      = $(CC)
INC      = -I. -Isrc -Isrc/convert

CDEBUGFLAGS = -O2 -DNDEBUG

CFLAGS   = /D__WIN32__ /D__MSVC__ $(CDEBUGFLAGS)
CXXFLAGS = $(CFLAGS)
LDFLAGS   = $(DYNRTLDFLAGS) /SUBSYSTEM:CONSOLE

.SUFFIXES: .cc .c .obj

OBJS_ =  +aboutdlg.obj +aboutosp.obj +cnvinfo.obj +config.obj +convert.obj +ospedit1.obj +ospedit2.obj +ospedit3.obj\
            +sound.obj +verinfo.obj +unicode.obj

OBJS_2 = obj\w32msvc\aboutdlg.obj obj\w32msvc\aboutosp.obj obj\w32msvc\cnvinfo.obj obj\w32msvc\config.obj obj\w32msvc\convert.obj obj\w32msvc\ospedit1.obj\
            obj\w32msvc\ospedit2.obj obj\w32msvc\ospedit3.obj obj\w32msvc\sound.obj\
            obj\w32msvc\verinfo.obj obj\w32msvc\unicode.obj

OBJS = $(OBJS_:+=obj\w32msvc\)

all: bin/w32msvc/ospedit.exe bin/w32msvc/txtrtf.cnv bin/w32msvc/txtwrite.cnv bin/w32msvc/msconv.cnv

bin/w32msvc/ospedit.exe: $(OBJS) obj/w32msvc/ospedit.res
	$(LD) /OUT:bin\w32msvc\ospedit.exe $(LDFLAGS) $(OBJS_2) librhtv.lib kernel32.lib user32.lib winmm.lib gdi32.lib advapi32.lib obj\w32msvc\ospedit.res

bin/w32msvc/txtrtf.cnv: obj/w32msvc/rtfactn.obj obj/w32msvc/rtfreadr.obj obj/w32msvc/txtrtf.res
	$(LD) /OUT:bin\w32msvc\txtrtf.cnv $(LDFLAGS) obj\w32msvc\rtfactn.obj obj\w32msvc\rtfreadr.obj kernel32.lib user32.lib obj\w32msvc\txtrtf.res

bin/w32msvc/txtwrite.cnv: obj/w32msvc/txtwrite.obj obj/w32msvc/txtwrite.res
	$(LD) /OUT:bin\w32msvc\txtwrite.cnv $(LDFLAGS) obj\w32msvc\txtwrite kernel32.lib user32.lib obj\w32msvc\txtwrite.res

bin/w32msvc/msconv.cnv: obj/w32msvc/msconv.obj obj/w32msvc/msconv.res
	$(LD) /OUT:bin\w32msvc\msconv.cnv $(LDFLAGS) obj\w32msvc\msconv.obj kernel32.lib user32.lib obj\w32msvc\msconv.res

{src/}.cpp{obj\w32msvc}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) /Foobj/w32msvc/ $<

{src/}.c{obj\w32msvc}.obj:
	$(CC) -c $(CFLAGS) $(INC) /Foobj/w32msvc/ $<

{src/convert}.cpp{obj\w32msvc}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) /Foobj/w32msvc/ $<

{src/convert}.c{obj\w32msvc}.obj:
	$(CC) -c $(CFLAGS) $(INC) /Foobj/w32msvc/ $<

obj/w32msvc/rtfactn.obj:
	$(CC) -c $(CFLAGS) $(INC) /Foobj/w32msvc/ src/convert/rtfactn.c

obj/w32msvc/rtfreadr.obj:
	$(CC) -c $(CFLAGS) $(INC) /Foobj/w32msvc/ src/convert/rtfreadr.c

obj/w32msvc/txtwrite.obj:
	$(CC) -c $(CFLAGS) $(INC) /Foobj/w32msvc/ src/convert/txtwrite.c

obj/w32msvc/msconv.obj:
	$(CC) -c $(CFLAGS) $(INC) /Foobj/w32msvc/ src/convert/msconv.c

obj/w32msvc/ospedit.res:
	$(RC) $(INC) /d__MSVC__ /r /fo obj/w32msvc/ospedit.res src/ospedit.rc

obj/w32msvc/txtrtf.res:
	$(RC) $(INC) /d__MSVC__ /r /fo obj/w32msvc/txtrtf.res src/convert/txtrtf.rc

obj/w32msvc/txtwrite.res:
	$(RC) $(INC) /d__MSVC__ /r /fo obj/w32msvc/txtwrite.res src/convert/txtwrite.rc

obj/w32msvc/msconv.res:
	$(RC) $(INC) /d__MSVC__ /r /fo obj/w32msvc/msconv.res src/convert/msconv.rc

clean:
	-del obj\w32msvc\*.obj
	-del bin\w32msvc\*.exe
	-del bin\w32msvc\*.cnv

compress:
	upx --best --compress-resources=0 bin/w32msvc/ospedit.exe bin/w32msvc/txtwrite.cnv bin/w32msvc/txtrtf.cnv bin/w32msvc/msconv.cnv
