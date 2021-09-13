/*
 *  Reset Sun2 Functions keys
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sundev/kbio.h>
#include <sundev/kbd.h>
#include <sun/fbio.h>


struct kiockey def120[] = {
    { 0,  21, RF(1),   0 },                 /* R1  (d0) */
    { 0,  22, RF(2),   0 },                 /* R2  (d1) */
    { 0,  23, RF(3),   0 },                 /* R3  (d2) */
    { 0,  45, RF(4),   0 },                 /* R4  (d3) */
    { 0,  46, RF(5),   0 },                 /* R5  (d4) */
    { 0,  47, RF(6),   0 },                 /* R6  (d5) */
    { 0,  68, RF(7),   0 },                 /* R7  (d6) */
    { 0,  69, STRING+UPARROW, "\033[A"},    /* R8  (b1) */
    { 0,  70, RF(9),   0 },                 /* R9  (d8) */
    { 0,  91, STRING+LEFTARROW, "\033[D"},  /* R10 (b3) */
    { 0,  92, RF(11),  0 },                 /* R11 (da) */
    { 0,  93, STRING+RIGHTARROW, "\033[C"}, /* R12 (b4) */
    { 0, 112, RF(13),  0 },                 /* R13 (dc) */
    { 0, 113, STRING+DOWNARROW, "\033[B"},  /* R14 (b2) */
    { 0, 114, RF(15),  0 },                 /* R15 (de) */
    { SHIFTMASK, 114, RF(15), 0 },          /* SHIFT R15 (de) */

    { 0,   5, TF(1),   0 },                 /* F1  (e0) */
    { 0,   6, TF(2),   0 },                 /* F2  (e1) */
    { 0,   8, TF(3),   0 },                 /* F3  (e3) */
    { 0,  10, TF(4),   0 },                 /* F4  (e5) */
    { 0,  12, TF(5),   0 },                 /* F5  (e7) */
    { 0,  14, TF(6),   0 },                 /* F6  (e9) */
    { 0,  16, TF(7),   0 },                 /* F7  (eb) */
    { 0,  17, TF(8),   0 },                 /* F8  (ec) */
    { 0,  18, TF(9),   0 },                 /* F9  (ed) */

    { 0,   3, LF(2),   0 },                 /* L2  (c2) */
    { 0,  25, LF(3),   0 },                 /* L3  (c3) */
    { 0,  26, LF(4),   0 },                 /* L4  (c4) */
    { 0,  49, LF(5),   0 },                 /* L5  (c6) */
    { 0,  51, LF(6),   0 },                 /* L6  (c7) */
    { 0,  72, LF(7),   0 },                 /* L7  (c9) */
    { 0,  73, LF(8),   0 },                 /* L8  (ca) */
    { 0,  95, LF(9),   0 },                 /* L9  (cc) */
    { 0,  97, LF(10),  0 },                 /* L10 (ce) */
    { 0, 0, 0, 0 }
};


main() {
    int i, r, kb;
    struct kiockey *defs;
    char *name;

    i = 0;
    kb = open("/dev/kbd", 2);
    if(kb < 0) perror("open");

    defs = &def120[0];

    while(defs->kio_station > 0) {
	r = ioctl(kb, KIOCSETKEY, defs);
	if(r < 0) perror("ioctl");
	defs++;
    }
}
