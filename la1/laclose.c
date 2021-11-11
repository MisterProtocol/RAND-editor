#include "lalocal.h"
#include <stdlib.h>

#include "la_prototypes.h"

La_linepos
la_close (plas)
Reg1 La_stream *plas;
{
    Reg2 La_file *nlaf;
    Reg3 La_flag freelaf;

    if (!la_verify (plas))
	return -1;
    if (   plas == la_chglas
	|| (nlaf = plas->la_file)->la_refs < 1
       ) {
	la_errno = LA_BADSTREAM;
	return -1;
    }

    freelaf = NO;
    if (nlaf->la_refs > 1)
	nlaf->la_refs--;
    else {
	if (nlaf->la_fsrefs > 0)
	    la_freefsd (nlaf->la_ffsd);
	if (nlaf->la_fsrefs > 0)
	    return nlaf->la_fsrefs;
	else if (nlaf->la_fsrefs < 0)
	    la_abort ("nlaf->la_fsrefs < 0 in la_close");
	else {
	    ff_close (nlaf->la_ffs);
	    freelaf = YES;
	    la_chans--;
	}
    }

    /* unlink the stream from the chain of all streams */
    if (plas->la_sback)
	plas->la_sback->la_sforw = plas->la_sforw;
    if (plas->la_sforw)
	plas->la_sforw->la_sback = plas->la_sback;
    else
	la_laststream = plas->la_sback;

    /* unlink the stream from the chain streams into this file */
    if (plas->la_fback)
	plas->la_fback->la_fforw = plas->la_fforw;
    else
	nlaf->la_fstream = plas->la_fforw;
    if (plas->la_fforw)
	plas->la_fforw->la_fback = plas->la_fback;
    else
	nlaf->la_lstream = plas->la_fback;

    if (freelaf)
	free ((char *) nlaf);
    if (plas->la_sflags & LA_ALLOCED)
	free ((char *) plas);
    return 0;
}
