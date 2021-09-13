#
# The e.man file has two ".so" - make sure the filenames
# point at where you install the "e.*.out" files.
#

MANPATH=/usr/local/share/man/man1

e.1:    e.man
	soelim e.man | sed -f sed.script > e.1

#	soelim e.man > e.tmp
#	ed e.tmp < ed.script
#	rm e.tmp

e.man:  e.stdkey.out  e.term.out

e.stdkey.out: e.stdkey.tbl
	tbl e.stdkey.tbl > e.stdkey.out

e.term.out: e.term.tbl
	tbl e.term.tbl > e.term.out

hardcopy:  e.1
#	ditroff -Tpsc -man  e.1 | psdit > e.1.ps
	man -t ./e.1 > e.1.ps

install:   e.1
	install -c e.1 $(MANPATH)
