#include <stdio.h>
#include "e.keys.h"

char *getEkeyname(int i);

void exit();

main()
{
    int i;
    int len1 = sizeof(E_Keys);
    int len2 =  sizeof(E_Keys[0]);
    int max = len1/len2;
    printf("len1=%d, len2=%d max=%d\n", len1, len2, max);
/*  exit(0);*/

    for( i=0; i < max; i++ ) {

      if( E_Keys[i].val == -1 )
	break;

      printf("i=%02d key (%03o) = %s\n", i, E_Keys[i].val, E_Keys[i].name);

    }

    printf("%s %s %s\n", getEkeyname(4), getEkeyname(15), getEkeyname(0236));

}


char *
getEkeyname(int c) {

    int i;

    for(i=0; i < sizeof(E_Keys) / sizeof(E_Keys[0]); i++) {
       if( E_Keys[i].val == c )
	 return E_Keys[i].name;
    }

    return "NOT FOUND";
}

