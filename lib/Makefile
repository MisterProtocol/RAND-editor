#
#	Copyright abandoned, 1983, The Rand Corporation
#

CC = gcc -fpcc-struct-return
LIB     = libtmp.a
MALLOC  =
#CFLAGS  = -DUNIXV7 -DBCOPY
#CFLAGS  = -DUNIXV7 -traditional -traditional-cpp
CFLAGS  = -g -DUNIXV7
LOCALINCL = ../include/
RANLIB = ranlib

OFILES  =\
	getlong.o \
	getshort.o \
	intss.o \
	putlong.o \
	putshort.o \
	delay.o \
	empty.o \
	move.o \
	fill.o \
	$(MALLOC) \
	mygethostname.o \
	fatal.o

.c.o:
	$(CC) $(CFLAGS) -I$(LOCALINCL) -c $*.c


$(LIB): $(OFILES)
	-mv $(LIB) ,$(LIB)
	ar r $(LIB) $(OFILES)
	$(RANLIB) $(LIB)

# Malloc is here for testing purposes only.  If it is compiled with 'debug'
# defined, it runs slower but does rigorous checking to insure that the
# heap has not been corrupted.
# It is also modified to call the E 'fatal' routine if an ASSERT fails.
# Has 'veralloc()' to verify the arena
malloc.o: malloc.c
	$(CC) -c -Ddebug -O -g malloc.c
#       $(CC) -c -Ddebug -Dlongdebug -O -g malloc.c

clean:
	rm -f $(OFILES) malloc.o $(LIB)
