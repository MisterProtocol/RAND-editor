#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main()
{
   char *s1 = "abc";
   char msg[30];
   char *t = &msg[0];

   /*strcpy((char *)msg, s1);*/
   strncpy(t, s1, 3);

   /*printf("msg=(%s), s=(%s)\n", msg, s1);*/
   printf("t=(%s), s=(%s)\n", t, s1);

   int i = 3;
   printf("i=%d\n", i);
   exit(0);
}

