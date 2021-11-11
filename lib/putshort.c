#include	<stdio.h>
#include	<sys/types.h>

#include	"prototypes.h"

void
putshort (i, iop)
short i;
register FILE *iop;
{
    register size_t tmp;
    register char *cp;

    cp = (char *) &i;
    tmp = sizeof (short);
    do
	putc (*cp++, iop);
    while (--tmp);
}
