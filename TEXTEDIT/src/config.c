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

/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      Configuration routines.
 *
 *      By Shawn Hargreaves.
 *
 *      Hook functions added by Martijn Versteegh.
 *
 *      Annie Testes lifted several hardcoded length limitations.
 *
 *      See readme.txt for copyright information.
 */

#ifndef WITH_ALLEG

#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "unicode.h"

#ifdef __LINUX__
	#include <sys/types.h>
	#include <unistd.h>
#else
	#include <io.h>
#endif

#define PACKFILE   FILE
#define ASSERT(x)

#ifndef _USERENTRY
	#define _USERENTRY
#endif

#define pack_fopen(x, y)   fopen(x, y)
#define pack_fputs(x, y)   fputs(x, y)
#define pack_fread(x, y, z) fread(x, y, 1, z)
#define pack_putc(x, y)    putc(x, y)
#define pack_fclose(x)     fclose(x)

#ifndef __LINUX__
	#define OTHER_PATH_SEPARATOR   '\\'
	#define DEVICE_SEPARATOR       ':'
#else
	#define OTHER_PATH_SEPARATOR   '/'
	#define DEVICE_SEPARATOR       '/'
#endif

void set_config_file(AL_CONST char *filename);

typedef struct CONFIG_ENTRY
{
	char *name;                      /* variable name (NULL if comment) */
	char *data;                      /* variable value */
	struct CONFIG_ENTRY *next;       /* linked list */
} CONFIG_ENTRY;


typedef struct CONFIG
{
	CONFIG_ENTRY *head;              /* linked list of config entries */
	char *filename;                  /* where were we loaded from? */
	int dirty;                       /* has our data changed? */
} CONFIG;


typedef struct CONFIG_HOOK
{
   char *section;                   /* hooked config section info */
   int (*intgetter)(AL_CONST char *name, int def);
   AL_CONST char *(*stringgetter)(AL_CONST char *name, AL_CONST char *def);
   void (*stringsetter)(AL_CONST char *name, AL_CONST char *value);
   struct CONFIG_HOOK *next; 
} CONFIG_HOOK;


#define MAX_CONFIGS     4

static CONFIG *config[MAX_CONFIGS] = { NULL, NULL, NULL, NULL };
static CONFIG *config_override = NULL;
static CONFIG *config_language = NULL;
static CONFIG *system_config = NULL;

static CONFIG_HOOK *config_hook = NULL;

static int config_installed = FALSE;

/* get_filename:
 *  When passed a completely specified file path, this returns a pointer
 *  to the filename portion. Both '\' and '/' are recognized as directory
 *  separators.
 */
char *get_filename(AL_CONST char *path)
{
   int pos, c;

   pos = ustrlen(path);

   while (pos>0) {
      c = ugetat(path, pos-1);
      if ((c == '/') || (c == OTHER_PATH_SEPARATOR) || (c == DEVICE_SEPARATOR) || (c == '#'))
	 break;
      pos--;
   }

   return (char *)path + uoffset(path, pos);
}

/* file_size:
 *  Returns the size of a file, in bytes.
 *  If the file does not exist or an error occurs, it will return zero
 *  and store the system error code in errno.
 */
long file_size(AL_CONST char *filename)
{
	FILE *f;
	long offset;

	f = fopen(filename, "rb");

	if (!f)
		return(0);

	fseek(f, 0, SEEK_END);
	offset = ftell(f);
	fclose(f);

	return(offset);
}

/* flush_config:
 *  Writes out a config structure to disk if the contents
 *  have changed.
 */
static void flush_config(CONFIG *cfg)
{
   CONFIG_ENTRY *pos;
   PACKFILE *f;
   char cr[16];

   usetc(cr+usetc(cr, '\n'), 0);

   if (cfg && cfg->filename && cfg->dirty) {
      /* write changed data to disk */
		f = fopen(cfg->filename, "wt");

      if (f) {
	 pos = cfg->head;

	 while (pos) {
	    if (pos->name) {
	       pack_fputs(pos->name, f);

	       if (ugetc(pos->name) != '[') {
		  pack_putc(' ', f);
		  pack_putc('=', f);
		  pack_putc(' ', f);
	       }
	    }

	    if (pos->data)
	       pack_fputs(pos->data, f);

	    pack_fputs(cr, f);

	    pos = pos->next;
         }

	 pack_fclose(f);
	 cfg->dirty = FALSE;
      }
   }
}



