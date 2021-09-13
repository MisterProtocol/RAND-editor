*** e.re.c.old	Tue Jun 11 12:38:26 1985
--- e.re.c	Tue Jun 11 12:40:29 1985
***************
*** 122,127
  	    }
  	    if (ncline == 1) {      /* blank line */
  		retval = FOUND_SRCH;
  		goto pret;
  	    }
  	}

--- 122,128 -----
  	    }
  	    if (ncline == 1) {      /* blank line */
  		retval = FOUND_SRCH;
+ 		at = cline;
  		goto pret;
  	    }
  	}
