# Unified gcc-based makefile for OSPlus Text Editor
# By Owen Rudge, 28/12/2004

-include makefile.cfg

null: all

ifeq ($(COMPILER),)
	$(error Please run configure.bat or configure before make)
endif

ifeq ($(COMPILER),undefined)
	$(error Please run configure.bat or configure before make)
endif

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

ifeq ($(WITH_ALLEG),1 )
OBJ_DIR = obj/djgppsnd
BIN_DIR = bin/djgppsnd

LD_LIBS += -lalleg
else
OBJ_DIR = obj/djgpp
BIN_DIR = bin/djgpp

CFLAGS          += -DDJGPP_NO_SOUND_SUPPORT=1
CPPFLAGS        += -DDJGPP_NO_SOUND_SUPPORT=1
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

OBJ_DIR       = obj/mingw32
BIN_DIR       = bin/mingw32

ECHO          = @echo
RM            = rm

LD_LIBS       = -lwinmm

endif

CFLAGS    += $(RHTV_CFLAGS) $(RHTV_INCLUDE)
CPPFLAGS  += $(RHTV_CPPFLAGS) $(RHTV_INCLUDE)
LDFLAGS   += $(RHTV_DIR_LIBS)
LD_LIBS   += $(RHTV_SLIBS)

OBJ_LIST = \
$(OBJ_DIR)/aboutdlg.o \
$(OBJ_DIR)/aboutosp.o \
$(OBJ_DIR)/cnvinfo.o \
$(OBJ_DIR)/convert.o \
$(OBJ_DIR)/ospedit1.o \
$(OBJ_DIR)/ospedit2.o \
$(OBJ_DIR)/ospedit3.o \
$(OBJ_DIR)/sound.o \
$(OBJ_DIR)/verinfo.o

EXE_LIST = \
$(BIN_DIR)/ospedit.exe \
$(BIN_DIR)/txtrtf.cnv \
$(BIN_DIR)/txtwrite.cnv

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

$(BIN_DIR)/ospedit.exe: $(OBJ_LIST) $(OSPEDIT_RES)
	gcc $(LDFLAGS) -o $@ $(OBJ_LIST) $(LD_LIBS)

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
