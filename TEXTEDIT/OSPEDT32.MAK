#
# Borland C++ IDE generated makefile
#
.AUTODEPEND

BCPATH = D:\BC45

#
# Borland C++ tools
#
IMPLIB  = IMPLIB
BCCD32  = BCC32 +BCCD32.CFG
TLINK32 = TLINK32
TLIB    = TLIB
TASM32  = TASM2
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSD32 =  -L$(BCPATH)\LIB
LLATD32_ospedt32dexe =  -L$(BCPATH)\LIB -Tpe -ax
RLATD32_ospedt32dexe = 
BLATD32_ospedt32dexe = 
CNIEAT_ospedt32dexe = -I$(BCPATH)\INCLUDE -D
LNIEAT_ospedt32dexe = -x
LEAT_ospedt32dexe = $(LLATD32_ospedt32dexe)
REAT_ospedt32dexe = $(RLATD32_ospedt32dexe)
BEAT_ospedt32dexe = $(BLATD32_ospedt32dexe)

#
# Dependency List
#
Dep_ospedt32 = \
   ospedt32.exe

ospedt32 : BCCD32.CFG $(Dep_ospedt32)
  echo MakeNode 

Dep_ospedt32dexe = \
   sound32.obj\
   calc32.obj\
   aboutd32.obj\
   osped321.obj\
   osped322.obj\
   osped323.obj\
   ospedt32.def

ospedt32.exe : $(Dep_ospedt32dexe)

  DELOBJ32

  $(TLINK32) @&&|
 /v $(IDE_LFLAGSD32) $(LEAT_ospedt32dexe) $(LNIEAT_ospedt32dexe) +
$(BCPATH)\LIB\c0x32.obj+
sound32.obj+
calc32.obj+
aboutd32.obj+
osped321.obj+
osped322.obj+
osped323.obj
$<,$*
$(BCPATH)\LIB\import32.lib+
$(BCPATH)\LIB\tv32.lib+
$(BCPATH)\LIB\cw32.lib
ospedt32.def
|

sound32.obj :  sound.c
 COPY SOUND.C SOUND32.C

  $(BCCD32) -P- -c @&&|
 $(CEAT_ospedt32dexe) $(CNIEAT_ospedt32dexe) -o$@ sound32.c
|

calc32.obj :  calc.cpp
 COPY CALC.CPP CALC32.CPP

  $(BCCD32) -c @&&|
 $(CEAT_ospedt32dexe) $(CNIEAT_ospedt32dexe) -o$@ calc32.cpp
|

aboutd32.obj :  aboutdlg.cpp
 COPY ABOUTDLG.CPP ABOUTD32.CPP

  $(BCCD32) -c @&&|
 $(CEAT_ospedt32dexe) $(CNIEAT_ospedt32dexe) -o$@ aboutd32.cpp
|

osped321.obj :  ospedit1.cpp
  COPY OSPEDIT1.CPP OSPED321.CPP

  $(BCCD32) -c @&&|
 $(CEAT_ospedt32dexe) $(CNIEAT_ospedt32dexe) -o$@ osped321.cpp
|

osped322.obj :  ospedit2.cpp
  COPY OSPEDIT2.CPP OSPED322.CPP

  $(BCCD32) -c @&&|
 $(CEAT_ospedt32dexe) $(CNIEAT_ospedt32dexe) -o$@ osped322.cpp
|

osped323.obj :  ospedit3.cpp
  COPY OSPEDIT3.CPP OSPED323.CPP

  $(BCCD32) -c @&&|
 $(CEAT_ospedt32dexe) $(CNIEAT_ospedt32dexe) -o$@ ospedit3.cpp
|

# Compiler configuration file
BCCD32.cfg : 
   COPY &&|
-R
-v
-vi
-H
-H=ospedt32.csm
-v-
-R-
-k-
-WX
| $@


