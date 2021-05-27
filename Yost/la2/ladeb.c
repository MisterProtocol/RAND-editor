/*
	Copyright 1988, 1987, 1986 Grand Software, Inc. All Rights Reserved
	Copyright 1986, 1985 David Yost                 All Rights Reserved

	This file is proprietary to and a trade secret of Grand Software,
	Inc. (Los Angeles, CA, USA, Telephone +1 213-650-1089) and may be
	used only in accordance with the terms of the license agreement
	that accompanies the product.  An electronic copy of that license
	is provided in the file "AGREEMENT" in the main directory of the
	distribution.
 */


#include "lalocal.h"

la_fsddump (ffsd, lfsd, fsdbyteflg, txt)
Reg3 La_fsd *ffsd;
Reg4 La_fsd *lfsd;
Reg5 La_flag fsdbyteflg;
char *txt;
{
    printf ("FSD dump:                  %r", &txt);
    putchar ('\n');
    for (;ffsd;) {
	printf ("fsd 0x%lx\n", ffsd);
	printf (" back= 0x%04x, forw= 0x%04x, ",
		ffsd->fsdback,  ffsd->fsdforw);
#ifdef LA_BP
	printf ("nbytes =%5d, ", ffsd->fsdnbytes);
#endif
	printf ("nlines= %3d, file= 0x%04x, ", ffsd->fsdnlines,
		ffsd->fsdfile);
	printf ("pos= %ld\n", ffsd->fsdpos);
	if (fsdbyteflg) {
	    if (   ffsd->fsdnlines == 1
		&& ffsd->fsdbytes[0] == 0
	       ) {
		La_linesize speclength;

		move (&ffsd->fsdbytes[1], (char *) &speclength,
		      sizeof speclength);
#ifdef LA_LONGLINES
		printf ("   Special: %ld\n",
#else
		printf ("   Special: %d\n",
#endif
			speclength);
	    } else {
		Reg1 int j;
		Reg2 int k;

		printf ("   ");
		k = ffsd->fsdnlines;
		for (j = 0; j < k; j++) {
		    printf ("%5d, ", ffsd->fsdbytes[j]);
#ifndef NOSIGNEDCHAR
		    if (ffsd->fsdbytes[j] < 0)
#else
		    if (ffsd->fsdbytes[j] & LA_LLINE)
#endif
			k++;
		}
		putchar ('\n');
	    }
	}
	if (ffsd == lfsd)
	    break;
	ffsd = ffsd->fsdforw;
    }
    return;
}

la_sdump (plas, txt)
Reg1 La_stream *plas;
char *txt;
{
    printf ("La_stream dump:                  %r", &txt);
    putchar ('\n');
    printf ("  file= 0x%lx, cfsd= 0x%lx, fsline= %d, ",
	    plas->la_file, plas->la_cfsd, plas->la_fsline);
    printf ("fsbyte= %d, lbyte= %d, ffpos= %d\n",
	    plas->la_fsbyte, plas->la_lbyte, plas->la_ffpos);
    printf ("  lpos= %d, bpos= %d, back= 0x%lx, forw= 0x%lx, sflags= %d\n",
	    plas->la_lpos,
#ifdef LA_BP
	    plas->la_bpos,
#else
	    0,
#endif
	    plas->la_sback, plas->la_sforw, plas->la_sflags);
    return;
}

la_fdump (plaf, txt)
Reg1 La_file *plaf;
char *txt;
{
    printf ("La_file dump:                  %r", &txt);
    putchar ('\n');
    printf ("  ffsd= 0x%04x, lfsd= 0x%04x, ffs= 0x%04x, ",
	    plaf->la_ffsd, plaf->la_lfsd, plaf->la_ffs);
    printf ("fsrefs= %d, refs= %d, mode= %d\n",
	    plaf->la_fsrefs, plaf->la_refs, plaf->la_mode);
    printf ("  maxline= %d, ", plaf->la_maxline);
    printf ("nlines= %d", plaf->la_nlines);
#ifdef LA_BP
    printf (", nbytes= %D\n", plaf->la_nbytes);
#else
    printf ("\n");
#endif
    return;
}

la_schaindump (txt)
char *txt;
{
    Reg1 La_stream *tlas;

    printf ("La_stream chain dump:             %r", &txt);
    putchar ('\n');
    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw) {
	printf ("  0x%lx\n", tlas);
    }
    return;
}

la_fschaindump (plas, txt)
La_stream *plas;
char *txt;
{
    Reg1 La_stream *tlas;

    printf ("La_stream file chain dump:        %r", &txt);
    putchar ('\n');
    for (tlas = plas->la_file->la_fstream; tlas; tlas = tlas->la_fforw) {
	printf ("  0x%lx\n", tlas);
    }
    return;
}
