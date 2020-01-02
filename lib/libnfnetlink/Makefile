.PHONY: all build clean distclean

TOPDIR=../
include $(TOPDIR)Rules.mak

ifneq ($(KERNEL_HEADERS),)
LINUXDIR = $(KERNEL_HEADERS)
endif 
CURDIR  = $(shell pwd)
SRCDIR=$(CURDIR)/libnfnetlink-1.0.1
KERNEL_INCLUDE_DIR=$(ROOTDIR)/$(LINUXDIR)/include
#export CFLAGS = -Dlinux -D__linux__ -Dunix  -D_LINUX_2_6_ -I. -I$(TOOLCHAINS_SYSROOT)/include -I$(KERNEL_INCLUDE_DIR) -I$(TOOLCHAINS_SYSROOT)/../lib/gcc-lib/mips-linux-uclibc/3.3.5/include

all:configure build
configure:
	if [ ! -d "$(SRCDIR)" ]; then \
		tar -jxvf libnfnetlink-1.0.1.tar.bz2; \
	fi
	cd $(SRCDIR); \
	find | xargs touch; \
	export CC="/opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc" \
	AR="/opt/buildroot-gcc463/usr/bin/mipsel-linux-ar" \
	LD="/opt/buildroot-gcc463/usr/bin/mipsel-linux-ld" \
	STRIP="/opt/buildroot-gcc463/usr/bin/mipsel-linux-strip" \
	RANLIB="/opt/buildroot-gcc463/usr/bin/mipsel-linux-ranlib" ;\
	./configure --prefix=$(SRCDIR)/build --host=mipsel-linux --disable-shared;
	touch configure
build:
	make -C $(SRCDIR) all install
	cp -rf $(SRCDIR)/src/.libs/libnfnetlink.a $(TOPDIR)lib

clean:
	cd $(CURDIR); \
	rm -rf configure libnfnetlink-1.0.1
	
distclean:	
	cd $(CURDIR); \
	rm -rf configure libnfnetlink-1.0.1
	
shared: all
	
