
#ifdef HOSTTMPNAMES

#include <stdio.h>
#include <sys/param.h>      /* for MAXHOSTNAMELEN */
#include <unistd.h>


#if SYSV || SOLARIS
#include <sys/utsname.h>
#endif

char my_hostname[MAXHOSTNAMELEN];

char *
mygethostname()
{
#if SYSV || SOLARIS
    static struct utsname u;

    if (uname(&u) < 0)
	 return NULL;

#ifdef DEBUG_XXX_XXX
    printf("sysname =  %s\n", u.sysname);
    printf("nodename=  %s\n", u.nodename);
    printf("release =  %s\n", u.release);
    printf("version =  %s\n", u.version);
    printf("machine =  %s\n", u.machine);
#endif

    return u.nodename;
#else   /* BSD */

    if (gethostname(my_hostname, MAXHOSTNAMELEN) < 0)
	return NULL;
    my_hostname[MAXHOSTNAMELEN-1] = '\0';

    return my_hostname;
#endif
}

#endif /* HOSTTMPNAMES */
