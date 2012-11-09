/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Platform-specific sound-related functions - Real-mode DOS */
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
 * 04/02/2001: Created file
 * 31/12/2004: Improvements to version detection under DOS (orudge)
 */

#ifndef __DJGPP__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

#define _get_dos_version(n)      ((_osmajor << 8) | _osminor)

typedef int BOOL;

#ifndef TRUE
   #define TRUE   -1
	#define FALSE  0
#endif

#include "realdos.h"

#ifndef MAXPATH
	#ifdef PATH_MAX
      #define MAXPATH PATH_MAX
   #else
      #define MAXPATH  200
   #endif
#endif

int os_type = 0;

/* detect_os:
 *  Operating system autodetection routine. Nicked from Allegro.
 */
static void detect_os(void)
{
	union REGS r;
	char *p;
	int os_version, os_revision;

	os_type = OSTYPE_UNKNOWN;

	/* check for Windows 3.x or 9x */
	r.x.ax = 0x1600;
	int86(0x2F, &r, &r);

	if ((r.h.al != 0) && (r.h.al != 1) && (r.h.al != 0x80) && (r.h.al != 0xFF)) {
		os_version = r.h.al;
		os_revision = r.h.ah;

		if (os_version == 4) {
			if (os_revision == 90)
				os_type = OSTYPE_WINME;
			else if (os_revision == 10)
				os_type = OSTYPE_WIN98;
			else
				os_type = OSTYPE_WIN95;
		}
		else
			os_type = OSTYPE_WIN3;

		return;
	}

	/* check for Windows NT */
	p = getenv("OS");

	if (((p) && (stricmp(p, "Windows_NT") == 0)) || (_get_dos_version(1) == 0x0532)) {
		os_type = OSTYPE_WINNT;
		return;
	}

	/* check for OS/2 */
	r.x.ax = 0x4010;
	int86(0x2F, &r, &r);

	if (r.x.ax != 0x4010) {
		if (r.x.ax == 0)
			os_type = OSTYPE_WARP;
		else
			os_type = OSTYPE_OS2;
		return;
	}

	/* check if running under OpenDOS */
	r.x.ax = 0x4452;
	int86(0x21, &r, &r);

	if ((r.x.ax >= 0x1072) && !(r.x.flags & 1)) {
		os_type = OSTYPE_OPENDOS;
		return;
	}

	/* check for that stupid win95 "stealth mode" setting */
	r.x.ax = 0x8102;
	r.x.bx = 0;
	r.x.dx = 0;
	int86(0x4B, &r, &r);

	if ((r.x.bx == 3) && !(r.x.flags & 1)) {
		os_type = OSTYPE_WIN95;
		return;
	}

	/* fetch DOS version if pure DOS is likely to be the running OS */
	if (os_type == OSTYPE_UNKNOWN) {
		r.x.ax = 0x3000;
		int86(0x21, &r, &r);
		os_version = r.h.al;
		os_revision = r.h.ah;
	}
}

#endif /* #ifndef __DJGPP__ */

