#include "lalocal.h"

#include "la_prototypes.h"

void
la_fsddump (ffsd, lfsd, fsdbyteflg, txt)
Reg3 La_fsd *ffsd;
Reg4 La_fsd *lfsd;
Reg5 La_flag fsdbyteflg;
char *txt;
{
    printf ("FSD dump:                  %0lx", (unsigned long)&txt);
    putchar ('\n');
    for (;ffsd;) {
	printf ("fsd 0x%lx\n", (unsigned long)ffsd);
#ifdef OLD_PRINTF
	printf (" back= 0x%04x, forw= 0x%04x, ",
		(unsigned int)ffsd->fsdback,  (unsigned int)ffsd->fsdforw);
#else
	printf (" back= %p, forw= %p, ",
		ffsd->fsdback,  ffsd->fsdforw);
#endif /* OLD_PRINTF */
#ifdef LA_BP
	printf ("nbytes =%5d, ", ffsd->fsdnbytes);
#endif
	printf ("nlines= %3d, file= %p, ", ffsd->fsdnlines,
		ffsd->fsdfile);
	printf ("pos= %ld\n", ffsd->fsdpos);
	if (fsdbyteflg) {
	    if (   ffsd->fsdnlines == 1
		&& ffsd->fsdbytes[0] == 0
	       ) {
		La_linesize speclength;

		my_move (&ffsd->fsdbytes[1], (char *) &speclength,
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

void
la_sdump (plas, txt)
Reg1 La_stream *plas;
char *txt;
{
    printf ("La_stream dump:                  %0lx", (unsigned long)&txt);
    putchar ('\n');
    printf ("  file= 0x%lx, cfsd= 0x%lx, fsline= %d, ",
	    (unsigned long)plas->la_file, (unsigned long)plas->la_cfsd, plas->la_fsline);
    printf ("fsbyte= %d, lbyte= %d, ffpos= %d\n",
	    plas->la_fsbyte, plas->la_lbyte, plas->la_ffpos);
    printf ("  lpos= %ld, bpos= %d, back= 0x%lx, forw= 0x%lx, sflags= %d\n",
	    plas->la_lpos,
#ifdef LA_BP
	    plas->la_bpos,
#else
	    0,
#endif
	    (unsigned long)plas->la_sback, (unsigned long)plas->la_sforw, plas->la_sflags);
    return;
}

void
la_fdump (plaf, txt)
Reg1 La_file *plaf;
char *txt;
{
    printf ("La_file dump:                  %0lx", (unsigned long)&txt);
    putchar ('\n');
#ifdef OLD_PRINTF
    printf ("  ffsd= 0x%04x, lfsd= 0x%04x, ffs= 0x%04x, ",
	    (unsigned int)plaf->la_ffsd, (unsigned int)plaf->la_lfsd, (unsigned int)plaf->la_ffs);
#else
    printf ("  ffsd= %p, lfsd= %p, ffs= %p, ",
	    plaf->la_ffsd, plaf->la_lfsd, plaf->la_ffs);
#endif /* OLD_PRINTF */
    printf ("fsrefs= %ld, refs= %d, mode= %d\n",
	    plaf->la_fsrefs, plaf->la_refs, plaf->la_mode);
    printf ("  maxline= %ld, ", plaf->la_maxline);
    printf ("nlines= %ld", plaf->la_nlines);
#ifdef LA_BP
    printf (", nbytes= %D\n", plaf->la_nbytes);
#else
    printf ("\n");
#endif
    return;
}

void
la_schaindump (txt)
char *txt;
{
    Reg1 La_stream *tlas;

    printf ("La_stream chain dump:             %0lx", (unsigned long)&txt);
    putchar ('\n');
    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw) {
	printf ("  0x%lx\n", (unsigned long)tlas);
    }
    return;
}

void
la_fschaindump (plas, txt)
La_stream *plas;
char *txt;
{
    Reg1 La_stream *tlas;

    printf ("La_stream file chain dump:        %0lx", (unsigned long)&txt);
    putchar ('\n');
    for (tlas = plas->la_file->la_fstream; tlas; tlas = tlas->la_fforw) {
	printf ("  0x%lx\n", (unsigned long)tlas);
    }
    return;
}
