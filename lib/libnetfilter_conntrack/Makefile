.PHONY: all build clean distclean 
TOPDIR=../
include $(TOPDIR)Rules.mak

ifneq ($(KERNEL_HEADERS),)
LINUXDIR = $(KERNEL_HEADERS)
endif 
CURDIR  = $(shell pwd)

SRCDIR=$(CURDIR)/libnetfilter_conntrack-1.0.6
KERNEL_INCLUDE_DIR=$(ROOTDIR)/$(LINUXDIR)/include
#export CFLAGS = -Dlinux -D__linux__ -Dunix  -D_LINUX_2_6_ -I. -I$(TOOLCHAINS_SYSROOT)/include -I$(KERNEL_INCLUDE_DIR) -I$(TOOLCHAINS_SYSROOT)/../lib/gcc-lib/mips-linux-uclibc/3.3.5/include

all:configure build
configure:
	if [ ! -d "$(SRCDIR)" ]; then \
		tar -jxvf libnetfilter_conntrack-1.0.6.tar.bz2; \
	fi
	cd $(SRCDIR); \
	find | xargs touch; \
	export PKG_CONFIG_PATH=$(CURDIR)/../libnfnetlink/libnfnetlink-1.0.1/build/lib/pkgconfig; \
	LIBNFNETLINK_CFLAGS=-I$(CURDIR)/../libnfnetlink/libnfnetlink-1.0.1/build/include \
	LIBNFNETLINK_LIBS="-L$(CURDIR)/../libnfnetlink/libnfnetlink-1.0.1/build/lib -lnfnetlink" \
	LIBNETFILTER_CONNTRACK_LIBS="-L$(CURDIR)/build/lib -lnetfilter_conntrack" \
	LIBMNL_CFLAGS=-I$(CURDIR)/../libmnl/libmnl-1.0.4/build/include \
	LIBMNL_LIBS="-L$(CURDIR)/../libmnl/libmnl-1.0.4/build/lib -lmnl" \
	CC="/opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc" \
	AR="/opt/buildroot-gcc463/usr/bin/mipsel-linux-ar" \
	LD="/opt/buildroot-gcc463/usr/bin/mipsel-linux-ld" \
	STRIP="/opt/buildroot-gcc463/usr/bin/mipsel-linux-strip" \
	RANLIB="/opt/buildroot-gcc463/usr/bin/mipsel-linux-ranlib" \
	./configure --prefix=$(SRCDIR)/build --host=mipsel-linux --disable-shared;
	touch configure

build:
	make -C $(SRCDIR) all install
	cp -rf $(SRCDIR)/build/lib/libnetfilter_conntrack.a $(TOPDIR)lib
	cd $(SRCDIR)/utils; \
	$(CROSS_COMPILE)gcc -o flush_conntrack conntrack_flush.c -I$(SRCDIR)/build/include -I$(CURDIR)/../libnfnetlink/libnfnetlink-1.0.1/build/include -L$(SRCDIR)/build/lib -lnetfilter_conntrack -L$(CURDIR)/../libnfnetlink/libnfnetlink-1.0.1/build/lib -lnfnetlink;
	$(ROMFSINST) -d $(SRCDIR)/utils/flush_conntrack /sbin/flush_conntrack

clean:
	cd $(CURDIR); \
	rm -rf configure libnetfilter_conntrack-1.0.6
	
distclean:	
	cd $(CURDIR); \
	rm -rf configure libnetfilter_conntrack-1.0.6
	
shared: all
	
