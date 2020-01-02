
TOPDIR=../
include $(TOPDIR)Rules.mak
#SEP=echo "\033[41;1m   $@   \033[0m"
CC = /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
TARGET := mipsel-linux
GNU_TARGET_NAME := $(TARGET)
GNU_HOST_NAME := $(shell gcc -dumpmachine)
CURDIR := $(shell pwd)

.PHONY:	all clean shared 

all: db-4.8.30
	
clean: db-4.8.30-clean

shared: 
	install -D db-4.8.30/build_unix/.libs/libdb-4.8.so $(ROOTDIR)/lib/lib/libdb-4.8.so
	$(STRIP) $(ROOTDIR)/lib/lib/libdb-4.8.so
	cd $(ROOTDIR)/lib/lib && ln -sf libdb-4.8.so libdb.so
	cd $(ROOTDIR)/lib/lib && ln -sf libdb-4.8.so libdb-4.so

db-4.8.30/build_unix/stamp-h1:
	cd db-4.8.30/build_unix && \
	../dist/configure CC=$(CC) LD=$(CROSS_COMPILE)ld --build=$(GNU_HOST_NAME) --host=$(GNU_TARGET_NAME) \
	CFLAGS="-Os" \
	--disable-cryptography \
	--disable-hash \
	--disable-queue \
	--disable-replication \
	--disable-statistics \
	--disable-verify \
	--disable-compat185 \
	--disable-diagnostic \
	--disable-dump185 \
	--disable-java \
	--disable-mingw \
	--disable-o_direct \
	--enable-posixmutexes \
	--disable-smallbuild \
	--disable-tcl \
	--disable-test \
	--disable-uimutexes \
	--enable-umrw \
	@touch db-4.8.30/build_unix/stamp-h1
	
db-4.8.30: db-4.8.30/build_unix/stamp-h1
#	@$(SEP)
	@$(MAKE) -C db-4.8.30/build_unix
#	@$(MAKE) -C db-4.8.30/build_unix install
#	make -C db-4.8.30/build_unix

db-4.8.30-clean:
	-@$(MAKE) -C db-4.8.30 clean
	-@$(MAKE) -C db-4.8.30/build_unix clean
	@rm -f db-4.8.30/build_unix/stamp-h1	
	
	