/* flush_config_file:
 *  Writes out the config file to disk if the contents
 *  have changed.
 */
void flush_config_file(void)
{
   flush_config(config[0]);
}



/* destroy_config:
 *  Destroys a config structure, writing it out to disk if the contents
 *  have changed.
 */
static void destroy_config(CONFIG *cfg)
{
   CONFIG_ENTRY *pos, *prev;

   if (cfg) {
      flush_config(cfg);

      if (cfg->filename)
	 free(cfg->filename);

      /* destroy the variable list */
      pos = cfg->head;

      while (pos) {
	 prev = pos;
	 pos = pos->next;

	 if (prev->name)
	 {
	    free(prev->name);
		prev->name = 0;
	 }

	 if (prev->data)
	 {
	    free(prev->data);
		prev->data = 0;
	 }

	 free(prev);
      }

      free(cfg);
   }
}



/* config_cleanup:
 *  Called at shutdown time to free memory being used by the config routines,
 *  and write any changed data out to disk.
 */
static void _USERENTRY config_cleanup(void)
{
	CONFIG_HOOK *hook, *nexthook;
	int i;

	if (config_installed == TRUE)
	{
		for (i=0; i<MAX_CONFIGS; i++) {
			if (config[i]) {
		destroy_config(config[i]);
		config[i] = NULL;
			}
		}

		if (config_override) {
			destroy_config(config_override);
			config_override = NULL;
		}

		if (config_language) {
			destroy_config(config_language);
			config_language = NULL;
		}

		if (system_config) {
			destroy_config(system_config);
			system_config = NULL;
		}

		if (config_hook) {
			hook = config_hook;

			while (hook) {
		if (hook->section)
			free(hook->section);

		 nexthook = hook->next;
		free(hook);
		hook = nexthook;
			}

			config_hook = NULL;
		}

//	_remove_exit_func(config_cleanup);
		config_installed = FALSE;
	}
}



/* init_config:
 *  Sets up the configuration routines ready for use, also loading the
 *  default config file if the loaddata flag is set and no other config
 *  file is in memory.
 */
static void init_config(int loaddata)
{
	char tmp[128], *cfg_name;

	if (!config_installed) {
		atexit(config_cleanup);
		config_installed = TRUE;
	}

	if ((loaddata) && (!config[0])) {
		cfg_name = uconvert_ascii("ospedit.ini", tmp);
		set_config_file(cfg_name);
	}

	if (!system_config) {
		system_config = malloc(sizeof(CONFIG));
		if (system_config) {
			system_config->head = NULL;
			system_config->filename = NULL;
			system_config->dirty = FALSE;
		}
	}
}



/* get_line:
 *  Helper for splitting files up into individual lines. Returns the length
 *  in bytes of the sequence of characters delimited by the first EOL marker
 *  in the array DATA of length LENGTH, and allocates NAME and VAL to record
 *  the name and the value of the config entry respectively; otherwise set
 *  NAME to NULL and returns a copy of the line through VAL if the line was
 *  blank or a comment. Returns -1 and set errno on failure.
 */
