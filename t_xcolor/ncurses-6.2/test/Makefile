# $Id: Makefile.in,v 1.121 2020/02/02 23:34:34 tom Exp $
##############################################################################
# Copyright 2020 Thomas E. Dickey                                            #
# Copyright 1998-2017,2018 Free Software Foundation, Inc.                    #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, distribute    #
# with modifications, sublicense, and/or sell copies of the Software, and to #
# permit persons to whom the Software is furnished to do so, subject to the  #
# following conditions:                                                      #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
# Except as contained in this notice, the name(s) of the above copyright     #
# holders shall not be used in advertising or otherwise to promote the sale, #
# use or other dealings in this Software without prior written               #
# authorization.                                                             #
##############################################################################
#
# Author: Thomas E. Dickey 1996-on
#
# Makefile for ncurses tests.

# turn off _all_ suffix rules; we'll generate our own
.SUFFIXES:

SHELL		= /bin/sh




destdir		=
x		= 
o		= .o

MODEL		= ../test
top_srcdir	= .
srcdir		= .
prefix		= /usr/local
exec_prefix	= ${prefix}
datarootdir	= ${prefix}/share

bindir		= ${exec_prefix}/bin
libdir		= ${exec_prefix}/lib
includedir	= ${prefix}/include
includesubdir	= 
datarootdir	= ${prefix}/share
datadir		= ${datarootdir}

BINDIR		= $(DESTDIR)$(bindir)
LIBDIR		= $(DESTDIR)$(libdir)
INCLUDEDIR	= $(DESTDIR)$(includedir)$(includesubdir)
DATADIR		= $(DESTDIR)$(datadir)

PACKAGE		= ncurses-examples

LIBTOOL		= 
LIBTOOL_OPTS	= 
LIBTOOL_CLEAN	= 
LIBTOOL_COMPILE	= 
LIBTOOL_LINK	= ${CC}

INSTALL		= /usr/bin/install -c 
INSTALL_PROG	= ${INSTALL} -s
INSTALL_SCRIPT	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
transform	= s,x,x,
TRANSFORM	= sed 's/$x$$//'|sed '$(transform)'|sed 's/$$/$x/'

CTAGS		= ctags
ETAGS		= etags

CC		= gcc
CPP		= gcc -E

CFLAGS		= -g -O2 -Qunused-arguments 
CPPFLAGS	= -I. -I$(srcdir) -I../test -DHAVE_CONFIG_H -DDATA_DIR=\"$(datadir)\" -D_DARWIN_C_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_XPG5

CCFLAGS		= $(CPPFLAGS) $(CFLAGS)

CFLAGS_LIBTOOL	= $(CCFLAGS)
CFLAGS_NORMAL	= $(CCFLAGS)
CFLAGS_DEBUG	= $(CCFLAGS) -g -DTRACE
CFLAGS_PROFILE	= $(CCFLAGS) -pg
CFLAGS_SHARED	= $(CCFLAGS) unknown

CFLAGS_DEFAULT	= $(CFLAGS_NORMAL)

REL_VERSION	= 
ABI_VERSION	= 
LOCAL_LIBDIR	= /Users/obrien/Projects/t_xcolor/ncurses-6.2/test/lib
LOCAL_LIBS	= 
MATH_LIB	= 
TEST_LIBS	= -lutil 

LD		= ld
LINK		=  $(LIBTOOL_LINK) $(CFLAGS)

LDFLAGS		=   

LDFLAGS_LIBTOOL	= $(LDFLAGS) $(CFLAGS_LIBTOOL)
LDFLAGS_NORMAL	= $(LDFLAGS) $(CFLAGS_NORMAL)
LDFLAGS_DEBUG	= $(LDFLAGS) $(CFLAGS_DEBUG)
LDFLAGS_PROFILE	= $(LDFLAGS) $(CFLAGS_PROFILE)
LDFLAGS_SHARED	= $(LDFLAGS) $(CFLAGS_SHARED) 

TEST_ARGS	=  -lform -lmenu -lpanel -lncurses  

