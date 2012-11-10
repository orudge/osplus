/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2012. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Write Converter                                    */
/* TXTWRITE.CNV                                              */
/*************************************************************/

/*
 * write.c : converts Windows Write and DOS Word to RTF
 *
 * Copyright 2002, Sean Young <sean@mess.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * TODO:
 * - testing. Check for possible infinite loops or buffer over/underflow
 * - add MS Word for DOS support. This format is very similar.
 */

/*
 The output has been validated against the Microsoft Write converter,
 write32.cnv 98012700, version 8.0. There are still bugs:

 - OLE objects

 The converter does not atempt to minimize the output. Sometimes the output
 is even invalid (for example "\par \pard\qj some text \qc more text"), but
 it tries to keep as faithful as possible to the Microsoft converter,
 allowing the output to be compared easily to the Microsoft one. 

 The code should be fast and thread-safe. It does not recover very well from
 corrupted files, but then again it could be corrupted anyway.
*/

/* Revision History (as of 05/02/2004):
 *
 * 05/02/2004: Updated for OSPlus (orudge)
 * 27/12/2004: Updates to use new converter interface (orudge)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "convert.h"

#if defined(__DJGPP__) || defined(__BORLANDC__) || (defined(_MSC_VER) && (_MSC_VER < 1500))  // DJGPP + Borland C++ don't have snprintf/vsnprintf

int snprintf(char *str, size_t n, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#define vsnprintf(buf,lim,fmt,ap) vsprintf(buf, fmt, ap)

int snprintf(char *str, size_t n, const char *fmt, ...)
{
  va_list ap;
  int len;

  va_start(ap, fmt);
  len = vsnprintf(str, n, fmt, ap);
  va_end(ap);

  return len;
}
#endif

#if (_MSC_VER >= 1500)
	#define snprintf _snprintf
#endif


#define EMULATE_BUGS 

static const char rcsid[] = 
  "$Id: txtwrite.c,v 1.3 2005-01-01 19:50:03 orudge Exp $";

/* helper macros endianness */
#define read_word(p)	((*(p)) + ((*((p)+1)) << 8))

#define read_dword(p)	((*(p)) + (((*((p)+1)) << 8)) + (((*((p)+2)) << 16)) + \
			(((*((p)+3)) << 24)))


struct footnote { 
    int  cpRef, cpFtn;
};

#define	TYPE_WRITE	(1)
#define TYPE_DOSWORD	(2)

#define NOTE_FOOTNOTE	(1)
#define NOTE_ANNOTATION (2)

struct section {
    int cpLim;
    int yaMac, xaMac, page_start, yaTop, dyaText, xaLeft, dxaText; 
    int yaFooter, yaHeader;
    /* dosword fields */
    int fStyled, stc, bkc, nfcPgn, lnc, rhc, fMirrorMargins, fLnn, fAutoPgn, 
	fEndFtns, cColumns, dxaColumns, dxaGutter, yaPgn, xaPgn, dxaLnn, 
	nLnnMod, fHardMargTop, fHardMargBottom;
};

struct fileinformationblock {
    int		fcMac;
    int		pnPara, pnFntb, pnSep, pnSetb, pnBftb, pnFfntb, pnMac;
    /* For Word for DOS */
    int		pnBkmk, pnSumd;
    int		version, asv;
    int		codepage;
};

struct dosword {
    int  type;
    int  wri_fonts_count;
    int  footnote_count;
    struct footnote  *footnote_refs;
    int  fcMac;
    int  section_count, current_section;
    struct section *sections;
    struct fileinformationblock *fib;
    char *filename;
    FILE *f, *fout;
    char *print;
    int print_len;
    int in_footnote;	/* because recursive footnotes/annotations are not
			   allowed */
};

static int print_footnote (struct dosword *wd, int fcRef, int type);
static int read_sect (struct dosword *wd);
static int print_section (struct dosword *wd, int section_no);
static int wri_object (struct dosword *wd, const unsigned char *data, int size);
static void translate_char (struct dosword *wd, unsigned char *ch, size_t len,
	int fc, int fSpecial);

enum { lvl_error = 0, lvl_warning, lvl_info };

static void report (const struct dosword *wd, int level, const char *err)
{
#ifdef DEBUG
    static const char *level_str[] = { "error", "warning", "info" };

    fprintf (stderr, "%s: %s: %s\n", wd->filename, level_str[level], err);
#endif
}

/* our fprintf */
static void our_fprintf (struct dosword *wd, const char *format, ...)
{
    va_list ap;
    int size, ret;
    char *p = NULL;

    if (!wd->print) {
	p = (char *) malloc (0x4000);
	if (p == NULL) {
	    report (wd, lvl_error, "Out of memory");
	    exit (1);
	}
	wd->print = p;
	wd->print_len = 0x4000;
    }
    va_start (ap, format);
    while (1) {
	size = vsnprintf (wd->print, wd->print_len, format, ap);
	if ((size != -1) && (size < wd->print_len)) break;
	wd->print_len *= 2;
	p = (char *) realloc (wd->print, wd->print_len);
	if (p == NULL) {
	    report (wd, lvl_error, "Out of memory");
	    exit (1);
	}
	wd->print = p;
    }
    va_end (ap);

    ret = (size != (int)fwrite (wd->print, 1, size, wd->fout));
    if (ret) {
#ifdef DEBUG
	perror (wd->filename);
#endif
	exit(1);
    }
}

/* the file header */
#define OFFSET_HEADER_W_WIDENT		(0)
#define OFFSET_HEADER_W_DTY		(2)
#define OFFSET_HEADER_W_WTOOL		(4)
#define OFFSET_HEADER_D_FCMAC		(14)
#define OFFSET_HEADER_W_PNPARA		(18)
#define OFFSET_HEADER_W_PNFNTB		(20)
#define OFFSET_HEADER_W_PNSETB		(24)
#define OFFSET_HEADER_W_PNBFTB		(26)

#define OFFSET_WORD_HEADER_W_PNBKMK	(22)
#define OFFSET_WORD_HEADER_W_PNSUMD	(28)
#define OFFSET_WORD_HEADER_W_PNMAC	(106)
#define OFFSET_WORD_HEADER_B_VERSION	(116)
#define OFFSET_WORD_HEADER_B_ASV	(117)
#define OFFSET_WORD_HEADER_W_CODEPAGE	(126)

#define OFFSET_WRITE_HEADER_W_PNSEP	(22)
#define OFFSET_WRITE_HEADER_W_PNFFNTB	(28)
#define OFFSET_WRITE_HEADER_W_PNMAC	(96)

static int read_fib (struct dosword *wd)
{
    unsigned char header[0x80];
    int wIdent, wTool, dty;
    char string[256]; /* more than enough */

    if (0x80 != fread (header, 1, 0x80, wd->f)) {
	if (feof (wd->f)) {
	    report (wd, lvl_error, "Premature end of file");
	} else {
	    report (wd, lvl_error, sys_errlist[errno]);
	}
	return 2;
    }

    wIdent = read_word (header + OFFSET_HEADER_W_WIDENT);
    dty = read_word (header + OFFSET_HEADER_W_DTY);
    wTool = read_word (header + OFFSET_HEADER_W_WTOOL);

    switch (wIdent) {
	case 0xbe31:	/* Write 3.0 */
	case 0xbe32:	/* Write 3.1 */
	case 0x6031:	/* Word for DOS */
	    if (wTool == 0xab00) break;
	default:
	    report (wd, lvl_error, "Not Write or Word for DOS file");
	    return 1;
    }

    if (dty) {
	const char	*dty_types[] = { "normal", "glossary", 
					"style sheet", "printer driver" };
	if (dty < 4) {
	    snprintf (string, sizeof (string), 
		"This is a Word %s file\n", dty_types[dty]);
	    report (wd, lvl_error, string);
	} else {
	    report (wd, lvl_error, "Not Write or Word for DOS file");
	}
	return 1;
    }

    /* get common members */
    wd->fib->fcMac = read_dword (header + OFFSET_HEADER_D_FCMAC);
    wd->fib->pnPara = read_word (header + OFFSET_HEADER_W_PNPARA);
    wd->fib->pnFntb = read_word (header + OFFSET_HEADER_W_PNFNTB);
    wd->fib->pnSetb = read_word (header + OFFSET_HEADER_W_PNSETB);
    wd->fib->pnBftb = read_word (header + OFFSET_HEADER_W_PNBFTB);

    /* figure out if it's Word or Write */
    wd->fib->pnMac = read_word (header + OFFSET_WRITE_HEADER_W_PNMAC);

    if (!wd->fib->pnMac) {
	wd->type = TYPE_DOSWORD;
	/* Word for DOS */
	wd->fib->pnBkmk = read_word (header + OFFSET_WORD_HEADER_W_PNBKMK);
	wd->fib->pnSumd = read_word (header + OFFSET_WORD_HEADER_W_PNSUMD);
	wd->fib->version = header[OFFSET_WORD_HEADER_B_VERSION];
	wd->fib->asv = header[OFFSET_WORD_HEADER_B_ASV];
	wd->fib->codepage = read_word (header + OFFSET_WORD_HEADER_W_CODEPAGE);
	wd->fib->pnMac = read_word (header + OFFSET_WORD_HEADER_W_PNMAC);

	strcpy (string, "Microsoft Word for DOS");
	switch (wd->fib->version) {
	case 0: 
	    strcat (string, " version 4.0 or earlier");
	    break;
	case 3:
	    strcat (string, " version 5.0 OEM");
	    break;
	case 4:
	    strcat (string, " version 5.0");
	    break;
	default:
	    strcat (string, " unknown version");
	    break;
	}
	report (wd, lvl_info, string);

	wd->fib->asv &= 2;
	if (wd->fib->asv) {
	    report (wd, lvl_error, "cannot read Word for DOS autosaved file");
	    return 1;
	}
    } else { 
	/* Write */
	wd->type = TYPE_WRITE;
	wd->fib->pnSep = read_word (header + OFFSET_WRITE_HEADER_W_PNSEP);
	wd->fib->pnFfntb = read_word (header + OFFSET_WRITE_HEADER_W_PNFFNTB);

	snprintf (string, sizeof (string), "Microsoft Write 3.%d", 
	    (wIdent == 0xbe31) ? 0 : 1);

	report (wd, lvl_info, string);
    }

    return 0;
}
/*
 * all the font stuff
 */

