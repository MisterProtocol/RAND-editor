#include "lalocal.h"

#include "la_prototypes.h"

int
la_error ()
{
    Reg1 int error;

    error = la_errno;
    la_errno = 0;
    return (error);
}