# use these for linking with all of the libraries
LIBS_DEFAULT	= $(TEST_ARGS)  $(TEST_LIBS) $(MATH_LIB)
LDFLAGS_DEFAULT	= $(LDFLAGS_NORMAL) $(LIBS_DEFAULT)

# use these for linking with the (n)curses library and possibly pthreads
LIBS_THREADS	= `echo "$(TEST_ARGS) " | sed -e 's/-lform.*-lpanel[^ ]*//'` $(TEST_LIBS) $(MATH_LIB) -lm
LDFLAGS_THREADS	= $(LDFLAGS_NORMAL) $(LIBS_THREADS)

# use these for linking with the (n)curses library
LIBS_CURSES	= `echo "$(TEST_ARGS) " | sed -e 's/-lform.*-lpanel[^ ]*//'` $(TEST_LIBS) $(MATH_LIB)
LDFLAGS_CURSES	= $(LDFLAGS_NORMAL) $(LIBS_CURSES)

# use these for linking with the tinfo library if we have it, or curses library if not
LIBS_TINFO	=  $(LIBS_CURSES)   $(TEST_LIBS) $(MATH_LIB)
LDFLAGS_TINFO	=  $(LDFLAGS_NORMAL) $(LIBS_TINFO)

LINT		= 
LINT_OPTS	= 
LINT_LIBS	= -lform -lmenu -lpanel -lncurses 

HEADER_DEPS	= \
	../include/curses.h \
	../include/term.h

# The rest is generated from the "programs" and "modules" files...
# generated by mk-test.awk

TESTS	= $(destdir)background$x \
	  $(destdir)blue$x \
	  $(destdir)bs$x \
	  $(destdir)cardfile$x \
	  $(destdir)chgat$x \
	  $(destdir)clip_printw$x \
	  $(destdir)color_content$x \
	  $(destdir)color_set$x \
	  $(destdir)demo_altkeys$x \
	  $(destdir)demo_defkey$x \
	  $(destdir)demo_forms$x \
	  $(destdir)demo_keyok$x \
	  $(destdir)demo_menus$x \
	  $(destdir)demo_new_pair$x \
	  $(destdir)demo_panels$x \
	  $(destdir)demo_tabs$x \
	  $(destdir)demo_termcap$x \
	  $(destdir)demo_terminfo$x \
	  $(destdir)ditto$x \
	  $(destdir)dots$x \
	  $(destdir)dots_curses$x \
	  $(destdir)dots_mvcur$x \
	  $(destdir)dots_termcap$x \
	  $(destdir)dots_xcurses$x \
	  $(destdir)echochar$x \
	  $(destdir)extended_color$x \
	  $(destdir)filter$x \
	  $(destdir)firework$x \
	  $(destdir)firstlast$x \
	  $(destdir)foldkeys$x \
	  $(destdir)form_driver_w$x \
	  $(destdir)gdc$x \
	  $(destdir)hanoi$x \
	  $(destdir)hashtest$x \
	  $(destdir)inch_wide$x \
	  $(destdir)inchs$x \
	  $(destdir)ins_wide$x \
	  $(destdir)insdelln$x \
	  $(destdir)inserts$x \
	  $(destdir)key_names$x \
	  $(destdir)keynames$x \
	  $(destdir)knight$x \
	  $(destdir)list_keys$x \
	  $(destdir)lrtest$x \
	  $(destdir)movewindow$x \
	  $(destdir)ncurses$x \
	  $(destdir)newdemo$x \
	  $(destdir)padview$x \
	  $(destdir)pair_content$x \
	  $(destdir)picsmap$x \
	  $(destdir)railroad$x \
	  $(destdir)rain$x \
	  $(destdir)redraw$x \
	  $(destdir)savescreen$x \
	  $(destdir)sp_tinfo$x \
	  $(destdir)tclock$x \
	  $(destdir)test_add_wchstr$x \
	  $(destdir)test_addchstr$x \
	  $(destdir)test_addstr$x \
	  $(destdir)test_addwstr$x \
	  $(destdir)test_arrays$x \
	  $(destdir)test_get_wstr$x \
	  $(destdir)test_getstr$x \
	  $(destdir)test_instr$x \
	  $(destdir)test_inwstr$x \
	  $(destdir)test_opaque$x \
	  $(destdir)test_setupterm$x \
	  $(destdir)test_sgr$x \
	  $(destdir)test_termattrs$x \
	  $(destdir)test_vid_puts$x \
	  $(destdir)test_vidputs$x \
	  $(destdir)testaddch$x \
	  $(destdir)testcurs$x \
	  $(destdir)testscanw$x \
	  $(destdir)view$x \
	  $(destdir)worm$x \
	  $(destdir)xmas$x
