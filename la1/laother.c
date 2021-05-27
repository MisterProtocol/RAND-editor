#include "lalocal.h"

La_stream *
la_other (plas)
Reg2 La_stream *plas;
{
    Reg1 La_stream *tlas;
    Reg3 La_file *nlaf;

    if (plas->la_file->la_refs > 1) {
	nlaf = plas->la_file;
	for (tlas = nlaf->la_fstream; tlas; tlas = tlas->la_fforw)
	    if (tlas != plas)
		return tlas;
#ifdef LA_DEBUG
	{
	    static char errbuf[]
		= "la_other can't find stream. Refs = %000000\n";
	    extern char *rindex ();

	    sprintf (rindex (errbuf, '%'), "%d\n", plas->la_file->la_refs);
	    la_abort (errbuf);
	}
#endif
    }
    return (La_stream *) 0;
}