static int get_line(AL_CONST char *data, int length, char **name, char **val)
{
   char *buf;
   int buf_size=256;
   int inpos, outpos, i, j;
   int c, c2, w0;

   inpos = 0;
   outpos = 0;
   w0 = ucwidth(0);

   buf = malloc(buf_size);
   if (!buf) {
     errno = ENOMEM;
     return -1;
   }

   /* search for an EOL marker */
   while (inpos<length) {
      c = ugetc(data+inpos);
      if ((c == '\r') || (c == '\n')) {
	 inpos += uwidth(data+inpos);
	 if (inpos < length) {
	    c2 = ugetc(data+inpos);
	    if (((c == '\r') && (c2 == '\n')) || ((c == '\n') && (c2 == '\r')))
	       inpos += uwidth(data+inpos);
	 }
	 break;
      }

      /* increase the buffer size if needed */
      if (outpos>=(int)buf_size-w0) {
	 buf_size *= 2;
	 buf = realloc(buf, buf_size);
	 if (!buf) {
	    errno = ENOMEM;
	    return -1;
	 }
      }

      outpos += usetc(buf+outpos, c);
      inpos += uwidth(data+inpos);
   }

   usetc(buf+outpos, 0);

   /* skip leading spaces */
   i = 0;
   c = ugetc(buf);

   while ((c) && (uisspace(c))) {
      i += uwidth(buf+i);
      c = ugetc(buf+i);
   }

   /* read name string */
   j = 0;

   /* compute name length */
   while ((c) && (!uisspace(c)) && (c != '=') && (c != '#')) {
      j += ucwidth(c);
      i += uwidth(buf+i);
      c = ugetc(buf+i);
   }

   if (j) {
      /* got a variable */
      *name = malloc(j+w0);
      if (!(*name)) {
	 errno = ENOMEM;
	 free(buf);
	 return -1;
      }

      ustrzcpy(*name, j+w0, buf+i-j);

      while ((c) && ((uisspace(c)) || (c == '='))) {
	 i += uwidth(buf+i);
	 c = ugetc(buf+i);
      }

      *val = ustrdup(buf+i);
      if (!(*val)) {
	 free(name);
	 free(buf);
	 return -1;
      }

      /* strip trailing spaces */
      i = ustrlen(*val) - 1;
      while ((i >= 0) && (uisspace(ugetat(*val, i))))
	 usetat(*val, i--, 0);
   }
   else {
      /* blank line or comment */
      *name = NULL;
      *val = ustrdup(buf);
      if (!(*val)) {
	 free(buf);
	 return -1;
      }
   }

   free(buf);

   return inpos;
}



/* set_config:
 *  Does the work of setting up a config structure.
 */
static void set_config(CONFIG **config, AL_CONST char *data, int length, AL_CONST char *filename)
{
   CONFIG_ENTRY **prev, *p;
   char *name, *val;
   int ret, pos;

   init_config(FALSE);

   if (*config) {
      destroy_config(*config);
      *config = NULL;
   }

   *config = malloc(sizeof(CONFIG));
   if (!(*config)) {
      errno = ENOMEM;
      return;
   }

   (*config)->head = NULL;
   (*config)->dirty = FALSE;

   if (filename) {
      (*config)->filename = ustrdup(filename);
      if (!(*config)->filename) {
	 free(*config);
	 *config = NULL;
	 return;
      }
   }
   else
      (*config)->filename = NULL;

   prev = &(*config)->head;
   pos = 0;

   while (pos < length) {
      ret = get_line(data+pos, length-pos, &name, &val);
      if (ret<0) {
	 free(*config);
	 *config = NULL;
	 return;
      }

      pos += ret;

      p = malloc(sizeof(CONFIG_ENTRY));
      if (!p) {
	 errno = ENOMEM;
	 free(*config);
	 *config = NULL;
	 return;
      }

      p->name = name;
      p->data = val;

      p->next = NULL;
      *prev = p;
      prev = &p->next;
   }
}



/* load_config_file:
 *  Does the work of loading a config file.
 */
static void load_config_file(CONFIG **config, AL_CONST char *filename, AL_CONST char *savefile)
{
   char *tmp, *tmp2;
   long length;

   if (*config) {
      destroy_config(*config);
      *config = NULL;
   }

   length = file_size(filename);

   if (length > 0) {
      PACKFILE *f = pack_fopen(filename, "rt");

      if (f) {
	 tmp = malloc(length+1);

	 if (tmp) {
		 pack_fread(tmp, length, f);
		 tmp[length] = 0;

	    if (need_uconvert(tmp, U_UTF8, U_CURRENT)) {
	       length = uconvert_size(tmp, U_UTF8, U_CURRENT);
	       tmp2 = malloc(length);

	       if (tmp2)
		  do_uconvert(tmp, U_UTF8, tmp2, U_CURRENT, length);

	       length -= ucwidth(0);
	    }
	    else
	       tmp2 = tmp;

	    if (tmp2) {
	       set_config(config, tmp2, length, savefile);

	       if (tmp2 != tmp)
		  free(tmp2);
	    }

	    free(tmp);
	 }
	 else
	    set_config(config, NULL, 0, savefile);

	 pack_fclose(f);
      }
      else
	 set_config(config, NULL, 0, savefile);
   }
   else
      set_config(config, NULL, 0, savefile);
}



