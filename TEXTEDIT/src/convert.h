/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor - Standalone                           */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* Converter routines - header                               */
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
 * 05/01/2002: Cleaned up indentation (orudge)
 * 27/12/2004: Finished new converter system (orudge)
 */

// Converters

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct CONVERTER_INFO
{
   int id;          // ID number
   char *ext;       // File extension (eg, "RTF")
   char *load_fn;   // Converter to load file
   char *params;    // Any extra parameters to pass
   char *title;     // Title
   struct CONVERTER_INFO *next;
} CONVERTER_INFO;

int convert_file(char *fn_in, char *fn_out, char *converter, char *params, char *error_out);
void register_converter_file_type(char *ext, char *load_fn, char *params, char *title);
void register_microsoft_converters();
void register_ini_converters();
int convert_text_file(char *filename_in, char *filename_out, char *error_out, int start_from);
int get_number_of_converters();
int get_converter_info(int id, char *fn, char *ext, char *params, char *title);

#define CONVERT_DELETE_FILE           2

#define ERROR_CNV_OK                  1
#define ERROR_CNV_CANNOT_OPEN_SRC     4
#define ERROR_CNV_CANNOT_OPEN_DEST    8
#define ERROR_CNV_PARSE_ERROR         16
#define ERROR_CNV_INCORRECT_FORMAT    32
#define ERROR_CNV_FROM_RTF            64
#define ERROR_CNV_RTF_ERROR           128
#define ERROR_CNV_LOAD                256
#define ERROR_CNV_INIT_FAILED         512
#define ERROR_CNV_FAILED              1024
#define ERROR_CNV_PARSE_ERROR_RTF     2048

#ifdef WIN32_DLL
   #define CONV_EXPORT      __declspec(dllexport)
#else
   #define CONV_EXPORT
#endif

#ifdef __cplusplus
	}
#endif
