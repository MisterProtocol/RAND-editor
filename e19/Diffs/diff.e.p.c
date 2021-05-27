*** e.p.c.old	Tue Jun 11 16:28:52 1985
--- e.p.c	Wed Jun 12 14:37:11 1985
***************
*** 87,93
      if (!optstick)
  	if (cursorcol == curwin->rtext) {
  #ifdef LMCAUTO
! 	    if (key == ' ' || (autofill && curcol <= linewidth)) {
  		horzmvwin (defrwin);
  		getline (ln);
  	    }

--- 87,93 -----
      if (!optstick)
  	if (cursorcol == curwin->rtext) {
  #ifdef LMCAUTO
! 	    if (!autofill || (autofill && curcol <= linewidth)) {
  		horzmvwin (defrwin);
  		getline (ln);
  	    }
***************
*** 124,130
  	    putch (key, YES);
      }
      else {
! 	if (insmode) {
  	    if (ncline >= lcline)
  		excline ((Ncols) 0);
  	    /* (ncline - curcol > 1) at this point */

--- 124,131 -----
  	    putch (key, YES);
      }
      else {
! 	if (insmode) Block {
! 		Ncols thiscol;
  	    if (ncline >= lcline)
  		excline ((Ncols) 0);
  	    /* (ncline - curcol > 1) at this point */
***************
*** 132,138
  		  (Uint) (ncline - curcol));
  	    ncline++;
  	    cline[curcol] = key;
! 	    curcol -= curwksp->wcol;
  	    putupdelta = 1;
  	    putup (-1, cursorline, (Scols) curcol, MAXWIDTH);
  	    poscursor ((Scols) (++curcol), cursorline);

--- 133,139 -----
  		  (Uint) (ncline - curcol));
  	    ncline++;
  	    cline[curcol] = key;
! 	    thiscol = curcol - curwksp->wcol;
  	    putupdelta = 1;
  	    putup (-1, cursorline, (Scols) thiscol, MAXWIDTH);
  	    poscursor ((Scols) thiscol+1, cursorline);
***************
*** 134,141
  	    cline[curcol] = key;
  	    curcol -= curwksp->wcol;
  	    putupdelta = 1;
! 	    putup (-1, cursorline, (Scols) curcol, MAXWIDTH);
! 	    poscursor ((Scols) (++curcol), cursorline);
  	}
  	else {
  	    cline[curcol] = key;

--- 135,142 -----
  	    cline[curcol] = key;
  	    thiscol = curcol - curwksp->wcol;
  	    putupdelta = 1;
! 	    putup (-1, cursorline, (Scols) thiscol, MAXWIDTH);
! 	    poscursor ((Scols) thiscol+1, cursorline);
  	}
  	else {
  	    cline[curcol] = key;
***************
*** 187,192
      Flag cmdmod;
  .
  	This function sets/resets the autofill capability.
  #endif
  void
  infoauto (cmdmod)

--- 188,194 -----
      Flag cmdmod;
  .
  	This function sets/resets the autofill capability.
+ 	Changed to not toggle:  wp sets (cmdmod=NO), -wp resets (cmdmod=YES).
  #endif
  void
  infoauto (cmdmod)
***************
*** 194,199
  {
      Flag moved;
  
      autofill = autofill ? NO : YES;
      if (autofill) {
  	info (inf_auto, 2, "WP");

--- 196,204 -----
  {
      Flag moved;
  
+     if (!cmdmod && autofill || cmdmod && !autofill)  /* already set/reset */
+ 	return;
+ 
      autofill = autofill ? NO : YES;
      if (autofill) {
  	info (inf_auto, 2, "WP");
***************
*** 197,208
      autofill = autofill ? NO : YES;
      if (autofill) {
  	info (inf_auto, 2, "WP");
- 	if (cmdmod)
- #ifdef LMCMARG
- 	    setmarg (&linewidth, cursorcol);
- #else  LMCMARG
- 	    linewidth = cursorcol;
- #endif LMCMARG
  	if (curmark) {
  	    moved = gtumark(YES);
  #ifdef LMCMARG

--- 202,207 -----
      autofill = autofill ? NO : YES;
      if (autofill) {
  	info (inf_auto, 2, "WP");
  	if (curmark) {
  	    moved = gtumark(YES);
  #ifdef LMCMARG
***************
*** 207,212
  	    moved = gtumark(YES);
  #ifdef LMCMARG
  	    setmarg (&autolmarg, leftmark());
  #else  LMCMARG
  	    autolmarg = leftmark();
  #endif LMCMARG

--- 206,213 -----
  	    moved = gtumark(YES);
  #ifdef LMCMARG
  	    setmarg (&autolmarg, leftmark());
+ 	    if (markcols)
+ 		setmarg (&linewidth, autolmarg + markcols);
  #else  LMCMARG
  	    autolmarg = leftmark();
  	    linewidth = autolmarg + markcols;
***************
*** 209,214
  	    setmarg (&autolmarg, leftmark());
  #else  LMCMARG
  	    autolmarg = leftmark();
  #endif LMCMARG
  	    domark(moved);
  	}

--- 210,216 -----
  		setmarg (&linewidth, autolmarg + markcols);
  #else  LMCMARG
  	    autolmarg = leftmark();
+ 	    linewidth = autolmarg + markcols;
  #endif LMCMARG
  	    domark (moved);
  	}
***************
*** 210,216
  #else  LMCMARG
  	    autolmarg = leftmark();
  #endif LMCMARG
! 	    domark(moved);
  	}
      } else
  	info (inf_auto, 2, "");

--- 212,218 -----
  	    autolmarg = leftmark();
  	    linewidth = autolmarg + markcols;
  #endif LMCMARG
! 	    domark (moved);
  	}
      } else
  	info (inf_auto, 2, "");
***************
*** 391,396
      Reg3 Ncols cwcol;
      Reg4 Nlines cwln;
      Ncols nchfol;
  
      curcol = cursorcol + curwksp->wcol;
      getline (ln = curwksp->wlin + cursorline);

--- 393,399 -----
      Reg3 Ncols cwcol;
      Reg4 Nlines cwln;
      Ncols nchfol;
+     Nlines sav_clineno;
  
      curcol = cursorcol + curwksp->wcol;
      getline (ln = curwksp->wlin + cursorline);
***************
*** 442,447
  	deletdwd = salloc ((int) 1 - putupdelta, YES);
  	move (&cline[curcol], deletdwd, (int) - putupdelta);
  /*      deletdwd [-putupdelta] = '\0';  */
  	/*
  	 * previous statement gave the following compiler error on
  	 * the Sun: "expression causes compiler loop: try simplifying"

--- 445,451 -----
  	deletdwd = salloc ((int) 1 - putupdelta, YES);
  	move (&cline[curcol], deletdwd, (int) - putupdelta);
  /*      deletdwd [-putupdelta] = '\0';  */
+ 
  	/*
  	 * previous statement gave the following compiler error on
  	 * the Sun: "expression causes compiler loop: try simplifying"
***************
*** 446,455
  	 * previous statement gave the following compiler error on
  	 * the Sun: "expression causes compiler loop: try simplifying"
  	 */
! 	{ int i;
! 	    i = -putupdelta;
! 	    deletdwd [i] = '\0';
! 	}
  	move (&cline[cwcol], &cline[curcol], (Uint) nchfol);
  	shortencline ();
  	cwcol = cursorcol + curwksp->wcol;

--- 450,458 -----
  	 * previous statement gave the following compiler error on
  	 * the Sun: "expression causes compiler loop: try simplifying"
  	 */
! 
! 	*(deletdwd - putupdelta) = '\0';
! 
  	move (&cline[cwcol], &cline[curcol], (Uint) nchfol);
  	shortencline ();
  	cwcol = cursorcol + curwksp->wcol;
***************
*** 453,458
  	move (&cline[cwcol], &cline[curcol], (Uint) nchfol);
  	shortencline ();
  	cwcol = cursorcol + curwksp->wcol;
  	/* get the cursor caught up to the beginning of the deletion */
  	if (curcol < cwcol)
  	    movecursor(LT, cwcol - curcol);

--- 456,472 -----
  	move (&cline[cwcol], &cline[curcol], (Uint) nchfol);
  	shortencline ();
  	cwcol = cursorcol + curwksp->wcol;
+ 	sav_clineno = clineno;
+ 
+ 	/*
+ 	 *  At this point, the word has been deleted from
+ 	 *  'cline'.  But, since the following 'movecursor' calls
+ 	 *  may result in a 'putup' (and calls to getline),
+ 	 *  the changed line never gets written out before it
+ 	 *  is reread from disk.  To fix, set fcline here.
+ 	 */
+ 	fcline = YES;
+ 
  	/* get the cursor caught up to the beginning of the deletion */
  	if (curcol < cwcol)
  	    movecursor(LT, cwcol - curcol);
***************
*** 463,468
  	    movecursor(UP, cwln - ln);
  	else if (cwln < ln)
  	    movecursor(DN, ln - cwln);
      } else {           /* ind=NO ==> restore the last deleted word */
  	putupdelta = strlen (deletdwd);
  	if (xcline)

--- 477,488 -----
  	    movecursor(UP, cwln - ln);
  	else if (cwln < ln)
  	    movecursor(DN, ln - cwln);
+ 	/*
+ 	 * Since one of the above movecursor calls may result
+ 	 * in a putup() call, repeat the getline() here.
+ 	 */
+ 	if (clineno != sav_clineno)
+ 	    getline(ln);
      } else {           /* ind=NO ==> restore the last deleted word */
  	fcline = YES;
  	putupdelta = strlen (deletdwd);
***************
*** 464,469
  	else if (cwln < ln)
  	    movecursor(DN, ln - cwln);
      } else {           /* ind=NO ==> restore the last deleted word */
  	putupdelta = strlen (deletdwd);
  	if (xcline)
  	    extend (ln - la_lsize(curlas));

--- 484,490 -----
  	if (clineno != sav_clineno)
  	    getline(ln);
      } else {           /* ind=NO ==> restore the last deleted word */
+ 	fcline = YES;
  	putupdelta = strlen (deletdwd);
  	if (xcline)
  	    extend (ln - la_lsize(curlas));
***************
*** 474,480
      curcol -= curwksp->wcol;
      putup (-1, cursorline, (Slines) curcol, MAXWIDTH);
      restcurs();
!     fcline = YES;
      return CROK;
  }
  #endif LMCDWORD

--- 495,501 -----
      curcol -= curwksp->wcol;
      putup (-1, cursorline, (Slines) curcol, MAXWIDTH);
      restcurs();
! 
      return CROK;
  }
  #endif LMCDWORD
