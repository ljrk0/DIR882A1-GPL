TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux CFLAGS=-I$(TOPDIR)../user/openssl-1.0.2j/include #wwzh --with-event

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

MAJOR_VERSION=2
MINOR_VERSION=0
SUBLEVEL=2
LIBEVENT_A= libevent.a
LIBEVENT_SHARED=libevent-$(MAJOR_VERSION).$(MINOR_VERSION).so
LIBEVENT_SHARED_FULLNAME=libevent-$(MAJOR_VERSION).$(MINOR_VERSION).so

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:

shared: all
	$(INSTALL) -d $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent-2.0.so.5.1.9 $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_core-2.0.so.5.1.9 $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_extra-2.0.so.5.1.9 $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_pthreads-2.0.so.5.1.9 $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_openssl-2.0.so.5.1.9 $(TOPDIR)lib
	$(LN) -sf $(TOPDIR)lib/libevent_core-2.0.so.5.1.9 $(TOPDIR)lib/libevent_core.so 
	$(LN) -sf $(TOPDIR)lib/libevent_core-2.0.so.5.1.9 $(TOPDIR)lib/libevent_core-2.0.so.5
	$(LN) -sf $(TOPDIR)lib/libevent-2.0.so.5.1.9 $(TOPDIR)lib/libevent.so
	$(LN) -sf $(TOPDIR)lib/libevent-2.0.so.5.1.9 $(TOPDIR)lib/libevent-2.0.so.5
	$(LN) -sf $(TOPDIR)lib/libevent_extra-2.0.so.5.1.9 $(TOPDIR)lib/libevent_extra.so 
	$(LN) -sf $(TOPDIR)lib/libevent_extra-2.0.so.5.1.9 $(TOPDIR)lib/libevent_extra-2.0.so.5 
	$(LN) -sf $(TOPDIR)lib/libevent_pthreads-2.0.so.5.1.9 $(TOPDIR)lib/libevent_pthreads-2.0.so.5
	$(LN) -sf $(TOPDIR)lib/libevent_pthreads-2.0.so.5.1.9 $(TOPDIR)lib/libevent_pthreads.so 
	$(LN) -sf $(TOPDIR)lib/libevent_openssl-2.0.so.5.1.9 $(TOPDIR)lib/libevent_openssl.so
	$(LN) -sf $(TOPDIR)lib/libevent_openssl-2.0.so.5.1.9 $(TOPDIR)lib/libevent_openssl-2.0.so.5


