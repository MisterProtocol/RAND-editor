/*   C Standard defines.
 *   This file containes machine- and compiler-specific #defines
 **/

#ifdef OUT_OUT_OUT
#ifndef SUN
#ifndef vax
# define vax
#endif /* vax */
#endif /* SUN */
#endif /* OUT_OUT_OUT */

/*  In the #define's that follow, define the first n to 'register'
    and the rest to nothing, where 'n' is the number of registers
    supported by your compiler.
    For an explanation of this, see ../man/man5/c_env.5
 **/
#define Reg1  register
#define Reg2  register
#define Reg3  register
#define Reg4  register
#define Reg5  register
#define Reg6  register
#define Reg7
#define Reg8
#define Reg9
#define Reg10
#define Reg11
#define Reg12

#define CHARMASK   0xFF
#define CHARNBITS  8
#define MAXCHAR    0x7F

#define SHORTMASK  0xFFFF
#define SHORTNBITS 16
#define MAXSHORT   0x7FFF

/*
#define LONGMASK  0xFFFFFFFF
#define LONGNBITS 32
#define MAXLONG   0x7FFFFFFF
*/

#define LONGMASK  0xFFFFFFFFFFFFFFFF
#define LONGNBITS 64
#define MAXLONG   0x7FFFFFFFFFFFFFFF

#define INTMASK  0xFFFFFFFF
#define INTNBITS 32
#define MAXINT   0x7FFFFFFF

#define BIGADDR         /* text address space > 64K */
/* fine ADDR64K          * text and data share 64K of memory (no split I&D */

#define INT4            /* sizeof (int) == 4 */
/* fine INT2             * sizeof (int) == 2 */

#define PTR4            /* sizeof (char *) == 4 */
/* fine PTR2             * sizeof (char *) == 2 */

			/* unsigned types supported by the compiler: */
#define UNSCHAR         /* unsigned char  */
#define UNSSHORT        /* unsigned short */
#define UNSLONG         /* unsigned long  */

/* fine NOSIGNEDCHAR     * there is no signed char type */

#define STRUCTASSIGN	/* Compiler does struct assignments */

#define UNIONS_IN_REGISTERS     /* compiler allows unions in registers */

/* fine void int         * Fake the new 'void' type to an int */
