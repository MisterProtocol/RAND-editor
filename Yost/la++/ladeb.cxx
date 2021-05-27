/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986, 1987 Grand Software, Inc. All Rights Reserved
	THIS FILE CONTAINS UNPUBLISHED PROPRIETARY SOURCE CODE
	which is property of Grand Software, Inc.
	Los Angeles, CA 90046 U.S.A 213-650-1089
	The copyright notice above does not evidence any
	actual or intended publication of such source code.
	This file is not to be copied by anyone except as
	covered by written agreement with Grand Software, Inc.,
	and this notice is not to be removed.
 */


#include "lalocal.hxx"

void
la_fsddump (
    Reg3 La_fsd* ffsd,
    Reg4 La_fsd* lfsd,
    Reg5 La_flag fsdbyteflg,
    char*        txt
) {
    fprintf (stderr, "FSD dump:                  %r", &txt);
    putchar ('\n');
    for (;ffsd;) {
	fprintf (stderr, "fsd 0x%lx\n", ffsd);
	fprintf (stderr, " back= 0x%04x, forw= 0x%04x, ",
		ffsd->fsdback,  ffsd->fsdforw);
#ifdef LA_BP
	fprintf (stderr, "nbytes =%5d, ", ffsd->fsdnbytes);
#endif
	fprintf (stderr, "nlines= %3d, file= 0x%04x, ", ffsd->fsdnlines,
		ffsd->fsdfile);
	fprintf (stderr, "pos= %ld\n", ffsd->fsdpos);
	if (fsdbyteflg) {
	    if (   ffsd->fsdnlines == 1
		&& ffsd->fsdbytes[0] == 0
	       ) {
		La_linesize speclength;

		move (&ffsd->fsdbytes[2], (char *) &speclength,
		      sizeof speclength);
#ifdef LA_LONGLINES
		fprintf (stderr, "   Special: %d, %D\n",
#else
		fprintf (stderr, "   Special: %d, %d\n",
#endif
			ffsd->fsdbytes[1], speclength);
	    } else {
		Reg1 int j;
		Reg2 int k;

		fprintf (stderr, "   ");
		k = ffsd->fsdnlines;
		for (j = 0; j < k; j++) {
		    fprintf (stderr, "%5d, ", ffsd->fsdbytes[j]);
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

void
La_stream::la_sdump (
    char *txt
)
{
    fprintf (stderr, "La_stream dump:                  %r", &txt);
    putchar ('\n');
    fprintf (stderr, "  file= 0x%lx, cfsd= 0x%lx, fsline= %d, ",
	    la_file, la_cfsd, la_fsline);
    fprintf (stderr, "fsbyte= %d, lbyte= %d, ffpos= %d\n",
	    la_fsbyte, la_lbyte, la_ffpos);
    fprintf (stderr, "  lpos= %d, bpos= %d, back= 0x%lx, forw= 0x%lx, sflags= %d\n",
	    la_lpos,
#ifdef LA_BP
	    la_bpos,
#else
	    0,
#endif
	    la_sback, la_sforw, la_sflags);
    return;
}

void
la_fdump (
    Reg1 La_file* plaf,
    char* txt
)
{
    fprintf (stderr, "La_file dump:                  %r", &txt);
    putchar ('\n');
    fprintf (stderr, "  ffsd= 0x%04x, lfsd= 0x%04x, ffs= 0x%04x, ",
	    plaf->la_ffsd, plaf->la_lfsd, plaf->la_ffs);
    fprintf (stderr, "fsrefs= %d, refs= %d, mode= %d\n",
	    plaf->la_fsrefs, plaf->la_refs, plaf->la_mode);
    fprintf (stderr, "  maxline= %d, ", plaf->la_maxline);
    fprintf (stderr, "nlines= %d", plaf->la_nlines);
#ifdef LA_BP
    fprintf (stderr, ", nbytes= %D\n", plaf->la_nbytes);
#else
    fprintf (stderr, "\n");
#endif
    return;
}

void
La_stream::la_schaindump (
    char *txt
)
{
    Reg1 La_stream *tlas;

    fprintf (stderr, "La_stream chain dump:             %r", &txt);
    putchar ('\n');
    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw) {
	fprintf (stderr, "  0x%lx\n", tlas);
    }
    return;
}

void
La_stream::la_fschaindump (
    char *txt
)
{
    Reg1 La_stream *tlas;

    fprintf (stderr, "La_stream file chain dump:        %r", &txt);
    putchar ('\n');
    for (tlas = la_file->la_fstream; tlas; tlas = tlas->la_fforw) {
	fprintf (stderr, "  0x%lx\n", tlas);
    }
    return;
}
