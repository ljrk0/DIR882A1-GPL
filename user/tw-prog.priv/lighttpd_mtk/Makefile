#
# Samba Makefile
#
# $ Copyright Open Broadcom Corporation 2010 $
#
# $Id: Makefile,v 1.6 2010-07-17 02:57:34 kenlo Exp $
#

LIBZLIB_SRC=lighttpd-1.4.20
#PKG_SOURCE=lighttpd-1.4.20.tar
CP:=cp -fpR

.PHONY: all build clean distclean
all: build

build:
	$(MAKE) -C $(LIBZLIB_SRC) DESTDIR=$(TARGETDIR) all

clean:
	cd $(LIBZLIB_SRC); \
	make clean;

distclean:
	cd $(LIBZLIB_SRC); \
	make clean;

install: 
	$(INSTALL_DIR) $(TARGETDIR)/etc/lighttpd
	$(INSTALL_DIR) $(TARGETDIR)/usr/lib/lighttpd/
	$(CP) conf/lighttpd.conf.mtk $(TARGETDIR)/etc/lighttpd/ ; 
	$(MAKE) -C $(LIBZLIB_SRC) DESTDIR=$(TARGETDIR) install
	for m in dirlisting indexfile staticfile; do \
		$(CP) $(LIBZLIB_SRC)/src/.libs/mod_$${m}.so $(TARGETDIR)/usr/lib/lighttpd/ ; \
		echo "$(CP) $(LIBZLIB_SRC)/src/.libs/mod_$${m}.so $(TARGETDIR)/usr/lib/lighttpd/ ;" ; \
	done
	$(CP) $(LIBZLIB_SRC)/src/.libs/*.so $(TARGETDIR)/usr/lib/lighttpd/ ; 

romfs:
	make -C lighttpd-1.4.20 romfs
	$(ROMFSINST) conf/lighttpd.conf.mtk /etc_ro/lighttpd/lighttpd.conf

