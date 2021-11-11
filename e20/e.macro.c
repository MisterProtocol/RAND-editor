/*
 * trw - just testing....
 */

#ifdef COMMENT
--------
file e.macro.c
    simple macros
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#ifdef MACROS
#include "e.h"
#include "e.cm.h"
#include "e.m.h"


#define NMACROS 50
struct macros {
    char *name;
    char *text;
};
static struct macros macros[NMACROS];
static int n_macros;

struct macros *findname();
char *index();

Cmdret definemacro (char *);



#ifdef COMMENT
Cmdret
definemacro(args)
.
    Format:     name=<text of macro>

    No features yet, just simple text replacement.
#endif /* COMMENT */

Cmdret
definemacro(args)
    char *args;
{
	Reg1 char *s;
	Reg2 char *name;
	Reg3 struct macros *mp;
	Reg4 int i;
char buf[100];

	name = args;
	if (!(s = index(args, '='))) {
	    mesg (ERRALL+1, "No \"=\" in macro definition");
	    return CROK;
	}
	*s++ = '\0';

	if (n_macros == 0) {        /* none defined */
	    mp = &macros[n_macros++];
	    mp->name = salloc(strlen(name)+1, 0);
	    if (!mp->name)          /* salloc failed */
		return CROK;
	    strcpy(mp->name, name);
	}
	else {
	    if ((mp = findname(name)) == (struct macros *)NULL) {
		if (n_macros >= NMACROS) {
		    mesg (ERRALL+1, "Too many macros");
		    return CROK;
		}
		mp = &macros[n_macros++];
		mp->name = salloc(strlen(name)+1, 0);
		if (!mp->name)
		    return CROK;
		strcpy(mp->name, name);
	    }
	    else
		free(mp->text);         /* redefining existing name */
	}

	s = index(cmdopstr, '=') + 1;

	if (!*s) {      /* delete definition */
	    free(mp->name);
	    free(mp->text);
	    mp->name = (char *)NULL;
	    mp->text = (char *)NULL;
	    return CROK;
	}

	mp->text = salloc (strlen(s)+1, 0);
	if (!mp->text)      /* salloc failed */
	    return CROK;
	strcpy(mp->text, s);

sprintf(buf, "definemacro: name=%s,body=%s\n",
  mp->name, mp->text);
/*dbgpr(buf);*/
	return CROK;
}


struct macros *
findname(name)
Reg1 char *name;
{
	Reg2 int i;

	for (i = 0; i < n_macros; i++)
	    if (!strcmp(name, macros[i].name))
		return(&macros[i]);

	return((struct macros *)NULL);
}
#endif /* MACROS */
