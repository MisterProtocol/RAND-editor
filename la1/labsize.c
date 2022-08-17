#include "lalocal.h"

#include "la_prototypes.h"

#ifndef LA_BP
La_bytepos
la_bsize (plas)
La_stream *plas;
{
    Reg3 La_fsd *tfsd;
    Reg4 La_bytepos nbytes;
    Reg5 La_linepos nlines;

    nbytes = 0;
    tfsd = plas->la_file->la_ffsd;
    for (nlines = plas->la_file->la_nlines; nlines > 0; ) {
	Reg1 char *cp;
	Reg2 int j;

	nlines -= j = tfsd->fsdnlines;
	if (*(cp = tfsd->fsdbytes)) {
	    while (j--) {
#ifndef NOSIGNEDCHAR
		if (*cp < 0)
		    nbytes += -(*cp++) << LA_NLLINE;
#else
		if (*cp & LA_LLINE)
		    nbytes += - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
		nbytes += *cp++;
	    }
	} else {
	    La_linelength speclength;

	    if (*++cp)
		nbytes++;
	    my_move (&cp[1], (char *) &speclength,
		  (unsigned int) sizeof speclength);
	    nbytes += speclength;
	}
	if (!(tfsd = tfsd->fsdforw))
	    la_abort ("fsdforw problem in la_bsize");
    }
    return nbytes;
}
#endif
