#include "lalocal.h"

#ifdef GCC
#include <stdlib.h>
#endif

/* VARARGS1 */
void
la_abort (str)
char *str;
{
    fprintf (stderr, "LA ABORT: %s\n", str);
    fflush (stderr);
    abort ();
    /* NOTREACHED */
}
