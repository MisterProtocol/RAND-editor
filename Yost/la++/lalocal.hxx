/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986 Grand Software, Inc. All Rights Reserved
	THIS FILE CONTAINS UNPUBLISHED PROPRIETARY SOURCE CODE
	which is property of Grand Software, Inc.
	Los Angeles, CA 90046 U.S.A 213-650-1089
	The copyright notice above does not evidence any
	actual or intended publication of such source code.
	This file is not to be copied by anyone except as
	covered by written agreement with Grand Software, Inc.,
	and this notice is not to be removed.
 */

#include <c_env.h>
#include <localenv.h>
#include <malloc.h>

#if defined(NOREGCHAR) || defined(NOREGSHORT)
#undef Reg1
#undef Reg2
#undef Reg3
#undef Reg4
#undef Reg5
#undef Reg6
#define Reg1
#define Reg2
#define Reg3
#define Reg4
#define Reg5
#define Reg6
#endif

#include <sys/types.h>

#define Block

#define LA_FSDSIZE (sizeof (La_fsd) - 1)

#ifdef LA_DEBUG
#define LA_FSDLMAX  5       /* max for fsdnlines */
#else
#define LA_FSDLMAX  127     /* max for fsdnlines */
#endif
#define LA_FSDBMAX  255     /* max for fsdbytes */
#define LA_FSDNBMAX 32767   /* max for fsdnbytes */

#include "la.hxx"

#define BRK_REAL  0
#define BRK_COPY  1
#define BRK_AGAIN 2

extern void la_freefsd (La_fsd *tfsd);

La_linepos la_parse (
    Ff_stream*  pffs,   /* NULL means parse memory buffer */
    long        seekpos,
    La_fsd**    ffsd,
    La_fsd**    lfsd,
    La_file*    plaf,
    La_bytepos  nchars,
    char*       buf
);

extern int        la_errno;     /* last non-La_stream error that occurred */