static int read_ffntb (struct dosword *wd)
{
    int	page, font_count, cbFfn, sane, ffid;
    unsigned char byt[2];
    unsigned char *ffn, *p;
    char string[256];
    size_t ffn_length, ffn_size, i;

    ffn = NULL;
    ffn_size = 0;
    font_count = 0;

    our_fprintf (wd, "\\fonttbl");

    /* if the page is the same as pnMac, there are no fonts */
    page = wd->fib->pnFfntb;
    if ((page == wd->fib->pnMac) || !wd->fib->pnMac) {
	wd->wri_fonts_count = 0;
	font_count = 0;
    } else {
	if (fseek (wd->f, page++ * 0x80, SEEK_SET) ) goto file_ellol;
	/* the first two bytes are the number of fonts */
	if (2 != fread (byt, 1, 2, wd->f) ) goto file_ellol;
	wd->wri_fonts_count = read_word (byt);
    }

    if (wd->wri_fonts_count) {
	while (1) {
	    if (2 != fread (byt, 1, 2, wd->f)) goto file_ellol;

	    cbFfn = read_word (byt);
	    if (cbFfn == 0) {
		break;
	    }
	    if (cbFfn == 0xffff) {
		if (fseek (wd->f, page++ * 0x80, SEEK_SET)) goto file_ellol;
		continue;
	    }

	    /* font family identifier */
	    if (1 != fread (byt, 1, 1, wd->f)) goto file_ellol;

	    ffid = (int)byt[0];

	    /* font family name */
	    ffn_length = (size_t) cbFfn - 1;
	    if (ffn_size < ffn_length) {
		p = (unsigned char*) realloc (ffn, ffn_length);
		if (!p) {
		    if (ffn) free (ffn);
		    report (wd, lvl_error, "Out of memory");
		    return 1;
		}
		ffn = p;
	    }

	    if (ffn_length != fread (ffn, 1, ffn_length, wd->f)) 
		goto file_ellol;

	    /* check that it's sane */
	    for (sane=0,i=ffn_length-1;i>0;i--) if (!ffn[i]) sane = 1;
	    if (!sane) {
		report (wd, lvl_warning, "font table not sane");
		break;
	    }
	
	    /* output the font family */
	    our_fprintf (wd, "{\\f%d", font_count);
	    switch (ffid) {
	    default:
		snprintf (string, sizeof (string), 
		    "font #%d: unknown ffid %02x", font_count, ffid);
		
		report (wd, lvl_warning, string);
	    case 0x00:	/* FF_DONTCARE */
		our_fprintf (wd, "\\fnil ");
		break;
	    case 0x10:	/* FF_ROMAN */
		our_fprintf (wd, "\\froman ");
		break;
	    case 0x20:	/* FF_SWISS */
		our_fprintf (wd, "\\fswiss ");
		break;
	    case 0x30:	/* FF_MODERN */
		our_fprintf (wd, "\\fmodern ");
		break;
	    case 0x40:	/* FF_SCRIPT */
		our_fprintf (wd, "\\fscript ");
		break;
	    case 0x50:	/* FF_DECORATIVE */
		our_fprintf (wd, "\\fdecor ");
		break;
	    }

	    translate_char (wd, ffn, ffn_length, 0, 0);

	    our_fprintf (wd, ";}", ffn);

	    font_count++;
	}
    }

    if (font_count != wd->wri_fonts_count) {
	/* write file lied about number of fonts */
	wd->wri_fonts_count = font_count;
    }

    snprintf (string, sizeof (string), "%d font%s used in file", font_count,
	font_count > 1 ? "s" : "");
    report (wd, lvl_info, string);

    if (font_count == 0) {
	/* Arial is the default font in Write */
	our_fprintf (wd, "{\\f0\\fswiss Arial; }");
	wd->wri_fonts_count = 1;
    }

    if (ffn) free (ffn);

    return 0;

file_ellol:
    if (feof (wd->f)) {
	report (wd, lvl_error, "Premature end of file");
    } else {
	report (wd, lvl_error, sys_errlist[errno]);
    }

    if (ffn) free (ffn);

    return 1;
}

static void translate_char (struct dosword *wd, unsigned char *p, 
	size_t len, int fc, int fSpecial)
{
    int i;
    unsigned char ch;

    for (i=0; i<len; i++) {
	ch = p[i];
	if (ch < 0x20) {
	    if (fSpecial) {
		/* current page number is the only "special" character Write 
		   understands */
		if (ch == 1) our_fprintf (wd, "\\chpgn ");

		if (wd->type == TYPE_DOSWORD) {
		    switch (ch) {
		    case 2: /* current date */
			our_fprintf (wd, "\\chdate ");
			break;
		    case 3: /* current time */
			our_fprintf (wd, "\\chtime ");
			break;
		    case 4: /* autonumbered annotation reference */
			print_footnote (wd, fc, NOTE_ANNOTATION);
			break;
		    case 5: /* autonumbered footnote reference */
			print_footnote (wd, fc, NOTE_FOOTNOTE);
			break;
		    case 7: /* sequence mark */
			fprintf (stderr, 
			    "warning: sequence mark encountered\n");
			break;
		    case 8: /* sequence reference mark */
			fprintf (stderr, 
			    "warning: sequence reference mark encountered\n");
		    }
		}
	    }
	    else switch (ch) {
	    case 9:
		our_fprintf (wd, "\\tab ");
		break;
	    case 10:
		our_fprintf (wd, "\\par \r\n");
		break;
	    case 11:
		/* these can also exist in Write files */
		our_fprintf (wd, "\\line ");
		break;
	    case 12:
		our_fprintf (wd, "\\page ");
		break;
#ifdef EMULATE_BUGS
	    case 1:
	    case 26:
		our_fprintf (wd, "%c", ch);
#endif
	    }
	} else if (ch < 0x80) {
	    switch (ch) {
	    case '\\': 
		our_fprintf (wd, "\\\\"); 
		break;
	    case '}': 
		our_fprintf (wd, "\\}"); 
		break;
	    case '{': 
		our_fprintf (wd, "\\{"); 
		break;
	    default: 
		our_fprintf (wd, "%c", ch);
	    }
	} else {
	    our_fprintf (wd, "\\'%02x", (int)ch);
	}
    }
}

struct character {
    int fcFirst, fcLim;
    unsigned char char_page[0x80];
    int page, fod, cfod;
    int ftc, hps, bold, italic, underline, hpsPos, fSpecial;
    /* DOS-Word fields */
    int strike, dline, csm, hidden, clr;
    unsigned char fprop[8];
    int nodiff;
};

