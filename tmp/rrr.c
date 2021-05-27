#include <stdio.h>
#include <stdlib.h>

static void xxx();

#ifdef XXX
int c = 3;
#endif /* XXX */

#ifdef YYY
int d = 1;
#endif /* myxxx */

main() {
  int i;
  printf("hello\n");
  xxx();
  exit(0);
}

static void
xxx ()
{
  printf("xxx\n");
}
