# Unified gcc-based makefile for OSPlus Text Editor
# By Owen Rudge, 28/12/2004 - last updated 31/03/2005

-include makefile.cfg

null: $(COMPILER)

ifeq ($(COMPILER),)
	$(error Please run configure.bat or configure before make)
endif

ifeq ($(COMPILER),undefined)
	$(error Please run configure.bat or configure before make)
endif

undefined :

djgpp : all
mingw : all
linux : all

ifeq ($(DEBUG),1)
CFLAGS   = -g -Isrc/ -Isrc/convert/
CPPFLAGS = -g -Isrc/ -Isrc/convert/
LDFLAGS  =
else
CFLAGS   = -s -Isrc/ -Isrc/convert/
CPPFLAGS = -s -Isrc/ -Isrc/convert/
LDFLAGS  = -s
RCFLAGS  = --include-dir=src/ --include-dir=src/convert/
LD_LIBS  = 
endif

ifeq ($(COMPILER),djgpp )
# Since `rhtv-config` doesn't seem to work on DJGPP, please update the following by hand:
RHTV_CFLAGS   = -O2
RHTV_CPPFLAGS = -O2
RHTV_INCLUDE  = -Ic:/Various/tv203/contrib/tvision/include -Ic:/djgpp/include/rhtvision
RHTV_DIR_LIBS = -Lc:/Various/tv203/contrib/tvision/makes -Lc:/djgpp/lib
RHTV_SLIBS    = -lrhtv -lstdcxx

EXE_NAME      = ospedit.exe

ifeq ($(WITH_ALLEG),1 )
OBJ_DIR = obj/djgppsnd
BIN_DIR = bin/djgppsnd

CFLAGS     +=  -DSOUND_SUPPORT=1 -DWITH_ALLEG=1
CPPFLAGS   +=  -DSOUND_SUPPORT=1 -DWITH_ALLEG=1
LD_LIBS += -lalleg
else
OBJ_DIR = obj/djgpp
BIN_DIR = bin/djgpp

CPPFLAGS        += -DDJGPP_NO_SOUND_SUPPORT=1 -DSOUND_SUPPORT=0
CFLAGS          += -DLINUX_NO_SOUND_SUPPORT=1 -DSOUND_SUPPORT=0
endif

ECHO            = @echo
RENAME_TXTRTF   = mv $(BIN_DIR)/txtrtf.exe $(BIN_DIR)/txtrtf.cnv
RENAME_TXTWRITE = mv $(BIN_DIR)/txtwrite.exe $(BIN_DIR)/txtwrite.cnv

RHTV_SLIBS      += -lm

endif

#### MINGW/CYGWIN ####

ifeq ($(COMPILER),mingw )
RHTV_CFLAGS   = `rhtv-config --cflags`
RHTV_CPPFLAGS = `rhtv-config --cppflags`
RHTV_INCLUDE  = `rhtv-config --include`
RHTV_DIR_LIBS = `rhtv-config --dir-libs`
RHTV_SLIBS    = `rhtv-config --slibs`

EXE_NAME      = ospedit.exe

OBJ_DIR       = obj/mingw32
BIN_DIR       = bin/mingw32

ECHO          = @echo
RM            = rm

LD_LIBS       = -lwinmm

endif

#### *NIX ####

ifeq ($(COMPILER),linux)
RHTV_CFLAGS   = -D__LINUX__=1 `rhtv-config --cflags`
RHTV_CPPFLAGS = -D__LINUX__=1 `rhtv-config --cppflags`
RHTV_INCLUDE  = `rhtv-config --include`
RHTV_DIR_LIBS = `rhtv-config --dir-libs`
RHTV_SLIBS    = `rhtv-config --slibs`

EXE_NAME      = ospedit

ifeq ($(WITH_ALLEG),1)
OBJ_DIR = obj/linuxsnd
BIN_DIR = bin/linuxsnd

CFLAGS     +=  -DSOUND_SUPPORT=1 -DWITH_ALLEG=1
CPPFLAGS   +=  -DSOUND_SUPPORT=1 -DWITH_ALLEG=1
RHTV_SLIBS += `allegro-config --libs`
else
OBJ_DIR = obj/linux
BIN_DIR = bin/linux

CFLAGS          += -DLINUX_NO_SOUND_SUPPORT=1 -DSOUND_SUPPORT=0
CPPFLAGS        += -DLINUX_NO_SOUND_SUPPORT=1 -DSOUND_SUPPORT=0
endif

