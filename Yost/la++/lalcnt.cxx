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


#include "lalocal.hxx"

#define min(a,b) ((a)<(b)?(a):(b))
#define abs(a) ((a)>=0?(a):(-a))

/*
   La_stream::la_lcount (start, number, mode)
	Mode: -2 and 2 - paragraphs backward and forward
	Mode: -1 and 1 - lines backward and forward
	Return the actual number of lines.
	End of file can cause the number to be less than nl.
	A paragraph is a block of non-blank lines.
	A line with a form-feed in the first column is considered blank.
 */
La_linepos
La_stream::la_lcount (
    La_linepos start,
    La_linepos number,
    int mode
) {
    switch (abs (mode)) {
    case 1:
	{
	    Reg1 La_linepos nlines;

	    if (mode > 0) {
		if ((nlines = la_lsize () - start) <= 0)
		    return 0;
		return min (nlines, number);
	    } else
		return min (start, number);
	}

    case 2:
	if (mode > 0) {
	    Reg1 La_stream *tlas;
	    Reg2 La_linepos nlines;
	    Reg5 La_linepos ngap;

	    La_flag gap;
	    La_stream llas;
	    if (   number <= 0
		|| la_lsize () - start <= 0
	       )
		return 0;
	    if (la_clone (tlas = &llas) == NULL)
		return -1;
	    (void) tlas->la_lseek (start, 0);
	    nlines = 0;
	    ngap = 0;
	    gap = YES;
	    for (;;) {
		Reg3 char *buf = "";
		Reg4 La_linesize size;

		if ((size = (tlas->la_lwsize ())) == 0)
		    break;
		if (gap) {
		    /* inside blank lines */
		    if (size == 1)
			ngap++;
		    else {
			if (tlas->la_lget (buf, 1) < 1) {
			    nlines = -1;
			    break;
			}
			if (   *buf == '\f'
			    && size == 2
			   )
			    ngap++;
			else {
			    nlines += ngap + 1;
			    ngap = 0;
			    gap = NO;
			}
		    }
		} else {
		    /* in text lines */
		    if (size == 1)
			goto newgap;
		    if (tlas->la_lget (buf, 1) < 1) {
			nlines = -1;
			break;
		    }
		    if (*buf == '\f') {
 newgap:                if (--number <= 0)
			    break;
			if (size <= 2) {
			    gap = YES;
			    ngap = 1;
			} else
			    nlines++;
		    } else
			nlines++;
		}
		tlas->la_clseek ((La_linesize) 0);
	    }
	    (void) tlas->la_close ();
	    return nlines;
	} else
	    /* backwards paragraph counting not implemented yet */
	    return 0;
    }
    la_errno = LA_INVMODE;
    return -1;
}