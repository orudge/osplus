/* fold -- wrap each input line to fit in specified width.
   Copyright (C) 91, 1995-2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by David MacKenzie, djm@gnu.ai.mit.edu. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#define TAB_WIDTH 8

#if __STDC_VERSION__ < 199901L
	typedef char bool;

	#define false	0
	#define true	1
#else
	#include <stdbool.h>
#endif

/* If nonzero, try to break on whitespace. */
static bool break_spaces = true;

/* If nonzero, count bytes, not column positions. */
static bool count_bytes = false;

/* If nonzero, at least one of the files we read was standard input. */
static bool have_read_stdin;


void *
xrealloc (void *p, size_t n)
{
  if (!n && p)
    {
      /* The GNU and C99 realloc behaviors disagree here.  Act like
         GNU, even if the underlying realloc is C99.  */
      free (p);
      return NULL;
    }

  p = realloc (p, n);
  if (!p && n)
    abort();
  return p;
}

static void *
x2nrealloc (void *p, size_t *pn, size_t s)
{
  size_t n = *pn;

  if (! p)
    {
      if (! n)
        {
          /* The approximate size to use for initial small allocation
             requests, when the invoking code specifies an old size of
             zero.  This is the largest "small" request for the GNU C
             library malloc.  */
          enum { DEFAULT_MXFAST = 64 * sizeof (size_t) / 4 };

          n = DEFAULT_MXFAST / s;
          n += !n;
        }
    }
  else
    {
      /* Set N = ceil (1.5 * N) so that progress is made if N == 1.
         Check for overflow, so that N * S stays in size_t range.
         The check is slightly conservative, but an exact check isn't
         worth the trouble.  */
      if ((size_t) -1 / 3 * 2 / s <= n)
        abort ();
      n += (n + 1) / 2;
    }

  *pn = n;
  return xrealloc (p, n * s);
}


/* Change the size of an allocated block of memory P to N bytes,
   with error checking.  */

void *
x2realloc (void *p, size_t *pn)
{
  return x2nrealloc (p, pn, 1);
}


/* Assuming the current column is COLUMN, return the column that
   printing C will move the cursor to.
   The first column is 0. */

static size_t
adjust_column (size_t column, char c)
{
  if (!count_bytes)
    {
      if (c == '\b')
	{
	  if (column > 0)
	    column--;
	}
      else if (c == '\r')
	column = 0;
      else if (c == '\t')
	column += TAB_WIDTH - column % TAB_WIDTH;
      else /* if (isprint (c)) */
	column++;
    }
  else
    column++;
  return column;
}

static bool fold_isblank(char c)
{
	if ((c == ' ') || (c == '\t'))
		return true;

	return false;
}

static unsigned char to_uchar(char ch)
{
	return ch;
}

/* Fold file FILENAME, or standard input if FILENAME is "-",
   to stdout, with maximum line length WIDTH.
   Return true if successful.  */

bool
fold_file (char const *filename, char const *output, size_t width)
{
  FILE *istream, *ostream;
  int c;
  size_t column = 0;		/* Screen column where next char will go. */
  size_t offset_out = 0;	/* Index in `line_out' for next char. */
  static char *line_out = NULL;
  static size_t allocated_out = 0;

  istream = fopen (filename, "r");

  if (istream == NULL)
    {
      return false;
    }

  ostream = fopen (output, "wt");

  if (ostream == NULL)
    {
      return false;
    }

  while ((c = getc (istream)) != EOF)
    {
      if (offset_out + 1 >= allocated_out)
	line_out = x2realloc (line_out, &allocated_out);

      if (c == '\n')
	{
	  line_out[offset_out++] = c;
	  fwrite (line_out, sizeof (char), offset_out, ostream);
	  column = offset_out = 0;
	  continue;
	}

    rescan:
      column = adjust_column (column, c);

      if (column > width)
	{
	  /* This character would make the line too long.
	     Print the line plus a newline, and make this character
	     start the next line. */
	  if (break_spaces)
	    {
	      bool found_blank = false;
	      size_t logical_end = offset_out;

	      /* Look for the last blank. */
	      while (logical_end)
		{
		  --logical_end;
		  if (fold_isblank (to_uchar (line_out[logical_end])))
		    {
		      found_blank = true;
		      break;
		    }
		}

	      if (found_blank)
		{
		  size_t i;

		  /* Found a blank.  Don't output the part after it. */
		  logical_end++;
		  fwrite (line_out, sizeof (char), (size_t) logical_end,
			  ostream);
		  fputs("\n", ostream);
		  /* Move the remainder to the beginning of the next line.
		     The areas being copied here might overlap. */
		  memmove (line_out, line_out + logical_end,
			   offset_out - logical_end);
		  offset_out -= logical_end;
		  for (column = i = 0; i < offset_out; i++)
		    column = adjust_column (column, line_out[i]);
		  goto rescan;
		}
	    }

	  if (offset_out == 0)
	    {
	      line_out[offset_out++] = c;
	      continue;
	    }

	  line_out[offset_out++] = '\n';
	  fwrite (line_out, sizeof (char), (size_t) offset_out, ostream);
	  column = offset_out = 0;
	  goto rescan;
	}

      line_out[offset_out++] = c;
    }

  if (offset_out)
    fwrite (line_out, sizeof (char), (size_t) offset_out, ostream);

  if (ferror (istream))
    {
	fclose (istream);
	fclose (ostream);
      return false;
    }

	fclose (istream);
	fclose (ostream);

	return true;
}
