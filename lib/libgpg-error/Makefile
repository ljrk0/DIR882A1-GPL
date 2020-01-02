
TOPDIR=../
include $(TOPDIR)Rules.mak
SEP=echo "\033[41;1m   $@   \033[0m"
CC = /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
TARGET := mipsel-linux
GNU_TARGET_NAME := $(TARGET)
GNU_HOST_NAME := $(shell gcc -dumpmachine)

.PHONY:	all clean shared 

all: libgpg-error-1.10
	
clean: libgpg-error-1.10-clean

shared: 
	install -D libgpg-error-1.10/src/.libs/libgpg-error.so.0.8.0 $(ROOTDIR)/lib/lib/libgpg-error.so.0.8.0
	$(STRIP) $(ROOTDIR)/lib/lib/libgpg-error.so.0.8.0
	cd $(ROOTDIR)/lib/lib && ln -sf libgpg-error.so.0.8.0 libgpg-error.so.0
	
libgpg-error-1.10/stamp-h1:
	cd libgpg-error-1.10 && \
	./configure mipsel-linux LD=$(CROSS_COMPILE)ld --build=$(GNU_HOST_NAME) --host=$(GNU_TARGET_NAME)  
	touch libgpg-error-1.10/stamp-h1

libgpg-error-1.10: libgpg-error-1.10/stamp-h1
	@$(SEP)
	@$(MAKE) -C libgpg-error-1.10

libgpg-error-1.10-clean:
	-@$(MAKE) -C libgpg-error-1.10 clean
	@rm -f libgpg-error-1.10/stamp-h1