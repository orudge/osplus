/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2002. All Rights Reserved.  */
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

typedef int BOOL;

#ifndef TRUE
   #define TRUE   -1
   #define FALSE  0
#endif

#if !defined(NO_SOUND_SUPPORT)
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
