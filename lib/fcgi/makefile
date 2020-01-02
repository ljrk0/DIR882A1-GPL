TOPDIR=../
include $(TOPDIR)Rules.mak

LIBZLIB_SRC=fcgi-2.4.0
PKG_SOURCE:=fcgi-2.4.0.tar.gz
DIRS := ${LIBZLIB_SRC}

LIB_SHARED_FULLNAME=libfcgi.so.0.0.0
LIB_SHARED_MAJOR=libfcgi.so.0
LIB_SHARED=libfcgi.so

CONFIGURE_CMD = ./configure
CONFMAKEFILE = ./Makefile
TARGET_CXX := g++-uc
CONFOPTS= --host=mipsel-linux

STRIP_CC=$(strip $(subst ",,${CC}))
TARGET_CONFIGURE_OPTS = \
  AR=$(strip $(subst ",,${AR})) \
  AS=$(strip $(subst ",,${AS})) \
  LD=$(strip $(subst ",,${LD})) \
  NM=$(strip $(subst ",,${NM})) \
  CC=$(strip $(subst ",,${CC})) \
  GCC=$(strip $(subst ",,${GCC})) \
  CXX=$(strip $(subst ",,${TARGET_CXX})) \
  RANLIB=$(strip $(subst ",,${RANLIB})) \
  STRIP=$(strip $(subst ",,${STRIP})) \
  OBJCOPY=$(strip $(subst ",,${OBJCOPY})) \
  OBJDUMP=$(strip $(subst ",,${OBJDUMP})) \
  SIZE=$(strip $(subst ",,${SIZE}))

TARGET := mipsel-linux
GNU_TARGET_NAME := $(TARGET)
GNU_HOST_NAME := $(shell gcc -dumpmachine)

CONFIGURE_PREFIX:=/usr
CONFIGURE_PREFIX_LIB:=$(TARGETDIR)/usr
CONFIGURE_ARGS = \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(CONFIGURE_PREFIX) \
		--exec-prefix=$(EXE_PREFIX) \
		--bindir=$(CONFIGURE_PREFIX_LIB)/bin \
		--sbindir=$(CONFIGURE_PREFIX_LIB)/sbin \
		--libexecdir=$(CONFIGURE_PREFIX_LIB)/lib \
		--includedir=$(CONFIGURE_PREFIX_LIB)/include \
		--sysconfdir=$(CONFIGURE_PREFIX)/etc \
		--datadir=$(CONFIGURE_PREFIX)/share \
		--localstatedir=$(CONFIGURE_PREFIX)/var \
		--mandir=$(CONFIGURE_PREFIX)/man \
		--infodir=$(CONFIGURE_PREFIX)/info \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		$(DISABLE_IPV6)

TARGET_CFLAGS += $(FPIC)



LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

all:  Makefile configure
	make -C fcgi-2.4.0 all-recursive all 

Makefile:
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  tar -xvf $(PKG_SOURCE); \
	fi
	
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  echo "tar \"$(PKG_SOURCE)\" not exists! "; \
	  exit 1;  \
	fi;


configure:
	(cd $(LIBZLIB_SRC); \
		if [ -x $(CONFIGURE_CMD) ]; then \
			if [ ! -f $(CONFMAKEFILE) ]; then \
				LIBS="-lm" $(TARGET_CONFIGURE_OPTS) $(CONFIGURE_CMD) $(CONFIGURE_ARGS) ;\
			fi; \
		fi; \
	)
# the is a proxy section --YY
all-recursive: Makefile
	make -C fcgi-2.4.0 all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -C fcgi-2.4.0 all-am

# the is a proxy section --YY
clean-am: Makefile
	make -C fcgi-2.4.0 clean-am

clean: Makefile
	make -C fcgi-2.4.0 clean

romfs:

shared: all
	$(INSTALL) -d $(TOPDIR)lib		
	$(RM) $(TOPDIR)lib/$(LIB_SHARED)*
	$(INSTALL) -m 644 fcgi-2.4.0/libfcgi/.libs/$(LIB_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED)
	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED_MAJOR)


