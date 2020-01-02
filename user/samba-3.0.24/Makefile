CROSS_TOOL=mipsel-buildroot-linux-uclibc
CC=/opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
RUNDIR=/var/etc/samba
PROGDIR=$(shell pwd)/source

.PHONY: all build clean distclean

all: configure build

configure:
	cd source; \
	./configure CC=$(CC) \
		--target=$(CROSS_TOOL) \
		--host=$(CROSS_TOOL) \
		--prefix=$(TARGET) \
		--localstatedir=$(RUNDIR) \
		--with-privatedir=$(RUNDIR) \
		--with-lockdir=$(RUNDIR)/locks \
		--with-piddir=$(RUNDIR) \
		--with-configdir=$(RUNDIR) \
		--with-logfilebase=$(RUNDIR) \
		--with-sendfile-support \
		--with-included-popt \
		--with-included-iniparser \
		--without-aio-support \
		--without-automount \
		--without-winbind \
		--without-ldap \
		--without-sys-quotas \
		--without-ads \
		--disable-cups \
		--disable-pie \
		--enable-static=yes \
		--enable-shared=no
	sed -i 's/\/\* #undef HAVE_GETTIMEOFDAY_TZ \*\//#define HAVE_GETTIMEOFDAY_TZ 1/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef USE_SETRESUID \*\//#define USE_SETRESUID 1/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef HAVE_SECURE_MKSTEMP \*\//#define HAVE_SECURE_MKSTEMP 1/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef HAVE_EXPLICIT_LARGEFILE_SUPPORT \*\//#define HAVE_EXPLICIT_LARGEFILE_SUPPORT 1/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef HAVE_IFACE_IFCONF \*\//#define HAVE_IFACE_IFCONF 1/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef HAVE_LONGLONG \*\//#define HAVE_LONGLONG 1/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef HAVE_OFF64_T \*\//#define HAVE_OFF64_T 1/g' $(PROGDIR)/include/config.h
	sed -i 's/#define HAVE_SIGSET 1/\/\* #undef HAVE_SIGSET \*\//g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef _FILE_OFFSET_BITS \*\//#define _FILE_OFFSET_BITS 64/g' $(PROGDIR)/include/config.h
	sed -i 's/\/\* #undef _LARGEFILE64_SOURCE \*\//#define _LARGEFILE64_SOURCE 1/g' $(PROGDIR)/include/config.h
		
build:
	cd source; \
	make

romfs:

#	$(ROMFSINST) source/bin/CP850.so /lib/CP850.so
#	$(ROMFSINST) source/bin/CP437.so /lib/CP437.so
#	$(ROMFSINST) source/bin/libbigballofmud.so /lib/libbigballofmud.so
#	ln -snf libbigballofmud.so $(ROOTDIR)/romfs/lib/libbigballofmud.so.0
#	$(ROMFSINST) source/bin/nmbd /sbin/nmbd
	$(ROMFSINST) source/bin/smbd /sbin/smbd
	$(ROMFSINST) source/bin/smbpasswd /sbin/smbpasswd
#	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL /bin/samba.sh
#	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL /bin/samba_add_dir.sh

clean:
	$(MAKE) -C source clean
