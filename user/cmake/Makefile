#
# Samba Makefile
#
# $ Copyright Open Broadcom Corporation 2010 $
#
# $Id: Makefile,v 1.6 2010-07-17 02:57:34 kenlo Exp $
#
include ../../.config

LIBZLIB_SRC=cmake-2.8.12.2
PKG_SOURCE:=cmake-2.8.12.2.tar.gz
CFLAGS += -Os
CFLAGS += -DLINUX
CFLAGS += -fPIC

DIRS := ${LIBZLIB_SRC}

PATH_PREFIX:=./staging_dir/host
#STAGING_DIR :=  $(TOP)/tw-prog.priv/lib/cmake/cmake_prefix
CMAKE_PATH := $(PATH_PREFIX)/bin

.PHONY: all build clean distclean
all: prebuild configure build

prebuild:
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  tar -xvf $(PKG_SOURCE); \
	  cp config.sub $(LIBZLIB_SRC); \
	  cp config.guess $(LIBZLIB_SRC); \
	fi
	
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  echo "tar \"$(PKG_SOURCE)\" not exists! "; \
	  exit 1;  \
	fi;
	
configure:
	if [ ! -x $(LIBZLIB_SRC)/bin/cmake ]; then\
		cd $(LIBZLIB_SRC); \
		CC=gcc CXX=g++ ./configure  --prefix=$(PATH_PREFIX); \
	fi;

build:
	+$(MAKE) -C $(LIBZLIB_SRC) all install 

clean:
	echo "---------null done for cmake clean,please make cmake_distclean;---------"
	+$(MAKE) -C $(LIBZLIB_SRC) clean


distclean:
	rm $(LIBZLIB_SRC) -rf;

install:
#	install -d $(INSTALLDIR)/usr/lib
#	+$(MAKE) -C $(LIBZLIB_SRC) install 
#	echo "------------ install -d $(INSTALLDIR)/usr/lib ------------------"