/* set_config_file:
 *  Sets the file to be used for all future configuration operations.
 */
void set_config_file(AL_CONST char *filename)
{
   ASSERT(filename);
   load_config_file(&config[0], filename, filename);
}



/* set_config_data:
 *  Sets the block of data to be used for all future configuration 
 *  operations.
 */
void set_config_data(AL_CONST char *data, int length)
{
   ASSERT(data);
   set_config(&config[0], data, length, NULL);
}



/* override_config_file:
 *  Sets the file that will override all future configuration operations.
 */
void override_config_file(AL_CONST char *filename)
{
   ASSERT(filename);
   load_config_file(&config_override, filename, NULL);
}



/* override_config_data:
 *  Sets the block of data that will override all future configuration 
 *  operations.
 */
void override_config_data(AL_CONST char *data, int length)
{
   ASSERT(data);
   set_config(&config_override, data, length, NULL);
}



/* push_config_state:
 *  Pushes the current config state onto the stack.
 */
void push_config_state(void)
{
   int i;

   if (config[MAX_CONFIGS-1])
      destroy_config(config[MAX_CONFIGS-1]);

   for (i=MAX_CONFIGS-1; i>0; i--)
      config[i] = config[i-1];

   config[0] = NULL;
}



/* pop_config_state:
 *  Pops the current config state off the stack.
 */
void pop_config_state(void)
{
   int i;

   if (config[0])
      destroy_config(config[0]);

   for (i=0; i<MAX_CONFIGS-1; i++)
      config[i] = config[i+1];

   config[MAX_CONFIGS-1] = NULL;
}



/* prettify_section_name:
 *  Helper for ensuring that a section name is enclosed by [ ] braces.
 */
static void prettify_section_name(AL_CONST char *in, char *out, int out_size)
{
   int p;

   if ((in) && (ustrlen(in))) {
      if (ugetc(in) != '[') {
	 p = usetc(out, '[');
	 usetc(out+p, 0);
      }
      else
	 usetc(out, 0);

      ustrzcat(out, out_size - ucwidth(']'), in);

      out += uoffset(out, -1);

      if (ugetc(out) != ']') {
	 out += uwidth(out);
	 out += usetc(out, ']');
	 usetc(out, 0);
      }
   }
   else
      usetc(out, 0);
}



/* hook_config_section:
 *  Hooks a config section to a set of getter/setter functions. This will 
 *  override the normal table of values, and give the provider of the hooks 
 *  complete control over that section.
 */
void hook_config_section(AL_CONST char *section, int (*intgetter)(AL_CONST char *, int), AL_CONST char *(*stringgetter)(AL_CONST char *, AL_CONST char *), void (*stringsetter)(AL_CONST char *, AL_CONST char *))
{
   CONFIG_HOOK *hook, **prev;
   char section_name[256];

   init_config(FALSE);

   prettify_section_name(section, section_name, sizeof(section_name));

   hook = config_hook;
   prev = &config_hook;

   while (hook) {
      if (ustricmp(section_name, hook->section) == 0) {
	 if ((intgetter) || (stringgetter) || (stringsetter)) {
	    /* modify existing hook */
	    hook->intgetter = intgetter;
	    hook->stringgetter = stringgetter;
	    hook->stringsetter = stringsetter;
	 }
	 else {
	    /* remove a hook */
	    *prev = hook->next;
	    free(hook->section);
	    free(hook);
	 }

	 return;
      }

      prev = &hook->next;
      hook = hook->next;
   }

   /* add a new hook */
   hook = malloc(sizeof(CONFIG_HOOK));
   if (!hook)
      return;

   hook->section = ustrdup(section_name);
   if (!(hook->section)) {
      free(hook);
      return;
   }

   hook->intgetter = intgetter;
   hook->stringgetter = stringgetter;
   hook->stringsetter = stringsetter;

   hook->next = config_hook;
   config_hook = hook;
}



