#
#	Copyright abandoned, 1983, The Rand Corporation
#

O       = o/
A       =
STD     = /usr/include/
E       = xe
FF      = ../../ff1/
FFLIB   = ../../ff1/libff.a
DEFS    = -DUNIXV7 -DVAX -DUNSCHAR -DRAND
MISCLIB = ../../lib/libtmp.a
INCL    = -I. -I$(FF)
CFLAGS  = $(A) -O $(DEFS) $(INCL)

lint:
	lint $(DEFS) -DLINT $(INCL) ../e.tt.c \
	$(FF)/llib-lff \
	-lc
