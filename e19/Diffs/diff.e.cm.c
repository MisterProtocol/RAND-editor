*** e.cm.c.old	Fri May 17 11:17:08 1985
--- e.cm.c	Thu Jun 13 11:47:01 1985
***************
*** 764,769
  #define SET_RMSTICK     18
  #define SET_RMNOSTICK   19
  #endif  LMCSRMFIX
  
  #ifdef COMMENT
  Cmdret

--- 764,771 -----
  #define SET_RMSTICK     18
  #define SET_RMNOSTICK   19
  #endif  LMCSRMFIX
+ #define SET_HY          20
+ #define SET_NOHY        21
  
  
  #ifdef COMMENT
***************
*** 765,770
  #define SET_RMNOSTICK   19
  #endif  LMCSRMFIX
  
  #ifdef COMMENT
  Cmdret
  setoption( showflag )

--- 767,773 -----
  #define SET_HY          20
  #define SET_NOHY        21
  
+ 
  #ifdef COMMENT
  Cmdret
  setoption( showflag )
***************
*** 780,785
  	Reg2 Small ind;
  	Reg3 Small value;
  	Cmdret retval;
  	static S_looktbl setopttable[] = {
  	    "+line",        SET_PLLINE,    /* defplline */
  	    "+page",        SET_PLPAGE,    /* defplpage */

--- 783,789 -----
  	Reg2 Small ind;
  	Reg3 Small value;
  	Cmdret retval;
+ 	extern Flag fill_hyphenate;
  	static S_looktbl setopttable[] = {
  	    "+line",        SET_PLLINE,    /* defplline */
  	    "+page",        SET_PLPAGE,    /* defplpage */
***************
*** 788,793
  	    "?",            SET_SHOW,      /* show options */
  	    "bell",         SET_BELL,      /* echo \07 */
  	    "debug",        SET_DEBUG,
  	    "left",         SET_WINLEFT,   /* deflwin */
  	    "line",         SET_LINE,      /* defplline and defmiline */
  #ifdef LMCAUTO

--- 792,798 -----
  	    "?",            SET_SHOW,      /* show options */
  	    "bell",         SET_BELL,      /* echo \07 */
  	    "debug",        SET_DEBUG,
+ 	    "hy",           SET_HY,        /* fill: split hyphenated words */
  	    "left",         SET_WINLEFT,   /* deflwin */
  	    "line",         SET_LINE,      /* defplline and defmiline */
  #ifdef LMCAUTO
***************
*** 794,799
  	    "lmargin",      SET_LMARG,     /* left margin */
  #endif LMCAUTO
  	    "nobell",       SET_NOBELL,    /* do not echo \07 */
  #ifdef LMCSRMFIX
  	    "nostick",      SET_RMNOSTICK, /* auto scroll past rt edge */
  #endif LMCSRMFIX

--- 799,805 -----
  	    "lmargin",      SET_LMARG,     /* left margin */
  #endif LMCAUTO
  	    "nobell",       SET_NOBELL,    /* do not echo \07 */
+ 	    "nohy",         SET_NOHY,      /* fill: don't split hy-words */
  #ifdef LMCSRMFIX
  	    "nostick",      SET_RMNOSTICK, /* auto scroll past rt edge */
  #endif LMCSRMFIX
***************
*** 840,846
  	    case SET_SHOW:
  		{       char buf[80];
  #ifndef LMCVBELL
! sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
      linewidth, NoBell ? "off" : "on" );
  #else LMCVBELL

--- 846,853 -----
  	    case SET_SHOW:
  		{       char buf[80];
  #ifndef LMCVBELL
! sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, \
! bell %s, hy %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
      linewidth, NoBell ? "off" : "on", fill_hyphenate ? "on" : "off" );
  #else LMCVBELL
***************
*** 842,848
  #ifndef LMCVBELL
  sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
!     linewidth, NoBell ? "off" : "on" );
  #else LMCVBELL
  sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s, vb %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,

--- 849,855 -----
  sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, \
  bell %s, hy %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
!     linewidth, NoBell ? "off" : "on", fill_hyphenate ? "on" : "off" );
  #else LMCVBELL
  sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, \
  bell %s, vb %s, hy %s",
***************
*** 844,850
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
      linewidth, NoBell ? "off" : "on" );
  #else LMCVBELL
! sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s, vb %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
      linewidth, NoBell ? "off" : "on", VBell ? "on" : "off" );
  #endif

--- 851,858 -----
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
      linewidth, NoBell ? "off" : "on", fill_hyphenate ? "on" : "off" );
  #else LMCVBELL
! sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, \
! bell %s, vb %s, hy %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
      linewidth, NoBell ? "off" : "on", VBell ? "on" : "off",
      fill_hyphenate ? "on" : "off" );
***************
*** 846,852
  #else LMCVBELL
  sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s, vb %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
!     linewidth, NoBell ? "off" : "on", VBell ? "on" : "off" );
  #endif
  		    mesg (TELALL + 1, buf);
  		}

--- 854,861 -----
  sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, \
  bell %s, vb %s, hy %s",
      defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
!     linewidth, NoBell ? "off" : "on", VBell ? "on" : "off",
!     fill_hyphenate ? "on" : "off" );
  #endif
  		    mesg (TELALL + 1, buf);
  		}
***************
*** 939,944
  	    case SET_WIDTH:
  		if ((value = abs( atoi( arg ))) == 0)
  		    goto BadVal;
  		setmarg (&linewidth, value);
  		retval = CROK;
  		break;

--- 948,958 -----
  	    case SET_WIDTH:
  		if ((value = abs( atoi( arg ))) == 0)
  		    goto BadVal;
+ #ifdef LMCAUTO
+ 		if (value <= autolmarg)
+ 		    mesg (ERRALL+1, "rmar must be greater than lmar.");
+ 		else
+ #endif LMCAUTO
  		setmarg (&linewidth, value);
  		retval = CROK;
  		break;
***************
*** 946,952
  	    case SET_LMARG:
  		if ((value = abs( atoi( arg ))) < 0)
  		    goto BadVal;
! 		setmarg (&autolmarg, value);
  		retval = CROK;
  		break;
  #endif LMCAUTO

--- 960,969 -----
  	    case SET_LMARG:
  		if ((value = abs( atoi( arg ))) < 0)
  		    goto BadVal;
! 		if (value >= linewidth)
! 		    mesg (ERRALL+1, "lmar must be less than rmar.");
! 		else
! 		    setmarg (&autolmarg, value);
  		retval = CROK;
  		break;
  #endif LMCAUTO
***************
*** 997,1002
  		break;
  #endif LMCSRMFIX
  
  
  	    default:
  BadVal:         retval = CRBADARG;

--- 1014,1026 -----
  		break;
  #endif LMCSRMFIX
  
+ 	    case SET_HY:
+ 		fill_hyphenate = YES;
+ 		break;
+ 
+ 	    case SET_NOHY:
+ 		fill_hyphenate = NO;
+ 		break;
  
  	    default:
  BadVal:         retval = CRBADARG;