static int read_chp (struct dosword *wd, struct character *chp)
{
    unsigned int bfProp, cch, i;
    unsigned char fprop[8];

    if (chp->page == 0) {
	chp->page = (wd->fib->fcMac + 127) / 128;
	chp->fcLim = 0x80;
	/* force loading of page */
	chp->cfod = 1;
	chp->fod = 1;
    }

    if (chp->fcLim >= wd->fib->fcMac) {
	report (wd, lvl_error, "tried to read chp beyound end");
	return 1;
    }

    if (chp->fod >= chp->cfod) {
	if (fseek (wd->f, chp->page++ * 0x80, SEEK_SET) ||
	    (0x80 != fread (chp->char_page, 1, 0x80, wd->f))) {
	    if (feof (wd->f)) {
		report (wd, lvl_error, "Premature end of file");
	    } else {
		report (wd, lvl_error, sys_errlist[errno]);
	    }
	    return 1;
	}
	chp->cfod = chp->char_page[0x7f];
	if (chp->cfod > 20) chp->cfod = 20;
	chp->fod = 0;
	if (read_dword (chp->char_page) != chp->fcLim) {
	    report (wd, lvl_warning, "fcFirst of chp is wrong");
	    return 0;
	}
    }

    chp->fcFirst = chp->fcLim;
    chp->fcLim = read_dword (chp->char_page + 4 + chp->fod * 6);
    bfProp = read_word ((chp->char_page + 8 + chp->fod * 6));

    /* work-around */
    if (chp->fcLim == 0x7f) chp->fcLim = 0x80;

    /* default values */
    memset (fprop, 0, sizeof (fprop));
    fprop[0] = 1;
    fprop[2] = (wd->type == TYPE_WRITE) ? 24 : 18;

    if (bfProp <= (0x7f - 4)) {
	cch = chp->char_page[bfProp + 4];
	if ((cch + bfProp) > (0x7f - 4)) {
	    report (wd, lvl_warning, "cch points of page, ignoring chp");
	} else {
	    if (cch > sizeof (fprop)) {
		report (wd, lvl_info, "chp larger than expected");
		cch = sizeof (fprop);
	    }
	    if (cch) memcpy (fprop, chp->char_page + bfProp + 5, cch);
	}
    }

    chp->fod++;

    /* cmp, if only fSpecial is different, set nodiff */
    chp->nodiff = 1;
    for (i=0; i<8; i++) {
	if(i != 3) {
	    if (chp->fprop[i] != fprop[i]) {
		chp->nodiff = 0;
		break;
	    }
	}
	else if ((chp->fprop[3] ^ 0x40) != fprop[3]) {
	    chp->nodiff = 0;
	    break;
	} 
    }

    chp->fSpecial = fprop[3] & 0x40;
    memcpy (chp->fprop, fprop, 8);

    if(chp->nodiff) return 0; 

    chp->ftc = fprop[1] / 4;
    chp->bold = fprop[1] & 1;
    chp->italic = fprop[1] & 2;
    chp->hps = fprop[2];
    chp->underline = fprop[3] & 1;
    chp->hpsPos = fprop[5];

    if (wd->type == TYPE_DOSWORD) {
	chp->strike = fprop[3] & 2;
	chp->dline = fprop[3] & 4;
	chp->csm = (fprop[3] / 16) & 3;
	chp->hidden = fprop[3] & 0x80;
	chp->clr = fprop[6] & 7;
    } else {
	/* Write */
	chp->ftc |= (fprop[4] & 4) * 64;
	if (chp->ftc >= wd->wri_fonts_count) chp->ftc = 0;
    }

    return 0;
}

struct paragraph {
    int fcFirst, fcLim;
    unsigned char pap_page[0x80];
    int page, fod, cfod;
    int jc, fGraphics, dyaLine, header, rhcPage, rhcFirst;
    int tab_count, tabs[14], jcTab[14], dxaRight, dxaLeft, dxaLeft1;
    /* DOS-Word things */
    int keep, keep_follow, side_by_side, dyaBefore, dyaAfter;
    int btc, bsc, b_top, b_bottom, b_left, b_right, b_clr, shade, shade_clr;
};

static int read_pap (struct dosword *wd, struct paragraph *pap)
{
    unsigned int bfProp, cch;
    unsigned char fprop[108];

    if (pap->page == 0) {
	pap->page = wd->fib->pnPara;
	pap->fcLim = 0x80;
	/* force loading of page */
	pap->cfod = 1;
	pap->fod = 1;
    }

    if (pap->fcLim >= wd->fib->fcMac) {
	report (wd, lvl_error, "tried to read pap beyound end");
	return 1;
    }

    if (pap->fod >= pap->cfod) {
	if (fseek (wd->f, pap->page++ * 0x80, SEEK_SET) ||
	    (0x80 != fread (pap->pap_page, 1, 0x80, wd->f))) {
	    if (feof (wd->f)) {
		report (wd, lvl_error, "premature end of file");
	    } else {
		report (wd, lvl_error, sys_errlist[errno]);
	    }
	    return 1;
	}
	pap->cfod = pap->pap_page[0x7f];
	if (pap->cfod > 20) pap->cfod = 20;
	pap->fod = 0;
	if (read_dword (pap->pap_page) != pap->fcLim) {
	    report (wd, lvl_warning, "fcFirst of pap is wrong");
	    return 0;
	}
    }

    pap->fcFirst = pap->fcLim;
    pap->fcLim = read_dword (pap->pap_page + 4 + pap->fod * 6);
    bfProp = read_word ((pap->pap_page + 8 + pap->fod * 6));

    /* work-around */
    if (pap->fcLim == 0x7f) pap->fcLim = 0x80;

    /* default values */
    memset (fprop, 0, sizeof (fprop));
    fprop[0] = 61;
    fprop[2] = 30;
    fprop[10] = (wd->type == TYPE_WRITE) ? 240 : 192;

    if (bfProp <= (0x7f -4)) {
	cch = pap->pap_page[bfProp + 4];
	if ((cch + bfProp) > (0x7f - 4)) {
	    report (wd, lvl_warning, "cch points of page, ignoring pap");
	} else {
	    if (cch > sizeof (fprop)) {
		report (wd, lvl_info, "pap larger than expected");
		cch = sizeof (fprop);
	    }
	    if (cch) memcpy (fprop, pap->pap_page + bfProp + 5, cch);
	}
    }

    pap->jc = fprop[1] & 3;
    pap->dxaRight = read_word (fprop + 4);
    if (pap->dxaRight & 0x8000) pap->dxaRight = -0x10000 + pap->dxaRight;
    pap->dxaLeft = read_word (fprop + 6);
    if (pap->dxaLeft & 0x8000) pap->dxaLeft = -0x10000 + pap->dxaLeft;
    pap->dxaLeft1 = read_word (fprop + 8);
    if (pap->dxaLeft1 & 0x8000) pap->dxaLeft1 = -0x10000 + pap->dxaLeft1;
    pap->dyaLine = read_word (fprop + 10);
    for (pap->tab_count=0; pap->tab_count < 14; pap->tab_count++) {
	pap->tabs[pap->tab_count] = 
	    read_word (fprop + pap->tab_count * 4 + 22);
	if (!pap->tabs[pap->tab_count]) break;
	pap->jcTab[pap->tab_count] = 
	    fprop[pap->tab_count * 4 + 24] & 3;
    }

    if (wd->type == TYPE_WRITE) {
	pap->fGraphics = fprop[16] & 0x10;
	pap->header = fprop[16] & 6;
	pap->rhcPage = fprop[16] & 1;
	pap->rhcFirst = fprop[16] & 8;
    } else {
	pap->keep = fprop[1] & 4;
	pap->keep_follow = fprop[1] & 8;
	pap->side_by_side = fprop[1] & 0x10;
	pap->dyaBefore = read_word (fprop + 12);
	pap->dyaAfter = read_word (fprop + 14);
	pap->btc = (fprop[16] / 16) & 3;
	pap->bsc = (fprop[16] / 64) & 3;
	pap->b_left = fprop[17] & 1;
	pap->b_right = fprop[17] & 2;
	pap->b_top = fprop[17] & 4;
	pap->b_bottom = fprop[17] & 8;
	pap->b_clr = (fprop[17] / 16) & 7;
	pap->shade = fprop[18] & 0x7f;
	pap->shade_clr = (fprop[19] / 16) & 7;
   }

   /*
    * for reasons unknown, the left and right indents are from the paper,
    * not from the margins in headers
    */
   if (pap->header) {
	int dist;
	struct section *s = NULL;

	if (wd->section_count) {
	    s = wd->sections + wd->current_section;
	}

	if (s) dist = s->xaLeft;
	else dist = 1800;
	pap->dxaLeft -= dist;

	if (s) dist = s->xaMac - s->xaLeft - s->dxaText;
	else dist = 1800;
	pap->dxaRight -= dist;
    }

    pap->fod++;

    return 0;
}

