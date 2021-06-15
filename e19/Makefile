#
#	Copyright abandoned, 1983, The Rand Corporation
#

# Rand Editor with LA package
# The following groups go together
#   CFLAGS=-g and LIBC=-lg for debugging on the VAX
#   DEFS=-DTERMCAP and TERMCAP=-ltermcap
#   DEFS=-DPROFILE and PROFILE=-p
#
LOCALINCL = ../include/
E       = le
O       = o/
OLA     = ../la1/o/
LA      = ../la1/
LALIB   = ../la1/libla.a
FF      = ../ff3/
FFLIB   = ../ff3/libff.a
LIBDIR  = ../lib/
MISCLIB = ../lib/libtmp.a
#MIS2LIB = /usr/src/linux/lib/lib.a
#Sun:    DEFS    = -DTERMSIM $(DFS) -DTERMCAP -DSUN -DBLAND # -DDEBUGALLOC -DPROFILE
#SysIII and V: DEFS    = -DTERMSIM $(DFS) -DTERMCAP -DSYSIII # -DDEBUGALLOC -DPROFILE
#(or make sure SYSIII is defined in ../../include/localenv.h)

INCL    = -I. -I$(LA) -I$(FF) -I"$(LOCALINCL)"
SIG_INCL =signal.h

# The following definition option is for the Mac, which has eliminated
# /usr/include on all versions of MacOS after Mojave.  The first version
# put it in its standard place.  Under Mojave this means first installing
# the developer package to put everything in its proper place.  The second
# version is for Catalina (MacOS X 10.15) in particular.  Big Sur on
# forward will require changing to whatever whims Apple is fronting
# that year.  Or, you can change Mojave to use the SDK for 10.14, but
# I haven't tried that.    -- Mike O'Brien, 3/7/21, mid-COVID
STD     = /usr/include/
#STD     = /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include/

# --begin 2/21/2020 comments, amended MOB 3/8/2021
# if libtinfo is available (provided by ncurses package)
# use next two
#TINFO = -DLIBTINFO
TERMCAP = $(E_TCLIB)

# Otherwise, use some form of the old termcap library,
# overriding definition in top-level Makefile
# use nxt if libtermcap installed on system
#TERMCAP = -ltermcap
# use nxt if linking a local copy of libtermcap
#TERMCAP = ../termcap/libtermcap.a
#
# If switching back and forth to load le with the other library (ie, testing)
# remove o/e.tt.o
# ---end 2/21/2020 comments

CC      = cc
RM      = rm -f

HFILES  =  e.cm.h e.e.h e.fn.h e.func.h \
	e.h e.inf.h e.m.h e.ru.h e.se.h \
	e.sg.h e.t.h e.tt.h e.wi.h

OFILES  = \
	$(O)e.o $(O)e.cm.o $(O)e.d.o $(O)e.dif.o $(O)e.e.o \
	$(O)e.f.o $(O)e.fi.o $(O)e.h.o $(O)e.iit.o \
	$(O)e.it.o $(O)e.la.o $(O)e.m.o $(O)e.main.o \
	$(O)e.mk.o $(O)e.nm.o $(O)e.p.o $(O)e.pa.o \
	$(O)e.profile.o $(O)e.put.o $(O)e.q.o $(O)e.r.o \
	$(O)e.ra.o $(O)e.re.o $(O)e.record.o $(O)e.ru.o $(O)e.sb.o \
	$(O)e.se.o $(O)e.sv.o $(O)e.t.o $(O)e.tb.o $(O)e.tt.o \
	$(O)e.u.o $(O)e.wi.o $(O)e.wk.o $(O)e.x.o \
	$(O)e.mouse.o \
	$(CHKMALLOC)

CFILES  = \
	e.c \
	e.cm.c \
	e.d.c \
	e.dif.c \
	e.e.c \
	e.f.c \
	e.fi.c \
	e.h.c \
	e.iit.c \
	e.it.c \
	e.la.c \
	e.m.c \
	e.mouse.c \
	e.main.c \
	e.mk.c \
	e.nm.c \
	e.p.c \
	e.pa.c \
	e.profile.c \
	e.put.c \
	e.q.c \
	e.r.c \
	e.ra.c \
	e.re.c \
	e.record.c \
	e.ru.c \
	e.sb.c \
	e.se.c \
	e.sv.c \
	e.t.c \
	e.tb.c \
	e.tt.c \
	e.u.c \
	e.wi.c \
	e.wk.c \
	e.x.c

