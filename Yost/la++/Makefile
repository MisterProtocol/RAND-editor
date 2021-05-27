#
#       Copyright (c) 1985, 1986 David Yost     All Rights Reserved
#       Copyright (c) 1986, 1987 Grand Software, Inc. All Rights Reserved
#       THIS FILE CONTAINS UNPUBLISHED PROPRIETARY SOURCE CODE
#       which is property of Grand Software, Inc.
#       Los Angeles, CA 90046 U.S.A 213-650-1089
#       The copyright notice above does not evidence any
#       actual or intended publication of such source code.
#       This file is not to be copied by anyone except as
#       covered by written agreement with Grand Software, Inc.,
#       and this notice is not to be removed.
#

#
#       MAKEFILE FOR RAND "LA" LINE-ACCESS I/O PACKAGE
#
#       Original Design by David Yost
#       Coded by David Yost and Rick Kiessig
#
G       = # -g
O       = o/
DEFS    =      
LIB     = libla.a
#
FF      = ../ff3/
FFLIB   = ../ff3/libff.a
LOCALINCL = ../include
INCL    = -I. -I$(FF) -I$(LOCALINCL)

CFLAGS  = $G -O $(DEFS) $(INCL) $(CFLAGSm)
OVCFLAGS = $(CFLAGS) # -Z

LDFLAGS = $G

LIBC    = -lc

CCXX    = ccxx -DNEWARGS
CC      = ../tools/cc
RANLIB  = ../tools/ranlib
SH      = /bin/sh
MV      = mv -f

# laopen.o can be first because it always HAS to be loaded anyway,
# and it might speed up the linker to get it out of the way.
OFILES =\
	$Olaopen.o \
	$Olaalign.o \
	$Olablank.o \
	$Olabsize.o \
	$Olaclseek.o \
	$Oladeb.o \
	$Olaerr.o \
	$Olafrpl.o \
	$Olalcnt.o \
	$Olalcopy.o \
	$Olalflush.o \
	$Olalget.o \
	$Olalpnt.o \
	$Olalrpl.o \
	$Olaldel.o \
	$Olalcol.o \
	$Olalins.o \
	$Olabreak.o \
	$Olaclose.o \
	$Olalink.o \
	$Olalrsize.o \
	$Olalseek.o \
	$Olalwsize.o \
	$Olacfile.o \
	$Olaint.o \
	$Olaother.o \
	$Olaabort.o

CFILES =\
	laabort.c \
	laalign.c \
	lablank.c \
	labreak.c \
	labsize.c \
	lacfile.c \
	laclose.c \
	laclseek.c \
	ladeb.c \
	laerr.c \
	lafrpl.c \
	laint.c \
	lalcnt.c \
	lalcol.c \
	lalcopy.c \
	laldel.c \
	lalflush.c \
	lalget.c \
	lalink.c \
	lalins.c \
	lalpnt.c \
	lalrpl.c \
	lalrsize.c \
	lalseek.c \
	lalwsize.c \
	laopen.c \
	laother.c


all $(LIB) : $P $(OFILES) $(OVFILES)
	-$(SH) -c "if test -r $(LIB) ; then $(MV) $(LIB) ,$(LIB) ; fi"
	ar r $(LIB) $(OFILES)
	$(RANLIB) $(LIB)

init : Makefile

Makefile : Makef
	-$(SH) -c "if test -r $@ ; then $(MV) $@ ,$@ ; fi"
	/bin/sh ../Make.make Makef > $@

ranlib :
	-$(RANLIB) $(LIB)

test :   $Otest.o $(LIB)
	$(CCXX) $(LDFLAGS) $Otest.o $(LIB) $(FFLIB)
	-$(SH) -c "if test -r test ; then $(MV) test ,test ; fi"
	mv o/test test

$(FFLIB) : $(.PRECIOUS)
	cd $(FF) ; make