static int print_text_part (struct dosword *wd, int fcFirst, int fcMac)
{
    struct paragraph pap;
    struct character chp;
    int fc, text_size, text_len, textp, n, fcMac2;
    unsigned char *text, *p;
    int new_pap;
    int skip_tab;

    text = NULL;
    text_size = 0;
    fc = fcFirst;

    memset (&pap, 0, sizeof (pap));
    memset (&chp, 0, sizeof (chp));

    while (fc < fcMac) {
	/* find the right pap */
	while (1) {
	    if (read_pap (wd, &pap)) {
		if (text) free (text);
		return 1;
	    }
	    if ((fc >= pap.fcFirst) && (fc < pap.fcLim)) break;
	}

	/* read the paragraph text */
	text_len = pap.fcLim - pap.fcFirst;
	if (text_len > text_size) {
	    p = realloc (text, text_len + 0x4000);
	    if (!p) {
		report (wd, lvl_error, "Out of memory");
		if (text) free (text);
		return 1;
	    }
	    text = p;
	    text_size = text_len + 0x4000;
	}
	if (fseek (wd->f, fc, SEEK_SET) || 
	   (text_len != (int)fread (text, 1, text_len, wd->f))) {
	    if (feof (wd->f)) {
		report (wd, lvl_error, "Premature end of file");
	    } else {
		report (wd, lvl_error, sys_errlist[errno]);
	    }
	    if (text) free (text);
	    return 1;
	}

	our_fprintf (wd, "\\pard ");
	new_pap = 1;

	/* justify */
	if (pap.jc) {
	    static const char text_align[4] = "lcrj";
	    our_fprintf (wd, "\\q%c", text_align[pap.jc]);
	}

	/* indentation */
	if (pap.dxaRight) our_fprintf (wd, "\\ri%d", pap.dxaRight);
	if (pap.dxaLeft) our_fprintf (wd, "\\li%d", pap.dxaLeft);
	if (pap.dxaLeft1) our_fprintf (wd, "\\fi%d", pap.dxaLeft1);
	if (pap.dyaBefore) our_fprintf (wd, "\\sb%d", pap.dyaBefore);
	if (pap.dyaAfter) our_fprintf (wd, "\\sa%d", pap.dyaAfter);

	/* interline spacing */
	our_fprintf (wd, "\\sl%d", pap.dyaLine);

	/* tabs */
	skip_tab = 0;
	if (pap.tab_count) {
	    for (n=0;n<pap.tab_count;n++) {
		if (pap.jcTab[n]) {
		    our_fprintf (wd, "\\tqdec");
		}
			
		our_fprintf (wd, "\\tx%d", pap.tabs[n]);
	        /* If a tab is equal to the starting position of the
	         * text, the first tab is discarded. */
	        if (pap.dxaLeft + pap.dxaLeft1 == pap.tabs[n]) skip_tab = 1;
	    }
	}

	/* various DOS-Word things */
	if (pap.keep) our_fprintf (wd, "\\keep");
	if (pap.keep_follow) our_fprintf (wd, "\\keepn");
	if (pap.side_by_side) our_fprintf (wd, "\\sbys");

	/* shading */
	if (pap.shade) {
	    our_fprintf (wd, "\\shading%d\\cbpat%d", 
		pap.shade * 100, pap.shade_clr);
	}

	/* border colour */
	if (pap.btc) {
	    static const char *bsc_string[] = { 
		"\\brdrhair", "\\brdrs", "\\brdrdb", "\\brdrth" };
		
	    if (pap.btc == 1) {
		our_fprintf (wd, " \\box%s\\brdrcf%d",
			bsc_string[pap.bsc], pap.b_clr);
	    } else if (pap.btc == 2) {
		if (pap.b_left) {
		    our_fprintf (wd, " \\brdrl%s\\brdrcf%d",
			    bsc_string[pap.bsc], pap.b_clr);
		}
		if (pap.b_right) {
		    our_fprintf (wd, " \\brdrr%s\\brdrcf%d",
			    bsc_string[pap.bsc], pap.b_clr);
		}
		if (pap.b_top) {
		    our_fprintf (wd, " \\brdrt%s\\brdrcf%d",
			    bsc_string[pap.bsc], pap.b_clr);
		}
		if (pap.b_bottom) {
		    our_fprintf (wd, " \\brdrb%s\\brdrcf%d",
			    bsc_string[pap.bsc], pap.b_clr);
		}
	    }
	} 

	our_fprintf (wd, " ");

	textp = 0;

	while ((fc < fcMac) && (fc < pap.fcLim)) {
	    int print_space;

	    /* get the chp */
	    while ((fc < chp.fcFirst) || (fc >= chp.fcLim)) {
		if (read_chp (wd, &chp)) {
		    if (text) free (text);
		    return 1;
		}
	    }

	    if (new_pap || !chp.nodiff) {
		print_space = 0;
		our_fprintf (wd, "\\plain ");

		if (chp.bold) {
		    our_fprintf (wd, "\\b");
		    print_space = 1;
		}
		if (chp.italic) {
		    our_fprintf (wd, "\\i");
		    print_space = 1;
		}
		if (chp.dline) {
		    our_fprintf (wd, "\\uldb");
		    print_space = 1;
		} else if (chp.underline) {
		    our_fprintf (wd, "\\ul");
		    print_space = 1;
		}
		if (chp.strike) {
		    our_fprintf (wd, "\\strike");
		    print_space = 1;
		}
		if (chp.ftc) {
		    our_fprintf (wd, "\\f%d", chp.ftc);
		    print_space = 1;
		}
		if (chp.csm) {
		    print_space = 1;
		    if (chp.csm == 1) {
			our_fprintf (wd, "\\caps");
		    } else {
			our_fprintf (wd, "\\scaps");
		    }
		}
		if (chp.hidden) {
		    print_space = 1;
		    our_fprintf (wd, "\\v");
		}
		if (chp.clr) {
		    print_space = 1;
		    our_fprintf (wd, "\\cf%d", chp.clr);
		}
		if (chp.hps != 24) {
		    print_space = 1;
		    our_fprintf (wd, "\\fs%d", chp.hps);
		}
		if (chp.hpsPos) {
		    print_space = 1;
		    our_fprintf (wd, "\\up%d", chp.hpsPos);
		}

		if (print_space) our_fprintf (wd, " ");
	    }

	    if (pap.fGraphics) {
		if (wri_object (wd, text, text_len)) {
		    if (text) free (text);
		    return 1;
		}
		fc += text_len;
		break;
	    }

	    fcMac2 = fcMac;
	    if (chp.fcLim < fcMac2) fcMac2 = chp.fcLim;
	    if (pap.fcLim < fcMac2) fcMac2 = pap.fcLim;

	    if (fc != fcMac2) { /* no text */
		if (skip_tab && new_pap && text[textp] == 9) {
		    textp++; fc++;
		    skip_tab = 0;
		}
		if (fc != fcMac2) {
		    translate_char (wd, text + textp, fcMac2 - fc, 
			fc, chp.fSpecial);

		     textp += fcMac2 - fc;
		     fc = fcMac2;
		}
	    }

	    new_pap = 0;
	}
    }

    if (text) free (text);

    return 0;
}

static int wri_process_document (struct dosword *wd)
{
    struct paragraph pap;
    int header, footer, header_page1, footer_page1, fc, footer_first;
    int header_fcFirst, header_fcLim, footer_fcFirst, footer_fcLim;
    int fcFirst;

    memset (&pap, 0, sizeof (pap));
    header = footer = header_page1 = footer_page1 = footer_first = 0;
    header_fcFirst = header_fcLim = footer_fcFirst = footer_fcLim = 0;

    our_fprintf (wd, "{\\rtf1\\ansi \\deff0{");
    if (read_ffntb (wd) || read_sect (wd)) return 1;
    our_fprintf (wd, "}");

    if (wd->section_count) {
	if (print_section (wd, 0)) return 1;
	wd->current_section = 0;
    } else {
	our_fprintf (wd, "\\headery1080\\footery1080 ");
    }

    /* do header/footer stuff. Write only recognises headers and footers
       at the very beginning; all others are ignored */
    fc = 0;
    while (fc < wd->fib->fcMac) {
	if (read_pap (wd, &pap)) return 1;

	if (!pap.header) break;

	if (pap.rhcPage) {
	    if (!footer_first) footer_first = 1;
	    footer = 1;
	    footer_page1 = pap.rhcFirst;
	    if (!footer_fcFirst) footer_fcFirst = pap.fcFirst;
	    footer_fcLim = pap.fcLim/* - 2*/;
	} else {
	    if (!footer_first) footer_first = -1;
	    header = 1;
	    header_page1 = pap.rhcFirst;
	    if (!header_fcFirst) header_fcFirst = pap.fcFirst;
	    header_fcLim = pap.fcLim/* - 2*/;
	}
	fc = pap.fcLim;
    }

    fcFirst = fc ? fc : 0x80;

    /* sanity check */
    if (header && (header_fcFirst >= header_fcLim)) header = 0;
    if (footer && (footer_fcFirst >= footer_fcLim)) footer = 0;

    /* print the headers/footers */
    if (footer || header) {
	int titlepg = 0;
	if ((footer && !footer_page1) || (header && !header_page1)) {
	    our_fprintf (wd, "\\titlepg ");
	    titlepg = 1;
	}

	while (footer || header) {
	    if (footer && (footer_first >= 0)) {
		if (footer) {
		    our_fprintf (wd, "{\\footer ");
		    if (print_text_part (wd, footer_fcFirst, footer_fcLim))
			return 1;
		     our_fprintf (wd, "}");
		     footer = 0;
		 }
	    
		 if (titlepg && footer_page1) {
		     our_fprintf (wd, "{\\footerf ");
		     if (print_text_part (wd, footer_fcFirst, footer_fcLim))
			  return 1;
		     our_fprintf (wd, "}");
		 }
	    }

	    if (header) {
		our_fprintf (wd, "{\\header ");
		if (print_text_part (wd, header_fcFirst, header_fcLim))
		    return 1;
		our_fprintf (wd, "}");
		header = 0;
	    }

	    if (titlepg && header_page1) {
		our_fprintf (wd, "{\\headerf ");
		if (print_text_part (wd, header_fcFirst, header_fcLim))
		    return 1;
		our_fprintf (wd, "}");
		header_page1 = 0;
	    }
	    footer_first = 0;
	}
    }

    /* print the actual document text */
    if (print_text_part (wd, fcFirst, wd->fib->fcMac)) return 1;

    our_fprintf (wd, "}");

    return 0;
}

