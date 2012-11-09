/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Unicode handling routines (based on Allegro code)         */
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

#include <stdarg.h>

#ifndef TRUE 
   #define TRUE         -1
   #define FALSE        0
#endif

#undef MIN
#undef MAX
#undef MID
     
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))

#define AL_ID(a,b,c,d)     (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

#ifndef AL_CONST
   #define AL_CONST
#endif

#ifndef AL_VAR
   #define AL_VAR(type, name)                      extern type name
#endif

#ifndef AL_ARRAY
   #define AL_ARRAY(type, name)                    extern type name[]
#endif

#ifndef AL_FUNC
   #define AL_FUNC(type, name, args)               type name args
#endif

#ifndef AL_PRINTFUNC
   #define AL_PRINTFUNC(type, name, args, a, b)    AL_FUNC(type, name, args)
#endif

#ifndef AL_METHOD
   #define AL_METHOD(type, name, args)             type (*name) args
#endif

#ifndef AL_FUNCPTR
   #define AL_FUNCPTR(type, name, args)            extern type (*name) args
#endif

#ifndef AL_INLINE
   #define AL_INLINE(type, name, args, code)       type name args;
#endif

#define U_ASCII         AL_ID('A','S','C','8')
#define U_ASCII_CP      AL_ID('A','S','C','P')
#define U_UNICODE       AL_ID('U','N','I','C')
#define U_UTF8          AL_ID('U','T','F','8')
#define U_CURRENT       AL_ID('c','u','r','.')

char *get_filename(AL_CONST char *path);
char *replace_filename(char *dest, AL_CONST char *path, AL_CONST char *filename, size_t size);

AL_FUNC(void, set_uformat, (int type));
AL_FUNC(int, get_uformat, (void));
AL_FUNC(void, register_uformat, (int type, AL_METHOD(int, u_getc, (AL_CONST char *s)), AL_METHOD(int, u_getx, (char **s)), AL_METHOD(int, u_setc, (char *s, int c)), AL_METHOD(int, u_width, (AL_CONST char *s)), AL_METHOD(int, u_cwidth, (int c)), AL_METHOD(int, u_isok, (int c)), int u_width_max));
AL_FUNC(void, set_ucodepage, (AL_CONST unsigned short *table, AL_CONST unsigned short *extras));

AL_FUNC(int, need_uconvert, (AL_CONST char *s, int type, int newtype));
AL_FUNC(int, uconvert_size, (AL_CONST char *s, int type, int newtype));
AL_FUNC(void, do_uconvert, (AL_CONST char *s, int type, char *buf, int newtype, int size));
AL_FUNC(char *, uconvert, (AL_CONST char *s, int type, char *buf, int newtype, int size));
AL_FUNC(int, uwidth_max, (int type));

#define uconvert_ascii(s, buf)      uconvert(s, U_ASCII, buf, U_CURRENT, sizeof(buf))
#define uconvert_toascii(s, buf)    uconvert(s, U_CURRENT, buf, U_ASCII, sizeof(buf))

#define EMPTY_STRING    "\0\0\0"

AL_ARRAY(char, empty_string);

AL_FUNCPTR(int, ugetc, (AL_CONST char *s));
AL_FUNCPTR(int, ugetx, (char **s));
AL_FUNCPTR(int, ugetxc, (AL_CONST char **s));
AL_FUNCPTR(int, usetc, (char *s, int c));
AL_FUNCPTR(int, uwidth, (AL_CONST char *s));
AL_FUNCPTR(int, ucwidth, (int c));
AL_FUNCPTR(int, uisok, (int c));
AL_FUNC(int, uoffset, (AL_CONST char *s, size_t index));
AL_FUNC(int, ugetat, (AL_CONST char *s, size_t index));
AL_FUNC(int, usetat, (char *s, int index, int c));
AL_FUNC(int, uinsert, (char *s, int index, int c));
AL_FUNC(int, uremove, (char *s, int index));
AL_FUNC(int, utolower, (int c));
AL_FUNC(int, utoupper, (int c));
AL_FUNC(int, uisspace, (int c));
AL_FUNC(int, uisdigit, (int c));
AL_FUNC(int, ustrsize, (AL_CONST char *s));
AL_FUNC(int, ustrsizez, (AL_CONST char *s));
AL_FUNC(char *, _ustrdup, (AL_CONST char *src, AL_METHOD(void *, malloc_func, (size_t))));
AL_FUNC(char *, ustrzcpy, (char *dest, size_t size, AL_CONST char *src));
AL_FUNC(char *, ustrzcat, (char *dest, int size, AL_CONST char *src));
AL_FUNC(int, ustrlen, (AL_CONST char *s));
AL_FUNC(int, ustrcmp, (AL_CONST char *s1, AL_CONST char *s2));
AL_FUNC(char *, ustrzncpy, (char *dest, size_t size, AL_CONST char *src, size_t n));
AL_FUNC(char *, ustrzncat, (char *dest, int size, AL_CONST char *src, int n));
AL_FUNC(int, ustrncmp, (AL_CONST char *s1, AL_CONST char *s2, int n));
AL_FUNC(int, ustricmp, (AL_CONST char *s1, AL_CONST char *s2));
AL_FUNC(char *, ustrlwr, (char *s));
AL_FUNC(char *, ustrupr, (char *s));
AL_FUNC(char *, ustrchr, (AL_CONST char *s, int c));
AL_FUNC(char *, ustrrchr, (AL_CONST char *s, int c));
AL_FUNC(char *, ustrstr, (AL_CONST char *s1, AL_CONST char *s2));
AL_FUNC(char *, ustrpbrk, (AL_CONST char *s, AL_CONST char *set));
AL_FUNC(char *, ustrtok, (char *s, AL_CONST char *set));
AL_FUNC(char *, ustrtok_r, (char *s, AL_CONST char *set, char **last));
AL_FUNC(double, uatof, (AL_CONST char *s));
AL_FUNC(long, ustrtol, (AL_CONST char *s, char **endp, int base));
AL_FUNC(double, ustrtod, (AL_CONST char *s, char **endp));
AL_FUNC(AL_CONST char *, ustrerror, (int err));
AL_PRINTFUNC(int, uszprintf, (char *buf, int size, AL_CONST char *format, ...), 3, 4);
AL_FUNC(int, uvszprintf, (char *buf, int size, AL_CONST char *format, va_list args));
AL_PRINTFUNC(int, usprintf, (char *buf, AL_CONST char *format, ...), 2, 3);

#ifndef ustrdup
   #define ustrdup(src)               _ustrdup(src, malloc)
#endif

#define ustrcpy(dest, src)            ustrzcpy(dest, INT_MAX, src)
#define ustrcat(dest, src)            ustrzcat(dest, INT_MAX, src)
#define ustrncpy(dest, src, n)        ustrzncpy(dest, INT_MAX, src, n)
#define ustrncat(dest, src, n)        ustrzncat(dest, INT_MAX, src, n)
#define uvsprintf(buf, format, args)  uvszprintf(buf, INT_MAX, format, args)

/* helper structure for talking to Unicode strings */
typedef struct UTYPE_INFO
{
   int id;
   AL_METHOD(int, u_getc, (AL_CONST char *s));
   AL_METHOD(int, u_getx, (char **s));
   AL_METHOD(int, u_setc, (char *s, int c));
   AL_METHOD(int, u_width, (AL_CONST char *s));
   AL_METHOD(int, u_cwidth, (int c));
   AL_METHOD(int, u_isok, (int c));
   int u_width_max;
} UTYPE_INFO;

AL_FUNC(UTYPE_INFO *, _find_utype, (int type));
#endif
