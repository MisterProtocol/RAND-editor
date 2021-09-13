#include <sys/types.h>
#include <sys/time.h>




main()
{       int rc;
	int readmask;
	struct timeval timeout;

	while( 1 ) {
	    timeout.tv_sec = 5;
	    timeout.tv_usec = 0;
	    readmask = 1;   /* for fd 0 */
	    sleep(2);
	    rc = select( 1, &readmask, 0, 0, &timeout);
	    printf( "rc = %d\n", rc);
	    if( rc > 0 ) {
		int c;
		do {
		    c = getchar();
		} while( c != '\n' );
	    }
	    else if (rc == 0)
		printf(" timed out\n" );
	    else
		perror("select:");
	}
}
