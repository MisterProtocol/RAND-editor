#include <unistd.h>

int
intss()
{
    return isatty(0);
}

#ifdef OUT
#include <sgtty.h>
#include	<sys/types.h>

size_t
intss()
{   int buf[3];

#ifdef UNIXV7
    return(gtty(0,buf) != -1);
#else
    isatty (0);
#endif
}
#endif /* OUT */
