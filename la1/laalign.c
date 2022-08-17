#include "lalocal.h"

#include "la_prototypes.h"

La_linepos
la_align (olas, nlas)
Reg1 La_stream *olas;
Reg2 La_stream *nlas;
{
    if (!la_verify (olas))
	return -1;
    if (olas->la_file != nlas->la_file) {
	la_errno = LA_NOTSAME;
	return -1;
    }

    STRUCT_COPY (&olas->la_spos, &nlas->la_spos);

    return nlas->la_lpos;
}
