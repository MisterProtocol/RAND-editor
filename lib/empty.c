
#include <localenv.h>
#ifdef SOLARIS
#include <sys/filio.h>
#endif

#ifdef NOIOCTL_H
#include <sys/tty.h>
#else
#include <sys/ioctl.h>
#endif

#ifdef FIONREAD
int
empty (int fd)
{
    long count;

    if (ioctl (fd, FIONREAD, &count) < 0)
	return 1;
    return count <= 0;
}
#endif /* FIONREAD */
