
TOPDIR=../
include $(TOPDIR)Rules.mak
SEP=echo "\033[41;1m   $@   \033[0m"
CC = /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
TARGET := mipsel-linux
LIBGPGERRODIR=$(ROOTDIR)/lib/libgpg-error
GNU_TARGET_NAME := $(TARGET)
GNU_HOST_NAME := $(shell gcc -dumpmachine)

.PHONY:	all clean shared 

all: libgcrypt-1.5.1

shared: 
	install -D libgcrypt-1.5.1/src/.libs/libgcrypt.so.11.8.0 $(ROOTDIR)/lib/lib/libgcrypt.so.11.8.0
	$(STRIP) $(ROOTDIR)/lib/lib/libgcrypt.so.11.8.0
	cd $(ROOTDIR)/lib/lib && ln -sf libgcrypt.so.11.8.0 libgcrypt.so.11

clean: libgcrypt-1.5.1-clean

libgcrypt-1.5.1/stamp-h1: 
	cd libgcrypt-1.5.1 && \
	./configure mipsel-linux LD=$(CROSS_COMPILE)ld --build=$(GNU_HOST_NAME) --host=$(GNU_TARGET_NAME) --with-gpg-error-prefix=$(LIBGPGERRODIR)/libgpg-error-1.10/src \
	LDFLAGS="-L$(LIBGPGERRODIR)/libgpg-error-1.10/src/.libs"
	touch libgcrypt-1.5.1/stamp-h1

libgcrypt-1.5.1: libgcrypt-1.5.1/stamp-h1
	@$(SEP)
	@$(MAKE) -C libgcrypt-1.5.1	
	
libgcrypt-1.5.1-clean:
	-@$(MAKE) -C libgcrypt-1.5.1 clean
	@rm -f libgcrypt-1.5.1/stamp-h1	