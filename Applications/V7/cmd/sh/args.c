/* UNIX V7 source code: see /COPYRIGHT or www.tuhs.org for details. */
/* Changes: Copyright (c) 1999 Robert Nordier. All rights reserved. */

#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include <stdlib.h>
#include	"defs.h"

static STRING *copyargs(STRING from[], int n);
static DOLPTR dolh;

CHAR flagadr[10];

CHAR flagchar[] = {
	'x', 'n', 'v', 't', 's', 'i', 'e', 'r', 'k', 'u', 0
};

int flagval[] = {
	execpr, noexec, readpr, oneflg, stdflg, intflg, errflg, rshflg,
	    keyflg, setflg, 0
};

/* ========	option handling	======== */


int options(int argc, STRING *argv)
{
	register STRING cp;
	register STRING *argp = argv;
	register STRING flagc;
	STRING flagp;

	if (argc > 1 && *argp[1] == '-') {
		cp = argp[1];
		flags &= ~(execpr | readpr);
		while (*++cp) {
			flagc = flagchar;

			while (*flagc && *flagc != *cp) {
				flagc++;
			}
			if (*cp == *flagc) {
				flags |= flagval[flagc - flagchar];
			} else if (*cp == 'c' && argc > 2 && comdiv == 0) {
				comdiv = argp[2];
				argp[1] = argp[0];
				argp++;
				argc--;
			} else {
				failed(argv[1], badopt);
				;
			}
			;
		}
		argp[1] = argp[0];
		argc--;
		;
	}

	/* set up $- */
	flagc = flagchar;
	flagp = flagadr;
	while (*flagc) {
		if (flags & flagval[flagc - flagchar]
		    ) {
			*flagp++ = *flagc;
			;
		}
		flagc++;
		;
	}
	*flagp++ = 0;

	return (argc);
}

void setargs(STRING argi[])
{
	/* count args */
	register STRING *argp = argi;
	register int argn = 0;

	while (Rcheat(*argp++) != ENDARGS) {
		argn++;
	}

	/* free old ones unless on for loop chain */
	freeargs(dolh);
	dolh = (DOLPTR) copyargs(argi, argn);	/* sets dolv */
	assnum(&dolladr, dolc = argn - 1);
}

DOLPTR freeargs(DOLPTR blk)
{
	register STRING *argp;
	register DOLPTR argr = 0;
	register DOLPTR argblk;

	if (argblk = blk) {
		argr = argblk->dolnxt;
		if ((--argblk->doluse) == 0) {
			for (argp = (STRING *) argblk->dolarg;
			     Rcheat(*argp) != ENDARGS; argp++) {
				free(*argp);
			}
			free(argblk);
			;
		};
	}
	return (argr);
}

static STRING *copyargs(STRING from[], int n)
{
	register STRING *np =
	    (STRING *) alloc(sizeof(STRING *) * n + 3 * BYTESPERWORD);
	register STRING *fp = from;
	register STRING *pp = np;

	((DOLPTR) np)->doluse = 1;	/* use count */
	np = (STRING *) ((DOLPTR) np)->dolarg;
	dolv = np;

	while (n--) {
		*np++ = make(*fp++);
	}
	*np++ = ENDARGS;
	return (pp);
}

void clearup(void)
{
	/* force `for' $* lists to go away */
	while (argfor = freeargs(argfor));

	/* clean up io files */
	while (pop());
}

DOLPTR useargs(void)
{
	if (dolh) {
		dolh->doluse++;
		dolh->dolnxt = argfor;
		return (argfor = dolh);
	} else {
		return (0);
		;
	}
}