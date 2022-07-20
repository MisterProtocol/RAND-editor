#
# The e.man file has two ".so" - make sure the filenames
# point at where you install the "e.*.out" files.
#

MANPATH=/usr/local/share/man/man1


e.1:    e.man
	soelim e.man | sed -f sed.script > e.1
#       ed e.tmp < ed.script
#       rm e.tmp

e.man:	e.stdkey.out  e.term.out

e.stdkey.out: e.stdkey.tbl
	tbl e.stdkey.tbl > e.stdkey.out

e.term.out: e.term.tbl
	tbl e.term.tbl > e.term.out

hardcopy:  e.1
#	ditroff -Tpsc -man  e.1 | psdit > e.1.ps
	man -t ./e.1 > tmp.ps
# "mandoc" does support ".di". Things still work but pollute the output
# file with mandoc error messages.  Gotta get rid of them.  The resulting
# PS file still works.
	sed -e"/^mandoc/d" < tmp.ps > e.1.ps
	rm tmp.ps

install:
	install -c e.1 $(MANPATH)

pdf:
	ps2pdf e.1.ps > e.1.pdf
