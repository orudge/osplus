/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2001. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor                                        */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* File converter management routines                        */
/* Based on Allegro's readbmp.c                              */
/*************************************************************/

/* UNDER CONSTRUCTION! */

#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <stdio.h>

#ifdef __WIN32__
	#include <windows.h>
#endif

#include "write.h"
#include "rtferror.h"

#include "convert.h"

static CONVERTER_INFO *converter_type_list = NULL;

/* register_converter_file_type:
 *  Registers a new converter.
 */
void register_converter_file_type(char *ext, char *load_fn)
{
	char tmp[32], *aext;
	CONVERTER_INFO *iter = converter_type_list;

	aext = ext;
	if (strlen(aext) == 0) return;

	if (!iter)
		iter = converter_type_list = (CONVERTER_INFO *) malloc(sizeof(struct CONVERTER_INFO));
	else
	{
		for (iter = converter_type_list; iter->next; iter = iter->next);
		iter = iter->next = (CONVERTER_INFO *) malloc(sizeof(struct CONVERTER_INFO));
	}

	if (iter)
	{
		iter->load_fn = load_fn;
		iter->ext = strdup(aext);
		iter->next = NULL;
	}
}


/* convert_text_file:
 *  Converts a file to plain text. Give filename_in and a pointer to a buffer
 *  in filename_out.
 */
int convert_text_file(char *filename_in, char *filename_out, char *error_out, int *delete_file)
{
	char tmp[16], *aext;
	CONVERTER_INFO *iter;

	aext = strrchr(filename_in, '.');
	delete_file = 0;

	if (aext == NULL)
	{
		strcpy(filename_out, filename_in);
		delete_file = 0;

		return(0);
	}
	else
		aext++; // point to extension without the dot

	for (iter = converter_type_list; iter; iter = iter->next)
	{
		if (stricmp(iter->ext, aext) == 0)
		{
			if (iter->load_fn)
				return(convert_file(filename_in, filename_out, iter->load_fn, error_out, delete_file));
		}
	}

	strcpy(filename_out, filename_in);
	delete_file = 0;

	return(NULL);
}

#ifdef __WIN32__  // Win32 uses DLLs - generally better
int convert_file(char *fn_in, char *fn_out, char *converter, char *error_out, int *delete_file)
{
	HINSTANCE hInstDLL;
   int (*ConvertProc)(char *src, char *dest, char *error);
   char error_buf[200];
	char tmp_dest[MAX_PATH];
	int ret;

	hInstDLL = LoadLibrary(converter);

	if (hInstDLL == NULL)
	{
		strcpy(error_out, "Unable to load converter. This file\nwill not be converted.");
		strcpy(fn_out, fn_in);
		*delete_file = 0;
		return(0);
	}

	ConvertProc = (FARPROC) GetProcAddress(hInstDLL, "ConvertProc");

	if (ConvertProc == NULL)
	{
		strcpy(error_out, "Unable to load converter. This file\nwill not be converted.");
		strcpy(fn_out, fn_in);
		*delete_file = 0;
		return(0);
	}

	// Generate temporary filename
	tmpnam(tmp_dest);

	strcpy(fn_out, tmp_dest);

	// Run the converter
	if (ret = ConvertProc(fn_in, tmp_dest, error_buf) != 0) // error
	{
		strcpy(error_out, error_buf);
		FreeLibrary(hInstDLL);
		return(1);
	}

	// Unload DLL
	FreeLibrary(hInstDLL);

	*delete_file = 1;
	return(ret);
}
#else
int convert_file(char *fn_in, char *fn_out, char *converter, char *error_out, int *delete_file)
{
	char tmp_dest[200];
	int tmpret;

	// Generate temporary filename
	tmpnam(tmp_dest);

	strcpy(fn_out, tmp_dest);
	*delete_file = 1;

	// Not a very good way of checking errors, but it'll do for now.
#ifdef __LINUX__
	char tmp[200];

	sprintf(tmp, "./%s \"%s\" \"%s\"", converter, fn_in, tmp_dest);
	system(tmp);
#else
	switch(tmpret = spawnl(P_WAIT, converter, converter, fn_in, tmp_dest, NULL))
	{
		case ERROR_RTF_CANNOT_OPEN_SRC:
			error_out = "Unable to open source file";
			break;
		case ERROR_RTF_CANNOT_OPEN_DEST:
			error_out = "Unable to open temporary destination file.";
			break;
		case ERROR_WRI_UNABLE_OPEN_SRC:
			error_out = "Unable to open source file.";
			break;
		case ERROR_WRI_UNABLE_OPEN_DEST:
			error_out = "Unable to open temporary destination file.";
			break;
		case ERROR_NOT_WRITE_FILE:
			error_out = "This file is not a Write file. Displaying contents anyway.";

			strcpy(fn_out, fn_in);
			*delete_file = 0;
			break;

		default:
			if (tmpret >= ERROR_RTF_PARSE_ERROR)
				sprintf(error_out, "Error %d while parsing RTF file", tmpret - ERROR_RTF_PARSE_ERROR);
			break;
	}
#endif

	return(tmpret);
}
#endif