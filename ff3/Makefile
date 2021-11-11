#
#       Copyright abandoned, 1983, The Rand Corporation
#

O       = o/

RANLIB=ranlib

#CFLAGS  = -g -DUNIXV7 -traditional -traditional-cpp
CFLAGS  = -g -DUNIXV7
LOCALINCL = ../include/
DEFS    = -DMEMSET
LIB     = libff.a
# Move and fill are required by FFIO, but they are generally useful,
# and really belong in libc
MOVE = ../lib/move.o
FILL = ../lib/fill.o

# open.o can be first because it always HAS to be loaded anyway,
# and it might speed up the linker to get it out of the way.
OFILES = \
	$(O)open.o \
	$(O)close.o \
	$(O)fd.o \
	$(O)free.o \
	$(O)getc.o \
	$(O)grow.o \
	$(O)pos.o \
	$(O)putc.o \
	$(O)read.o \
	$(O)seek.o \
	$(O)size.o \
	$(O)sync.o \
	$(O)flush.o \
	$(O)sort.o \
	$(O)write.o \
	$(O)point.o

CFILES = \
	close.c \
	fd.c \
	flush.c \
	free.c \
	getc.c \
	grow.c \
	open.c \
	pos.c \
	putc.c \
	read.c \
	seek.c \
	size.c \
	sort.c \
	sync.c \
	write.c \
	point.c

.c.o:
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c $*.c


$(LIB): $(O) $(OFILES) $(FILL) $(MOVE)
	ar r $(LIB) $(OFILES) $(MOVE) $(FILL)
	$(RANLIB) $(LIB)

$(O):
	mkdir $(O)

ranlib:
	$(RANLIB) $(LIB)

lint:
	lint -h $(DEFS) $(INCL) $(CFILES) -lc

lintlib:
	lint -Cff $(DEFS) $(INCL) $(CFILES) -lc

# Be sure the defs in "Ifdefs" agree with $(DEFS)
Calls:  $(CFILES)
	Makecalls $(CFILES)

clean :
	rm -f $(OFILES) $(LIB)

$(OFILES) : ff.local.h ff.h $(LOCALINCL)/c_env.h # /usr/include/stdio.h

#ff.local.h : ff.h $(LOCALINCL)/c_env.h
#ff.h : /usr/include/stdio.h

$(O)close.o: close.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c close.c
	mv close.o $(O)close.o

$(O)fd.o: fd.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c fd.c
	mv fd.o $(O)fd.o

$(O)flush.o: flush.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c flush.c
	mv flush.o $(O)flush.o

$(O)free.o: free.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c free.c
	mv free.o $(O)free.o

$(O)getc.o: getc.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c getc.c
	mv getc.o $(O)getc.o

$(O)grow.o: grow.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c grow.c
	mv grow.o $(O)grow.o

$(O)open.o: open.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c open.c
	mv open.o $(O)open.o

$(O)pos.o: pos.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c pos.c
	mv pos.o $(O)pos.o

$(O)putc.o: putc.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c putc.c
	mv putc.o $(O)putc.o

$(O)read.o: read.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c read.c
	mv read.o $(O)read.o

$(O)seek.o: seek.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c seek.c
	mv seek.o $(O)seek.o

$(O)size.o: size.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c size.c
	mv size.o $(O)size.o

$(O)write.o: write.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c write.c
	mv write.o $(O)write.o

$(O)point.o: point.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c point.c
	mv point.o $(O)point.o

$(O)sort.o: sort.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c sort.c
	mv sort.o $(O)sort.o

$(O)sync.o: sync.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c sync.c
	mv sync.o $(O)sync.o

$(MOVE): move.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c move.c
	mv move.o $(O)move.o

$(FILL): fill.c
	$(CC) $(CFLAGS) $(DEFS) -I. -I$(LOCALINCL) -c fill.c
	mv fill.o $(O)fill.o
