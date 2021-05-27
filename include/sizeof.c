#include <stdio.h>


main()
{
	char c;
	unsigned char uc;

	int i;
	unsigned int ui;

	short s;
	unsigned short us;

	long l;
	unsigned long ul;

	printf ("sizeof   c=%d\n", sizeof(c));
	printf ("sizeof  uc=%d\n", sizeof(uc));

	printf ("sizeof   s=%d\n", sizeof(s));
	printf ("sizeof  us=%d\n", sizeof(us));

	printf ("sizeof   i=%d\n", sizeof(i));
	printf ("sizeof  ui=%d\n", sizeof(ui));

	printf ("sizeof   l=%d\n", sizeof(l));
	printf ("sizeof  ul=%d\n", sizeof(ul));
}
