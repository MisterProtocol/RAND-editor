#include	<stdio.h>
#include	<sys/types.h>

#include	"prototypes.h"

short
getshort (iop)
register FILE *iop;
{
    short i;
    register size_t tmp;
    register char *cp;

    cp = (char *) &i;
    tmp = sizeof (short);
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