all:
#       $(RM) le; cd ..; make e19
#       cd ..; make bsd
#       cd ..; make sys5
	cd ..; make linux


$(E):   newrev $(OFILES) $(LALIB) $(FFLIB) $(MISCLIB)
#       $(LD) $(LDFLAGS) /lib/crt0.o
	$(CC) $(CFLAGS) $(LDFLAGS) \
	   $(O)e.main.o $(O)e.o $(O)e.pa.o $(O)e.iit.o $(O)e.q.o \
	   $(O)e.tb.o $(O)e.put.o $(O)e.nm.o $(O)e.sv.o \
	   $(O)e.wi.o $(O)e.wk.o $(OLA)lalflush.o $(O)e.u.o \
	   $(O)e.f.o $(O)e.fi.o $(OLA)laopen.o $(O)e.dif.o \
	   $(O)e.h.o $(O)e.se.o $(O)e.e.o $(O)e.mk.o \
	   $(O)e.ra.o $(O)e.re.o $(O)e.record.o $(O)e.t.o \
	   $(O)e.ru.o $(O)e.cm.o $(O)e.d.o $(O)e.it.o \
	   $(O)e.la.o $(O)e.m.o $(O)e.p.o $(O)e.r.o $(O)e.sb.o \
	   $(O)e.tt.o $(O)e.x.o $(O)e.profile.o $(O)e.mouse.o \
	$(LALIB) \
	$(FFLIB) \
	$(MISCLIB) \
	$(TERMCAP) \
	$(CHKMALLOC) \
	$(LIBC)
	-mv $(E) $(E).old
	mv a.out $(E)

#       $(MIS2LIB)


ofiles: $(OFILES)

newrev:
	/bin/sh $(NEWVERSION)
	$(CC) $(CFLAGS) $(INCL) -c e.r.c
	mv e.r.o $(O)e.r.o

$(LALIB):

$(FFLIB):

$(MISCLIB):

# Be sure the defs in "Ifdefs" agree with $(DEFS)
depend:
	/bin/sh Makedep $(HFILES) $(CFILES)

# Be sure the defs in "Ifdefs" agree with $(DEFS)
Comments: $(CFILES)
	Makedoc $(CFILES)

# Be sure the defs in "Ifdefs" agree with $(DEFS)
Calls:  $(CFILES)
	Makecalls $(CFILES)

lint:
	lint $(DEFS) $(INCL) $(CFILES) \
	$(FF)/llib-lff.ln \
	$(LA)/llib-lla.ln \
	-lc

pres: $(O)pres.o $(MISCLIB)
	$(CC) $(CLFAGS) $(INCL) \
	$(O)pres.o \
	$(MISCLIB)
	-mv pres pres.old
	mv a.out pres

clean:
	$(RM) $(OFILES) $(O)pres.o
	$(RM) $(E) $(E).old pres pres.old

$(O)e.cm.o: e.cm.c
	$(CC) $(CFLAGS) $(INCL) -DETC=\"$(ETC_D)\" -c e.cm.c
	mv e.cm.o $(O)e.cm.o

$(O)e.d.o: e.d.c
	$(CC) $(CFLAGS) $(INCL) -c e.d.c
	mv e.d.o $(O)e.d.o

$(O)e.dif.o: e.dif.c
	$(CC) $(CFLAGS) $(INCL) -c e.dif.c
	mv e.dif.o $(O)e.dif.o

$(O)e.e.o: e.e.c
	$(CC) $(CFLAGS) $(INCL) -c e.e.c
	mv e.e.o $(O)e.e.o

$(O)e.f.o: e.f.c
	$(CC) $(CFLAGS) $(INCL) -c e.f.c
	mv e.f.o $(O)e.f.o

