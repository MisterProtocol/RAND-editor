#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main()
{

    int fd;
    char template[200] = "./,esvXXXXXX";
    char *tempfile = &template[0];

    fd = mkstemp(tempfile);
    printf("tempfile name is (%s) fd=(%d)\n", tempfile,fd);
    /*sleep(3);*/
    write(fd, "hi\n", 3);
    close(fd);
/*  unlink(tempfile);*/
    exit(0);
}

