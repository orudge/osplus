/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* Microsoft -> OSPlus conversion layer                      */
/* MSCONV.CNV                                                */
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

/* NOTE: This program needs improved error checking, etc */

/* Revision History (as of 27/12/2004):
 *
 * 27/12/2004: Integrated with new converter system (orudge)
 * 27/12/2004: Fixed bug in WriteOutStatic where a crash would occur if cch = 0  (orudge)
 *
 */

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
//#include <io.h>

#include "convert.h"

#define ERR_INITFAILED       1000
#define ERR_FORMATINCORRECT  1001
#define ERR_CONVFAILED       1002

//#define CNV_PROGRESS         WM_USER + 100

typedef int (CALLBACK *PFN_RTF_CALLBACK)(int, int);

int WINAPI InitConverter(HWND, char *);
/* Window handle, app name */

HANDLE WINAPI RegisterApp(unsigned long, void *);
/* Register app with converter */

int WINAPI IsFormatCorrect(HGLOBAL, HGLOBAL);
/* Filename, Description (returned) */

int WINAPI ForeignToRtf(HANDLE, void *, HANDLE, HANDLE, HANDLE, PFN_RTF_CALLBACK);

int CALLBACK WriteOutStatic(int cch, int nPercentComplete);

typedef int (CALLBACK *LPFNOUT)(int cch, int nPercentComplete);
typedef BOOL (FAR PASCAL *PINITCONVERTER)(HWND hWnd, LPCSTR lpszModuleName);
typedef BOOL (FAR PASCAL *PISFORMATCORRECT)(HANDLE ghszFile, HANDLE ghszClass);
typedef int (FAR PASCAL *PFOREIGNTORTF)(HANDLE ghszFile, LPVOID lpv, HANDLE ghBuff,
	HANDLE ghszClass, HANDLE ghszSubset, LPFNOUT lpfnOut);

HINSTANCE m_hLibCnv;
PINITCONVERTER m_pInitConverter;
PISFORMATCORRECT m_pIsFormatCorrect;
PFOREIGNTORTF m_pForeignToRtf;

HINSTANCE hInst;
HWND hWnd;
FILE *hndl;

HGLOBAL m_hBuff;

BOOL CNV_Init(LPSTR lpDLLName)
{
   m_hLibCnv = LoadLibrary(lpDLLName);

   if (m_hLibCnv < (HINSTANCE)HINSTANCE_ERROR)
   {
      m_hLibCnv = NULL;
      return(FALSE);
   }
   else
   {
      m_pInitConverter = (PINITCONVERTER)GetProcAddress(m_hLibCnv, "InitConverter32");
      m_pIsFormatCorrect = (PISFORMATCORRECT)GetProcAddress(m_hLibCnv, "IsFormatCorrect32");
      m_pForeignToRtf = (PFOREIGNTORTF)GetProcAddress(m_hLibCnv, "ForeignToRtf32");

      if (m_pInitConverter == NULL)
            return(FALSE);
      else
      {
            m_pInitConverter(NULL, "OSPlus Text Editor");
            return(TRUE);
      }
   }
}

BOOL CNV_ForeignToRTF(LPSTR lpFileName, LPSTR lpDest)
{
   HGLOBAL hFileName = GlobalAlloc(GHND, 256);
   HGLOBAL hDesc = GlobalAlloc(GHND, 256);
   HGLOBAL hSubset = GlobalAlloc(GHND, 256);
   LPSTR buf;

   m_hBuff = GlobalAlloc(GHND, 4096);

   buf = GlobalLock(hFileName);
   lstrcpy(buf, lpFileName);
   GlobalUnlock(hFileName);

   hndl = fopen(lpDest, "wt");

   if (hndl == NULL)
      return(FALSE);

   if (m_pForeignToRtf == NULL)
      return(FALSE);
   else
      m_pForeignToRtf(hFileName, NULL, m_hBuff, hDesc, hSubset, (PFN_RTF_CALLBACK) WriteOutStatic);

   GlobalFree(hFileName);
   GlobalFree(hDesc);
   GlobalFree(hSubset);

   GlobalFree(m_hBuff);
   fclose(hndl);

   return(TRUE);
}

BOOL CNV_IsFormatCorrect(LPSTR lpFileName)
{
   int nRet;
   HGLOBAL hFileName;
   HGLOBAL hDesc = GlobalAlloc(GHND, 256);
   LPSTR buf;

   hFileName = GlobalAlloc(GHND, 256);

   buf = GlobalLock(hFileName);
   lstrcpy(buf, lpFileName);
   GlobalUnlock(hFileName);

   if (m_pIsFormatCorrect == NULL)
      return(FALSE);

   nRet = m_pIsFormatCorrect(hFileName, hDesc);
   GlobalFree(hDesc);
   GlobalFree(hFileName);

   return (nRet == 1) ? TRUE : FALSE;
}

int CALLBACK WriteOutStatic(int cch, int nPercentComplete)
{
   LPSTR buf;

//   SendMessage(hWnd, CNV_PROGRESS, (WPARAM) nPercentComplete, NULL);
	//printf("cch = %d, nPercentComplete = %d\n", cch, nPercentComplete);
   buf = GlobalLock(m_hBuff);

   if (cch > 0)
      fwrite(buf, cch-1, 1, hndl);

   GlobalUnlock(m_hBuff);

   return(0);
}

int main(int argc, char *argv[])
{
   if (argc != 4)  // msconv write32.cnv input.wri output.rtf
      return(-1);

//   hWnd = (HWND) atol(argv[1]);

   if (CNV_Init(argv[1]) == FALSE)
      return(ERROR_CNV_INIT_FAILED);

   if (CNV_IsFormatCorrect(argv[2]) == FALSE)
      return(ERROR_CNV_INCORRECT_FORMAT);

   if (CNV_ForeignToRTF(argv[2], argv[3]) == FALSE)
      return(ERROR_CNV_FAILED);

   return(ERROR_CNV_FROM_RTF);
}
