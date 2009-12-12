/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Platform-specific sound-related functions - DJGPP/Linux   */
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
 * 04/02/2001: Created and added MIDI support
 */

#include <stdio.h>

#if defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)
   #define NO_SOUND_SUPPORT
#endif

#if defined(__MSDOS__) && !defined(WITH_ALLEG)
   static void detect_os(void);
   int os_type = 0;

   #include <dpmi.h>
   #include <go32.h>
   #include <dos.h>

   #include "realdos.h"
#endif

typedef int BOOL;

#ifndef TRUE
   #define TRUE   -1
   #define FALSE  0
#endif

#if !defined(NO_SOUND_SUPPORT)
   #define USE_CONSOLE
   #include <allegro.h>

   SAMPLE *waveSample;
   MIDI *midi;
#endif

#ifndef MAXPATH
   #ifdef PATH_MAX
      #define MAXPATH PATH_MAX
   #else
      #define MAXPATH  200
   #endif
#endif

char WAVName[MAXPATH];          // file name of WAV
BOOL WAVLoaded = FALSE;         // WAV loaded?

char MIDName[MAXPATH];          // file name of WAV
BOOL MIDLoaded = FALSE;         // WAV loaded?

BOOL SoundEnabled = FALSE;

void snd_LoadWAV()
{
   WAVLoaded = TRUE;

   #ifdef ALLEGRO_H
	destroy_sample(waveSample);
	waveSample = load_sample(WAVName);
   #endif
}

void snd_PlayWAV()
{
   if (WAVLoaded == TRUE)
   {
#ifdef ALLEGRO_H
      play_sample(waveSample, 255, 128, 1000, FALSE);
#endif
   }
}

void snd_StopWAV()
{
   if (WAVLoaded == TRUE)
#ifdef ALLEGRO_H
      stop_sample(waveSample);
#else
      SoundEnabled = SoundEnabled;
#endif
}

void snd_LoadMID()
{
   MIDLoaded = TRUE;

#ifdef ALLEGRO_H
   destroy_midi(midi);
   midi = load_midi(MIDName);
#endif
}

void snd_PlayMID()
{
   if (MIDLoaded == TRUE)
   {
#ifdef ALLEGRO_H
      play_midi(midi, TRUE);
#endif
   }
}

void snd_StopMID()
{
#ifdef ALLEGRO_H
   if (MIDLoaded == TRUE)
      stop_midi();
#endif
}

void snd_Init()
{
#if defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)
   SoundEnabled = FALSE;

#ifdef __MSDOS__
   detect_os();
#endif
#else
   allegro_init();

   if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "") == -1)
      SoundEnabled = FALSE;
   else
      SoundEnabled = TRUE;
#endif
}

void snd_Exit()
{
   #ifdef ALLEGRO_H
       destroy_midi(midi);
   #endif
}

/* detect_os:
 *  Operating system autodetection routine.
 */
#ifdef __MSDOS__
static void detect_os(void)
{
   __dpmi_regs r;
   char buf[16];
   char *p;
   int i, os_version, os_revision;

   os_type = OSTYPE_UNKNOWN;

   /* check for Windows 3.x or 9x */
   r.x.ax = 0x1600; 
   __dpmi_int(0x2F, &r);

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
   __dpmi_int(0x2F, &r);

   if (r.x.ax != 0x4010) {
      if (r.x.ax == 0)
	 os_type = OSTYPE_WARP;
      else
	 os_type = OSTYPE_OS2;
      return;
   }

   /* check for Linux DOSEMU */
   _farsetsel(_dos_ds);

   for (i=0; i<8; i++) 
      buf[i] = _farnspeekb(0xFFFF5+i);

   buf[8] = 0;

   if (!strcmp(buf, "02/25/93")) {
      r.x.ax = 0;
      __dpmi_int(0xE6, &r);

      if (r.x.ax == 0xAA55) {
	 os_type = OSTYPE_DOSEMU;
	 return;
      }
   }

   /* check if running under OpenDOS */
   r.x.ax = 0x4452;
   __dpmi_int(0x21, &r);

   if ((r.x.ax >= 0x1072) && !(r.x.flags & 1)) {
      os_type = OSTYPE_OPENDOS;
      return;
   }

   /* check for that stupid win95 "stealth mode" setting */
   r.x.ax = 0x8102;
   r.x.bx = 0;
   r.x.dx = 0;
   __dpmi_int(0x4B, &r);

   if ((r.x.bx == 3) && !(r.x.flags & 1)) {
      os_type = OSTYPE_WIN95;
      return;
   }

   /* fetch DOS version if pure DOS is likely to be the running OS */
   if (os_type == OSTYPE_UNKNOWN) {
      r.x.ax = 0x3000;
      __dpmi_int(0x21, &r);
      os_version = r.h.al;
      os_revision = r.h.ah;
   }
}
#endif
