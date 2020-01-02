
PWD = $(shell pwd)
SEP=echo "\033[41;1m   $@   \033[0m"
LIBDIR=$(ROOTDIR)/lib
TARGET := mipsel-linux
GNU_TARGET_NAME := $(TARGET)
GNU_HOST_NAME := $(shell gcc -dumpmachine)
OPENSSL_CIPHERS:=
CC = /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
CROSS_COMPILE = /opt/buildroot-gcc463/usr/bin/mipsel-linux-

EXECUTABLES = netatalk-3.1.10 

.PHONY:	all clean romfs

all: netatalk-3.1.10 
	
clean: netatalk-3.1.10-clean

romfs: all netatalk-3.1.10-install	

netatalk-3.1.10/stamp-h1:
#	touch $(TOP)/netatalk-3.1.10/*
	cd netatalk-3.1.10 && touch Makefile.in && touch libevent/Makefile.in && \
	./configure CC=$(CC) LD=$(CROSS_COMPILE)ld --build=$(GNU_HOST_NAME) --host=$(GNU_TARGET_NAME) \
	CPPFLAGS="-I$(LIBDIR)/db/db-4.8.30/build_unix \
	-I$(LIBDIR)/libgcrypt-1.5.1/libgcrypt-1.5.1/src" \
	LDFLAGS="-L$(LIBDIR)/db/db-4.8.30/build_unix/.libs \
	-L$(LIBDIR)/libgcrypt-1.5.1/libgcrypt-1.5.1/src/.libs" \
	LIBS="-L$(LIBDIR)/libgpg-error/libgpg-error-1.10/src/.libs -lgpg-error -L$(LIBDIR)/libgcrypt-1.5.1/libgcrypt-1.5.1/src/.libs -lgcrypt" \
	ac_cv_path_LIBGCRYPT_CONFIG=$(LIBDIR)/libgcrypt-1.5.1/libgcrypt-1.5.1/src/libgcrypt-config \
	--without-kerberos \
	--with-bdb=$(LIBDIR)/db/db-4.8.30/build_unix \
	--with-ssl-dir=$(ROOTDIR)/user/openssl-1.0.2j \
	--with-libgcrypt-dir=$(LIBDIR)/libgcrypt-1.5.1/libgcrypt-1.5.1 \
#	--with-ld-library-path=$(SRCBASE)/toolchains/hndtools-arm-linux-2.6.36-uclibc-4.5.3/lib
	touch $@
	cd netatalk-3.1.10 && echo "#define _LARGE_FILES 1 " >> config.h && echo "#define _LARGEFILE64_SOURCE 1 " >> config.h

netatalk-3.1.10: netatalk-3.1.10/stamp-h1	
	@$(SEP)
	@touch netatalk-3.1.10/configure
	@$(MAKE) -C netatalk-3.1.10

netatalk-3.1.10-install: 
	$(ROMFSINST) netatalk-3.1.10/etc/afpd/.libs/afpd /sbin/afpd
	$(ROMFSINST) netatalk-3.1.10/etc/cnid_dbd/.libs/cnid_dbd /sbin/cnid_dbd
	$(ROMFSINST) netatalk-3.1.10/etc/cnid_dbd/.libs/cnid_metad /sbin/cnid_metad
	$(ROMFSINST) netatalk-3.1.10/bin/ad/.libs/ad /bin/ad
	$(ROMFSINST) netatalk-3.1.10/bin/afppasswd/.libs/afppasswd /bin/afppasswd
	$(ROMFSINST) netatalk-3.1.10/etc/cnid_dbd/.libs/dbd /bin/dbd
#	$(ROMFSINST) netatalk-3.1.10/bin/uniconv/.libs/uniconv /bin/uniconv
	$(ROMFSINST) netatalk-3.1.10/libatalk/.libs/libatalk.so.18.0.0 /lib/libatalk.so.18.0.0
	$(ROMFSINST) netatalk-3.1.10/etc/uams/.libs/uams_randnum.so /lib/uams_randnum.so
	$(ROMFSINST) netatalk-3.1.10/etc/uams/.libs/uams_passwd.so /lib/uams_passwd.so
	$(ROMFSINST) netatalk-3.1.10/etc/uams/.libs/uams_guest.so /lib/uams_guest.so
	$(ROMFSINST) netatalk-3.1.10/etc/uams/.libs/uams_dhx_passwd.so /lib/uams_dhx_passwd.so
	$(ROMFSINST) netatalk-3.1.10/etc/uams/.libs/uams_dhx2_passwd.so /lib/uams_dhx2_passwd.so
	cd $(ROMFSDIR)/lib && ln -sf libatalk.so.18.0.0 libatalk.so.18
	cd $(ROMFSDIR)/lib && ln -sf uams_passwd.so uams_clrtxt.so
	cd $(ROMFSDIR)/lib && ln -sf uams_dhx_passwd.so uams_dhx.so
	cd $(ROMFSDIR)/lib && ln -sf uams_dhx2_passwd.so uams_dhx2.so
	
netatalk-3.1.10-clean:
	-@$(MAKE) -C netatalk-3.1.10 distclean
	@rm -f netatalk-3.1.10/stamp-h1	
	
	
	
