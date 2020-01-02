#
# Samba Makefile
#
# $ Copyright Open Broadcom Corporation 2010 $
#
# $Id: Makefile,v 1.6 2010-07-17 02:57:34 kenlo Exp $
#

LIBZLIB_SRC=omcproxy-master
#PKG_SOURCE:=libubox-2014-08-04.tar.gz

TARGETDIR = /opt/buildroot-gcc463/

CFLAGS += -Os
CFLAGS += -DLINUX
CFLAGS += -fPIC -Wl,-z,relro,-z,now -I$(SRCBASE)/include

TARGET_CFLAGS += -Os  -DLINUX -fPIC
TARGET_CFLAGS += -fno-caller-saves -fstack-protector 
TARGET_CFLAGS += -Wl,-z,relro,-z,now 


CMAKE_C_COMPILER := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc 
CMAKE_C_COMPILER_ARG1 := 
CMAKE_CXX_COMPILER := /opt/buildroot-gcc463/usr/bin/mipsel-linux-g++
CMAKE_CXX_COMPILER_ARG1 := 
CMAKE_INSTALL_PREFIX := $(ROOTDIR)/romfs/usr/

#TOOLCHAIN_DIR := $(TOOLCHAINS_PATH)/../

PATH_PREFIX:=../../cmake/cmake-2.8.12.2/staging_dir/host
#STAGING_DIR :=  $(TOP)/tw-prog.priv/lib/cmake/cmake_prefix
CMAKE_PATH := $(PATH_PREFIX)/bin


.PHONY: all build clean distclean
all: configure build

prebuild:
	if [ -d ../cmake/ ]; then \
		cd ../cmake/; \
		make; \
		cd ../mld/; \
	fi

configure:
	(cd $(LIBZLIB_SRC); \
        $(CMAKE_PATH)/cmake \
                -DCMAKE_SYSTEM_NAME=Linux \
                -DCMAKE_SYSTEM_VERSION=1 \
                -DCMAKE_SYSTEM_PROCESSOR=$(ARCH) \
                -DCMAKE_C_COMPILER="$(CMAKE_C_COMPILER)" \
                -DCMAKE_C_COMPILER_ARG1="$(CMAKE_C_COMPILER_ARG1)" \
                -DCMAKE_INSTALL_PREFIX="$(CMAKE_INSTALL_PREFIX)" \
	) \

build:
	+$(MAKE) -C $(LIBZLIB_SRC) all install 

clean:
	+$(MAKE) -C $(LIBZLIB_SRC) clean;
#	rm $(LIBZLIB_SRC) -rf;
#	cd $(LIBZLIB_SRC); \
#	make clean;

distclean:
	rm $(LIBZLIB_SRC) -rf;

romfs:install

install:
#	+$(MAKE) -C $(LIBZLIB_SRC) all install 
#	install -d $(INSTALLDIR)/usr/lib
#	echo "------------ install -d $(INSTALLDIR)/usr/lib ------------------"
#	$(STRIP) $(SRCBASE)/router/trm_lib/lib/libpcap.so.1.3.0
#	cp -Pf $(SRCBASE)/router/trm_lib/lib/libpcap.so* $(INSTALLDIR)/usr/lib/

#export  TERM=vt100
#export  TERMINFO=/usr/tut/terminfo