SCRIPTS = \
	$(srcdir)/savescreen.sh \
	$(srcdir)/tput-colorcube \
	$(srcdir)/tput-initc \
	$(srcdir)/tracemunch
DATAFILES = \
	$(srcdir)/*.x* \
	$(srcdir)/*.dat

all: $(TESTS)

sources:

check:
	@ echo The test-programs are interactive
tags:
	$(CTAGS) *.[ch]

# no libraries here
libs \
install.libs \
uninstall.libs:

# we might install the test-programs
install \
install.test: $(BINDIR) $(DATADIR) $(TESTS)
	@$(SHELL) -c 'for src in $(TESTS); do \
	dst=`echo $$src | $(TRANSFORM)`; \
	echo "installing $$src -> $(BINDIR)/$$dst"; \
	$(INSTALL_PROG) $$src $(BINDIR)/$$dst; \
	done'
	@$(SHELL) -c 'for src in $(SCRIPTS); do \
	dst=`echo $$src | sed -e 's,^.*/,,' | $(TRANSFORM)`; \
	echo "installing $$src -> $(BINDIR)/$$dst"; \
	$(INSTALL_SCRIPT) $$src $(BINDIR)/$$dst; \
	done'
	@$(SHELL) -c 'for src in $(DATAFILES); do \
	dst=`echo $$src | sed -e 's,^.*/,,'`; \
	echo "installing $$src -> $(DATADIR)/$$dst"; \
	$(INSTALL_DATA) $$src $(DATADIR)/$$dst; \
	done'

uninstall \
uninstall.test:
	@$(SHELL) -c 'for src in $(TESTS); do \
	dst=`echo $$src | $(TRANSFORM)`; \
	rm -f $(BINDIR)/$$dst; \
	done'
	@$(SHELL) -c 'for src in $(SCRIPTS); do \
	dst=`echo $$src | sed -e 's,^.*/,,' | $(TRANSFORM)`; \
	rm -f $(BINDIR)/$$dst; \
	done'
	@$(SHELL) -c 'for src in $(DATAFILES); do \
	dst=`echo $$src | sed -e 's,^.*/,,'`; \
	rm -f $(DATADIR)/$$dst; \
	done'

mostlyclean ::
	-rm -f core tags TAGS *~ *.bak *.i *.ln *.atac trace

clean :: mostlyclean
	-$(SHELL) -c "if test -n '$x' ; then $(MAKE) clean x=''; fi"
	-rm -rf *$o screendump *.lis $(TESTS) .libs *.dSYM

distclean :: clean
	-rm -f Makefile ncurses_cfg.h config.status config.log

realclean :: distclean

lint:
	$(SHELL) -c 'for N in $(TESTS); do echo LINT:$$N; $(LINT) $(LINT_OPTS) $(CPPFLAGS) $(srcdir)/$$N.c $(LINT_LIBS); done'
$(BINDIR) $(DATADIR) :
	mkdir -p $@

$(destdir)background$x: $(MODEL)/background$o $(MODEL)/dump_window$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/background$o $(MODEL)/dump_window$o $(LDFLAGS_CURSES)

$(destdir)blue$x: $(MODEL)/blue$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/blue$o $(LDFLAGS_CURSES)

