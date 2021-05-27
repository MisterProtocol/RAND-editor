#
# Makefile for libtermcap. It is intended for Linux/ELF only.
#

LIBNAME=termcap

VERSION=2.0.8
SONAME_VERSION=2

#CC=gcc -m32
CC=gcc
CFLAGS=-O -I. -g
CFLAGS=-O -I.

AR=ar
ARFLAGS=ucvr

MAKEINFO=makeinfo

OWNER=bin.bin

STATIC_LIB=lib$(LIBNAME).a
SHARED_LIB=lib$(LIBNAME).so.$(VERSION)
SONAME_SHARED_LIB=lib$(LIBNAME).so.$(SONAME_VERSION)

TARGETS=$(STATIC_LIB) $(SHARED_LIB)

# Where is include and dir located?
prefix=/

.c.o:
	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -fPIC -c $< -o pic/$@

SRCS = termcap.c tparam.c version.c
OBJS= $(SRCS:.c=.o)

all lib: pic .depend $(TARGETS) info

$(STATIC_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(SHARED_LIB): $(OBJS)
	cd pic; \
#	$(CC) -shared -o ../$@ -Wl,-soname,$(SONAME_SHARED_LIB) $(OBJS)
	$(CC) -shared -o ../$@ -dynamiclib \
	-install_name lib$(LIBNAME).$(SONAME_VERSION).dylib \
	-current_version $(VERSION) -compatibility_version $(SONAME_VERSION).0 \
	-o ../$@ $(OBJS)

pic:
	-if [ ! -d pic ]; then mkdir pic; fi

install: lib install-dirs install-data
	-if [ -f $(prefix)/lib/$(SHARED_LIB) ]; then \
	   mkdir -p $(prefix)/lib/backup; \
	   mv $(prefix)/lib/$(SHARED_LIB) \
		$(prefix)/lib/backup/$(SHARED_LIB).$$$$; \
	fi
	cp $(SHARED_LIB) $(prefix)/lib
	chown $(OWNER) $(prefix)/lib/$(SHARED_LIB)
	if [ -x /sbin/ldconfig -o -x /etc/ldconfig ]; then \
	  ldconfig; \
	fi
	-if [ $(prefix) = "/" ]; then \
	  cp -f $(STATIC_LIB) /usr/lib; \
	  rm -f /lib/libtermcap.so; \
	  rm -f /usr/lib/libtermcap.so; \
	  ln -s /lib/$(SHARED_LIB) /usr/lib/libtermcap.so; \
	  cp -f termcap.h /usr/include; \
	  cp termcap.info* /usr/info; \
	  chown $(OWNER) \
		/usr/info/termcap.info* \
		/usr/lib/$(STATIC_LIB) \
		/usr/lib/libtermcap.so \
		/usr/include/termcap.h; \
	else \
	  cp -f $(STATIC_LIB) $(prefix)/lib; \
	  rm -f $(prefix)/lib/libtermcap.so; \
	  ln -s $(prefix)/lib/$(SHARED_LIB) \
		$(prefix)/lib/libtermcap.so; \
	  cp -f termcap.h $(prefix)/include; \
	  chown $(OWNER) \
		$(prefix)/lib/$(STATIC_LIB) \
		$(prefix)/lib/libtermcap.so \
		$(prefix)/include/termcap.h; \
	fi

install-dirs:
	-if [ $(prefix) = "/" ]; then \
	  mkdir -p /usr/lib /usr/info /usr/include /etc /lib; \
	fi

install-data:
	-if [ $(prefix) = "/" ]; then \
	  cp termcap.src /etc/termcap; \
	  chown $(OWNER) /etc/termcap; \
	fi

info: termcap.info

termcap.info: termcap.texi
	$(MAKEINFO) termcap.texi --output=$@

clean:
	-rm -fr core a.out *.o pic $(TARGETS)

distclean: clean
	-rm -f .depend

depend: .depend

.depend: $(SRCS) Makefile
	$(CC) $(CFLAGS) -M $(SRCS) | \
		sed -e 's,^[ 	]*\(.*\.o\)[ 	]*:,\1 pic/\1:,' > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