/* is_config_hooked:
 *  Checks whether a specific section is hooked in any way.
 */
int config_is_hooked(AL_CONST char *section)
{
   CONFIG_HOOK *hook = config_hook;
   char section_name[256];

   prettify_section_name(section, section_name, sizeof(section_name));

   while (hook) {
      if (ustricmp(section_name, hook->section) == 0)
	 return TRUE;

      hook = hook->next;
   }

   return FALSE;
}



/* find_config_string:
 *  Helper for finding an entry in the configuration file.
 */
static CONFIG_ENTRY *find_config_string(CONFIG *config, AL_CONST char *section, AL_CONST char *name, CONFIG_ENTRY **prev)
{
   CONFIG_ENTRY *p;
   int in_section;

   if (config) {
      p = config->head;

      if (prev)
	 *prev = NULL;

      if (section && ugetc(section))
         in_section = FALSE;
      else
         in_section = TRUE;

      while (p) {
	 if (p->name) {
	    if ((section) && (ugetc(p->name) == '[') && (ugetat(p->name, -1) == ']')) {
	       /* change section */
	       in_section = (ustricmp(section, p->name) == 0);
	    }
	    if ((in_section) || (ugetc(name) == '[')) {
	       /* is this the one? */
	       if (ustricmp(p->name, name) == 0)
		  return p;
	    }
	 }

	 if (prev)
	    *prev = p;

	 p = p->next;
      }
   }

   return NULL;
}



/* get_config_string:
 *  Reads a string from the configuration file.
 */
AL_CONST char *get_config_string(AL_CONST char *section, AL_CONST char *name, AL_CONST char *def)
{
   char section_name[256];
   CONFIG_HOOK *hook;
   CONFIG_ENTRY *p;

   init_config(TRUE);

   prettify_section_name(section, section_name, sizeof(section_name));

   /* check for hooked sections */
   hook = config_hook;

   while (hook) {
      if (ustricmp(section_name, hook->section) == 0) {
	 if (hook->stringgetter)
	    return hook->stringgetter(name, def);
	 else
	    return def;
      }
      hook = hook->next;
   }

   /* find the string */
   p = find_config_string(config_override, section_name, name, NULL);

   if (!p) {
      if ((ugetc(section_name) == '#') || ((ugetc(section_name) == '[') && (ugetat(section_name, 1) == '#'))) // OCR: was ugetc(name) == '#'
	 p = find_config_string(system_config, section_name, name, NULL);
      else
	 p = find_config_string(config[0], section_name, name, NULL);
   }

   if (p && p->data && (ustrlen(p->data) != 0))
      return p->data;
   else
      return def;
}



/* get_config_int:
 *  Reads an integer from the configuration file.
 */
int get_config_int(AL_CONST char *section, AL_CONST char *name, int def)
{
   CONFIG_HOOK *hook;
   char section_name[256];
   AL_CONST char *s;

   prettify_section_name(section, section_name, sizeof(section_name));

   /* check for hooked sections */
   hook = config_hook;

   while (hook) {
      if (ustricmp(section_name, hook->section) == 0) {
	 if (hook->intgetter) {
	    return hook->intgetter(name, def);
	 }
	 else if (hook->stringgetter) {
	    s = hook->stringgetter(name, NULL);
	    if ((s) && (ugetc(s)))
	       return ustrtol(s, NULL, 0);
	    else
	       return def;
	 }
	 else
	    return def;
      }
      hook = hook->next;
   }

   /* read normal data */
   s = get_config_string(section_name, name, NULL);

   if ((s) && (ugetc(s)))
      return ustrtol(s, NULL, 0);

   return def;
}



/* get_config_hex:
 *  Reads a hexadecimal integer from the configuration file.
 */
