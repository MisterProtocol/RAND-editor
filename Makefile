#--------------------------------------------------------------------------
# 2/11/2020:  if -ltinfo is not installed (provided by the ncurses package)
#  a local copy of termcap can now be linked with the le binary.
#
#  First do this:
#       cd termcap; make
#
#  Then specify the desired library in ./e19/Makefile before running
#  'make linux'
#------------------------------------------------------------------------
#
#       make linux
# old:
#       make sys5.install
#       make sys5
#       make bsd.install
#       make bsd
#
# These macros need to be set to the eventual resting place for the binary
# and man pages before bsd.install or s5.install is invoked.
#
# trw: 11/25/06, on linux, X86_64, a small bug editing marked areas occurs
# where the marked area is 'erased'.  Not sure what/where the problem is
# but it's probably a pointer problem in the la package.
# Workaround: compile with -m32
#
#CC = gcc -m32
#CC = gcc -Wcomment
CC = gcc -std=c99
#ORG = /usr/rand
#ORG = /usr/rand
ORG = /usr/local
BIN = $(ORG)/bin
MAN = $(ORG)/share/man
LIB = $(ORG)/lib/e
ETC = $(LIB)
TMP = /tmp/etmp
DOC = $(MAN)/man1/e.1
LEARN = $(LIB)/learn
NAME = e
OWNER = root
GROUP = wheel

# RAND CHANGES 88/89:
#    Do all makes from this top level makefile.  Usually:
#       make clean; make e19; make install.e19
#    -DWPMODESET will bring up E in WP mode by default.
#    -DSUNOS4 for SunOS4.x
#    -DETC=... completely defines the old /etc/e directory.  The default
#       is /usr/local/lib/e.
#    -DFSYNCKEYS will frequently do an fsync on the keystroke file.
#    -DASCIIKEYINFO .ek? file info at beginning is all ascii
#    -DHOSTTMPNAMES tmp files created with hostname, eg: ".ek1.hostname"
#    -DSAVETMPFILES .e{s,k,c}* are renamed ,.e{s,k,c}*.pid on exit
#    -DBUILTINFILL uses inline fill/just/center code instead of
#       ETC/{fill,center,just}  (THIS DOES NOT WORK YET! 9/89)
#-------------------------------------------------------------------------

# not on RH enterprise
#LINUX=-DFLOCK
#LINUX=-DLINUX -D_BSD_SOURCE -DTERMSIM -DGCC
LINUX=-DLINUX -D_BSD_SOURCE -D_DEFAULT_SOURCE -DTERMSIM -DGCC


# Use following for SysV
#
S5DEFINES = -DUNIXV7 -DASCIIKEYINFO -DSYSIII -DSYSV \
	  -DSAVETMPFILES \
	  -DHOSTTMPNAMES \
	  -DCBREAK -DTERMSIM -DTERMCAP -DFILELOCKING -DSAVETMPFILES $(LINUX)
#         -DCBREAK -DTERMSIM -DTERMCAP $(LINUX)


# Use following for Solaris 2.3
#
#S5DEFINES = -DUNIXV7 -DASCIIKEYINFO \
#	  -DSOLARIS \
#	  -DSAVETMPFILES \
#	  -DHOSTTMPNAMES \
#	  -DBSD_COMP \
#	  -DFSYNCKEYS \
#	  -DCBREAK -DTERMCAP -DFILELOCKING

BSDDEFINES = -DUNIXV7  -DASCIIKEYINFO -DFSYNCKEYS \
	  -DBSD \
	  -DHOSTTMPNAMES -DSAVETMPFILES \
	  -DTERMCAP $(LINUX)

# -DFILELOCKING -DFLOCK -DBCOPY


#trw:11.19.06
S5CFLAGS    = -g -Wall -Wextra -Wno-implicit-fallthrough # -O
#S5CFLAGS   = -g # -O
#S5CFLAGS   = -g -traditional -traditional-cpp # -O
#S5CFLAGS   = -g # -O
#end trw