static int dump_data (struct dosword *wd, const unsigned char *data, 
	int length, int bin)
{
    static const char hex[16] = "0123456789abcdef";
    int n, lines, y, x, line_len;
    char line[128], *p;

#ifdef EMULATE_BUGS
    if (bin) {
	our_fprintf (wd, " {\\bin%d ", length);

	if (length != (int)fwrite (data, 1, length, wd->fout))
	    return 1;	    

	our_fprintf (wd, "}");

	return 0;
    }
#endif
    
    our_fprintf (wd, "\r\n");
    lines = length / 40;
    line_len = 40 * 2
#ifndef EMULATE_BUGS
 + 2
#endif
    ;
    line[line_len - 2] = '\r';
    line[line_len - 1] = '\n';
    line[line_len] = 0;

    for (n=y=0;y<lines;y++) {
	p = line;
	for (x=0;x<40;x++) {
	    *p++ = hex[data[n] / 16];
	    *p++ = hex[data[n++] & 15];
	}
	our_fprintf (wd, line);
    }

    if (n < length) {
	p = line;
	while (n < length) {
	    *p++ = hex[data[n] / 16];
	    *p++ = hex[data[n++] & 15];
	}
	*p = 0;
	our_fprintf (wd, line);
    }
    
    return 0;
}

/* old-style pictures. These can also be Windows Meta Files */
#define OFFSET_PIC_W_MM			(0)
#define OFFSET_PIC_W_XEXT		(2)
#define OFFSET_PIC_W_YEXT		(4)
#define OFFSET_PIC_W_DXAOFFSET		(8)
#define OFFSET_PIC_W_DXASIZE		(10)
#define OFFSET_PIC_W_DYASIZE		(12)
#define OFFSET_PIC_W_BMWIDTH		(18)
#define OFFSET_PIC_W_BMHEIGHT		(20)
#define OFFSET_PIC_W_BMWIDTHBYTES	(22)
#define OFFSET_PIC_B_BMPLANES		(24)
#define OFFSET_PIC_B_BMBITSPIXEL	(25)
#define OFFSET_PIC_D_CBSIZE		(32)
#define OFFSET_PIC_W_MX			(36)
#define OFFSET_PIC_W_MY			(38)

/* OLE stuff */
#define OFFSET_OLE_W_MM			(0)
#define OFFSET_OLE_W_OBJECTTYPE		(6)
#define OFFSET_OLE_W_DXAOFFSET		(8)
#define OFFSET_OLE_W_DXASIZE		(10) 
#define OFFSET_OLE_W_DYASIZE		(12)
#define OFFSET_OLE_D_DWDATASIZE		(16)
#define OFFSET_OLE_W_MX			(36)
#define OFFSET_OLE_W_MY			(38)

static int wri_dib (struct dosword *wd, const unsigned char *obj, 
	const unsigned char *dib, int size)
{
    struct {
	unsigned int size, width, height, planes, bit_pixel, compression;
    } bmp;
    int width_bytes, bit_pixel;
    char string[256];

    if (size < 40) {
	report (wd, lvl_warning, "header of dib missing");
	return 0;
    }

    bmp.size = read_dword (dib);
    bmp.width = read_dword (dib + 4);
    bmp.height = read_dword (dib + 8);
    bmp.planes = read_word (dib+ 12);
    bmp.bit_pixel = read_word (dib + 14);
    bmp.compression = read_dword (dib + 16);

    if (bmp.size != 40) {
	report (wd, lvl_warning, "header of dib corrupt");
	return 0;
    }

    snprintf (string, sizeof (string), "device independent bitmap %dx%dx%d",
	bmp.width, bmp.height, bmp.bit_pixel);
    report (wd, lvl_info, string);

    bit_pixel = (bmp.compression == 2) ? bmp.bit_pixel << 1 : bmp.bit_pixel;
#ifdef EMULATE_BUGS
    width_bytes = bmp.width * bmp.bit_pixel / 8;
#else
    width_bytes = ((bmp.width * bmp.bit_pixel + 31) / 32) * 4;
#endif

    our_fprintf (wd, "{\\*\\pict\\dibitmap\\picw%d\\pich%d"
	"\\wbmbitspixel%d\\wbmplanes%d\\wbmwidthbytes%d",
	bmp.width, bmp.height, bit_pixel, bmp.planes, width_bytes);

    our_fprintf (wd, "\\picwGoal%d\\pichGoal%d\\picscalex%d\\picscaley%d",
	read_word (obj + OFFSET_PIC_W_DXASIZE),
	read_word (obj + OFFSET_PIC_W_DYASIZE),
	read_word (obj + OFFSET_PIC_W_MX) / 10,
	read_word (obj + OFFSET_PIC_W_MY) / 10);

    dump_data (wd, dib, size, 0);

    our_fprintf (wd, "}");

    return 0;
}

static int wri_bmp (struct dosword *wd, const unsigned char *obj, 
	const unsigned char *bmp, const unsigned char *data, int len, 
	int old_style)
{
    struct {
	unsigned int width, height, width_bytes, planes, bit_pixel;
    } bm;
    char string[256];
    int dxa, dya, data_len;

    bm.width = read_word (bmp + 2);
    bm.height = read_word (bmp + 4);
    bm.width_bytes = read_word (bmp + 6);
    bm.planes = bmp[8];
    bm.bit_pixel = bmp[9];

    data_len = bm.height * bm.width_bytes;
    if (len < data_len) {
	report (wd, lvl_warning, "bitmap data missing, skipping image");
	return 0;
    }

    snprintf (string, sizeof (string), "device dependent bitmap %dx%dx%d",
	bm.width, bm.height, bm.bit_pixel);
    report (wd, lvl_info, string);

    dxa = read_word (obj + OFFSET_PIC_W_DXASIZE);
    dya = read_word (obj + OFFSET_PIC_W_DYASIZE);

    if (old_style) {
	dxa = dxa ? dxa * 9 / 32 : 4320;
	dya = dya ? dya * 9 / 32 : 4320;
	our_fprintf (wd, 
	    "{\\pict\\wbitmap0\\picw%d\\pich%d\\wbmwidthbytes%d"
	    "\\wbmplanes%d\\wbmbitspixel%d"
	    "\\picwGoal%d\\pichGoal%d\\picscalex%d\\picscaley%d",
	    bm.width, bm.height, bm.width_bytes, bm.planes, bm.bit_pixel,
	    dxa, dya,
	    read_word (obj + OFFSET_PIC_W_MX) / 10,
	    read_word (obj + OFFSET_PIC_W_MY) / 10);
    } else {
	if (!dxa) dxa = 4320;
	if (!dya) dya = 4320;
	our_fprintf (wd, 
	    "{\\*\\pict\\wbitmap0\\pich%d\\picw%d\\wbmbitspixel%d"
	    "\\wbmplanes%d\\wbmwidthbytes%d"
	    "\\picwGoal%d\\pichGoal%d\\picscalex%d\\picscaley%d",
	    bm.height, bm.width, bm.bit_pixel, bm.planes, bm.width_bytes,
	    dxa, dya,
	    read_word (obj + OFFSET_PIC_W_MX) / 10,
	    read_word (obj + OFFSET_PIC_W_MY) / 10);
    }

    dump_data (wd, data, len, old_style);

    our_fprintf (wd, "}");

    return 0;
}

static int wri_wmf (struct dosword *wd, const unsigned char *obj, 
	const unsigned char *wmf, int size)
{
#ifdef DEBUG
    FILE *f;
    static int no = 0;
    char filename[256];

    snprintf (filename, sizeof (filename), "/tmp/wmf%05d.wmf", no++);
    f = fopen (string, "wb");
    fwrite (wmf + 8, 1, size - 8, f);
    fclose (f);
#endif

    report (wd, lvl_info, "Windows metafile (wmf)");

    our_fprintf (wd, "{\\*\\pict\\wmetafile%d\\picw%d\\pich%d",
	read_word (wmf), read_word (wmf + 2), read_word (wmf + 4));

    our_fprintf (wd, 
	"\\picwgoal%d\\pichgoal%d\\picscalex%d\\picscaley%d",
	read_word (obj + OFFSET_PIC_W_DXASIZE),
	read_word (obj + OFFSET_PIC_W_DYASIZE),
	read_word (obj + OFFSET_PIC_W_MX) / 10,
	read_word (obj + OFFSET_PIC_W_MY) / 10);

    dump_data (wd, wmf + 8, size - 8, 0);

    our_fprintf (wd, "}");

    return 0;
}