$(destdir)bs$x: $(MODEL)/bs$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/bs$o $(LDFLAGS_CURSES)

$(destdir)cardfile$x: $(MODEL)/cardfile$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/cardfile$o $(LDFLAGS_DEFAULT)

$(destdir)chgat$x: $(MODEL)/chgat$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/chgat$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)clip_printw$x: $(MODEL)/clip_printw$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/clip_printw$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)color_content$x: $(MODEL)/color_content$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/color_content$o $(LDFLAGS_CURSES)

$(destdir)color_set$x: $(MODEL)/color_set$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/color_set$o $(LDFLAGS_CURSES)

$(destdir)demo_altkeys$x: $(MODEL)/demo_altkeys$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_altkeys$o $(LDFLAGS_CURSES)

$(destdir)demo_defkey$x: $(MODEL)/demo_defkey$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_defkey$o $(LDFLAGS_CURSES)

$(destdir)demo_forms$x: $(MODEL)/demo_forms$o $(MODEL)/edit_field$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_forms$o $(MODEL)/edit_field$o $(MODEL)/popup_msg$o $(LDFLAGS_DEFAULT)

$(destdir)demo_keyok$x: $(MODEL)/demo_keyok$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_keyok$o $(LDFLAGS_CURSES)

$(destdir)demo_menus$x: $(MODEL)/demo_menus$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_menus$o $(LDFLAGS_DEFAULT)

$(destdir)demo_new_pair$x: $(MODEL)/demo_new_pair$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_new_pair$o $(MODEL)/popup_msg$o $(LDFLAGS_DEFAULT)

$(destdir)demo_panels$x: $(MODEL)/demo_panels$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_panels$o $(LDFLAGS_DEFAULT)

$(destdir)demo_tabs$x: $(MODEL)/demo_tabs$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_tabs$o $(LDFLAGS_CURSES)

$(destdir)demo_termcap$x: $(MODEL)/demo_termcap$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_termcap$o $(LDFLAGS_TINFO)

$(destdir)demo_terminfo$x: $(MODEL)/demo_terminfo$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/demo_terminfo$o $(LDFLAGS_TINFO)

$(destdir)ditto$x: $(MODEL)/ditto$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/ditto$o $(LDFLAGS_THREADS)

$(destdir)dots$x: $(MODEL)/dots$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/dots$o $(LDFLAGS_TINFO)

$(destdir)dots_curses$x: $(MODEL)/dots_curses$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/dots_curses$o $(LDFLAGS_CURSES)

$(destdir)dots_mvcur$x: $(MODEL)/dots_mvcur$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/dots_mvcur$o $(LDFLAGS_CURSES)

$(destdir)dots_termcap$x: $(MODEL)/dots_termcap$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/dots_termcap$o $(LDFLAGS_TINFO)

$(destdir)dots_xcurses$x: $(MODEL)/dots_xcurses$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/dots_xcurses$o $(LDFLAGS_CURSES)

$(destdir)echochar$x: $(MODEL)/echochar$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/echochar$o $(LDFLAGS_CURSES)

$(destdir)extended_color$x: $(MODEL)/extended_color$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/extended_color$o $(LDFLAGS_CURSES)

$(destdir)filter$x: $(MODEL)/filter$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/filter$o $(LDFLAGS_CURSES)

$(destdir)firework$x: $(MODEL)/firework$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/firework$o $(LDFLAGS_CURSES)

$(destdir)firstlast$x: $(MODEL)/firstlast$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/firstlast$o $(LDFLAGS_DEFAULT)

$(destdir)foldkeys$x: $(MODEL)/foldkeys$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/foldkeys$o $(LDFLAGS_CURSES)

$(destdir)form_driver_w$x: $(MODEL)/form_driver_w$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/form_driver_w$o $(MODEL)/popup_msg$o $(LDFLAGS_DEFAULT)

$(destdir)gdc$x: $(MODEL)/gdc$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/gdc$o $(LDFLAGS_CURSES)

