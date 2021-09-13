#ifdef COMMENT
	Proprietary Rand Corporation, 1981.
	Further distribution of this software
	subject to the terms of the Rand
	license agreement.
#endif


/****************************************/
/**** televideo 910 *************/

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1

extern Cmdret help_std();
extern Flag in_std_p();

in_tv910 (lexp, count)
char *lexp;
int *count;
{
    int nr, n;
    Uint chr;
    char *icp, *ocp, *ic, *oc;

	icp = ocp = lexp;
	nr = *count;
	for (; nr > 0; nr--) {
		ic = icp; oc = ocp; n = nr;
		switch (chr = *icp++ & 0177) {
		case CTRL('^'):
			*ocp++ = CCDELCH;
			goto doneit;
		case CTRL('['):
			if (nr < 2) break;
			nr --;
			if ((*icp++ & 0177) == 'I') {
				*ocp++ = CCBACKTAB;
				goto doneit;
			}
			break;
		case CTRL('Z'):
			*ocp++ = CCOPEN;
			goto doneit;
		}
		icp = ic; ocp = oc; nr = n;
		if (in_std_p (&icp, &ocp, &nr)) break;
doneit:         continue;
	}
	{
		Reg1 int conv;
		*count = nr;     /* number left over - still raw */
		conv = ocp - lexp;
		while (nr-- > 0)
			*ocp++ = *icp++;
		return conv;
	}
}

S_kbd kb_tv910 = {
/* kb_inlex */  in_tv910,
/* kb_init  */  nop,
/* kb_end   */  nop,
};