#mob:3.8.2021
target bsd: E_TCLIB = -lncurses
#target sys5: E_TCLIB = -ltinfo
target sys5: E_TCLIB = -lncurses
BSDCFLAGS  = -g # -O
S5LDFLAGS  = -g
BSDLDFLAGS = -g
S5LIBS     = s5.libtmp s5.fflib s5.lalib
BSDLIBS    = bsd.libtmp bsd.fflib bsd.lalib
LOCALINCL = ../include/
MANPATH = $(MAN)/man1

empty:
	@echo "you must type one of:"
	@echo "    $(MAKE) linux (works on RED HAT 5.2 and probably others)"
	@echo "    $(MAKE) sys5"
	@echo "    $(MAKE) bsd"
	@echo "    $(MAKE) sys5.install"
	@echo "    $(MAKE) bsd.install"
	@echo "    $(MAKE) clean"

linux:  sys5


sys5:   $(S5LIBS)
	cd e19; $(MAKE) E_TCLIB=$(E_TCLIB) \
		LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		CFLAGS="$(S5DEFINES) $(S5CFLAGS)" \
		LDFLAGS="$(S5LDFLAGS)" \
		ETC_D=$(ETC) \
		NEWVERSION=vers.s5.sh \
		le


bsd:    $(BSDLIBS)
	cd e19; $(MAKE) E_TCLIB=$(E_TCLIB) \
		LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		CFLAGS="$(BSDDEFINES) $(BSDCFLAGS)" \
		LDFLAGS="$(BSDLDFLAGS)" \
		ETC_D="$(ETC)" \
		NEWVERSION="vers.bsd.sh" \
		le

s5.lalib:
	cd la1; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		CFL="$(S5DEFINES) $(S5CFLAGS)" \
		LDFL="$(S5LDFLAGS)" \
		RANLIB="@echo archived"

bsd.lalib:
	cd la1; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		DEFS="$(BSDDEFINES) $(BSDCFLAGS)" \
		LDFL="$(BSDLDFLAGS)" \
		RANLIB="ranlib"

s5.fflib:
	cd ff3; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		ETC="$(LIB)" \
		CFLAGS="$(S5DEFINES) $(S5CFLAGS)" \
		LDFLAGS="$(S5LDFLAGS)" \
		RANLIB="@echo archived"

bsd.fflib:
	cd ff3; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		ETC="$(LIB)" \
		CFLAGS="$(BSDDEFINES) $(BSDCFLAGS)" \
		LDFLAGS="$(BSDLDFLAGS)" \
		RANLIB="ranlib"

s5.libtmp:
	cd lib; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		CFLAGS="$(S5DEFINES) -DMEMSET $(S5CFLAGS)" \
		LDFLAGS="$(S5LDFLAGS)" \
		RANLIB="@echo"

bsd.libtmp:
	cd lib; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		CFLAGS="$(BSDDEFINES) -DMEMSET $(BSDCFLAGS)" \
		LDFLAGS="$(BSDLDFLAGS)" \
		RANLIB="ranlib"

# First command in both "install" scripts needs to be a "touch"
# on the old binary, because on initial installation, there is no
# old binary. Attempting to move a non-existent binary will cause
# the installation to fail.
sys5.install:   sys5 s5.help s5.man s5.tmp
	touch $(BIN)/$(NAME)
	mv $(BIN)/$(NAME) $(BIN)/$(NAME).old
	ln e19/le $(NAME)
#       install -u $(OWNER) -g $(GROUP) -f $(BIN) $(NAME)
	install -s -o $(OWNER) -g $(GROUP) -m 755 $(NAME) $(BIN)
	strip $(BIN)/$(NAME)
	chmod 755 $(BIN)/$(NAME)
	rm -f $(NAME)

bsd.install:   bsd bsd.help bsd.man bsd.tmp
	touch $(BIN)/$(NAME)
	mv $(BIN)/$(NAME) /$(BIN)/$(NAME).old
	ln e19/le $(NAME)
	install -s -o $(OWNER) -g $(GROUP) -m 755 $(NAME) $(BIN)
	rm -f $(NAME)


