#include <stdio.h>
#include "e.keys.h"

char *getEkeyname(int i);
char *getCursesKeyname(int i);



main()
{
    int i;
    int len1 = sizeof(Curses_Keys);
    int len2 =  sizeof(Curses_Keys[0]);
    int max = len1/len2;
    printf("len1=%d, len2=%d max=%d\n", len1, len2, max);
/*  exit(0);*/

    for( i=0; i < max; i++ ) {

      if( Curses_Keys[i].val == -1 )
	break;

      printf("i=%02d key (%03o) = %s\n", i, Curses_Keys[i].val, Curses_Keys[i].name);

    }

    printf("%s %s %s\n", getCursesKeyname(0631), getCursesKeyname(0403), getCursesKeyname(0522));

}


char *
getCursesKeyname(int c) {

    int i;

    for(i=0; i < sizeof(Curses_Keys) / sizeof(Curses_Keys[0]); i++) {
       if( Curses_Keys[i].val == c )
	 return Curses_Keys[i].name;
    }

    return "NOT FOUND";
}

