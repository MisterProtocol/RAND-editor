#include <stdio.h>


char matched[30];
char *m, *p, *last;
char str[] = "abcdefghijklmnopqrs";

main()
{
	int i;

	last = str + 5;
	i = 0;

	p = str;
	m = p;
	while(m < last)
	    matched[m - p] = *m++;  /* compiler bug! */
	matched[m - p] = '\0';
	printf("matched = %s\n", matched);

	p = str;
	m = p;
	while(m < last)
	    matched[i++] = *m++;
	matched[m - p] = '\0';
	printf("matched = %s\n", matched);
}
