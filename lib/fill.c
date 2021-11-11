/*
 *   file fill.c   memory fill
 *
 *   D. Yost
 *
 *   VAX version 3/21/80  D. Yost
 **/
#include <c_env.h>

char *
fill (char *, unsigned int, int);

#ifdef MEMSET
#include <memory.h>

char *
fill (dest, count, fillchar)
char *dest;
unsigned int count;
char fillchar;
{
	memset (dest, fillchar, (size_t)count);
	return (dest+count);
}

#else /* MEMSET */

#ifdef vax

char *
fill (dest, count, fillchar)
char *dest;             /*  4(ap) */
unsigned int count;     /*  8(ap) */
char fillchar;          /* 12(ap) */
{
    char *retval;

    if (count == 0)
	return dest;
    else {
	retval = &dest[count];
	for (;;) {
	    if (count <= 65535) {
		asm ("        movc5   $0,(sp),12(ap),8(ap),*4(ap)")
		break;
	    }
	    else {
		asm ("        movc5   $0,(sp),12(ap),$65535,*4(ap)")
		count -= 65535;
		dest += 65535;
	    }
	}
    }
    return retval;
}

#else

#include <c_env.h>

#ifdef INT4

/*  dest should really be declared as a union, but the compilers
 *  either totally reject the idea of a union as an argument or won't put
 *  a union into a register even if it is a union of single items.
 *  Ritchie's Phototypesetter Version 7 compiler accepts dest as (char *)
 *  and generates the desired optimal code.
 **/
union bw {
    char         *b;
    short        *w;
    long         *l;
    unsigned int  i;
};

char *
fill (dest, count, fillchar)
# ifdef UNIONS_IN_REGISTERS
register union bw dest;
# else
register char *dest;
# endif
register unsigned int count;
register int fillchar;
{
    unsigned int nsave;

    if (count == 0)
	return dest.b;
    if (count < 10)
	do {
	    *dest.b++ = fillchar;
	} while (--count);
    else {
	fillchar &= CHARMASK;
	fillchar |= fillchar << CHARNBITS;
	fillchar |= fillchar << (2 * CHARNBITS);
	if (dest.i & 1) {
	    *dest.b++ = fillchar;
	    count--;
	}
	if (dest.i & 2) {
	    *dest.w++ = fillchar;
	    count -= 2;
	}
	nsave = count;
	count >>= 2;
	do {
	    *dest.l++ = fillchar;
	} while (--count);
	if (nsave & 2)
	    *dest.w++ = fillchar;
	if (nsave & 1)
	    *dest.b++ = fillchar;
    }
    return dest.b;
}

#else

/*  dest should really be declared as a union, but the compilers
 *  either totally reject the idea of a union as an argument or won't put
 *  a union into a register even if it is a union of single items.
 *  Ritchie's Phototypesetter Version 7 compiler accepts dest as (char *)
 *  and generates the desired optimal code.
 **/
union bw {
    char         *b;
    short        *w;
    unsigned int  i;
};

char *
fill (dest, count, fillchar)
# ifdef UNIONS_IN_REGISTERS
register union bw dest;
# else
register char *dest;
# endif
register unsigned int count;
register int fillchar;
{
    unsigned int nsave;

    if (count == 0)
	return dest;
    if (count < 10)
	do {
	    *dest.b++ = fillchar;
	} while (--count);
    else {
	fillchar &= CHARMASK;
	fillchar |= fillchar << CHARNBITS;
	if (dest.i & 1) {
	    *dest.b++ = fillchar;
	    count--;
	}
	nsave = count;
	count >>= 1;
	do {
	    *dest.w++ = fillchar;
	} while (--count);
	if (nsave & 1)
	    *dest.b++ = fillchar;
    }
    return dest.b;
}

#endif
#endif

#ifdef _FILLTEST

#include <stdio.h>

#ifdef vax
#define BOUND 65536
#else
#define BOUND 100
#endif

char alph[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char test[BOUND+500] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

main (argc, argv)
int argc;
char *argv[];
{
    char fillchar;
    char *dest;
    unsigned int cnt;

    if (argc != 4){
	    printf ("%s: dest count fillchar\n", argv[0]);
	    exit (1);
    }
    move (&alph[36], &test[BOUND-10-26], 26);
    move (alph, &test[BOUND-10], 62);

    printf ("%39.39s %39.39s\n", test, &test[BOUND-10]);
    fillchar = argv[3][0];
    dest =   &test[atoi (argv[1])];
    cnt = atoi (argv[2]);
    printf ("%d\n", fill (dest, cnt, fillchar) - dest);
    printf ("%39.39s %39.39s\n", test, &test[BOUND-10]);
    return 0;
}
#endif

#endif /* MEMSET */
