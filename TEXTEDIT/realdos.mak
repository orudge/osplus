#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = IMPLIB
BCCDOS  = BCC +bccdos.cfg 
TLINK   = TLINK
TLIB    = TLIB
TASM    = TASM

BCC_LIB = D:\BC45\LIB
BCC_INC = D:\BC45\INCLUDE
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -L$(BCC_LIB) -P=65535
IDE_BFLAGS = 
CLATDOS_OSPEDIT =  -v- -R- -k- -N- -1- -p- -ml -f
LLATDOS_OSPEDIT =  -L$(BCC_LIB) -P=65535 -c -Tde
RLATDOS_OSPEDIT = 
BLATDOS_OSPEDIT = 
CEAT_OSPEDIT = $(CLATDOS_OSPEDIT)
CNIEAT_OSPEDIT = -I$(BCC_INC);SRC;SRC\CONVERT -D
LNIEAT_OSPEDIT = -x
LEAT_OSPEDIT = $(LLATDOS_OSPEDIT)
REAT_OSPEDIT = $(RLATDOS_OSPEDIT)
BEAT_OSPEDIT = $(BLATDOS_OSPEDIT)
CLATDOS_TXTRTF =  -ml -f- -1- -v- -R- -k- -g0
LLATDOS_TXTRTF =  -c -Tde -L$(BCC_LIB) -P=65535
RLATDOS_TXTRTF = 
BLATDOS_TXTRTF = 
CEAT_TXTRTF = $(CLATDOS_TXTRTF)
CNIEAT_TXTRTF = -I$(BCC_INC);SRC;SRC\CONVERT -D
LNIEAT_TXTRTF = -x
LEAT_TXTRTF = $(LLATDOS_TXTRTF)
REAT_TXTRTF = $(RLATDOS_TXTRTF)
BEAT_TXTRTF = $(BLATDOS_TXTRTF)
CLATDOS_TXTWRITE =  -ml -f- -v- -R- -k- -1-
LLATDOS_TXTWRITE =  -c -Tde -L$(BCC_LIB) -P=65535
RLATDOS_TXTWRITE = 
BLATDOS_TXTWRITE = 
CEAT_TXTWRITE = $(CLATDOS_TXTWRITE)
CNIEAT_TXTWRITE = -I$(BCC_INC);SRC;SRC\CONVERT -D
LNIEAT_TXTWRITE = -x
LEAT_TXTWRITE = $(LLATDOS_TXTWRITE)
REAT_TXTWRITE = $(RLATDOS_TXTWRITE)
BEAT_TXTWRITE = $(BLATDOS_TXTWRITE)

#
# Dependency List
#
Dep_ospedit = \
   BIN\REALDOS\ospedit.exe\
   BIN\REALDOS\txtrtf.cnv\
   BIN\REALDOS\txtwrite.cnv

ospedit : BccDos.cfg $(Dep_ospedit)
  echo MakeNode 

Dep_OSPEDIT = \
   OBJ\REALDOS\convert.obj\
   OBJ\REALDOS\verinfo.obj\
   OBJ\REALDOS\aboutosp.obj\
   OBJ\REALDOS\gemsnd.obj\
   OBJ\REALDOS\aboutdlg.obj\
   OBJ\REALDOS\calc.obj\
   OBJ\REALDOS\ospedit1.obj\
   OBJ\REALDOS\ospedit2.obj\
   OBJ\REALDOS\ospedit3.obj\
   OBJ\REALDOS\sound.obj

BIN\REALDOS\ospedit.exe : $(Dep_OSPEDIT)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_OSPEDIT) $(LNIEAT_OSPEDIT) +
$(BCC_LIB)\c0l.obj+
OBJ\REALDOS\convert.obj+
OBJ\REALDOS\verinfo.obj+
OBJ\REALDOS\aboutosp.obj+
OBJ\REALDOS\gemsnd.obj+
OBJ\REALDOS\aboutdlg.obj+
OBJ\REALDOS\calc.obj+
OBJ\REALDOS\ospedit1.obj+
OBJ\REALDOS\ospedit2.obj+
OBJ\REALDOS\ospedit3.obj+
OBJ\REALDOS\sound.obj
$<,$*
$(BCC_LIB)\tv.lib+
$(BCC_LIB)\bidsl.lib+
$(BCC_LIB)\emu.lib+
$(BCC_LIB)\mathl.lib+
$(BCC_LIB)\cl.lib

|

OBJ\REALDOS\convert.obj :  SRC\convert.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\convert.c
|

OBJ\REALDOS\verinfo.obj :  SRC\verinfo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\verinfo.cpp
|

OBJ\REALDOS\aboutosp.obj :  SRC\aboutosp.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\aboutosp.cpp
|

OBJ\REALDOS\gemsnd.obj :  SRC\gemsnd.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\gemsnd.c
|

OBJ\REALDOS\aboutdlg.obj :  SRC\aboutdlg.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\aboutdlg.cpp
|

OBJ\REALDOS\calc.obj :  SRC\calc.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\calc.cpp
|

OBJ\REALDOS\ospedit1.obj :  SRC\ospedit1.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\ospedit1.cpp
|

OBJ\REALDOS\ospedit2.obj :  SRC\ospedit2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\ospedit2.cpp
|

OBJ\REALDOS\ospedit3.obj :  SRC\ospedit3.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\ospedit3.cpp
|

OBJ\REALDOS\sound.obj :  SRC\sound.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_OSPEDIT) $(CNIEAT_OSPEDIT) -o$@ SRC\sound.c
|

Dep_TXTRTF = \
   OBJ\REALDOS\rtfactn.obj\
   OBJ\REALDOS\rtfreadr.obj

BIN\REALDOS\txtrtf.cnv : $(Dep_TXTRTF)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_TXTRTF) $(LNIEAT_TXTRTF) +
$(BCC_LIB)\c0l.obj+
OBJ\REALDOS\rtfactn.obj+
OBJ\REALDOS\rtfreadr.obj
$<,$*
$(BCC_LIB)\cl.lib

|

OBJ\REALDOS\rtfactn.obj :  SRC\CONVERT\rtfactn.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_TXTRTF) $(CNIEAT_TXTRTF) -o$@ SRC\CONVERT\rtfactn.c
|

OBJ\REALDOS\rtfreadr.obj :  SRC\CONVERT\rtfreadr.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_TXTRTF) $(CNIEAT_TXTRTF) -o$@ SRC\CONVERT\rtfreadr.c
|

Dep_TXTWRITE = \
   OBJ\REALDOS\txtwrite.obj

BIN\REALDOS\txtwrite.cnv : $(Dep_TXTWRITE)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_TXTWRITE) $(LNIEAT_TXTWRITE) +
$(BCC_LIB)\c0l.obj+
OBJ\REALDOS\txtwrite.obj
$<,$*
$(BCC_LIB)\cl.lib

|

OBJ\REALDOS\txtwrite.obj :  SRC\CONVERT\txtwrite.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_TXTWRITE) $(CNIEAT_TXTWRITE) -o$@ SRC\CONVERT\txtwrite.c
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=OBJ\REALDOS\OSPEDIT.CSM
-v-
-R-
-k-
-2
-Om
-Op
-Oi
-Ov
-Z-
-O-
-O-l
-Og
| $@


