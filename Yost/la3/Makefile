#
#       Copyright (c) 1985, 1986 David Yost     All Rights Reserved
#       Copyright (c) 1986 Grand Software, Inc. All Rights Reserved
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

LINT    = lint
LIBC    = -lc

CC      = ../tools/cc
RANLIB  = ../tools/ranlib
MV      = mv -f

# laopen.o can be first because it always HAS to be loaded anyway,
# and it might speed up the linker to get it out of the way.
OFILES =\
	$Olaopen.o \
	$Olaabort.o \
	$Olaalign.o \
	$Olacfile.o \
	$Olaerr.o \
	$Olaint.o \
	$Olaclose.o

XO = \
	$Olablank.o \
	$Olabsize.o \
	$Olaclseek.o \
	$Oladeb.o \
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
	$Olalink.o \
	$Olalrsize.o \
	$Olalseek.o \
	$Olalwsize.o \
	$Olaother.o \

CFILES =\
	laclose.c \
	laabort.c \
	laalign.c \
	lacfile.c \
	laerr.c \
	laint.c \
	laopen.c

XC =\
	lablank.c \
	labreak.c \
	labsize.c \
	laclseek.c \
	ladeb.c \
	lafrpl.c \
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
	laother.c


all $(LIB) : $P $(OFILES) $(OVFILES)
	-mv $(LIB) ,$(LIB)
	ar r $(LIB) $(OFILES)
	$(RANLIB) $(LIB)

init : Makefile

Makefile : Makef
	-$(MV) $@ ,$@
	/bin/sh ../Make.make Makef > $@

ranlib :
	-$(RANLIB) $(LIB)

lint :
	$(LINT) $(DEFS) $(INCL) test.c $(CFILES) \
	$(FF)llib-lff $(LA)llib-lla -lc

Calls :  $(CFILES)
	Makecalls $(CFILES)

test :   $Otest.o $(LIB)
	$(CC) $(LDFLAGS) $Otest.o $(LIB) $(FFLIB) $(LIBC)
	-$(MV) test ,test
	$(MV) a.out test

$(FFLIB) : $(.PRECIOUS)
	cd $(FF) ; make

clean :
	-rm -f o/*.o *.o
	-rm -f $(OFILES)

distribution :
	-rm -f o/*.o *.o
	-rm -f $(OFILES) $(LIB) test

$(OFILES) : lalocal.h la.h $(LOCALINCL)/c_env.h $(FF)ff.h

$Olaabort.o : laabort.c
	$(CC) $(CFLAGS) -c laabort.c ; mv laabort.o $O

$Olaalign.o : laalign.c
	$(CC) $(CFLAGS) -c laalign.c ; mv laalign.o $O

$Olablank.o : lablank.c
	$(CC) $(CFLAGS) -c lablank.c ; mv lablank.o $O

$Olabreak.o : labreak.c
	$(CC) $(CFLAGS) -c labreak.c ; mv labreak.o $O

$Olabsize.o : labsize.c
	$(CC) $(CFLAGS) -c labsize.c ; mv labsize.o $O

$Olacfile.o : lacfile.c
	$(CC) $(CFLAGS) -c lacfile.c ; mv lacfile.o $O

$Olaclose.o : laclose.c
	$(CC) $(CFLAGS) -c laclose.c ; mv laclose.o $O

$Olaclseek.o : laclseek.c
	$(CC) $(CFLAGS) -c laclseek.c ; mv laclseek.o $O

$Oladeb.o : ladeb.c
	$(CC) $(CFLAGS) -c ladeb.c ; mv ladeb.o $O

$Olaerr.o : laerr.c
	$(CC) $(CFLAGS) -c laerr.c ; mv laerr.o $O

$Olafrpl.o : lafrpl.c
	$(CC) $(CFLAGS) -c lafrpl.c ; mv lafrpl.o $O

$Olaint.o : laint.c
	$(CC) $(CFLAGS) -c laint.c ; mv laint.o $O

$Olalcnt.o : lalcnt.c
	$(CC) $(CFLAGS) -c lalcnt.c ; mv lalcnt.o $O

$Olalcol.o : lalcol.c
	$(CC) $(CFLAGS) -c lalcol.c ; mv lalcol.o $O

$Olalcopy.o : lalcopy.c
	$(CC) $(CFLAGS) -c lalcopy.c ; mv lalcopy.o $O

$Olaldel.o : laldel.c
	$(CC) $(CFLAGS) -c laldel.c ; mv laldel.o $O

$Olalflush.o : lalflush.c
	$(CC) $(CFLAGS) -c lalflush.c ; mv lalflush.o $O

$Oovlalflush.o : lalflush.c
	$(CC) $(OVCFLAGS) -c lalflush.c ; mv lalflush.o $Oovlalflush.o

$Olalget.o : lalget.c
	$(CC) $(CFLAGS) -c lalget.c ; mv lalget.o $O

$Olalink.o : lalink.c
	$(CC) $(CFLAGS) -c lalink.c ; mv lalink.o $O

$Olalins.o : lalins.c
	$(CC) $(CFLAGS) -c lalins.c ; mv lalins.o $O

$Olalpnt.o : lalpnt.c
	$(CC) $(CFLAGS) -c lalpnt.c ; mv lalpnt.o $O

$Olalrpl.o : lalrpl.c
	$(CC) $(CFLAGS) -c lalrpl.c ; mv lalrpl.o $O

$Olalrsize.o : lalrsize.c
	$(CC) $(CFLAGS) -c lalrsize.c ; mv lalrsize.o $O

$Olalseek.o : lalseek.c
	$(CC) $(CFLAGS) -c lalseek.c ; mv lalseek.o $O

$Olalwsize.o : lalwsize.c
	$(CC) $(CFLAGS) -c lalwsize.c ; mv lalwsize.o $O

$Olaopen.o : laopen.c
	$(CC) $(CFLAGS) -c laopen.c ; mv laopen.o $O

$Oovlaopen.o : laopen.c
	$(CC) $(OVCFLAGS) -c laopen.c ; mv laopen.o $Oovlaopen.o

$Olaother.o : laother.c
	$(CC) $(CFLAGS) -c laother.c ; mv laother.o $O

$Otest.o : test.c
	$(CC) $(CFLAGS) -c test.c ; mv test.o $O

