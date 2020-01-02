TOPDIR=../
include $(TOPDIR)Rules.mak


ifeq ($(CONFIG_MCAFEE_SUPPORT), y)
	VERSION=0.12.1
else 
	VERSION=0.11
endif

LIBZLIB_SRC=json-c-$(VERSION)
PKG_SOURCE:=json-c-$(VERSION).tar.gz
DIRS := ${LIBZLIB_SRC}

LIB_SHARED_FULLNAME=libjson.so.0.1.0
LIB_SHARED_MAJOR=libjson.so.0
LIB_SHARED=libjson.so
ifeq ($(CONFIG_MCAFEE_SUPPORT), y)
LIBC_SHARED_FULLNAME=libjson-c.so.2.0.2
else
LIBC_SHARED_FULLNAME=libjson-c.so.2.0.1
endif
LIBC_SHARED_MAJOR=libjson-c.so.2
LIBC_SHARED=libjson-c.so

STRIP_CC=$(strip $(subst ",,${CC}))
TARGET_CONFIGURE_OPTS = \
  AR=$(strip $(subst ",,${AR})) \
  AS=$(strip $(subst ",,${AS})) \
  LD=$(strip $(subst ",,${LD})) \
  NM=$(strip $(subst ",,${NM})) \
  CC=$(strip $(subst ",,${CC})) \
  GCC=$(strip $(subst ",,${GCC})) \
  CXX=$(strip $(subst ",,${CXX})) \
  RANLIB=$(strip $(subst ",,${RANLIB})) \
  STRIP=$(strip $(subst ",,${STRIP})) \
  OBJCOPY=$(strip $(subst ",,${OBJCOPY})) \
  OBJDUMP=$(strip $(subst ",,${OBJDUMP})) \
  SIZE=$(strip $(subst ",,${SIZE}))
  
CONFIGURE_CMD = ./configure
CONFIG_LIB = .libs/$(LIB_SHARED_FULLNAME)
CONFOPTS= --host=mipsel-linux

TARGET := mipsel-linux
GNU_TARGET_NAME := $(TARGET)
GNU_HOST_NAME := $(shell gcc -dumpmachine)

EXTRA_CXXFLAGS = $(EXTRA_CFLAGS)
LIGHTTPD_PREFIX:=$(TARGETDIR)
LIGHTTPD_INSTALL:=$(TARGETDIR)/usr
CONFIGURE_PREFIX:=/usr
CONFIGURE_ARGS = \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(LIGHTTPD_PREFIX) \
		--exec-prefix=$(LIGHTTPD_INSTALL) \
		--bindir=$(CONFIGURE_PREFIX)/bin \
		--sbindir=$(CONFIGURE_PREFIX)/sbin \
		--libexecdir=$(LIGHTTPD_INSTALL)/lib \
		--sysconfdir=/etc \
		--datadir=$(CONFIGURE_PREFIX)/share \
		--localstatedir=$(CONFIGURE_PREFIX)/var \
		--mandir=$(CONFIGURE_PREFIX)/man \
		--infodir=$(CONFIGURE_PREFIX)/info \
		$(DISABLE_LARGEFILE) \
		$(DISABLE_IPV6)

TARGET_CFLAGS += $(FPIC)

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

all:  Makefile configure
	make -C $(LIBZLIB_SRC) all

Makefile:
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  	tar -xvf $(PKG_SOURCE); \
		cp config_$(VERSION).h.in $(LIBZLIB_SRC)/config.h.in; \
		cp json_object_$(VERSION).c $(LIBZLIB_SRC)/json_object.c; \
	fi
	
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  echo "tar \"$(PKG_SOURCE)\" not exists! "; \
	  exit 1;  \
	fi;

configure:
ifeq ($(CONFIG_MCAFEE_SUPPORT), y)
	(cd $(LIBZLIB_SRC); \
		sed -i s/-Werror// Makefile.in tests/Makefile.in; \
		if [ -x $(CONFIGURE_CMD) ]; then \
			if [ ! -f $(CONFIG_LIB) ]; then \
				make clean ; \
				LIBS="-lm" $(TARGET_CONFIGURE_OPTS) $(CONFIGURE_CMD) $(CONFOPTS) $(CONFIGURE_ARGS) ; \
			fi; \
		fi; \
	)
else
	(cd $(LIBZLIB_SRC); \
#		sed -i s/-Werror// Makefile.in tests/Makefile.in; \
		if [ -x $(CONFIGURE_CMD) ]; then \
			if [ ! -f $(CONFIG_LIB) ]; then \
				make clean ; \
				LIBS="-lm" $(TARGET_CONFIGURE_OPTS) $(CONFIGURE_CMD) $(CONFOPTS) $(CONFIGURE_ARGS) ; \
			fi; \
		fi; \
	)	
endif

# the is a proxy section --YY
all-recursive: Makefile
	make -C $(LIBZLIB_SRC) all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -C $(LIBZLIB_SRC) all-am

# the is a proxy section --YY
clean-am: Makefile
	make -C $(LIBZLIB_SRC) clean-am

clean: Makefile
	make -C $(LIBZLIB_SRC) clean

romfs:

shared: all
	$(INSTALL) -d $(TOPDIR)lib
ifeq ($(CONFIG_MCAFEE_SUPPORT), y)
#	$(RM) $(TOPDIR)lib/$(LIB_SHARED)*
	$(RM) $(TOPDIR)lib/$(LIBC_SHARED)*
#	$(INSTALL) -m 644 $(LIBZLIB_SRC)/.libs/$(LIB_SHARED_FULLNAME) $(TOPDIR)lib
#	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED)
#	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED_MAJOR)
else
	$(RM) $(TOPDIR)lib/$(LIB_SHARED)*
	$(RM) $(TOPDIR)lib/$(LIBC_SHARED)*
	$(INSTALL) -m 644 $(LIBZLIB_SRC)/.libs/$(LIB_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED)
	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED_MAJOR)
endif	
	$(INSTALL) -m 644 $(LIBZLIB_SRC)/.libs/$(LIBC_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBC_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBC_SHARED)
	$(LN) -sf $(LIBC_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBC_SHARED_MAJOR)


