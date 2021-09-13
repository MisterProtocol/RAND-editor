#include <sys/ioctl.h>


struct  winsize winsize;      /* 4.3 BSD window sizing        */


main()
{

    if (ioctl(0, TIOCGWINSZ, (char *) &winsize) != 0)
	printf("ioctl failed\n");
    else
	printf("rows = %d cols = %d\n", winsize.ws_row, winsize.ws_col );
}