$(destdir)hanoi$x: $(MODEL)/hanoi$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/hanoi$o $(LDFLAGS_CURSES)

$(destdir)hashtest$x: $(MODEL)/hashtest$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/hashtest$o $(LDFLAGS_CURSES)

$(destdir)inch_wide$x: $(MODEL)/inch_wide$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/inch_wide$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)inchs$x: $(MODEL)/inchs$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/inchs$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)ins_wide$x: $(MODEL)/ins_wide$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/ins_wide$o $(LDFLAGS_CURSES)

$(destdir)insdelln$x: $(MODEL)/insdelln$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/insdelln$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)inserts$x: $(MODEL)/inserts$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/inserts$o $(LDFLAGS_CURSES)

$(destdir)key_names$x: $(MODEL)/key_names$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/key_names$o $(LDFLAGS_CURSES)

$(destdir)keynames$x: $(MODEL)/keynames$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/keynames$o $(LDFLAGS_CURSES)

$(destdir)knight$x: $(MODEL)/knight$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/knight$o $(LDFLAGS_CURSES)

$(destdir)list_keys$x: $(MODEL)/list_keys$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/list_keys$o $(LDFLAGS_TINFO)

$(destdir)lrtest$x: $(MODEL)/lrtest$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/lrtest$o $(LDFLAGS_CURSES)

$(destdir)movewindow$x: $(MODEL)/movewindow$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/movewindow$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)ncurses$x: $(MODEL)/ncurses$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/ncurses$o $(LDFLAGS_DEFAULT)

$(destdir)newdemo$x: $(MODEL)/newdemo$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/newdemo$o $(LDFLAGS_CURSES)

$(destdir)padview$x: $(MODEL)/padview$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/padview$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)pair_content$x: $(MODEL)/pair_content$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/pair_content$o $(LDFLAGS_CURSES)

$(destdir)picsmap$x: $(MODEL)/picsmap$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/picsmap$o $(LDFLAGS_CURSES)

$(destdir)railroad$x: $(MODEL)/railroad$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/railroad$o $(LDFLAGS_TINFO)

$(destdir)rain$x: $(MODEL)/rain$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/rain$o $(MODEL)/popup_msg$o $(LDFLAGS_THREADS)

$(destdir)redraw$x: $(MODEL)/redraw$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/redraw$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)savescreen$x: $(MODEL)/savescreen$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/savescreen$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)sp_tinfo$x: $(MODEL)/sp_tinfo$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/sp_tinfo$o $(LDFLAGS_TINFO)

$(destdir)tclock$x: $(MODEL)/tclock$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/tclock$o $(LDFLAGS_CURSES)

$(destdir)test_add_wchstr$x: $(MODEL)/test_add_wchstr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_add_wchstr$o $(LDFLAGS_CURSES)

$(destdir)test_addchstr$x: $(MODEL)/test_addchstr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_addchstr$o $(LDFLAGS_CURSES)

$(destdir)test_addstr$x: $(MODEL)/test_addstr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_addstr$o $(LDFLAGS_CURSES)

$(destdir)test_addwstr$x: $(MODEL)/test_addwstr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_addwstr$o $(LDFLAGS_CURSES)

$(destdir)test_arrays$x: $(MODEL)/test_arrays$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_arrays$o $(LDFLAGS_TINFO)

$(destdir)test_get_wstr$x: $(MODEL)/test_get_wstr$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_get_wstr$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)test_getstr$x: $(MODEL)/test_getstr$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_getstr$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)test_instr$x: $(MODEL)/test_instr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_instr$o $(LDFLAGS_CURSES)

$(destdir)test_inwstr$x: $(MODEL)/test_inwstr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_inwstr$o $(LDFLAGS_CURSES)

$(destdir)test_opaque$x: $(MODEL)/test_opaque$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_opaque$o $(LDFLAGS_CURSES)

$(destdir)test_setupterm$x: $(MODEL)/test_setupterm$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_setupterm$o $(LDFLAGS_CURSES)

