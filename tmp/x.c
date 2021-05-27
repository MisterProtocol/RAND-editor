#include <stdio.h>
#include <string.h>

static char MesgBuf[256];

main()
{
  MesgHack(070+1, "one");
  MesgHack(070+2, "one", "two");
  MesgHack(070+3, "one", "two", "three" );
}



MesgHack (parm, s1, s2, s3, s4, s5, s6)
int parm;
char *s1,*s2,*s3,*s4,*s5,*s6;
{

printf("parm=(%d)\n", parm);

   int n = parm & 7;

printf("parm=(%d), n=%d\n", parm, n);

   char msg[200] = "";
   /*char *t = &msg[0];*/
   char *t = &MesgBuf[0];

   switch (n) {
     case 1:
       strcpy(t, s1);
       break;
     case 2:
       sprintf(t, "%s%s", s1,s2);
       break;
     case 3:
       sprintf(t, "%s%s%s", s1,s2,s3);
       break;
     case 4:
       sprintf(t, "%s%s%s%s", s1,s2,s3,s4);
       break;
     case 5:
       sprintf(t, "%s%s%s%s%s", s1,s2,s3,s4,s5);
       break;
     break;
   }
   msg[199] = '\0';
   MesgBuf[255] = '\0';
printf("msg=()\n", msg);


   parm = parm&0170;
   /*mesg(parm, &msg[0]);*/
   mesg(parm, &MesgBuf[0]);
}


mesg(parm, msg)
int parm;
char *msg;
{
    printf("PARM=(%d), msg=(%s)\n", parm, msg);
}