static int wri_ole_static (struct dosword *wd, 
	const unsigned char *data, const unsigned char *obj, unsigned int size)
{
    unsigned int len;
    char string[256];
 
    if (!strcmp ("DIB", (const char*)data + 4)) {
	if (size < (4 + 16)) {
	    report (wd, lvl_warning, 
		"incomplete DIB bitmap header, skipping");
	    return 0;
	}
	len = read_dword (data + 4 + 12);
	if (size < (len + 4 + 16)) {
	    report (wd, lvl_warning, 
		"incomplete DIB bitmap, skipping");
	    return 0;
	}
	return wri_dib (wd, obj, data + 4 + 16, len);
    }

    if (!strcmp ("BITMAP", (const char*)data + 4)) {
	if (size < (4 + 19 + 14)) {
	    report (wd, lvl_warning, 
		"incomplete DDB bitmap header, skipping");
	    return 0;
	}
	len = read_dword (data + 4 + 15);
	if (size < (len + 4 + 19)) {
	    report (wd, lvl_warning, 
		"incomplete DDB bitmap, skipping");
	    return 0;
	}
	return wri_bmp (wd, obj,	/* object */
	    data + 4 + 19,		/* bmp */
	    data + 4 + 19 + 14,		/* data */
	    len - 10, 0);		/* data length */
    }

    if (!strcmp ("METAFILEPICT", (const char*)data + 4)) {
	if (size < (4 + 25)) {
	    report (wd, lvl_warning, "incomplete wmf header, skipping");
	    return 0;
	}
	len = read_dword (data + 4 + 21);
	if (size < (4 + 25 + len)) {
	    report (wd, lvl_warning, "incomplete wmf, skipping");			    return 0;
	}
	return wri_wmf (wd, obj, data + 4 + 25, len);
    }

    snprintf (string, sizeof (string), "unknown static OLE object of "
	"type %s", data + 4);
    report (wd, lvl_warning, string);

    return 0;
}

static int wri_ole_embedded (struct dosword *wd, 
	const unsigned char *obj, unsigned int data_size)
{
    unsigned int size, len, total_len, i, n;
    char report_string[256], *strings[3];

    /*
     * 48 byte offset after the start of the object, the information
     * begins. 
     *
     * There are three strings, which I have dubbed: object type,
     * filename, parameter. For each string, we first read a 32 byte
     * string length, which is followed by that string (string length
     * includes zero-terminator). If the length is 0 there is no string,
     * not even a zero-terminator.
     */
    total_len = 48;

    for (i=0;i<3;i++) {

	if (data_size < (total_len + 4)) goto missing;
	len = read_dword (obj + total_len);
	if (len) {
	    if (data_size < (total_len + len + 4)) goto missing;

	    for (n=len;n>=0;n--) {
		if (!obj[total_len + 4 + n]) break;
	    }
	    if (n < 0) {
		report (wd, lvl_warning, "OLE header corrupt");
		return 0;
	    }
	    strings[i] = (char*) obj + total_len + 4;
	    len = n;
	} else {
	    strings[i] = NULL;
	}
	total_len += 4 + len;
    }

    snprintf (report_string, sizeof (report_string),
	"OLE embedded object %s,filename=%s,parameter=%s",
	strings[0] ? strings[0] : "unknown",
	strings[1] ? strings[1] : "none",
	strings[2] ? strings[2] : "none");
    report (wd, lvl_info, report_string);
    
    /* 
     * The three strings are immediately followed by the offset to the
     * next part of the OLE object
     */
    if (data_size < (total_len + 4)) goto missing;
    size = read_dword (obj + total_len);
	    
    our_fprintf (wd, 
	"{\\object\\objemb\\objw%d\\objh%d\\objscalex%d\\objscaley%d"
	"{\\*\\objdata",
	read_word (obj + OFFSET_OLE_W_DXASIZE),
	read_word (obj + OFFSET_OLE_W_DYASIZE),
	read_word (obj + OFFSET_OLE_W_MX) / 10,
	read_word (obj + OFFSET_OLE_W_MY) / 10);

    dump_data (wd, obj + 40, data_size - 40, 0);

    our_fprintf (wd, "}");
    total_len += size + 4;

    /* see if there is a result to print */
    if ((data_size > (total_len + 8)) && 
	0x501 == read_dword (obj + total_len) &&
	5 == read_dword (obj + total_len + 4)) {
	our_fprintf (wd, "{\\result");
	
	if (wri_ole_static (wd, obj + total_len + 8, obj, 
	    data_size - total_len - 8)) return 1;

	our_fprintf (wd, "}");
    }

    our_fprintf (wd, "}");
	
    return 0;

missing:
    report (wd, lvl_warning, "incomplete OLE object, skipping");

    return 0;
}

static int wri_ole_link (struct dosword *wd, const unsigned char *data, 
	const unsigned char *obj, int size)
{
    report (wd, lvl_error, "OLE link object not implemented");
    return 0;
}

static int wri_object (struct dosword *wd, 
	const unsigned char *data, int size) 
{
    int dxaOffset, mm, xa, ya;
    char string[256];

    if (size < 40) {
	report (wd, lvl_warning, "paragraph too small for object, skipping");
	return 0;
    }

    mm = read_word (data + OFFSET_PIC_W_MM);

    if (mm == 0x88) { /* Windows metafile (.wmf) */ 
	report (wd, lvl_info, "Windows metafile (wmf)");
	if (size < (read_dword (data + OFFSET_PIC_D_CBSIZE) + 40)) {
	    report (wd, lvl_warning, "metafile data missing");
	    return 0;
	}

	dxaOffset = read_word (data + OFFSET_PIC_W_DXAOFFSET);
#ifndef EMULATE_BUGS
	if (dxaOffset) {
	    our_fprintf (wd, "\\fi%d ", dxaOffset);
	}
#endif
  
	our_fprintf (wd, "{\\pict\\wmetafile%d\\picw%d\\pich%d",
	    8, /* ??? */
	    read_word (data + OFFSET_PIC_W_XEXT),
	    read_word (data + OFFSET_PIC_W_YEXT));
	   
	xa = read_word (data + OFFSET_PIC_W_XEXT);
	ya = read_word (data + OFFSET_PIC_W_YEXT);
	if (xa) xa = xa * 9 / 32;
	else xa = 4320;
	if (ya) ya = ya * 9 / 32;
	else ya = 4320;

	our_fprintf (wd, 
	    "\\picwGoal%d\\pichGoal%d\\picscalex%d\\picscaley%d",
	    xa, ya,
	    read_word (data + OFFSET_PIC_W_MX) / 10,
	    read_word (data + OFFSET_PIC_W_MY) / 10);

	dump_data (wd, data + 40, 
	    read_dword (data + OFFSET_PIC_D_CBSIZE), 1);

	our_fprintf (wd, "}");

	return 0;

    } else if (mm == 0xe3) { /* this is a picture */
	if (size < (read_dword (data + OFFSET_PIC_D_CBSIZE) + 40)) {
	    report (wd, lvl_warning, "bitmap data missing");
	    return 0;
	}

	dxaOffset = read_word (data + OFFSET_PIC_W_DXAOFFSET);
#ifndef EMULATE_BUGS
	if (dxaOffset) {
	    our_fprintf (wd, "\\fi%d ", dxaOffset);
	}
#endif

	return wri_bmp (wd, data, /* obj */
	    data + 16,		  /* bmp */
	    data + 40,		  /* data */
	    size - 40, 1);

    } else if (mm == 0xe4) { /* OLE object */
	int len, type;

	if (size < (read_dword (data + OFFSET_OLE_D_DWDATASIZE) + 40)) {
	    report (wd, lvl_error, "OLE data missing");
	    return 0;
	}

	dxaOffset = read_word (data + OFFSET_OLE_W_DXAOFFSET);
#ifndef EMULATE_BUGS
	if (dxaOffset) {
	    our_fprintf (wd, "\\fi%d ", dxaOffset);
	}
#endif
	/* find out what sort of object it is */
	if (read_dword (data + 40) != 0x501) {
	    report (wd, lvl_warning, "Unknown OLE identifier");
	    return 0;
	}
	type = read_dword (data + 44);
	len = read_dword (data + 48);
	if (len > 32) { /* FIXME: is this really the limit? */
	    report (wd, lvl_warning, "OLE type too long (>32)");
	    return 0;
	}
	if (data[51 + len]) {
	    report (wd, lvl_warning, "OLE name corrupt, skipping");
	    return 0;
	}

	switch (type) {
	case 3: 
	    return wri_ole_static (wd, data + 48, data, size - 48);
	case 2: 
	    return wri_ole_embedded (wd, data, size);
	case 1: 
	    return wri_ole_link (wd, data + 48, data, size - 48);
	default:
	    snprintf (string, sizeof (string), 
		"Unknown type OLE object %d\n", type);
	    report (wd, lvl_warning, string);
	    return 0;
	}

	snprintf (string, sizeof (string), "OLE %s object type %s",
	    type == 3 ? "link" : "embedded", data + 52);
	report (wd, lvl_info, string);
	
	our_fprintf (wd, "{\\object");
	if (type == 3) {
	    our_fprintf (wd, "\\objlink");
	} else {
	    our_fprintf (wd, "\\objemb");
	}

	our_fprintf (wd, 
	    "\\objw%d\\objh%d\\objscalex%d\\objscaley%d", 
	    read_word (data + OFFSET_OLE_W_DXASIZE),
	    read_word (data + OFFSET_OLE_W_DYASIZE),
	    read_word (data + OFFSET_OLE_W_MX) / 10,
	    read_word (data + OFFSET_OLE_W_MY) / 10);

	our_fprintf (wd, "{\\*\\objdata");

	dump_data (wd, data + 40, 
	    read_dword (data + OFFSET_OLE_D_DWDATASIZE), 0);
	
	our_fprintf (wd, "}}");

	return 0;

    } else {
	snprintf (string, sizeof (string), 
	    "Unknown object encountered (mm=%02x)\n", mm);
	report (wd, lvl_warning, string);
    }

    return 0;
}

