/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2004. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Text Editor                                        */
/* OSPEDIT.EXE                                               */
/*************************************************************/
/* File converter management routines                        */
/* Based on Allegro's readbmp.c                              */
/*************************************************************/

/* Revision History:
 *
 * 05/01/2002: Cleaned up indentation, etc (orudge)
 * 27/12/2004: Finished new converter system (orudge)
 */

#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#ifdef __WIN32__
	#include <windows.h>
#endif

#ifndef MAXPATH
	#ifdef PATH_MAX
		#define MAXPATH PATH_MAX
	#else
		#define MAXPATH  255
	#endif
#endif

#include "write.h"
#include "rtferror.h"

#include "convert.h"

static CONVERTER_INFO *converter_type_list = NULL;
static int num_converters = 0;

/* register_converter_file_type:
 *  Registers a new converter.
 */
void register_converter_file_type(char *ext, char *load_fn, char *params, char *title)
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
		iter->id = num_converters++;
		iter->load_fn = strdup(load_fn);
		iter->ext = strdup(ext);

		if (params == NULL)
			iter->params = NULL;
		else
			iter->params = strdup(params);

		iter->title = strdup(title);
		iter->next = NULL;

		fprintf(stderr, "Added '%d' '%s' '%s' '%s' '%s'\n", iter->id, iter->load_fn, iter->ext, iter->params, iter->title, iter->next);
	}
}

int get_number_of_converters()
{
 	return(num_converters);
}

int get_converter_info(int id, char *fn, char *ext, char *params, char *title)
{
	CONVERTER_INFO *iter;

	for (iter = converter_type_list; iter; iter = iter->next)
	{
		if (iter->id == id)
		{
			strcpy(fn, iter->load_fn);
			strcpy(ext, iter->ext);

			if (iter->params == NULL)
				strcpy(params, "");
			else
				strcpy(params, iter->params);

			strcpy(title, iter->title);

			return 1;
		}
	}

	return 0;
}

void register_microsoft_converters()
{
#ifdef WIN32
	LONG ret;
	HKEY hKey, hSubKey;
	BOOL stop = FALSE;
	char subkey_name[255];
	DWORD len = 255;
	FILETIME ignore;
	int i = 0;
	char extensions[255];
	char path[MAXPATH];
	char *pos, *lastpos;
	char ext_tmp[255];
	char tmplen;
	char short_path[MAXPATH];
	char name[255];
	char name2[260];

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Shared Tools\\Text Converters\\Import", 0, KEY_READ, &hKey);

	if (ret != ERROR_SUCCESS)
		return;

	do {
		len = sizeof(subkey_name);
		ret = RegEnumKeyEx(hKey, i, subkey_name, &len, NULL, NULL, NULL, &ignore);

		if (ret == ERROR_NO_MORE_ITEMS)
			stop = TRUE;
		else if (ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			stop = TRUE;
		}
		else
		{
			ret = RegOpenKeyEx(hKey, subkey_name, 0, KEY_QUERY_VALUE, &hSubKey);

			if (ret == ERROR_SUCCESS)
			{
				len = sizeof(extensions);
				ret = RegQueryValueEx(hSubKey, "Extensions", NULL, NULL, extensions, &len);

				if (ret == ERROR_SUCCESS)
				{
					len = sizeof(path);
					ret = RegQueryValueEx(hSubKey, "Path", NULL, NULL, path, &len);

					if (ret == ERROR_SUCCESS)
					{
						GetShortPathName(path, short_path, sizeof(short_path));

						if (strcmp(extensions, "*") != 0)
						{
		fprintf(stderr, "Here: '%s' '%s' '%s'\n", subkey_name, path, short_path);

							len = sizeof(name);
							ret = RegQueryValueEx(hSubKey, "Name", NULL, NULL, name, &len);

							if (ret == ERROR_SUCCESS)
								sprintf(name2, "MS: %s", name);
							else
								sprintf(name2, "MS: %s", subkey_name);

							if (strchr(extensions, ' ') == 0)
								register_converter_file_type(extensions, "msconv.cnv", short_path, name2);
							else
							{
								tmplen = strlen(extensions);

								extensions[tmplen] = ' ';  // append a space
								extensions[tmplen+1] = 0;

								lastpos = extensions;

								do {
									pos = strchr(lastpos, ' ');

									if (pos != NULL)
									{
										ZeroMemory(ext_tmp, sizeof(ext_tmp));
										pos++;
										strncpy(ext_tmp, lastpos, pos-lastpos-1);

										register_converter_file_type(ext_tmp, "msconv.cnv", short_path, name2);
									}

									lastpos = pos;
								} while (pos != NULL);
							}
						}
					}
				}

				RegCloseKey(hSubKey);
			}
		}

		i++;			
	} while (stop == FALSE);

	RegCloseKey(hKey);
#endif
}

/* convert_text_file:
 *  Converts a file to plain text. Give filename_in and a pointer to a buffer
 *  in filename_out.
 */
