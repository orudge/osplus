#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = IMPLIB
BCCDOS  = BCC +OBJ\REALDOS\BCCDOS.CFG
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
IDE_LFLAGSDOS =  -L$(BCPATH)\LIB -P=65535
IDE_BFLAGS = 
CLATDOS_BINbREALDOSbospeditdexe =  -v- -R- -k- -N- -1- -p- -ml -f
LLATDOS_BINbREALDOSbospeditdexe =  -L$(BCPATH)\LIB -P=65535 -c -Tde
RLATDOS_BINbREALDOSbospeditdexe = 
BLATDOS_BINbREALDOSbospeditdexe = 
CEAT_BINbREALDOSbospeditdexe = $(CLATDOS_BINbREALDOSbospeditdexe)
CNIEAT_BINbREALDOSbospeditdexe = -I$(BCPATH)\INCLUDE;SRC;SRC\CONVERT -D
LNIEAT_BINbREALDOSbospeditdexe = -x
LEAT_BINbREALDOSbospeditdexe = $(LLATDOS_BINbREALDOSbospeditdexe)
REAT_BINbREALDOSbospeditdexe = $(RLATDOS_BINbREALDOSbospeditdexe)
BEAT_BINbREALDOSbospeditdexe = $(BLATDOS_BINbREALDOSbospeditdexe)
CLATDOS_BINbREALDOSbtxtrtfdcnv =  -ml -f- -1- -v- -R- -k- -g0
LLATDOS_BINbREALDOSbtxtrtfdcnv =  -c -Tde -L$(BCPATH)\LIB -P=65535
RLATDOS_BINbREALDOSbtxtrtfdcnv = 
BLATDOS_BINbREALDOSbtxtrtfdcnv = 
CEAT_BINbREALDOSbtxtrtfdcnv = $(CLATDOS_BINbREALDOSbtxtrtfdcnv)
CNIEAT_BINbREALDOSbtxtrtfdcnv = -I$(BCPATH)\INCLUDE;SRC;SRC\CONVERT -D
LNIEAT_BINbREALDOSbtxtrtfdcnv = -x
LEAT_BINbREALDOSbtxtrtfdcnv = $(LLATDOS_BINbREALDOSbtxtrtfdcnv)
REAT_BINbREALDOSbtxtrtfdcnv = $(RLATDOS_BINbREALDOSbtxtrtfdcnv)
BEAT_BINbREALDOSbtxtrtfdcnv = $(BLATDOS_BINbREALDOSbtxtrtfdcnv)
CLATDOS_BINbREALDOSbtxtwritedcnv =  -ml -f- -v- -R- -k- -1-
LLATDOS_BINbREALDOSbtxtwritedcnv =  -c -Tde -L$(BCPATH)\LIB -P=65535
RLATDOS_BINbREALDOSbtxtwritedcnv = 
BLATDOS_BINbREALDOSbtxtwritedcnv = 
CEAT_BINbREALDOSbtxtwritedcnv = $(CLATDOS_BINbREALDOSbtxtwritedcnv)
CNIEAT_BINbREALDOSbtxtwritedcnv = -I$(BCPATH)\INCLUDE;SRC;SRC\CONVERT -D
LNIEAT_BINbREALDOSbtxtwritedcnv = -x
LEAT_BINbREALDOSbtxtwritedcnv = $(LLATDOS_BINbREALDOSbtxtwritedcnv)
REAT_BINbREALDOSbtxtwritedcnv = $(RLATDOS_BINbREALDOSbtxtwritedcnv)
BEAT_BINbREALDOSbtxtwritedcnv = $(BLATDOS_BINbREALDOSbtxtwritedcnv)

#
# Dependency List
#
Dep_ospedit = \
   BIN\REALDOS\ospedit.exe\
   BIN\REALDOS\txtrtf.cnv\
   BIN\REALDOS\txtwrite.cnv

ospedit : OBJ\REALDOS\BCCDOS.CFG $(Dep_ospedit)
  echo MakeNode