int get_config_hex(AL_CONST char *section, AL_CONST char *name, int def)
{
   AL_CONST char *s = get_config_string(section, name, NULL);
   char tmp[64];
	long i;

   if ((s) && (ugetc(s))) {
      i = ustrtol(s, NULL, 16);
      if ((i == 0x7FFFFFFF) && (ustricmp(s, uconvert_ascii("7FFFFFFF", tmp)) != 0))
	 i = -1;
      return i;
   }

   return def;
}



/* get_config_float:
 *  Reads a float from the configuration file.
 */
float get_config_float(AL_CONST char *section, AL_CONST char *name, float def)
{
   AL_CONST char* s = get_config_string(section, name, NULL);

   if ((s) && (ugetc(s)))
      return uatof(s);

   return def;
}



/* get_config_id:
 *  Reads a driver ID number from the configuration file.
 */
int get_config_id(AL_CONST char *section, AL_CONST char *name, int def)
{
   AL_CONST char *s = get_config_string(section, name, NULL);
   char tmp[4];
   char* endp;
   int val, i;

   if ((s) && (ugetc(s))) {
      val = ustrtol(s, &endp, 0);
      if (!ugetc(endp))
	 return val;

      tmp[0] = tmp[1] = tmp[2] = tmp[3] = ' ';

      for (i=0; i<4; i++) {
	 if (ugetat(s, i))
	    tmp[i] = utoupper(ugetat(s ,i));
	 else
	    break;
      }

      return AL_ID(tmp[0], tmp[1], tmp[2], tmp[3]);
   }

   return def;
}



/* get_config_argv:
 *  Reads an argc/argv style token list from the configuration file.
 */
char **get_config_argv(AL_CONST char *section, AL_CONST char *name, int *argc)
{
   #define MAX_ARGV  16

   static char *buf = NULL;
   static int buf_size = 0;
   static char *argv[MAX_ARGV];
   int pos, ac, q, c;
   int s_size;

   AL_CONST char *s = get_config_string(section, name, NULL);

   if (!s) {
      *argc = 0;
      return NULL;
   }

   /* increase the buffer size if needed */
   s_size = ustrsizez(s);
   if (s_size>buf_size) {
      buf_size = s_size;
		buf = realloc(buf, buf_size);
      if (!buf) {
	 errno = ENOMEM;
	 *argc = 0;
	 return NULL;
      }
   }

   ustrzcpy(buf, buf_size, s);
   pos = 0;
   ac = 0;

   c = ugetc(buf);

   while ((ac<MAX_ARGV) && (c) && (c != '#')) {
      while ((c) && (uisspace(c))) {
	 pos += ucwidth(c);
	 c = ugetc(buf+pos);
      }

      if ((c) && (c != '#')) {
	 if ((c == '\'') || (c == '"')) {
	    q = c;
	    pos += ucwidth(c);
	    c = ugetc(buf+pos);
	 }
	 else
	    q = 0;

	 argv[ac++] = buf+pos;

	 while ((c) && ((q) ? (c != q) : (!uisspace(c)))) {
	    pos += ucwidth(c);
	    c = ugetc(buf+pos);
	 }

	 if (c) {
	    usetat(buf+pos, 0, 0);
	    pos += ucwidth(0);
	    c = ugetc(buf+pos);
	 }
      }
   }

   *argc = ac;
   return argv;
}



/* insert_variable:
 *  Helper for inserting a new variable into a configuration file.
 */
static CONFIG_ENTRY *insert_variable(CONFIG *the_config, CONFIG_ENTRY *p, AL_CONST char *name, AL_CONST char *data)
{
   CONFIG_ENTRY *n = malloc(sizeof(CONFIG_ENTRY));

   if (!n)
      return NULL;

   if (name)
      n->name = ustrdup(name);
   else
      n->name = NULL;

   if (data)
      n->data = ustrdup(data);
   else
      n->data = NULL;

   if (p) {
      n->next = p->next;
      p->next = n; 
   }
   else {
      n->next = NULL;
      the_config->head = n;
   }

   return n;
}



/* set_config_string:
 *  Writes a string to the configuration file.
 */
