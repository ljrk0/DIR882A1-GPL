#
# Samba Makefile
#
# $ Copyright Open Broadcom Corporation 2010 $
#
# $Id: Makefile,v 1.6 2010-07-17 02:57:34 kenlo Exp $
#
# include ../../.config

LIBZLIB_SRC=minidlna-1.0.24
#PKG_SOURCE:=cmake-2.8.12.2.tar.gz
#CFLAGS += -Os
#CFLAGS += -DLINUX
#CFLAGS += -fPIC

CFLAGS += -I./ -I../ \
		  -I$(ROOTDIR)/lib/libffmpeg-0.5 \
		  -I$(ROOTDIR)/lib/libffmpeg-0.5/libavutil	\
		  -I$(ROOTDIR)/lib/libffmpeg-0.5/libavcodec \
		  -I$(ROOTDIR)/lib/libffmpeg-0.5/libavformat \
	 	  -I$(ROOTDIR)/lib/libid3tag-0.15.1b \
	 	  -I$(ROOTDIR)/lib/sqlite-3.7.2-src \
		  -I$(ROOTDIR)/lib/libexif \
	 	  -I$(ROOTDIR)/lib/libjpeg \
	 	  -I$(ROOTDIR)/lib/libogg/include \
	 	  -I$(ROOTDIR)/lib/libvorbis/include \
	 	  -I$(ROOTDIR)/lib/libflac/include
	 	  
LDFLAGS += -static
LIBS = -lpthread -lexif -ljpeg -lsqlite3 -lavformat -lavcodec -lavutil -lid3tag -lFLAC -lvorbis -logg -lm -lz -ldl

CROSS_TOOL=mipsel-buildroot-linux-uclibc
CC=/opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc

#DIRS := ${LIBZLIB_SRC}

#PATH_PREFIX:=./staging_dir/host
#STAGING_DIR :=  $(TOP)/tw-prog.priv/lib/cmake/cmake_prefix
#CMAKE_PATH := $(PATH_PREFIX)/bin

.PHONY: all prepare build clean 
all: build

#prebuild:
#	if [ ! -d $(LIBZLIB_SRC) ]; then\
#	  tar -xvf $(PKG_SOURCE); \
#	  cp config.sub $(LIBZLIB_SRC); \
#	  cp config.guess $(LIBZLIB_SRC); \
#	fi
#	
#	if [ ! -d $(LIBZLIB_SRC) ]; then\
#	  echo "tar \"$(PKG_SOURCE)\" not exists! "; \
#	  exit 1;  \
#	fi;
	
prepare:
	cd $(LIBZLIB_SRC); \
	./configure \
	CC=$(CC) \
	--host=$(CROSS_TOOL) \
	LIBS="-lpthread -lexif -ljpeg -lsqlite3 -lavformat -lavcodec -lavutil -lid3tag -lFLAC -lvorbis -logg -lm -lz -ldl"

build:
	+$(MAKE) -C $(LIBZLIB_SRC) all 

clean:
	echo "---------null done for cmake clean,please make cmake_distclean;---------"
	+$(MAKE) -C $(LIBZLIB_SRC) clean
	
romfs:
#	$(ROMFSINST) $(LIBZLIB_SRC)/minidlnad /sbin/minidlnad
	+$(MAKE) -C $(LIBZLIB_SRC) romfs


#distclean:
#	rm $(LIBZLIB_SRC) -rf;

install:
#	install -d $(INSTALLDIR)/usr/lib
#	+$(MAKE) -C $(LIBZLIB_SRC) install 
#	echo "------------ install -d $(INSTALLDIR)/usr/lib ------------------"

