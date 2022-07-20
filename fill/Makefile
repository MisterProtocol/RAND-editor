#
#	Copyright abandoned, 1983, The Rand Corporation
#
CC	= gcc
DFS     = -DUNIXV7
LOCALINCL = ../include/
#LOCALINCL = /Library/Developer/CommandLineTools/SDKs/MacOSX12.sdk/usr/include/
#CCF     = -g -traditional -traditional-cpp #
CCF     = -g
LDF     =  #

#CFLAGS = $(CCF) $(DFS) -Bstatic
#CFLAGS = $(CCF) $(DFS) -I$(LOCALINCL) -Wno-nullability-completeness -static
CFLAGS = $(CCF) $(DFS) -I$(LOCALINCL) -static

all: fill just center run

run: run.o
	$(CC) $(LDFLAGS) run.o
	-mv run run.old
	mv a.out run

fill: fill.o
	$(CC) $(LDFLAGS) fill.o
	-mv fill fill.old
	mv a.out fill

just: fill
	-rm -f just
	ln fill just

center: center.o
	$(CC) $(LDFLAGS) center.o
	-mv center center.old
	mv a.out center

clean:
	rm -f fill.o center.o run.o fill just center \
	      fill.old just.old center.old