$(destdir)test_sgr$x: $(MODEL)/test_sgr$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_sgr$o $(LDFLAGS_TINFO)

$(destdir)test_termattrs$x: $(MODEL)/test_termattrs$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_termattrs$o $(LDFLAGS_CURSES)

$(destdir)test_vid_puts$x: $(MODEL)/test_vid_puts$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_vid_puts$o $(LDFLAGS_CURSES)

$(destdir)test_vidputs$x: $(MODEL)/test_vidputs$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/test_vidputs$o $(LDFLAGS_CURSES)

$(destdir)testaddch$x: $(MODEL)/testaddch$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/testaddch$o $(LDFLAGS_CURSES)

$(destdir)testcurs$x: $(MODEL)/testcurs$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/testcurs$o $(LDFLAGS_CURSES)

$(destdir)testscanw$x: $(MODEL)/testscanw$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/testscanw$o $(LDFLAGS_CURSES)

$(destdir)view$x: $(MODEL)/view$o $(MODEL)/popup_msg$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/view$o $(MODEL)/popup_msg$o $(LDFLAGS_CURSES)

$(destdir)worm$x: $(MODEL)/worm$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/worm$o $(LDFLAGS_THREADS)

$(destdir)xmas$x: $(MODEL)/xmas$o $(LOCAL_LIBS)
	$(LINK) -o $@ $(MODEL)/xmas$o $(LDFLAGS_CURSES)

# These rules are generated so we do not rely on suffix rules, which do not
# work consistently for different make-programs (the '$(MODEL)/' confuses
# some, and the '$x' confuses others).

$(MODEL)/background.o : $(srcdir)/background.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="background" $(srcdir)/background.c

$(MODEL)/blue.o : $(srcdir)/blue.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="blue" $(srcdir)/blue.c

$(MODEL)/bs.o : $(srcdir)/bs.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="bs" $(srcdir)/bs.c

$(MODEL)/cardfile.o : $(srcdir)/cardfile.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="cardfile" $(srcdir)/cardfile.c

$(MODEL)/chgat.o : $(srcdir)/chgat.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="chgat" $(srcdir)/chgat.c

$(MODEL)/clip_printw.o : $(srcdir)/clip_printw.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="clip_printw" $(srcdir)/clip_printw.c

$(MODEL)/color_content.o : $(srcdir)/color_content.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="color_content" $(srcdir)/color_content.c

$(MODEL)/color_set.o : $(srcdir)/color_set.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="color_set" $(srcdir)/color_set.c

$(MODEL)/demo_altkeys.o : $(srcdir)/demo_altkeys.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_altkeys" $(srcdir)/demo_altkeys.c

$(MODEL)/demo_defkey.o : $(srcdir)/demo_defkey.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_defkey" $(srcdir)/demo_defkey.c

$(MODEL)/demo_forms.o : $(srcdir)/demo_forms.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_forms" $(srcdir)/demo_forms.c

$(MODEL)/demo_keyok.o : $(srcdir)/demo_keyok.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_keyok" $(srcdir)/demo_keyok.c

$(MODEL)/demo_menus.o : $(srcdir)/demo_menus.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_menus" $(srcdir)/demo_menus.c

$(MODEL)/demo_new_pair.o : $(srcdir)/demo_new_pair.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_new_pair" $(srcdir)/demo_new_pair.c

$(MODEL)/demo_panels.o : $(srcdir)/demo_panels.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_panels" $(srcdir)/demo_panels.c

$(MODEL)/demo_tabs.o : $(srcdir)/demo_tabs.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_tabs" $(srcdir)/demo_tabs.c

$(MODEL)/demo_termcap.o : $(srcdir)/demo_termcap.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_termcap" $(srcdir)/demo_termcap.c

$(MODEL)/demo_terminfo.o : $(srcdir)/demo_terminfo.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="demo_terminfo" $(srcdir)/demo_terminfo.c

$(MODEL)/ditto.o : $(srcdir)/ditto.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="ditto" $(srcdir)/ditto.c

