/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2002. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Platform-specific sound-related functions - Win32         */
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
 * 02/10/2001: Added midiInGetNumDevs() call to snd_Init()
 * 13/10/2001: Fixed MIDI open string to work properly with LFNs
 * 05/01/2002: Cleaned up indentation, etc
 * 13/10/2002: Fixed some compilation and runtime problems with MSVC, plus
 *             silly error in snd_LoadMID()
 */

#include <windows.h>
#include <stdio.h>

#ifndef MAXPATH
   #ifdef PATH_MAX
      #define MAXPATH PATH_MAX
   #else
      #define MAXPATH  200
   #endif
#endif

char WAVName[MAXPATH];          // file name of WAV
BOOL WAVLoaded = FALSE;         // WAV loaded?

char MIDName[MAXPATH];          // file name of MIDI
BOOL MIDLoaded = FALSE;         // MIDI loaded?

BOOL SoundEnabled = FALSE;

void snd_LoadWAV()
{
   WAVLoaded = TRUE;
}

void snd_PlayWAV()
{
   if (WAVLoaded == TRUE)
      sndPlaySound(WAVName, SND_ASYNC | SND_NODEFAULT);
}

void snd_StopWAV()
{
   if (WAVLoaded == TRUE)
       sndPlaySound(NULL, SND_SYNC | SND_NODEFAULT);
}

void snd_LoadMID()
{
   MCIERROR ret;
   char str[250];

   if (MIDLoaded == TRUE)
      mciSendString("close ospedtmid", NULL, NULL, NULL);

   wsprintf(str, "open \"%s\" type sequencer alias ospedtmid", MIDName);
   ret = mciSendString(str, NULL, NULL, NULL);

   if (ret == 0)
      MIDLoaded = TRUE;
   else
      MIDLoaded = FALSE;
}

void snd_PlayMID()
{
   mciSendString("play ospedtmid", NULL, NULL, NULL);
}

void snd_StopMID()
{
   mciSendString("stop ospedtmid", NULL, NULL, NULL);
}

void snd_Init()
{
   // Not really a sound routine, but it's in a Win32-specific file :-)
   SetConsoleTitle("OSPlus Text Editor");

   if ((waveInGetNumDevs() == 0) || midiInGetNumDevs() == 0)
      SoundEnabled = FALSE;
   else
      SoundEnabled = TRUE;
}

void snd_Exit()
{
   sndPlaySound(NULL, SND_NODEFAULT);

   if (MIDLoaded == TRUE)
   {
      mciSendString("stop ospedtmid", NULL, NULL, NULL);
      mciSendString("close ospedtmid", NULL, NULL, NULL);
   }
}
