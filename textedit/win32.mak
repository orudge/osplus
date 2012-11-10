# Makefile for OSPlus Text Editor for Win32
# By Owen Rudge, 12/10/2001
# Updated: 24/12/2001
# Updated: 07/02/2002 - msconv
#
# Based on TV32 makefile

# This and the MSVC makefile could probably be merged
# but I'll leave that for the next version ;)

# Borland C++
CC       = bcc32
LD       = ilink32
RC       = brcc32
CXX      = $(CC)
INC      = -I. -Isrc -Isrc/convert

!ifdef DEBUG
CDEBUGFLAGS = -v -vi -y -6 -Od
LDDEBUGFLAGS = /v
!else
CDEBUGFLAGS = -O2 -6 -T -DNDEBUG
!endif

!ifdef DYNRTL
DYNRTLDFLAGS = -WR
!endif

CFLAGS   = -D_BCPP -a8 $(DYNRTLDFLAGS) -WC -X -q $(CDEBUGFLAGS)
CXXFLAGS = -P -RT- $(CFLAGS)
LDFLAGS   = $(DYNRTLDFLAGS) $(LDDEBUGFLAGS) /Gn /x

.SUFFIXES: .cc .c .obj

OBJS_ =  +aboutdlg.obj +aboutosp.obj +cnvinfo.obj +config.obj +convert.obj +ospedit1.obj +ospedit2.obj +ospedit3.obj\
            +verinfo.obj +unicode.obj +fold.obj

OBJS_2 = obj\win32\aboutdlg.obj obj\win32\aboutosp.obj obj\win32\config.obj obj\win32\cnvinfo.obj obj\win32\convert.obj obj\win32\ospedit1.obj\
            obj\win32\ospedit2.obj obj\win32\ospedit3.obj obj\win32\verinfo.obj obj\win32\unicode.obj obj\win32\fold.obj

OBJS = $(OBJS_:+=obj\win32\)

all: bin/win32/ospedit.exe bin/win32/txtrtf.cnv bin/win32/txtwrite.cnv bin/win32/msconv.cnv

bin/win32/ospedit.exe: $(OBJS) obj/win32/ospedit.res
	$(LD) $(LDFLAGS) $(OBJS_2), bin\win32\ospedit.exe,, rhtv.lib import32.lib c0x32.obj cw32.lib,,obj\win32\ospedit.res
	del bin\win32\ospedit.tds

bin/win32/txtrtf.cnv: obj/win32/rtfactn.obj obj/win32/rtfreadr.obj obj/win32/txtrtf.res
	$(LD) $(LDFLAGS) obj\win32\rtfactn.obj obj\win32\rtfreadr.obj, bin\win32\txtrtf.cnv,, import32.lib c0x32.obj cw32.lib,,obj\win32\txtrtf.res
	del bin\win32\txtrtf.tds

bin/win32/txtwrite.cnv: obj/win32/txtwrite.obj obj/win32/txtwrite.res
	$(LD) $(LDFLAGS) obj\win32\txtwrite.obj, bin\win32\txtwrite.cnv,, import32.lib c0x32.obj cw32.lib,,obj\win32\txtwrite.res
	del bin\win32\txtwrite.tds

bin/win32/msconv.cnv: obj/win32/msconv.obj obj/win32/msconv.res
	$(LD) $(LDFLAGS) obj\win32\msconv.obj, bin\win32\msconv.cnv,, import32.lib c0x32.obj cw32.lib,,obj\win32\msconv.res
	del bin\win32\msconv.tds

{src/}.cpp{obj\win32}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobj/win32 $<

{src/}.c{obj\win32}.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobj/win32 $<

{src/convert}.cpp{obj\win32}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobj/win32 $<

{src/convert}.c{obj\win32}.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobj/win32 $<

obj/win32/rtfactn.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobj/win32 src/convert/rtfactn.c

obj/win32/rtfreadr.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobj/win32 src/convert/rtfreadr.c

obj/win32/txtwrite.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobj/win32 src/convert/txtwrite.c

obj/win32/msconv.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobj/win32 src/convert/msconv.c

obj/win32/ospedit.res:
	$(RC) $(INC) -foobj/win32/ospedit.res src/ospedit.rc

obj/win32/txtrtf.res:
	$(RC) $(INC) -foobj/win32/txtrtf.res src/convert/txtrtf.rc

obj/win32/txtwrite.res:
	$(RC) $(INC) -foobj/win32/txtwrite.res src/convert/txtwrite.rc

obj/win32/msconv.res:
	$(RC) $(INC) -foobj/win32/msconv.res src/convert/msconv.rc

clean:
	-del obj\win32\*.obj
	-del bin\win32\*.exe
	-del bin\win32\*.cnv

compress:
	upx --best --compress-resources=0 bin/win32/ospedit.exe bin/win32/txtwrite.cnv bin/win32/txtrtf.cnv bin/win32/msconv.cnv