$(MODEL)/dots.o : $(srcdir)/dots.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="dots" $(srcdir)/dots.c

$(MODEL)/dots_curses.o : $(srcdir)/dots_curses.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="dots_curses" $(srcdir)/dots_curses.c

$(MODEL)/dots_mvcur.o : $(srcdir)/dots_mvcur.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="dots_mvcur" $(srcdir)/dots_mvcur.c

$(MODEL)/dots_termcap.o : $(srcdir)/dots_termcap.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="dots_termcap" $(srcdir)/dots_termcap.c

$(MODEL)/dots_xcurses.o : $(srcdir)/dots_xcurses.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="dots_xcurses" $(srcdir)/dots_xcurses.c

$(MODEL)/dump_window.o : $(srcdir)/dump_window.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="dump_window" $(srcdir)/dump_window.c

$(MODEL)/echochar.o : $(srcdir)/echochar.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="echochar" $(srcdir)/echochar.c

$(MODEL)/edit_field.o : $(srcdir)/edit_field.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="edit_field" $(srcdir)/edit_field.c

$(MODEL)/extended_color.o : $(srcdir)/extended_color.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="extended_color" $(srcdir)/extended_color.c

$(MODEL)/filter.o : $(srcdir)/filter.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="filter" $(srcdir)/filter.c

$(MODEL)/firework.o : $(srcdir)/firework.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="firework" $(srcdir)/firework.c

$(MODEL)/firstlast.o : $(srcdir)/firstlast.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="firstlast" $(srcdir)/firstlast.c

$(MODEL)/foldkeys.o : $(srcdir)/foldkeys.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="foldkeys" $(srcdir)/foldkeys.c

$(MODEL)/form_driver_w.o : $(srcdir)/form_driver_w.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="form_driver_w" $(srcdir)/form_driver_w.c

$(MODEL)/gdc.o : $(srcdir)/gdc.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="gdc" $(srcdir)/gdc.c

$(MODEL)/hanoi.o : $(srcdir)/hanoi.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="hanoi" $(srcdir)/hanoi.c

$(MODEL)/hashtest.o : $(srcdir)/hashtest.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="hashtest" $(srcdir)/hashtest.c

$(MODEL)/inch_wide.o : $(srcdir)/inch_wide.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="inch_wide" $(srcdir)/inch_wide.c

$(MODEL)/inchs.o : $(srcdir)/inchs.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="inchs" $(srcdir)/inchs.c

$(MODEL)/ins_wide.o : $(srcdir)/ins_wide.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="ins_wide" $(srcdir)/ins_wide.c

$(MODEL)/insdelln.o : $(srcdir)/insdelln.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="insdelln" $(srcdir)/insdelln.c

$(MODEL)/inserts.o : $(srcdir)/inserts.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="inserts" $(srcdir)/inserts.c

$(MODEL)/key_names.o : $(srcdir)/key_names.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="key_names" $(srcdir)/key_names.c

$(MODEL)/keynames.o : $(srcdir)/keynames.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="keynames" $(srcdir)/keynames.c

$(MODEL)/knight.o : $(srcdir)/knight.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="knight" $(srcdir)/knight.c

$(MODEL)/list_keys.o : $(srcdir)/list_keys.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="list_keys" $(srcdir)/list_keys.c

$(MODEL)/lrtest.o : $(srcdir)/lrtest.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="lrtest" $(srcdir)/lrtest.c

$(MODEL)/movewindow.o : $(srcdir)/movewindow.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="movewindow" $(srcdir)/movewindow.c

$(MODEL)/ncurses.o : $(srcdir)/ncurses.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="ncurses" $(srcdir)/ncurses.c

$(MODEL)/newdemo.o : $(srcdir)/newdemo.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="newdemo" $(srcdir)/newdemo.c

$(MODEL)/padview.o : $(srcdir)/padview.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="padview" $(srcdir)/padview.c

$(MODEL)/pair_content.o : $(srcdir)/pair_content.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="pair_content" $(srcdir)/pair_content.c

