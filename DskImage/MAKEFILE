#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = IMPLIB
BCCDOS  = BCC +BCCDOS.CFG
TLINK   = TLINK
TLIB    = TLIB
TASM    = TASM

BCPATH  = C:\BC45

#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -LD:\BC45\LIB
IDE_BFLAGS = 
LLATDOS_dskimagedexe =  -P=65535 -L$(BCPATH)\LIB;D:\BC45\LIB -c -Tde
RLATDOS_dskimagedexe = 
BLATDOS_dskimagedexe = 
CNIEAT_dskimagedexe = -I$(BCPATH)\INCLUDE;D:\BC45\INCLUDE -D
LNIEAT_dskimagedexe = -x
LEAT_dskimagedexe = $(LLATDOS_dskimagedexe)
REAT_dskimagedexe = $(RLATDOS_dskimagedexe)
BEAT_dskimagedexe = $(BLATDOS_dskimagedexe)

#
# Dependency List
#
Dep_dskimage = \
   dskimage.exe

dskimage : BccDos.cfg $(Dep_dskimage)
  echo MakeNode 

Dep_dskimagedexe = \
   tprogbar.obj\
   quickmsg.obj\
   aboutosp.obj\
   window.obj\
   about.obj\
   read.obj\
   rawread.obj\
   write.obj\
   dskimage.obj

dskimage.exe : $(Dep_dskimagedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_dskimagedexe) $(LNIEAT_dskimagedexe) +
$(BCPATH)\LIB\c0l.obj+
tprogbar.obj+
quickmsg.obj+
aboutosp.obj+
window.obj+
about.obj+
read.obj+
rawread.obj+
write.obj+
dskimage.obj
$<,$*
$(BCPATH)\LIB\tv.lib+
$(BCPATH)\LIB\emu.lib+
$(BCPATH)\LIB\mathl.lib+
$(BCPATH)\LIB\cl.lib

|

tprogbar.obj :  tprogbar.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ tprogbar.cpp
|

quickmsg.obj :  quickmsg.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ quickmsg.cpp
|

aboutosp.obj :  aboutosp.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ aboutosp.cpp
|

window.obj :  window.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ window.c
|

about.obj :  about.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ about.cpp
|

read.obj :  read.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ read.cpp
|

rawread.obj :  rawread.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ rawread.cpp
|

write.obj :  write.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ write.cpp
|

dskimage.obj :  dskimage.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dskimagedexe) $(CNIEAT_dskimagedexe) -o$@ dskimage.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=dskimage.csm
-v-
-R-
-k-
-y-
-N-
-vi
-ml
-f
| $@


