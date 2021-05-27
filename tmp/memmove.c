#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main()
{
    char a[] = "123456789";
    char b[] = "aaaaaaaaa";
    memmove(a+3, b, 5);
    printf("a=%s\n", a);
    exit(0);
}

