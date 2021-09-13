#include <stdio.h>
#include <dirent.h>

main()
{
     DIR *dirp;
     struct dirent *direntp;

     dirp = opendir( "." );
     while ( (direntp = readdir( dirp )) != NULL )
	  (void)printf( "%s\n", direntp->d_name );
     (void)closedir( dirp );
     return (0);
}



