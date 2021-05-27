/*
 *   file move.c   memory move
 *
 **/
#include <c_env.h>
#ifdef GCC
#include <string.h>
#endif /* GCC */

char *
my_move (source, dest, count)
char *source, *dest;
/*unsigned int count;*/
unsigned long count;
{
	(void) memmove( dest, source, count );
	return( &dest[count] );
}