ECHO          = echo
RM            = rm

LD_LIBS       = 

endif

CFLAGS    += $(RHTV_CFLAGS) $(RHTV_INCLUDE)
CPPFLAGS  += $(RHTV_CPPFLAGS) $(RHTV_INCLUDE)
LDFLAGS   += $(RHTV_DIR_LIBS)
LD_LIBS   +=

OBJ_LIST = \
$(OBJ_DIR)/aboutdlg.o \
$(OBJ_DIR)/aboutosp.o \
$(OBJ_DIR)/config.o \
$(OBJ_DIR)/cnvinfo.o \
$(OBJ_DIR)/convert.o \
$(OBJ_DIR)/ospedit1.o \
$(OBJ_DIR)/ospedit2.o \
$(OBJ_DIR)/ospedit3.o \
$(OBJ_DIR)/sound.o \
$(OBJ_DIR)/verinfo.o \
$(OBJ_DIR)/unicode.o

EXE_LIST = \
$(BIN_DIR)/$(EXE_NAME) \
$(BIN_DIR)/txtrtf.cnv \
$(BIN_DIR)/txtwrite.cnv

ifeq ($(COMPILER),linux)
ifeq ($(WITH_WINE), 1)
EXE_LIST += $(BIN_DIR)/msconv.cnv
EXE_LIST += $(BIN_DIR)/msconv.cnv.exe.so
endif
endif

ifeq ($(COMPILER),mingw )
EXE_LIST += $(BIN_DIR)/msconv.cnv

OSPEDIT_RES   = $(OBJ_DIR)/ospedit.res
TXTRTF_RES    = $(OBJ_DIR)/txtrtf.res
TXTWRITE_RES  = $(OBJ_DIR)/txtwrite.res
endif

RTF_OBJ_LIST = \
$(OBJ_DIR)/rtfactn.o \
$(OBJ_DIR)/rtfreadr.o

all : $(OBJ_LIST) $(EXE_LIST)
	$(ECHO) OSPlus Text Editor has been built.

$(OBJ_DIR)/%.o: src/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/convert/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.cpp
	gcc $(CPPFLAGS) -c $< -o $@


ifeq ($(COMPILER), mingw )
$(OBJ_DIR)/%.res: src/%.rc
	windres $(RCFLAGS) -Irc -Ocoff -i $< -o $@

$(OBJ_DIR)/%.res: src/convert/%.rc
	windres $(RCFLAGS) -Irc -Ocoff -i $< -o $@

$(BIN_DIR)/msconv.cnv: $(OBJ_DIR)/msconv.o $(OBJ_DIR)/msconv.res
	gcc -o $@ $<
endif

ifeq ($(COMPILER), linux)
ifeq ($(WITH_WINE), 1)
WINE_CEXTRA    = -mno-cygwin
WINE_CXXEXTRA  = -mno-cygwin
WINE_LDFLAGS   = -mconsole -mno-cygwin
WINE_CC        = winegcc
WINE_WINEBUILD = winebuild
CP             = cp
MV             = mv

$(OBJ_DIR)/msconv.o : src/convert/msconv.c
	$(WINE_CC) -c $(CFLAGS) $(WINE_CEXTRA) -c $< -o $@

$(BIN_DIR)/msconv.cnv.exe.so: $(OBJ_DIR)/msconv.o
	$(WINE_CC) $(WINE_LDFLAGS) -o $@ $<

$(BIN_DIR)/msconv.cnv: src/convert/msconv.wine
	$(CP) $< $@
endif
endif

$(BIN_DIR)/$(EXE_NAME): $(OBJ_LIST) $(OSPEDIT_RES)
	gcc $(LDFLAGS) -o $@ $(OBJ_LIST) $(OSPEDIT_RES) $(LD_LIBS) $(RHTV_SLIBS)

$(BIN_DIR)/txtrtf.cnv: $(RTF_OBJ_LIST) $(TXTRTF_RES)
	gcc $(LDFLAGS) -o $@ $(RTF_OBJ_LIST) $(LD_LIBS)
	$(RENAME_TXTRTF)

$(BIN_DIR)/txtwrite.cnv: $(OBJ_DIR)/txtwrite.o $(TXTWRITE_RES)
	gcc $(LDFLAGS) -o $@ $< $(LD_LIBS)
	$(RENAME_TXTWRITE)

clean :
	$(RM) -f $(OBJ_DIR)/*.o $(EXE_LIST)

compress :
	upx --best $(EXE_LIST)