$(O)e.fi.o: e.fi.c
	$(CC) $(CFLAGS) $(INCL) -c e.fi.c
	mv e.fi.o $(O)e.fi.o

$(O)e.h.o: e.h.c
	$(CC) $(CFLAGS) $(INCL) -c e.h.c
	mv e.h.o $(O)e.h.o

$(O)e.iit.o: e.iit.c
	$(CC) $(CFLAGS) $(INCL) -c e.iit.c
	mv e.iit.o $(O)e.iit.o

$(O)e.it.o: e.it.c
	$(CC) $(CFLAGS) $(INCL) -c e.it.c
	mv e.it.o $(O)e.it.o

$(O)e.la.o: e.la.c
	$(CC) $(CFLAGS) $(INCL) -c e.la.c
	mv e.la.o $(O)e.la.o

$(O)e.m.o: e.m.c
	$(CC) $(CFLAGS) $(INCL) -c e.m.c
	mv e.m.o $(O)e.m.o

$(O)e.mouse.o: e.mouse.c
	$(CC) $(CFLAGS) $(INCL) -c e.mouse.c
	mv e.mouse.o $(O)e.mouse.o

$(O)e.main.o: e.main.c
	$(CC) $(CFLAGS) $(INCL) -c e.main.c
	mv e.main.o $(O)e.main.o

$(O)e.mk.o: e.mk.c
	$(CC) $(CFLAGS) $(INCL) -c e.mk.c
	mv e.mk.o $(O)e.mk.o

$(O)e.nm.o: e.nm.c
	$(CC) $(CFLAGS) $(INCL) -c e.nm.c
	mv e.nm.o $(O)e.nm.o

$(O)e.o: e.c
	$(CC) $(CFLAGS) $(INCL) -DETC=\"$(ETC_D)\" -c e.c
	mv e.o   $(O)e.o

$(O)e.p.o: e.p.c
	$(CC) $(CFLAGS) $(INCL) -c e.p.c
	mv e.p.o $(O)e.p.o

$(O)e.pa.o: e.pa.c
	$(CC) $(CFLAGS) $(INCL) -c e.pa.c
	mv e.pa.o $(O)e.pa.o

$(O)e.profile.o: e.profile.c
	$(CC) $(CFLAGS) $(INCL) -c e.profile.c
	mv e.profile.o $(O)e.profile.o

$(O)e.put.o: e.put.c
	$(CC) $(CFLAGS) $(INCL) -c e.put.c
	mv e.put.o $(O)e.put.o

$(O)e.q.o: e.q.c
	$(CC) $(CFLAGS) $(INCL) -c e.q.c
	mv e.q.o $(O)e.q.o

$(O)e.r.o: e.r.c
	$(CC) $(CFLAGS) $(INCL) -c e.r.c
	mv e.r.o $(O)e.r.o

$(O)e.ra.o: e.ra.c
	$(CC) $(CFLAGS) $(INCL) -c e.ra.c
	mv e.ra.o $(O)e.ra.o

$(O)e.re.o: e.re.c
	$(CC) $(CFLAGS) $(INCL) -c e.re.c
	mv e.re.o $(O)e.re.o

$(O)e.record.o: e.record.c
	$(CC) $(CFLAGS) $(INCL) -c e.record.c
	mv e.record.o $(O)e.record.o

$(O)e.ru.o: e.ru.c
	$(CC) $(CFLAGS) $(INCL) -c e.ru.c
	mv e.ru.o $(O)e.ru.o

$(O)e.sb.o: e.sb.c
	$(CC) $(CFLAGS) $(INCL) -c e.sb.c
	mv e.sb.o $(O)e.sb.o

$(O)e.se.o: e.se.c
	$(CC) $(CFLAGS) $(INCL) -c e.se.c
	mv e.se.o $(O)e.se.o

#$(O)e.showkeys.o: e.showkeys.c
#        $(CC) $(CFLAGS) $(INCL) -c e.showkeys.c
#        mv e.showkeys.o $(O)e.showkeys.o

$(O)e.sv.o: e.sv.c
	$(CC) $(CFLAGS) $(INCL) -c e.sv.c
	mv e.sv.o $(O)e.sv.o

