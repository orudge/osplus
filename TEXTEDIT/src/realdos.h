/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* DOS-specific routines                                     */
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

#ifdef __cplusplus
   extern "C" {
#endif

#if defined(__MSDOS__) && !defined(WITH_ALLEG)
#define AL_ID(a,b,c,d)     (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

#define OSTYPE_UNKNOWN     0
#define OSTYPE_WIN3        AL_ID('W','I','N','3')
#define OSTYPE_WIN95       AL_ID('W','9','5',' ')
#define OSTYPE_WIN98       AL_ID('W','9','8',' ')
#define OSTYPE_WINME       AL_ID('W','M','E',' ')
#define OSTYPE_WINNT       AL_ID('W','N','T',' ')
#define OSTYPE_WIN2000     AL_ID('W','2','K',' ')
#define OSTYPE_WINXP       AL_ID('W','X','P',' ')
#define OSTYPE_OS2         AL_ID('O','S','2',' ')
#define OSTYPE_WARP        AL_ID('W','A','R','P')
#define OSTYPE_DOSEMU      AL_ID('D','E','M','U')
#define OSTYPE_OPENDOS     AL_ID('O','D','O','S')
#define OSTYPE_LINUX       AL_ID('T','U','X',' ')
#define OSTYPE_SUNOS       AL_ID('S','U','N',' ')
#define OSTYPE_FREEBSD     AL_ID('F','B','S','D')
#define OSTYPE_NETBSD      AL_ID('N','B','S','D')
#define OSTYPE_IRIX        AL_ID('I','R','I','X')
#define OSTYPE_DARWIN      AL_ID('D','A','R','W')
#define OSTYPE_QNX         AL_ID('Q','N','X',' ')
#define OSTYPE_UNIX        AL_ID('U','N','I','X')
#define OSTYPE_BEOS        AL_ID('B','E','O','S')
#define OSTYPE_MACOS       AL_ID('M','A','C',' ')
#define OSTYPE_MACOSX      AL_ID('M','A','C','X')

extern int os_type;
#endif

#ifdef __cplusplus
   }
#endif
