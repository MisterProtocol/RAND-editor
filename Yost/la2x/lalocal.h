/*
	Copyright 1988, 1987, 1986 Grand Software, Inc. All Rights Reserved
	Copyright 1986, 1985 David Yost                 All Rights Reserved

	This file is proprietary to and a trade secret of Grand Software,
	Inc. (Los Angeles, CA, USA, Telephone +1 213-650-1089) and may be
	used only in accordance with the terms of the license agreement
	that accompanies the product.  An electronic copy of that license
	is provided in the file "AGREEMENT" in the main directory of the
	distribution.
 */

#include <c_env.h>
#include <localenv.h>

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

#include "la.h"

#define BRK_REAL  0
#define BRK_COPY  1
#define BRK_AGAIN 2

extern La_linepos   la_parse ();
extern La_flag      la_zbreak (), la_break ();

#ifdef  STRUCTASSIGN
#define STRUCT_COPY(from, to) (*(to) = *(from))
#else/* STRUCTASSIGN */
#define STRUCT_COPY(from, to) move (from, to, sizeof *from)
#endif/*STRUCTASSIGN */

extern union la_zero {
    La_stream las;
    La_file   laf;
};
extern union la_zero la_zero;
