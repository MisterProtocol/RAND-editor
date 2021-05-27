*** e.ru.c.old	Thu Jun 13 23:14:54 1985
--- e.ru.c	Thu Jun 13 11:48:52 1985
***************
*** 19,24
  
  #define CANTEXEC (-2)
  
  S_looktbl filltable[] = {
      "width"   , 0       ,
      0         , 0

--- 19,26 -----
  
  #define CANTEXEC (-2)
  
+ Flag    fill_hyphenate = NO;    /* default: don't split hyphenated words */
+ 
  S_looktbl filltable[] = {
      "width"   , 0       ,
      0         , 0
***************
*** 222,228
  Flag    puflg;
  Flag    closeflg;
  {
!     char *args[4];
      register Small ix;
  
      ix = Z;

--- 224,230 -----
  Flag    puflg;
  Flag    closeflg;
  {
!     char *args[5];              /* watch this size! */
      register Small ix;
  
      ix = Z;
***************
*** 234,239
  	    char buf[10];
  	    sprintf (buf, "-l%d", linewidth);
  	    args[++ix] = buf;
  	    break;
  	}
      case PRINTNAMEINDEX:

--- 236,243 -----
  	    char buf[10];
  	    sprintf (buf, "-l%d", linewidth);
  	    args[++ix] = buf;
+ 	    if (whichfilter != CENTERNAMEINDEX && fill_hyphenate == YES)
+ 		args[++ix] = "-h";
  	    break;
  	}
      case PRINTNAMEINDEX:
