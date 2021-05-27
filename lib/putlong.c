#include	<stdio.h>
#include	<sys/types.h>

void
putlong (i, iop)
long i;
register FILE *iop;
{
    register size_t tmp;
    register char *cp;

    cp = (char *) &i;
    tmp = sizeof (long);
    do
	putc (*cp++, iop);
    while (--tmp);
}
