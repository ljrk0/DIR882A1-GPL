TOPDIR=../
include $(TOPDIR)Rules.mak

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))
CONFOPTS= --host=mipsel-linux

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
	$(INSTALL) -m 644 .libs/libsqlite3.a $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libsqlite3.la $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libsqlite3.so $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libsqlite3.so.0 $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libsqlite3.so.0.8.6 $(TOPDIR)lib
	$(ROMFSINST) ./sqlite3 /sbin/sqlite3
