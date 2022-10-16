#include	<stdio.h>
#include	<sys/types.h>

#include	"prototypes.h"

long
getlong (iop)
register FILE *iop;
{
    long i;
    register size_t tmp;
    register char *cp;

    cp = (char *) &i;
    tmp = sizeof (long);
    *cp++ = (char) getc (iop);
/*    if (iop->_flag&_IOEOF) */
    if(feof(iop))
	return -1;
    --tmp;
    do
	*cp++ = (char) getc (iop);
    while (--tmp);

    return i;
}