int convert_text_file(char *filename_in, char *filename_out, char *error_out, int start_from)
{
	char tmp[16], *aext;
	CONVERTER_INFO *iter;
	int ret, ret2;

	aext = strrchr(filename_in, '.');

	if (aext == NULL)
	{
		strcpy(filename_out, filename_in);
		return(0);
	}
	else
		aext++; // point to extension without the dot

	for (iter = converter_type_list; iter; iter = iter->next)
	{
		if ((stricmp(iter->ext, aext) == 0) && (iter->id >= start_from))
		{
			if (iter->load_fn)
			{
				char filename_out2[MAXPATH], error_out2[255];

				ret = convert_file(filename_in, filename_out, iter->load_fn, iter->params, error_out);

				if ((ret & ERROR_CNV_INCORRECT_FORMAT) || (ret & ERROR_CNV_LOAD))
				{
					ret2 = convert_text_file(filename_in, filename_out2, error_out2, iter->id + 1);

					if (ret2 == -2)
					{
						return(ret);
					}
					else
					{
						if (ret & CONVERT_DELETE_FILE)
							unlink(filename_out);

						strcpy(filename_out, filename_out2);
						strcpy(error_out, error_out2);

						return(ret2);
					}
				}
				else
				{
					return(ret);
				}
			}
		}
	}


	strcpy(filename_out, filename_in);
	return(-2);
}

#if 0 ///__WIN32__  // Win32 uses DLLs - generally better
int convert_file(char *fn_in, char *fn_out, char *converter, char *error_out)
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
int convert_file(char *fn_in, char *fn_out, char *converter, char *params, char *error_out)
{
	char tmp_dest[200];
	char error_out_rtf[200];
	int tmpret=0, tmpret2;

	// Generate temporary filename
	tmpnam(tmp_dest);

	strcpy(fn_out, tmp_dest);

#ifdef __LINUX__
	char tmp[200];

	sprintf(tmp, "./%s \"%s\" \"%s\"", converter, fn_in, tmp_dest);
	system(tmp);
#else
	if (params == NULL)
	{
		tmpret2 = spawnl(P_WAIT, converter, converter, fn_in, tmp_dest, NULL);
	}
	else
	{
		tmpret2 = spawnl(P_WAIT, converter, converter, params, fn_in, tmp_dest, NULL);
	}

	switch(tmpret2)
	{
		case -1:
			{
			char spawn_error[50];

				switch (errno)
				{
					case E2BIG:
						strcpy(spawn_error, "Argument list too long");
						break;

					case EINVAL:
						strcpy(spawn_error, "Invalid argument");
						break;

					case ENOENT:
						strcpy(spawn_error, "Path or filename not found");
						break;

					case ENOEXEC:
						strcpy(spawn_error, "Executable format error");
						break;

					case ENOMEM:
						strcpy(spawn_error, "Not enough memory");
						break;

					default:
						sprintf(spawn_error, "error number %d", errno);
						break;
				}

				sprintf(error_out, "Unable to load converter '%s' (%s)", converter, spawn_error);
				tmpret2 = ERROR_CNV_LOAD;
				break;
			}

		case ERROR_CNV_CANNOT_OPEN_SRC:
			strcpy(error_out, "Unable to open source file");
			break;

		case ERROR_CNV_CANNOT_OPEN_DEST:
			strcpy(error_out, "Unable to open temporary destination file.");
			break;

		case ERROR_CNV_INCORRECT_FORMAT:
			strcpy(error_out, "This file is not the correct format. Displaying contents anyway.");

			strcpy(fn_out, fn_in);
			break;

		case ERROR_CNV_PARSE_ERROR:
			sprintf(error_out, "Error while parsing file.");
			strcpy(fn_out, fn_in);
			break;

		case ERROR_CNV_PARSE_ERROR_RTF:
			sprintf(error_out, "Error while parsing file. The resulting file may still be viewable.");
			tmpret |= CONVERT_DELETE_FILE;
			break;

		case ERROR_CNV_FROM_RTF:
			{
				int rtf_ret;

				tmpnam(tmp_dest);
				rtf_ret = convert_file(fn_out, tmp_dest, "txtrtf.cnv", NULL, error_out_rtf);

				if ((rtf_ret == ERROR_CNV_LOAD) || (!(rtf_ret & ERROR_CNV_OK)))
				{
					if (rtf_ret & ERROR_CNV_PARSE_ERROR_RTF)
					{
						tmpret = ERROR_CNV_OK | CONVERT_DELETE_FILE;
						tmpret2 = ERROR_CNV_OK;

						if (rtf_ret & CONVERT_DELETE_FILE) 
							unlink(fn_out);

						strcpy(fn_out, tmp_dest);

#ifdef DISPLAY_2ND_PASS_ERRORS
						strcpy(error_out, "Parse error during second-pass conversion. The resulting file may still be viewable.");
#else
						rtf_ret = ERROR_CNV_OK;
#endif					
					}
					else
					{
						sprintf(error_out, "Unable to perform second-pass conversion (%s). Displaying results of first pass.", error_out_rtf);

						if (rtf_ret & CONVERT_DELETE_FILE) 
							unlink(tmp_dest);

						tmpret = ERROR_CNV_RTF_ERROR | CONVERT_DELETE_FILE;
					}
				}
				else
				{
					tmpret = ERROR_CNV_OK | CONVERT_DELETE_FILE;
					tmpret2 = ERROR_CNV_OK;

					if (rtf_ret & CONVERT_DELETE_FILE) 
						unlink(fn_out);

					strcpy(fn_out, tmp_dest);
				}

				break;
			}

		case ERROR_CNV_INIT_FAILED:
			sprintf(error_out, "Error initialising converter ('%s')", converter);
			strcpy(fn_out, fn_in);
			break;

		case ERROR_CNV_FAILED:
			strcpy(error_out, "Conversion failed");
			strcpy(fn_out, fn_in);
			break;

		case ERROR_CNV_OK:
			tmpret |= CONVERT_DELETE_FILE;
			break;

		default:
			sprintf(error_out, "Unknown error: %d", tmpret2);
			break;
	}
#endif

	return(tmpret | tmpret2);
}
#endif