Dep_BINbREALDOSbospeditdexe = \
	OBJ\REALDOS\verinfo.obj\
   OBJ\REALDOS\aboutosp.obj\
   OBJ\REALDOS\gemsnd.obj\
   OBJ\REALDOS\aboutdlg.obj\
   OBJ\REALDOS\calc.obj\
   OBJ\REALDOS\ospedit1.obj\
   OBJ\REALDOS\ospedit2.obj\
   OBJ\REALDOS\ospedit3.obj\
   OBJ\REALDOS\sound.obj

BIN\REALDOS\ospedit.exe : $(Dep_BINbREALDOSbospeditdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_BINbREALDOSbospeditdexe) $(LNIEAT_BINbREALDOSbospeditdexe) +
$(BCPATH)\LIB\c0l.obj+
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
$(BCPATH)\LIB\tv.lib+
$(BCPATH)\LIB\bidsl.lib+
$(BCPATH)\LIB\emu.lib+
$(BCPATH)\LIB\mathl.lib+
$(BCPATH)\LIB\cl.lib

|

OBJ\REALDOS\verinfo.obj :  SRC\verinfo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\verinfo.cpp
|

OBJ\REALDOS\aboutosp.obj :  SRC\aboutosp.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\aboutosp.cpp
|

OBJ\REALDOS\gemsnd.obj :  SRC\gemsnd.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\gemsnd.c
|

OBJ\REALDOS\aboutdlg.obj :  SRC\aboutdlg.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\aboutdlg.cpp
|

OBJ\REALDOS\calc.obj :  SRC\calc.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\calc.cpp
|

OBJ\REALDOS\ospedit1.obj :  SRC\ospedit1.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\ospedit1.cpp
|

OBJ\REALDOS\ospedit2.obj :  SRC\ospedit2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\ospedit2.cpp
|

OBJ\REALDOS\ospedit3.obj :  SRC\ospedit3.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\ospedit3.cpp
|

OBJ\REALDOS\sound.obj :  SRC\sound.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_BINbREALDOSbospeditdexe) $(CNIEAT_BINbREALDOSbospeditdexe) -o$@ SRC\sound.c
|

Dep_BINbREALDOSbtxtrtfdcnv = \
   OBJ\REALDOS\rtfactn.obj\
   OBJ\REALDOS\rtfreadr.obj

BIN\REALDOS\txtrtf.cnv : $(Dep_BINbREALDOSbtxtrtfdcnv)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_BINbREALDOSbtxtrtfdcnv) $(LNIEAT_BINbREALDOSbtxtrtfdcnv) +
$(BCPATH)\LIB\c0l.obj+
OBJ\REALDOS\rtfactn.obj+
OBJ\REALDOS\rtfreadr.obj
$<,$*
$(BCPATH)\LIB\cl.lib

|

OBJ\REALDOS\rtfactn.obj :  SRC\CONVERT\rtfactn.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_BINbREALDOSbtxtrtfdcnv) $(CNIEAT_BINbREALDOSbtxtrtfdcnv) -o$@ SRC\CONVERT\rtfactn.c
|

OBJ\REALDOS\rtfreadr.obj :  SRC\CONVERT\rtfreadr.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_BINbREALDOSbtxtrtfdcnv) $(CNIEAT_BINbREALDOSbtxtrtfdcnv) -o$@ SRC\CONVERT\rtfreadr.c
|

Dep_BINbREALDOSbtxtwritedcnv = \
   OBJ\REALDOS\txtwrite.obj

BIN\REALDOS\txtwrite.cnv : $(Dep_BINbREALDOSbtxtwritedcnv)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_BINbREALDOSbtxtwritedcnv) $(LNIEAT_BINbREALDOSbtxtwritedcnv) +
$(BCPATH)\LIB\c0l.obj+
OBJ\REALDOS\txtwrite.obj
$<,$*
$(BCPATH)\LIB\cl.lib

|

OBJ\REALDOS\txtwrite.obj :  SRC\CONVERT\txtwrite.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_BINbREALDOSbtxtwritedcnv) $(CNIEAT_BINbREALDOSbtxtwritedcnv) -o$@ SRC\CONVERT\txtwrite.c
|

# Compiler configuration file
OBJ\REALDOS\BCCDOS.CFG :
	COPY &&|
-W-
-R
-v
-vi
-H
-H=obj\realdos\ospedit.csm
-v-
-R-
-k-
| $@


