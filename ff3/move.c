/*
 *   file move.c   memory move
 *
 **/
#include <c_env.h>

#ifdef BCOPY
char *
my_move (source, dest, count)
char *source, *dest;
unsigned int count;
{
	bcopy( source, dest, (int)count );
	return( &dest[count] );
}
#else
char *
my_move (source, dest, count)
char *source, *dest;
unsigned int count;
{
	memmove( dest, source, count );
	return( &dest[count] );
}
#endif
