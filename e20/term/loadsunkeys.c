/*
 *  Load the Sun2 Functions keys with the E standard (almost) keys
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sundev/kbio.h>
#include <sundev/kbd.h>
#include <sun/fbio.h>

#define ESC 033

struct kiockey def120[] = {
    { 0,  21, CTRL(e), 0 },             /* R1  (d0) */
    { 0,  22, CTRL(]), 0 },             /* R2  (d1) */
    { 0,  23, CTRL(r), 0 },             /* R3  (d2) */
    { 0,  45, CTRL(d), 0 },             /* R4  (d3) */
    { 0,  46, STRING+7, "\030\003" },   /* R5  (d4) */
    { 0,  47, CTRL(f), 0 },             /* R6  (d5) */
    { 0,  68, CTRL(t), 0 },             /* R7  (d6) */
    { 0,  69, CTRL(k), 0 },             /* R8  (b1) */
    { 0,  70, CTRL(y), 0 },             /* R9  (d8) */
    { 0,  91, CTRL(c), 0 },             /* R10 (b3) */
    { 0,  92, CTRL(g), 0 },             /* R11 (da) */
    { 0,  93, CTRL(l), 0 },             /* R12 (b4) */
    { 0, 112, CTRL(z), 0 },             /* R13 (dc) */
    { 0, 113, CTRL(j), 0 },             /* R14 (b2) */
    { 0, 114, CTRL(w), 0 },             /* R15 (de) */
    { SHIFTMASK, 114, STRING+6, "\030\016" },   /* SHIFT R15 (de) */

    { 0,   5, STRING+8, "\030\010" },   /* F1  (e0) */
    { 0,   6, STRING+9, "\030\014" },   /* F2  (e1) */
    { 0,   8, CTRL(_), 0 },             /* F3  (e3) */
    { 0,  10, CTRL(p), 0 },             /* F4  (e5) */
    { 0,  12, CTRL(^), 0 },             /* F5  (e7) */
    { 0,  14, CTRL(v), 0 },             /* F6  (e9) */
    { 0,  16, CTRL(o), 0 },             /* F7  (eb) */
    { 0,  17, CTRL(b), 0 },             /* F8  (ec) */
    { 0,  18, CTRL(n), 0 },             /* F9  (ed) */

    { 0,   3, 034, 0 },                 /* L2  (c2) */
    { 0,  25, STRING+10,"\01\01\037" }, /* L3  (c3) */
    { 0,  26, STRING+11,"\030\024" },   /* L4  (c4) */
    { 0,  49, STRING+12,"\030\012" },   /* L5  (c6) */
    { 0,  51, STRING+13,"\030\027" },   /* L6  (c7) */
    { 0,  72, STRING+14,"\030\002" },   /* L7  (c9) */
    { 0,  73, CTRL(u), 0 },             /* L8  (ca) */
    { 0,  95, STRING+15,"\030\025" },   /* L9  (cc) */
    { 0,  97, CTRL(a), 0 },             /* L10 (ce) */
    { 0, 0, 0, 0 }
};


main()
{
    int i, r, kb;
    struct kiockey *defs;
    char *name;

    i = 0;
    kb = open("/dev/kbd", 2);
    if(kb < 0) {
	perror("open");
	exit(1);
    }

    defs = &def120[0];

    while(defs->kio_station > 0) {
	r = ioctl(kb, KIOCSETKEY, defs);
	if(r < 0) {
	    perror("ioctl");
	    exit(1);
	}
	defs++;
    }
}
