/**********************************************************/
/* OSPlus Utility Kit 1.1                                 */
/* Copyright (c) Owen Rudge 2000. All Rights Reserved.    */
/**********************************************************/
/* OSPlus Text Editor - Standalone                        */
/* OSPEDIT.EXE                                            */
/**********************************************************/
/* Platform-specific sound-related functions              */
/**********************************************************/

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
 */

/* Platforms: __DJGPP__              - DJGPP
              DJGPP_NO_SOUND_SUPPORT - DJGPP with no sound support
              __WIN32__              - Win32
              __LINUX__              - Linux
              LINUX_NO_SOUND_SUPPORT - Linux with no sound support
              none of the above      - Real-mode DOS (Borland C++)
 */

#include <stdio.h>

typedef int BOOL;

#ifndef TRUE
   #define TRUE   -1
   #define FALSE  0
#endif

#ifdef __MSDOS__
   #include "gemsnd.h"
#endif

#ifdef __WIN32__
   #include <windows.h>
#endif

#if defined(__DJGPP__) && !defined(DJGPP_NO_SOUND_SUPPORT)
   #include <allegro.h>

   SAMPLE *waveSample;
#endif

#if defined(__LINUX__) && !defined(LINUX_NO_SOUND_SUPPORT)
   #include <allegro.h>

   SAMPLE *waveSample;
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

BOOL SoundEnabled = FALSE;

void snd_LoadWAV()
{
	WAVLoaded = TRUE;

#if defined(__DJGPP__) || defined(__LINUX__)
   #ifdef ALLEGRO_H
	destroy_sample(waveSample);
	waveSample = load_sample(WAVName);
   #endif
#endif
}

void snd_PlayWAV()
{
	if (WAVLoaded == TRUE)
	{
#if defined(__DJGPP__) || defined(__LINUX__)
   #ifdef ALLEGRO_H
		play_sample(waveSample, 255, 128, 1000, FALSE);
   #endif
#else
 #ifdef __WIN32__
		sndPlaySound(WAVName, SND_ASYNC | SND_NODEFAULT);
 #else
		if (isPlaying() == TRUE)  StopWAV();
			PlayIWAV(WAVName);
 #endif
#endif
	}
}

void snd_StopWAV()
{
	if (WAVLoaded == TRUE)
#if defined(__DJGPP__) || defined(__LINUX__)
   #ifdef ALLEGRO_H
		stop_sample(waveSample);
   #else
                SoundEnabled = SoundEnabled;
   #endif
#else
 #ifdef __WIN32__
		sndPlaySound(NULL, SND_SYNC | SND_NODEFAULT);
 #else
		if (isPlaying() == TRUE)  StopWAV();
 #endif
#endif
}

void snd_Init()
{
#if defined(DJGPP_NO_SOUND_SUPPORT) || defined(LINUX_NO_SOUND_SUPPORT)
	 SoundEnabled = FALSE;
#else
 #if defined(__DJGPP__) || defined(__LINUX__)
	 allegro_init();

	 if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") == -1)
		 SoundEnabled = FALSE;
	 else
		 SoundEnabled = TRUE;
 #else
  #ifdef __WIN32__
    if (waveInGetNumDevs() == 0)
       SoundEnabled = FALSE;
    else
       SoundEnabled = TRUE;
  #else
    if (getenv("OSPSOUND") != NULL) // only play sounds if OSPSOUND = TRUE
    {                               // this is really just a 'safety net'
       SpeakerOn();                 // as if Heinz's drivers are not loaded
       SoundEnabled = TRUE;         // it causes a pretty bad crash.
    }
  #endif
 #endif
#endif
}

void snd_Exit()
{
  #if defined(__DJGPP__) || defined(__LINUX__)
     #ifdef ALLEGRO_H
       destroy_sample(waveSample);
     #endif        
  #else
     #if defined(__WIN32__) 
        sndPlaySound(NULL, SND_NODEFAULT);
     #else
        if (SoundEnabled == TRUE)
        {
			  StopWAV();
           SpeakerOff();
        }
     #endif
  #endif
}
