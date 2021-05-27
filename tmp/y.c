#include <stdio.h>
#include <string.h>

main()
{
   char *s1 = "abc";
   char msg[30];
   char *t = &msg[0];

   /*strcpy((char *)msg, s1);*/
   strcpy(t, s1);

   /*printf("msg=(%s), s=(%s)\n", msg, s1);*/
   printf("t=(%s), s=(%s)\n", t, s1);

}