clean :
	-rm -f o/*.o *.o
	-rm -f $(OFILES)

distribution :
	-rm -f o/*.o *.o
	-rm -f $(OFILES) $(LIB) test

$(OFILES) : lalocal.hxx la.hxx $(LOCALINCL)/c_env.h $(FF)ff.h

$Olaabort.o : laabort.cxx
	$(CCXX) $(CFLAGS) -c laabort.cxx ; mv laabort.o $O

$Olaalign.o : laalign.cxx
	$(CCXX) $(CFLAGS) -c laalign.cxx ; mv laalign.o $O

$Olablank.o : lablank.cxx
	$(CCXX) $(CFLAGS) -c lablank.cxx ; mv lablank.o $O

$Olabreak.o : labreak.cxx
	$(CCXX) $(CFLAGS) -c labreak.cxx ; mv labreak.o $O

$Olabsize.o : labsize.cxx
	$(CCXX) $(CFLAGS) -c labsize.cxx ; mv labsize.o $O

$Olacfile.o : lacfile.cxx
	$(CCXX) $(CFLAGS) -c lacfile.cxx ; mv lacfile.o $O

$Olaclose.o : laclose.cxx
	$(CCXX) $(CFLAGS) -c laclose.cxx ; mv laclose.o $O

$Olaclseek.o : laclseek.cxx
	$(CCXX) $(CFLAGS) -c laclseek.cxx ; mv laclseek.o $O

$Oladeb.o : ladeb.cxx
	$(CCXX) $(CFLAGS) -c ladeb.cxx ; mv ladeb.o $O

$Olaerr.o : laerr.cxx
	$(CCXX) $(CFLAGS) -c laerr.cxx ; mv laerr.o $O

$Olafrpl.o : lafrpl.cxx
	$(CCXX) $(CFLAGS) -c lafrpl.cxx ; mv lafrpl.o $O

$Olaint.o : laint.cxx
	$(CCXX) $(CFLAGS) -c laint.cxx ; mv laint.o $O

$Olalcnt.o : lalcnt.cxx
	$(CCXX) $(CFLAGS) -c lalcnt.cxx ; mv lalcnt.o $O

$Olalcol.o : lalcol.cxx
	$(CCXX) $(CFLAGS) -c lalcol.cxx ; mv lalcol.o $O

$Olalcopy.o : lalcopy.cxx
	$(CCXX) $(CFLAGS) -c lalcopy.cxx ; mv lalcopy.o $O

$Olaldel.o : laldel.cxx
	$(CCXX) $(CFLAGS) -c laldel.cxx ; mv laldel.o $O

$Olalflush.o : lalflush.cxx
	$(CCXX) $(CFLAGS) -c lalflush.cxx ; mv lalflush.o $O

$Oovlalflush.o : lalflush.cxx
	$(CCXX) $(OVCFLAGS) -c lalflush.cxx ; mv lalflush.o $Oovlalflush.o

$Olalget.o : lalget.cxx
	$(CCXX) $(CFLAGS) -c lalget.cxx ; mv lalget.o $O

$Olalink.o : lalink.cxx
	$(CCXX) $(CFLAGS) -c lalink.cxx ; mv lalink.o $O

$Olalins.o : lalins.cxx
	$(CCXX) $(CFLAGS) -c lalins.cxx ; mv lalins.o $O

$Olalpnt.o : lalpnt.cxx
	$(CCXX) $(CFLAGS) -c lalpnt.cxx ; mv lalpnt.o $O

$Olalrpl.o : lalrpl.cxx
	$(CCXX) $(CFLAGS) -c lalrpl.cxx ; mv lalrpl.o $O

$Olalrsize.o : lalrsize.cxx
	$(CCXX) $(CFLAGS) -c lalrsize.cxx ; mv lalrsize.o $O

$Olalseek.o : lalseek.cxx
	$(CCXX) $(CFLAGS) -c lalseek.cxx ; mv lalseek.o $O

$Olalwsize.o : lalwsize.cxx
	$(CCXX) $(CFLAGS) -c lalwsize.cxx ; mv lalwsize.o $O

$Olaopen.o : laopen.cxx
	$(CCXX) $(CFLAGS) -c laopen.cxx ; mv laopen.o $O

$Oovlaopen.o : laopen.cxx
	$(CCXX) $(OVCFLAGS) -c laopen.cxx ; mv laopen.o $Oovlaopen.o

$Olaother.o : laother.cxx
	$(CCXX) $(CFLAGS) -c laother.cxx ; mv laother.o $O

$Otest.o : test.cxx
	$(CCXX) $(CFLAGS) -c test.cxx ; mv test.o $O