static int read_sumd (struct dosword *wd)
{
    char string[16];
    int ch, n, month, day, year, cch;
    static char *sum_types[] = { "title", "author", "operator", "keywords",
		"comments", "version", NULL };

    if (wd->type != TYPE_DOSWORD) {
	fprintf (stderr, 
	    "error: read_sumd called for non-DOS Word file, internal error\n");
	return 1;
    }

    if (wd->fib->pnSumd && (wd->fib->pnSumd != wd->fib->pnMac)) {
	
	if (fseek (wd->f, wd->fib->pnSumd * 128 + 0x12, SEEK_SET) == -1) 
	    goto ellol;
	
	if ((ch = getc (wd->f)) == -1) goto ellol;
	if (ch == 0xdc) return 0;	/* DC for deleted information */

	/* Summary Sheet is present */
	our_fprintf (wd, "{\\info");
	if (fseek (wd->f, wd->fib->pnSumd * 128 + 0x12, SEEK_SET) == -1) 
	    goto ellol;

	/* text */
	n = 0; 
	while (sum_types[n]) {
	    if ((ch = getc (wd->f)) == -1) goto ellol;
	    if (ch) {
		our_fprintf (wd, "{\\%s %c", sum_types[n], ch);
		while (ch) {
		    if ((ch = getc (wd->f)) == -1) goto ellol;
		    if (ch >= 0x20) translate_char (wd, 
				(unsigned char*)&ch, 1,  0, 0);
		}
		our_fprintf (wd, "}");
	    }
	    n++;
	}

	/* creation time */
	if (8 != fread (string, 1, 8, wd->f)) goto ellol;
	string[8] = 0;
	if (3 == sscanf (string, "%d/%d/%d", &month, &day, &year)) {
	    our_fprintf (wd, 
		"{\\creatim \\yr%d \\mo%d \\dy%d}", 
		year + 1900, month, day);
	}
	
	/* revision time */
	if (8 != fread (string, 1, 8, wd->f)) goto ellol;
	string[8] = 0;
	if (3 == sscanf (string, "%d/%d/%d", &month, &day, &year)) {
	    our_fprintf (wd, 
		"{\\revtim \\yr%d \\mo%d \\dy%d}", 
		year + 1900, month, day);
	}
	
	/* cchDoc (number of character in doc) */
	for (cch=0,n=0;n<4;n++) {
	    if ((ch = getc (wd->f)) == -1) goto ellol;
	    cch += ch << (n * 8);
	}
	if (cch) our_fprintf (wd, "{\\nofchars%d}", cch);

	our_fprintf (wd, "}");
    }

    return 0;

ellol:
    if (feof (wd->f)) {
	fprintf (stderr, "%s: Premature end of file\n", wd->filename);
    } else {
#ifdef DEBUG
	perror (wd->filename);
#endif
    }
    return 1;
}

static int read_fntb (struct dosword *wd)
{
    unsigned char string[8];
    int footnote_count, i;
    struct footnote *fnd = NULL;

    if (wd->fib->pnFntb && (wd->fib->pnFntb != wd->fib->pnBkmk)) {
	/* footnote table exists */
	if (fseek (wd->f, wd->fib->pnFntb * 0x80, SEEK_SET) == -1) goto ellol;
	if (fread (string, 1, 4, wd->f) != 4) goto ellol;

	footnote_count = read_word (string);

	if (!footnote_count) return 0;

	fnd = (struct footnote*) 
	    malloc (sizeof (struct footnote) * footnote_count);
	if (!fnd) {
	    fprintf (stderr, "error: Out of memory\n");
	    return 1;
	}

	for (i=0;i<footnote_count;i++) {
	    if (fread (string, 1, 8, wd->f) != 8) goto ellol;
	
	    fnd[i].cpFtn = read_dword (string + 4);
	    fnd[i].cpRef = read_dword (string);
	}

	wd->footnote_refs = fnd;
	wd->footnote_count = footnote_count;
	wd->fcMac = fnd[0].cpFtn + 0x80;

	fprintf (stderr, "info: %d footnotes\n", footnote_count - 1);
    }

    return 0;

ellol:
    if (fnd) free (fnd);

    if (feof (wd->f)) {
	fprintf (stderr, "%s: Premature end of file\n", wd->filename);
    } else {
#ifdef DEBUG
	perror (wd->filename);
#endif
    }
    return 1;
}

static int print_footnote (struct dosword *wd, int fcRef, int type)
{
    int i, cpRef, found;

    if (wd->in_footnote) {
	fprintf (stderr, 
	    "warning: recursive footnotes or annotations are not permitted\n");
	return 0;
    }

    wd->in_footnote = 1;
    cpRef = fcRef - 0x80;

    i = found = 0;
    while (i < (wd->footnote_count - 1)) {
	fprintf (stderr, "fcRef = %08x, cpRef = %08x\n",
	    fcRef, wd->footnote_refs[i].cpRef + 0x80);
	if (cpRef == wd->footnote_refs[i].cpRef) {
	    found = 1; 
	    break;
	}
	i++;
    }

    if (!found) {
	fprintf (stderr, "warning: footnote or annotation does not exist\n");
	wd->in_footnote = 0;
	return 0;
    }

    switch (type) {
	case NOTE_FOOTNOTE:
	    our_fprintf (wd, "{\\footnote ");
	    break;
	case NOTE_ANNOTATION:
	    our_fprintf (wd, "{\\annotation ");
	    break;
	default:
	    fprintf (stderr, 
		"error: internal error, print_footnote called with type %d\n",
		type);
	    return 1;
    }

    if (print_text_part (wd, wd->footnote_refs[i].cpFtn + 0x80, 
	wd->footnote_refs[i+1].cpFtn + 0x80)) return 1;

    our_fprintf (wd, "}");

    wd->in_footnote = 0;

    return 0;
}

static int read_sect (struct dosword *wd)
{
    unsigned char blob[256];
    int no_sections, i, cch;
    unsigned int fcSep;
    struct section *sect = NULL;
    char string[256];

    if (!wd->fib->pnSetb || (wd->fib->pnSetb == wd->fib->pnBftb)) 
	return 0;

    if (fseek (wd->f, wd->fib->pnSetb * 0x80, SEEK_SET) || 
       (4 != fread (blob, 1, 4, wd->f))) goto file_ellol;

    no_sections = read_word (blob);
    sect = (struct section*) calloc (no_sections, sizeof (struct section));
    if (sect == NULL) {
	report (wd, lvl_error, "out of memory");
	return 1;
    }
    snprintf (string, sizeof (string), "%d section%s in file", no_sections,
	no_sections > 1 ? "s" : "");
    report (wd, lvl_info, string);

    for (i=0;i<no_sections;i++) {
	if (fseek (wd->f, wd->fib->pnSetb * 0x80 + 4 + 10 * i, SEEK_SET) ||
	    (10 != fread (blob, 1, 10, wd->f))) goto file_ellol;
	
	sect[i].cpLim = read_dword (blob);
	fcSep = read_dword (blob + 6);
	if(fcSep == 0xd1d1d1d1) fcSep = 0xffffffff; /* FIXME: */

	/* default values */
	sect[i].yaMac = 15840; /* 11 * 1440 */
	sect[i].xaMac = 12240; /* 8.5 * 1440 */
	sect[i].page_start = 0xffff;
	sect[i].yaTop = 1440;
	sect[i].dyaText = 12960; /* 9 * 1440 */
	sect[i].xaLeft = 1800; /* 1.25 * 1440 */
	sect[i].dxaText = 8640; /* 6 * 1440 */
	sect[i].yaHeader = 1080; /* 0.75 * 1440 */
	sect[i].yaFooter = 14760; /* yaMac - 0.75 * 1440 */
	/* TODO: fill in defaults for dosword */

	if (fcSep != 0xffffffff) {
	    if (fseek (wd->f, fcSep, SEEK_SET)) goto file_ellol;
	    if (1 != (int)fread (blob, 1, 1, wd->f)) goto file_ellol;
	    cch = blob[0];
	    if (cch != (int)fread (blob + 1, 1, cch, wd->f)) goto file_ellol;

	    /* read stuff */
	    if (cch >= 1) {
		sect[i].fStyled = blob[1] & 1;
		sect[i].stc = (blob[1] / 2) & 127;
	    }
	    if (cch >= 2) {
		sect[i].bkc = blob[2] & 7;
		sect[i].nfcPgn = (blob[2] / 8) & 7;
		sect[i].lnc = (blob[2] / 64) & 3;
	    }
	    if (cch >= 4) sect[i].yaMac = read_word (blob + 3);
	    if (cch >= 6) sect[i].xaMac = read_word (blob + 5);
	    if (cch >= 8) sect[i].page_start = read_word (blob + 7);
	    if (cch >= 10) sect[i].yaTop = read_word (blob + 9);
	    if (cch >= 12) sect[i].dyaText = read_word (blob + 11);
	    if (cch >= 14) sect[i].xaLeft = read_word (blob + 13);
	    if (cch >= 16) sect[i].dxaText = read_word (blob + 15);
	    if (cch >= 17) {
		sect[i].rhc = blob[17] & 15;
		sect[i].fMirrorMargins = blob[17] & 0x10;
		sect[i].fLnn = blob[17] & 0x20;
		sect[i].fAutoPgn = blob[17] & 0x40;
		sect[i].fEndFtns = blob[17] & 0x80;
	    }
	    if (cch >= 18) sect[i].cColumns = blob[18];
	    if (cch >= 20) sect[i].yaHeader = read_word (blob + 19);
	    if (cch >= 22) sect[i].yaFooter = read_word (blob + 21);
	    if (cch >= 24) sect[i].dxaColumns = read_word (blob + 23);
	    if (cch >= 26) sect[i].dxaGutter = read_word (blob + 25);
	    if (cch >= 28) sect[i].yaPgn = read_word (blob + 27);
	    if (cch >= 30) sect[i].xaPgn = read_word (blob + 29);
	    if (cch >= 31) sect[i].nLnnMod = blob[31];
	    if (cch >= 32) {
		sect[i].fHardMargTop = blob[32] & 1;
		sect[i].fHardMargBottom = blob[32] & 2;
	    }
	}
    }

    wd->sections = sect;
    wd->section_count = no_sections;

    return 0;

file_ellol:

    if (sect) free (sect);
    wd->sections = NULL;
    wd->section_count = 0;

    if (feof (wd->f)) {
	report (wd, lvl_error, "Premature end of file while reading section");
    } else {
	snprintf (string, sizeof (string), "%s while reading section",
	    sys_errlist[errno]);
	report (wd, lvl_error, string);
    }
    return 0;
}

