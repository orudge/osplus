/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2001. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Platform-specific sound-related functions                 */
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

/* Revision History:
 *
 * 24/12/2000: Created
 * 04/02/2001: Added MIDI support and included different source files
 *             for different platforms
 */

/* Platforms: __DJGPP__              - DJGPP
              DJGPP_NO_SOUND_SUPPORT - DJGPP with no sound support
              __WIN32__              - Win32
              __LINUX__              - Linux
              LINUX_NO_SOUND_SUPPORT - Linux with no sound support
              none of the above      - Real-mode DOS (Borland C++)
 */

#ifdef __DJGPP__
   #include "djgpp.c"
#endif

#ifdef __LINUX__
   #include "djgpp.c"
#endif

#ifdef __WIN32__
   #include "win32.c"
#endif

#ifdef __MSDOS__
   #include "realdos.c"
#endif