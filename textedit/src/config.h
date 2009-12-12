/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Configuration file routines (from Allegro)                */
/*************************************************************/

/* This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

#ifdef WITH_ALLEG
	#include <allegro.h>
#else

void set_config_file(AL_CONST char *filename);
AL_CONST char *get_config_string(AL_CONST char *section, AL_CONST char *name, AL_CONST char *def);
int get_config_int(AL_CONST char *section, AL_CONST char *name, int def);
int get_config_hex(AL_CONST char *section, AL_CONST char *name, int def);
float get_config_float(AL_CONST char *section, AL_CONST char *name, float def);
void set_config_string(AL_CONST char *section, AL_CONST char *name, AL_CONST char *val);
void set_config_int(AL_CONST char *section, AL_CONST char *name, int val);
void set_config_hex(AL_CONST char *section, AL_CONST char *name, int val);
void set_config_float(AL_CONST char *section, AL_CONST char *name, float val);

#endif