$(O)e.t.o: e.t.c
	$(CC) $(CFLAGS) $(INCL) -c e.t.c
	mv e.t.o $(O)e.t.o

$(O)e.tt.o: e.tt.c term
	$(CC) $(CFLAGS) $(INCL) $(TINFO) -c e.tt.c
	mv e.tt.o $(O)e.tt.o

$(O)e.tb.o: e.tb.c
	$(CC) $(CFLAGS) $(INCL) -DETC=\"$(ETC_D)\" -c e.tb.c
	mv e.tb.o $(O)e.tb.o

$(O)e.u.o: e.u.c
	$(CC) $(CFLAGS) $(INCL) -c e.u.c
	mv e.u.o $(O)e.u.o

$(O)e.wi.o: e.wi.c
	$(CC) $(CFLAGS) $(INCL) -c e.wi.c
	mv e.wi.o $(O)e.wi.o

$(O)e.wk.o: e.wk.c
	$(CC) $(CFLAGS) $(INCL) -c e.wk.c
	mv e.wk.o $(O)e.wk.o

$(O)e.x.o: e.x.c
	$(CC) $(CFLAGS) $(INCL) -c e.x.c
	mv e.x.o $(O)e.x.o

$(O)pres.o: pres.c
	$(CC) $(CFLAGS) $(INCL) -c pres.c
	mv pres.o $(O)pres.o

#e.h:    $(LOCALINCL)/c_env.h $(.PRECIOUS)
#        touch e.h
#
#e.sg.h: $(.PRECIOUS)
#        touch e.sg.h

# DO NOT DELETE THIS LINE