$(MODEL)/picsmap.o : $(srcdir)/picsmap.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="picsmap" $(srcdir)/picsmap.c

$(MODEL)/popup_msg.o : $(srcdir)/popup_msg.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="popup_msg" $(srcdir)/popup_msg.c

$(MODEL)/railroad.o : $(srcdir)/railroad.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="railroad" $(srcdir)/railroad.c

$(MODEL)/rain.o : $(srcdir)/rain.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="rain" $(srcdir)/rain.c

$(MODEL)/redraw.o : $(srcdir)/redraw.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="redraw" $(srcdir)/redraw.c

$(MODEL)/savescreen.o : $(srcdir)/savescreen.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="savescreen" $(srcdir)/savescreen.c

$(MODEL)/sp_tinfo.o : $(srcdir)/sp_tinfo.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="sp_tinfo" $(srcdir)/sp_tinfo.c

$(MODEL)/tclock.o : $(srcdir)/tclock.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="tclock" $(srcdir)/tclock.c

$(MODEL)/test_add_wchstr.o : $(srcdir)/test_add_wchstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_add_wchstr" $(srcdir)/test_add_wchstr.c

$(MODEL)/test_addchstr.o : $(srcdir)/test_addchstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_addchstr" $(srcdir)/test_addchstr.c

$(MODEL)/test_addstr.o : $(srcdir)/test_addstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_addstr" $(srcdir)/test_addstr.c

$(MODEL)/test_addwstr.o : $(srcdir)/test_addwstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_addwstr" $(srcdir)/test_addwstr.c

$(MODEL)/test_arrays.o : $(srcdir)/test_arrays.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_arrays" $(srcdir)/test_arrays.c

$(MODEL)/test_get_wstr.o : $(srcdir)/test_get_wstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_get_wstr" $(srcdir)/test_get_wstr.c

$(MODEL)/test_getstr.o : $(srcdir)/test_getstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_getstr" $(srcdir)/test_getstr.c

$(MODEL)/test_instr.o : $(srcdir)/test_instr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_instr" $(srcdir)/test_instr.c

$(MODEL)/test_inwstr.o : $(srcdir)/test_inwstr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_inwstr" $(srcdir)/test_inwstr.c

$(MODEL)/test_opaque.o : $(srcdir)/test_opaque.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_opaque" $(srcdir)/test_opaque.c

$(MODEL)/test_setupterm.o : $(srcdir)/test_setupterm.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_setupterm" $(srcdir)/test_setupterm.c

$(MODEL)/test_sgr.o : $(srcdir)/test_sgr.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_sgr" $(srcdir)/test_sgr.c

$(MODEL)/test_termattrs.o : $(srcdir)/test_termattrs.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_termattrs" $(srcdir)/test_termattrs.c

$(MODEL)/test_vid_puts.o : $(srcdir)/test_vid_puts.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_vid_puts" $(srcdir)/test_vid_puts.c

$(MODEL)/test_vidputs.o : $(srcdir)/test_vidputs.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="test_vidputs" $(srcdir)/test_vidputs.c

$(MODEL)/testaddch.o : $(srcdir)/testaddch.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="testaddch" $(srcdir)/testaddch.c

$(MODEL)/testcurs.o : $(srcdir)/testcurs.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="testcurs" $(srcdir)/testcurs.c

$(MODEL)/testscanw.o : $(srcdir)/testscanw.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="testscanw" $(srcdir)/testscanw.c

$(MODEL)/view.o : $(srcdir)/view.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="view" $(srcdir)/view.c

$(MODEL)/worm.o : $(srcdir)/worm.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="worm" $(srcdir)/worm.c

$(MODEL)/xmas.o : $(srcdir)/xmas.c \
	$(srcdir)/test.priv.h \
	ncurses_cfg.h
	
	$(CC) -c $(CFLAGS_DEFAULT) -DMODULE_NAME="xmas" $(srcdir)/xmas.c

# if we used CF_INHERIT for getting these files, we can remove them
distclean::
	rm -f config.guess config.sub
