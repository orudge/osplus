/**********************************************************/
/* OSPlus Utility Kit 1.1                                 */
/* Copyright (c) Owen Rudge 2000. All Rights Reserved.    */
/**********************************************************/
/* OSPlus Write Converter                                 */
/* TXTWRITE.CNV                                           */
/**********************************************************/

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>

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

/* History:
 *
 * 26/12/2000: Changed open/write/read/close to fopen() etc for portability
 */

typedef unsigned short      WORD;

struct WRITE_FF_S
{
	WORD wIdent;
	WORD dty;
	WORD wTool;
	WORD x;
	WORD y;
	WORD z;
	WORD a;
	WORD fcMac;
	WORD fcMac2;
	WORD pnPara;
	WORD pnFntb;
	WORD pnSep;
	WORD pnSetb;
	WORD pnPgtb;
	WORD pnFfntb;
} WRITE_FF;

/* Main routine: NEEDS TO BE IMPROVED!!! */
int main(int argc, char *argv[])
{
	struct WRITE_FF_S ff;
	char rest[2048];
	FILE *hndl, *out;

	if (argc != 3)
	{
		printf("OSPlus Write to Text Converter 1.1\n");
		printf("Copyright (c) Owen Rudge 2000\n");
		printf("\n");
		printf("Usage:  TXTWRITE.CNV source dest\n");
		exit(1);
	}

	hndl = fopen(argv[1], "rb");
	out = fopen(argv[2], "wb");

        if (hndl == NULL || out == NULL)
           exit(1);

	fread(&ff, sizeof(WRITE_FF), 1, hndl);
	//printf("%d", ff.wIdent);
	//getch();

	//printf("README.WRI: %o %d %o %d %d %d %d %d %d %d %d %d %d %d %d\n", ff.wIdent, ff.dty, ff.wTool, ff.x, ff.y, ff.z, ff.a, ff.fcMac, ff.fcMac2, ff.pnPara, ff.pnFntb, ff.pnSep, ff.pnSetb, ff.pnPgtb, ff.pnFfntb);

//	flen = filelength(hndl) - (64 * sizeof(WORD));
	fseek(hndl, 64*sizeof(WORD), SEEK_SET);

	do
	{
		fread(&rest, sizeof(rest), 1, hndl);
		fwrite(rest, sizeof(rest), 1, out);
	} while (!feof(hndl)); /* need to stop when text ends and formatting codes etc start */

	fclose(hndl);
	fclose(out);

	return(1);
}