s5.help: etc help
	chown $(OWNER) $(LIB)/*

bsd.help: etc help
	chown $(OWNER) $(LIB)/*

etc:
	-mkdir $(LIB)
	chown $(OWNER) $(LIB)
	chmod 755 $(LIB)
	cp help/Crashdoc $(LIB)
	cp help/errmsg $(LIB)
	cp help/recovermsg $(LIB)
	chmod 444 $(LIB)/*
	cd fill; $(MAKE) "LOCALINCL=$(LOCALINCL)" \
		"ETC=$(LIB)" \
		"DFS=$(LIBDFS)" \
		"CCF=$(LIBCCF)"
	cp fill/fill $(LIB)
	-rm $(LIB)/just
	ln $(LIB)/fill $(LIB)/just
	cp fill/center $(LIB)
	cp fill/run $(LIB)
	chmod 755 $(LIB)/fill $(LIB)/just $(LIB)/center $(LIB)/run

help:   etc
	cp help/helpkey $(LIB)
	cp help/k?.* $(LIB)
	ln $(LIB)/kl.vt100 $(LIB)/kl.vt100w
	ln $(LIB)/kl.vt100 $(LIB)/kl.tab132
	ln $(LIB)/kl.vt100 $(LIB)/kl.tab132w
	ln $(LIB)/kl.vt100 $(LIB)/kl.cit101
	ln $(LIB)/kl.vt100 $(LIB)/kl.cit101w
	ln $(LIB)/kl.free100 $(LIB)/kl.fr100
	ln $(LIB)/kl.wy50 $(LIB)/kl.wy50w
	ln $(LIB)/kr.vt100 $(LIB)/kr.vt100w
	ln $(LIB)/kr.vt100 $(LIB)/kr.tab132
	ln $(LIB)/kr.vt100 $(LIB)/kr.tab132w
	ln $(LIB)/kr.vt100 $(LIB)/kr.cit101
	ln $(LIB)/kr.vt100 $(LIB)/kr.cit101w
	ln $(LIB)/kr.free100 $(LIB)/kr.fr100
	ln $(LIB)/kr.wy50 $(LIB)/kr.wy50w
	ln $(LIB)/kl.tv910 $(LIB)/kr.tv910
	ln $(LIB)/kl.standard $(LIB)/kr.standard
	chmod 444 $(LIB)/k?.* $(LIB)/helpkey

s5.tmp:
	-mkdir $(TMP)
	chown $(OWNER) $(TMP)
	chgrp $(GROUP) $(TMP)
	chmod 777 $(TMP)

bsd.tmp:
	-mkdir $(TMP)
	chown $(OWNER) $(TMP)
	chgrp $(GROUP) $(TMP)
	chmod 777 $(TMP)

bsd.man:
	cd man; $(MAKE) "MANPATH=$(MANPATH)" install

s5.man:
	cd man; $(MAKE) "MANPATH=$(MANPATH)" install

s5.clean: clean

s5.lint:
	cd e19; $(MAKE) "LOCALINCL=$(LOCALINCL)" \
		"CFLAGS=$(S5DEFINES) $(S5CFLAGS)" \
		"LINTFLAGS=$(S5LINTFLAGS)" \
		lint

learn:
	cp doc/man/e.1 $(DOC)
	-mkdir $(LEARN)
	chmod 755 $(LEARN)
	chown $(OWNER) $(LEARN)
	cp learn/* $(LEARN)
	chmod 644 $(LEARN)/*
	chown $(OWNER) $(LEARN)/*
	@echo "*** Remember to edit /usr/lib/learn/*info to advertize"
	@echo "*** the e editor lessons."

clean:
	cd e19; $(MAKE) clean
	cd fill; $(MAKE) clean
	cd la1; $(MAKE) clean
	cd ff3; $(MAKE) clean
	cd lib; $(MAKE) clean
	-rm -f ,* #* a.out .e?1*

pres:
	cd e19; $(MAKE) LOCALINCL="$(LOCALINCL)" \
		CC="$(CC)" \
		CFLAGS="$(S5DEFINES) $(S5CFLAGS)" \
		LDFLAGS="$(S5LDFLAGS)" \
		pres