static int print_section (struct dosword *wd, int section_no)
{
    int yaBottom, xaRight, yaFooter;
    struct section *s = wd->sections + section_no;
    int space = 0;

    /* paper size */
    if (s->xaMac != 12240) {
	space = 1;
	our_fprintf (wd, "\\paperw%d", s->xaMac);
    }
    if (s->yaMac != 15840) {
	space = 1;
	our_fprintf (wd, "\\paperh%d", s->yaMac);
    }

    /* margins */
    xaRight = s->xaMac - s->xaLeft - s->dxaText;
    yaBottom = s->yaMac - s->yaTop - s->dyaText;

    /* sanity check */
    if ((xaRight < 0) || (yaBottom < 0)) {
	report (wd, lvl_warning, "margins not sane, ignoring");
    } else {
	if (s->xaLeft != 1800) {
	    space = 1;
	    our_fprintf (wd, "\\margl%d", s->xaLeft);
	}
	if (xaRight != 1800) {
	    space = 1;
	    our_fprintf (wd, "\\margr%d", xaRight);
	}
	if (s->yaTop != 1440) {
	    space = 1;
	    our_fprintf (wd, "\\margt%d", s->yaTop);
	}
	if (yaBottom != 1440) {
	    space = 1;
	    our_fprintf (wd, "\\margb%d", yaBottom);
	}
    }
 
    /* header/footer distance */
    if (s->yaHeader != 720) {
	space = 1;
	our_fprintf (wd, "\\headery%d", s->yaHeader);
    }

    yaFooter = s->yaMac - s->yaFooter;
    if (yaFooter != 720) {
	space = 1;
	our_fprintf (wd, "\\footery%d", yaFooter);
    }

    /* start numbering pages at # */
    if (s->page_start != 0xffff) {
	space = 1;
	our_fprintf (wd, "\\pgnstart%d", s->page_start);
    }

    if (wd->type == TYPE_DOSWORD) {

    }

    if (space) our_fprintf (wd, " ");

    return 0;
}

/* this is nicked from the output of RTF_DOS.EXE (part of DOS-Word 5) */
static const char dosword_fonts[] = "\\deff0{\n\\fonttbl"
"{\\f0\\fmodern pica;}{\\f1\\fmodern Courier;}{\\f2\\fmodern elite;}\n"
"{\\f3\\fmodern prestige;}{\\f4\\fmodern lettergothic;}\n"
"{\\f5\\fmodern gothicPS;}{\\f6\\fmodern cubicPS;}\n"
"{\\f7\\fmodern lineprinter;}{\\f8\\fswiss Helvetica;}\n"
"{\\f9\\fmodern avantegarde;}{\\f10\\fmodern spartan;}{\\f11\\fmodern metro;}\n"
"{\\f12\\fmodern presentation;}{\\f13\\fmodern APL;}{\\f14\\fmodern OCRA;}\n"
"{\\f15\\fmodern OCRB;}{\\f16\\froman boldPS;}{\\f17\\froman emperorPS;}\n"
"{\\f18\\froman madaleine;}{\\f19\\froman zapf humanist;}\n"
"{\\f20\\froman classic;}{\\f21\\froman roman f;}{\\f22\\froman roman g;}\n"
"{\\f23\\froman roman h;}{\\f24\\froman timesroman;}{\\f25\\froman century;}\n"
"{\\f26\\froman palantino;}{\\f27\\froman souvenir;}{\\f28\\froman garamond;}\n"
"{\\f29\\froman caledonia;}{\\f30\\froman bodini;}{\\f31\\froman university;}\n"
"{\\f32\\fscript script;}{\\f33\\fscript scriptPS;}{\\f34\\fscript script c;}\n"
"{\\f35\\fscript script d;}{\\f36\\fscript commercial script;}\n"
"{\\f37\\fscript park avenue;}{\\f38\\fscript coronet;}\n"
"{\\f39\\fscript script h;}{\\f40\\fscript greek;}{\\f41\\froman kana;}\n"
"{\\f42\\froman hebrew;}{\\f43\\froman roman s;}{\\f44\\froman russian;}\n"
"{\\f45\\froman roman u;}{\\f46\\froman roman v;}{\\f47\\froman roman w;}\n"
"{\\f48\\fdecor narrator;}{\\f49\\fdecor emphasis;}\n"
"{\\f50\\fdecor zapf chancery;}{\\f51\\fdecor decor d;}\n"
"{\\f52\\fdecor old english;}{\\f53\\fdecor decor f;}{\\f54\\fdecor decor g;}\n"
"{\\f55\\fdecor cooper black;}{\\f56\\ftech Symbol;}{\\f57\\ftech linedraw;}\n"
"{\\f58\\ftech math7;}{\\f59\\ftech math8;}{\\f60\\ftech bar3of9;}\n"
"{\\f61\\ftech EAN;}{\\f62\\ftech pcline;}{\\f63\\ftech tech h;}}\n";

static const char dosword_colours[] = "{\\colortbl;"
"\\red0\\green0\\blue0;"	/* black */
"\\red255\\green0\\blue0;"	/* red */
"\\red0\\green255\\blue0;"	/* green */
"\\red0\\green0\\blue255;"	/* blue */
"\\red79\\green47\\blue79;"	/* violet */
"\\red255\\green0\\blue255;"	/* magenta */
"\\red255\\green255\\blue0;"	/* yellow */
"\\red255\\green255\\blue255;"	/* white */
"}";

int main (int argc, char *argv[]) 
{
    struct fileinformationblock fib;
    struct dosword wd;

    if (argc != 3) {
	fprintf (stderr, "Usage: txtwrite file.wri file.rtf\n");
	return 1;
    }

    memset (&wd, 0, sizeof (wd));
    memset (&fib, 0, sizeof (fib));
    wd.fib = &fib;

    wd.f = fopen (argv[1], "rb");
    if (wd.f == NULL) {
#ifdef DEBUG
	perror (argv[1]);
#endif
	return(ERROR_CNV_CANNOT_OPEN_SRC); // 2;
    }

    wd.filename = argv[1];
    if (read_fib (&wd)) {
	fclose (wd.f);
	return (ERROR_CNV_INCORRECT_FORMAT); //1;
    }

    wd.fout = fopen (argv[2], "wb");
    if (wd.fout == NULL) {
#ifdef DEBUG
	perror (argv[2]);
#endif
	return (ERROR_CNV_CANNOT_OPEN_DEST); //2;
    }

    while (1) {
	wd.fcMac = wd.fib->fcMac;
	if (wd.type == TYPE_WRITE) {
	    wri_process_document (&wd);
	} else {
	    /* rtf header and encoding */
	    our_fprintf (&wd, "{\\rtf1\\pc ");
	    if (wd.fib->codepage && (wd.fib->codepage != 437)) {
		our_fprintf (&wd, "\\cpg%d ", wd.fib->codepage);
	    }
	    /* font and colour stuff */
	    our_fprintf (&wd, dosword_fonts);
	    our_fprintf (&wd, dosword_colours);
	    /* summary */
	    if (read_sumd (&wd)) break;
	    if (read_fntb (&wd)) break;
	    print_text_part (&wd, 0x80, wd.fcMac);
	    our_fprintf (&wd, "}\n");
	}
	break;
    }

    fclose (wd.f);
    fclose (wd.fout);

    if (wd.footnote_refs) free (wd.footnote_refs); 
    if (wd.sections) free (wd.sections);
    if (wd.print) free (wd.print);

    return (ERROR_CNV_FROM_RTF);
}