e.h: $(LOCALINCL)c_env.h
e.h: $(LOCALINCL)localenv.h
e.h: $(STD)sys/types.h
e.h: $(LA)la.h
e.h: $(FF)ff.h
e.h: e.t.h
e.sg.h: $(STD)$(SGTT_INCL)
$(O)e.o: e.h
$(O)e.o: e.e.h
$(O)e.o: e.it.h
$(O)e.o: e.tt.h
$(O)e.o: e.wi.h
$(O)e.o: e.fn.h
$(O)e.o: e.sg.h
$(O)e.o: e.inf.h
$(O)e.o: $(STD)sys/stat.h
$(O)e.o: $(STD)$(SIG_INCL)
$(O)e.cm.o: e.h
$(O)e.cm.o: e.e.h
$(O)e.cm.o: e.m.h
$(O)e.cm.o: e.ru.h
$(O)e.cm.o: e.cm.h
$(O)e.cm.o: e.wi.h
$(O)e.cm.o: $(STD)$(SIG_INCL)
$(O)e.d.o: e.h
$(O)e.d.o: e.tt.h
$(O)e.d.o: e.it.h
$(O)e.d.o: $(STD)$(SIG_INCL)
$(O)e.dif.o: $(STD)ctype.h
$(O)e.dif.o: e.h
$(O)e.dif.o: e.m.h
$(O)e.dif.o: e.cm.h
$(O)e.e.o: e.h
$(O)e.e.o: e.m.h
$(O)e.e.o: e.cm.h
$(O)e.e.o: e.e.h
$(O)e.f.o: e.h
$(O)e.f.o: $(STD)sys/stat.h
$(O)e.iit.o: e.h
$(O)e.iit.o: e.it.h
$(O)e.iit.o: $(STD)ctype.h
$(O)e.it.o: e.h
$(O)e.it.o: e.it.h
$(O)e.la.o: e.h
$(O)e.la.o: e.m.h
$(O)e.la.o: $(STD)$(SIG_INCL)
$(O)e.m.o: e.h
$(O)e.m.o: e.cm.h
$(O)e.m.o: e.inf.h
$(O)e.m.o: e.m.h
$(O)e.m.o: e.tt.h
$(O)e.m.o: e.e.h
$(O)e.main.o: e.h
$(O)e.mk.o: e.h
$(O)e.mk.o: e.inf.h
$(O)e.mk.o: e.m.h
$(O)e.nm.o: e.h
$(O)e.nm.o: e.inf.h
$(O)e.nm.o: e.m.h
$(O)e.nm.o: e.cm.h
$(O)e.nm.o: $(STD)sys/stat.h
$(O)e.p.o: e.h
$(O)e.p.o: e.cm.h
$(O)e.pa.o: $(STD)ctype.h
$(O)e.pa.o: e.h
$(O)e.pa.o: e.m.h
$(O)e.pa.o: e.cm.h
$(O)e.put.o: e.h
$(O)e.put.o: e.m.h
$(O)e.put.o: e.cm.h
$(O)e.put.o: e.e.h
$(O)e.q.o: e.h
$(O)e.q.o: e.m.h
$(O)e.q.o: e.cm.h
$(O)e.q.o: e.fn.h
$(O)e.q.o: e.tt.h
$(O)e.q.o: $(STD)$(SIG_INCL)
$(O)e.ra.o: e.h
$(O)e.ra.o: e.m.h
$(O)e.ra.o: e.cm.h
$(O)e.re.o: e.h
$(O)e.re.o: e.m.h
$(O)e.re.o: e.cm.h
$(O)e.re.o: e.se.h
$(O)e.record.o: e.h
$(O)e.record.o: e.cm.h
$(O)e.record.o: e.m.h
$(O)e.record.o: e.inf.h
$(O)e.ru.o: e.h
$(O)e.ru.o: e.e.h
$(O)e.ru.o: e.m.h
$(O)e.ru.o: e.cm.h
$(O)e.ru.o: e.ru.h
$(O)e.ru.o: $(STD)$(SIG_INCL)
$(O)e.sb.o: e.h
$(O)e.sb.o: e.fn.h
$(O)e.sb.o: e.sg.h
$(O)e.sb.o: $(STD)$(SIG_INCL)
$(O)e.sb.o: $(STD)sys/stat.h
$(O)e.se.o: $(STD)ctype.h
$(O)e.se.o: e.h
$(O)e.se.o: e.m.h
$(O)e.se.o: e.cm.h
$(O)e.profile.o: e.h
$(O)e.sv.o: e.h
$(O)e.sv.o: e.m.h
$(O)e.sv.o: e.cm.h
$(O)e.sv.o: $(STD)sys/stat.h
$(O)e.t.o: e.h
$(O)e.t.o: e.inf.h
$(O)e.t.o: e.m.h
$(O)e.t.o: e.tt.h
$(O)e.t.o: $(STD)$(SIG_INCL)
$(O)e.tb.o: e.h
$(O)e.tb.o: e.m.h
$(O)e.tb.o: e.cm.h
$(O)e.tt.o: e.h
$(O)e.tt.o: e.tt.h
$(O)e.tt.o: term/standard.c
$(O)e.tt.o: term/tcap.c
$(O)e.tt.o: term/annarbor.c
$(O)e.tt.o: term/ambas.c
$(O)e.tt.o: term/adm3a.c
$(O)e.tt.o: term/adm31.c
$(O)e.tt.o: term/dy.c
$(O)e.tt.o: term/dm4000.c
$(O)e.tt.o: term/h19.c
$(O)e.tt.o: term/intext.c
$(O)e.tt.o: term/intext2.c
$(O)e.tt.o: term/adm42.c
$(O)e.tt.o: term/c100.c
$(O)e.tt.o: term/po.c
$(O)e.tt.o: term/vt100.c
$(O)e.u.o: e.h
$(O)e.u.o: e.m.h
$(O)e.u.o: e.cm.h
$(O)e.u.o: $(STD)sys/stat.h
$(O)e.wi.o: e.h
$(O)e.wi.o: e.cm.h
$(O)e.wi.o: e.inf.h
$(O)e.wi.o: e.tt.h
$(O)e.wi.o: e.wi.h
$(O)e.wk.o: e.h
$(O)e.wk.o: e.inf.h
$(O)e.x.o: e.h
$(O)e.x.o: e.e.h
$(O)e.x.o: e.inf.h
$(O)e.x.o: e.m.h
$(O)e.x.o: e.sg.h
$(O)e.x.o: $(STD)$(SIG_INCL)

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