void set_config_string(AL_CONST char *section, AL_CONST char *name, AL_CONST char *val)
{
   CONFIG *the_config;
   CONFIG_HOOK *hook;
   CONFIG_ENTRY *p, *prev;
   char section_name[256];

   init_config(TRUE);

   prettify_section_name(section, section_name, sizeof(section_name));

   /* check for hooked sections */
   hook = config_hook;

   while (hook) {
      if (ustricmp(section_name, hook->section) == 0) {
	 if (hook->stringsetter)
	    hook->stringsetter(name, val);
	 return;
      }
      hook = hook->next;
   }

   /* decide which config file to use */
   if ((ugetc(name) == '#') || ((ugetc(section_name) == '[') && (ugetat(section_name, 1) == '#')))
      the_config = system_config;
   else
      the_config = config[0];

   if (the_config) {
      p = find_config_string(the_config, section_name, name, &prev);

      if (p) {
	 if ((val) && (ugetc(val))) {
	    /* modify existing variable */
	    if (p->data)
	       free(p->data);

	    p->data = ustrdup(val);
	 }
	 else {
	    /* delete variable */
	    if (p->name)
	       free(p->name);

	    if (p->data)
	       free(p->data);

	    if (prev)
	       prev->next = p->next;
	    else
	       the_config->head = p->next;

	    free(p);
	 }
      }
      else {
	 if ((val) && (ugetc(val))) {
	    /* add a new variable */
	    if (ugetc(section_name)) {
	       p = find_config_string(the_config, NULL, section_name, &prev);

	       if (!p) {
		  /* create a new section */
		  p = the_config->head;
		  while ((p) && (p->next))
		     p = p->next;

		  if ((p) && (p->data) && (ugetc(p->data)))
		     p = insert_variable(the_config, p, NULL, NULL);

		  p = insert_variable(the_config, p, section_name, NULL);
	       }

	       /* append to the end of the section */
	       while ((p) && (p->next) && 
		      (((p->next->name) && (ugetc(p->next->name))) || 
		       ((p->next->data) && (ugetc(p->next->data)))))
		  p = p->next;

	       p = insert_variable(the_config, p, name, val);
	    }
	    else {
	       /* global variable */
	       p = the_config->head;
	       insert_variable(the_config, NULL, name, val);
	       the_config->head->next = p;
	    }
	 } 
      }

      the_config->dirty = TRUE;
   }
}



/* set_config_int:
 *  Writes an integer to the configuration file.
 */
void set_config_int(AL_CONST char *section, AL_CONST char *name, int val)
{
   char buf[32], tmp[32];
   uszprintf(buf, sizeof(buf), uconvert_ascii("%d", tmp), val);
   set_config_string(section, name, buf);
}



/* set_config_hex:
 *  Writes a hexadecimal integer to the configuration file.
 */
void set_config_hex(AL_CONST char *section, AL_CONST char *name, int val)
{
   char buf[32], tmp[32];

   if (val >= 0) {
      uszprintf(buf, sizeof(buf), uconvert_ascii("%X", tmp), val);
      set_config_string(section, name, buf);
   }
   else
      set_config_string(section, name, uconvert_ascii("-1", buf));
}



/* set_config_float:
 *  Writes a float to the configuration file.
 */
void set_config_float(AL_CONST char *section, AL_CONST char *name, float val)
{
   char buf[32], tmp[32];
   uszprintf(buf, sizeof(buf), uconvert_ascii("%f", tmp), val);
   set_config_string(section, name, buf);
}



/* set_config_id:
 *  Writes a driver ID to the configuration file.
 */
void set_config_id(AL_CONST char *section, AL_CONST char *name, int val)
{
   char buf[32], tmp[32];
   int v[4];
   int pos = 0;
   int i;

   if (val < 256) {
      uszprintf(buf, sizeof(buf), uconvert_ascii("%d", tmp), val);
   }
   else {
      v[0] = (val>>24)&0xFF;
      v[1] = (val>>16)&0xFF;
      v[2] = (val>>8)&0xFF;
      v[3] = val&0xFF;

      for (i=0; (i<4) && (v[i]) && (v[i] != ' '); i++)
	 pos += usetc(buf+pos, v[i]);

      usetc(buf+pos, 0);
   }

   set_config_string(section, name, buf);
}

#endif
