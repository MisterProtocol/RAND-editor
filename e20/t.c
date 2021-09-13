#include <stdlib.h>
#include <stdio.h>

void
makebuf(size_t s)
{
	char buf[s];

	snprintf (buf, s, "abcd");
	printf ("Buf holds: \'%s\' sizeof buf=%ld\n", buf, sizeof(buf));

	return;
}

int
main(int argc, char **argv)
{
	if (argc != 2) {
	    printf("usage:  t bufsize\n");
	    exit(1);
	}
	int i = atoi(argv[1]);
	printf ("i = %d\n", i);
	makebuf (i);

	exit(0);
}
