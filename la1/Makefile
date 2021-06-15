#
#       Copyright 1988, 1987, 1986 Grand Software, Inc. All Rights Reserved
#       Copyright 1986, 1985 David Yost                 All Rights Reserved
#
#       This file is proprietary to and a trade secret of Grand Software,
#       Inc. (Los Angeles, CA, USA, Telephone +1 213-650-1089) and may be
#       used only in accordance with the terms of the license agreement
#       that accompanies the product.  An electronic copy of that license
#       is provided in the file "AGREEMENT" in the main directory of the
#       distribution.
#

#
#       MAKEFILE FOR RAND "LA" LINE-ACCESS I/O PACKAGE
#
#       Original Design by David Yost
#       Coded by David Yost and Rick Kiessig
#
G       = -g
OPT     = # -O
O       = o/
DEFS    = -DUNIXV7
LIB     = libla.a
#
FF      = ../ff3/
FFLIB   = ../ff3/libff.a
LOCALINCL = ../include
INCL    = -I. -I$(FF) -I$(LOCALINCL)

#CFLAGS  = $G $(OPT) $(DEFS) $(INCL) $(CFL) -traditional -traditional-cpp
CFLAGS  = $G $(OPT) $(DEFS) $(INCL) $(CFL)
OVCFLAGS = $(CFLAGS) # -Z

LDFLAGS = $G $(LDFL)

LINT    = lint
LIBC    = -lc

CC      = cc    # gcc
RANLIB  = ranlib
SH      = /bin/sh
MV      = mv -f

# laopen.o can be first because it always HAS to be loaded anyway,
# and it might speed up the linker to get it out of the way.
#    The following line has been deleted because the file isn't used
#    by the editor, but only by "test.c".
#	$Oladeb.o \

OFILES =\
	$Olaopen.o \
	$Olaalign.o \
	$Olablank.o \
	$Olabsize.o \
	$Olaclseek.o \
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

#    The following line has been deleted because the file isn't used
#    by the editor, but only by "test.c".
#	$Oladeb.o \

CFILES =\
	laabort.c \
	laalign.c \
	lablank.c \
	labreak.c \
	labsize.c \
	lacfile.c \
	laclose.c \
	laclseek.c \
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


#ll $(LIB) : $P $(OFILES) $(OVFILES)
$(LIB): $(OFILES) $(OVFILES)
#       -$(SH) -c "if test -r $(LIB) ; then $(MV) $(LIB) ,$(LIB) ; fi"
	ar r $(LIB) $(OFILES)
	$(RANLIB) $(LIB)

#init : Makefile
#
#Makefile : Makef
#	-$(SH) -c "if test -r $@ ; then $(MV) $@ ,$@ ; fi"
#	/bin/sh ../Make.make Makef > $@

ranlib :
	-$(RANLIB) $(LIB)

lint :
	$(LINT) $(DEFS) $(INCL) test.c $(CFILES) \
	$(FF)llib-lff.c $(LA)llib-lla.c -lc

Calls :  $(CFILES)
	Makecalls $(CFILES)

test :   $Otest.o $(LIB)
	$(CC) $(LDFLAGS) $Otest.o $(LIB) $(FFLIB) $(LIBC)
	-$(SH) -c "if test -r test ; then $(MV) test ,test ; fi"
	mv a.out test

$(FFLIB) : $(.PRECIOUS)
	cd $(FF) ; make

clean :
	-rm -f o/*.o *.o $(LIB)
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

