#include <sys/ioctl.h>


struct  winsize winsize;      /* 4.3 BSD window sizing        */


main()
{

    if (ioctl(0, TIOCGWINSZ, (char *) &winsize) != 0)
	printf("ioctl failed\n");
    else
	printf("wid = %d ht = %d\n", winsize.ws_row, winsize.ws_col );
}
