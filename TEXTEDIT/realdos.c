/*************************************************************/
/* OSPlus Utility Kit 1.2                                    */
/* Copyright (c) Owen Rudge 2000-2001. All Rights Reserved.  */
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
 */

#ifndef __DJGPP__

#include <stdio.h>

typedef int BOOL;

#ifndef TRUE
   #define TRUE   -1
   #define FALSE  0
#endif

#include "gemsnd.h"

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
}

void snd_PlayWAV()
{
   if (WAVLoaded == TRUE)
   {
      if (isPlaying() == TRUE)  StopWAV();
         PlayIWAV(WAVName);
   }
}

void snd_StopWAV()
{
   if (WAVLoaded == TRUE)
   {
      if (isPlaying() == TRUE)
         StopWAV();
   }
}

void snd_LoadMID()
{
   /* no MIDI for DOS at present */
}

void snd_PlayMID()
{

}

void snd_StopMID()
{

}

void snd_Init()
{
   if (getenv("OSPSOUND") != NULL) // only play sounds if OSPSOUND = TRUE
   {                               // this is really just a 'safety net'
      SpeakerOn();                 // as if Heinz's drivers are not loaded
      SoundEnabled = TRUE;         // it causes a pretty bad crash.
   }
}

void snd_Exit()
{
   if (SoundEnabled == TRUE)
   {
      StopWAV();
      SpeakerOff();
   }
}
#endif /* #ifndef __DJGPP__ */