/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2025. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Header to include the Turbo Vision header file            */
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

#if defined(__BORLANDC__) && defined(__REALDOS__)
	#include <tvision\tv.h>
#else
	/* DJGPP requires the SET version of Turbo Vision; this can optionally be used on Win32, Linux, macOS, etc */
	#if defined(__DJGPP__) || defined(_SET_TVISION)
		#include <tv.h>
	#else
		/* The modern magiblot port of Turbo Vision */
		#include <tvision/tv.h>
	#endif
#endif
