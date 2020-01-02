.PHONY: all build clean distclean 
SRCDIR=$(ROOTDIR)/user/ppp-pppoe/ppp-2.4.5_bcm
INSTALLDIR=$(ROOTDIR)/romfs

EXTRA_CFLAGS=-idirafter $(KERNEL_DIR)/include
export EXTRA_CFLAGS

all:configure build
configure:
	cd $(SRCDIR);\
	./configure; 
	touch configure;
build:
	make -C $(SRCDIR) MFLAGS='$(if $(CONFIG_IPV6),HAVE_INET6=y,)' all 

clean:
	cd $(SRCDIR); \
	make clean
distclean:	
	cd $(SRCDIR); \
	make dist-clean; 
	rm -f configure

romfs:install
	
install:
	mkdir -p  $(INSTALLDIR)/usr/sbin
	cp -f $(SRCDIR)/chat/chat $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/chat
	cp -f $(SRCDIR)/pppd/pppd $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/pppd
	ln -sf ./pppd $(INSTALLDIR)/usr/sbin/pppd_v6
	cp -f $(SRCDIR)/pppd/plugins/rp-pppoe/pppoe-discovery $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/pppoe-discovery
	mkdir -p $(INSTALLDIR)/usr/lib/pppd/2.4.5
	cp -f $(SRCDIR)/pppd/plugins/rp-pppoe/rp-pppoe.so $(INSTALLDIR)/usr/lib/pppd/2.4.5
	$(STRIP) $(INSTALLDIR)/usr/lib/pppd/2.4.5/rp-pppoe.so
	cp -f $(SRCDIR)/pppd/plugins/pppol2tp/pppol2tp.so $(INSTALLDIR)/usr/lib/pppd/2.4.5
	$(STRIP) $(INSTALLDIR)/usr/lib/pppd/2.4.5/pppol2tp.so	
	cp -f $(SRCDIR)/pppd/plugins/pppol2tp/openl2tp.so $(INSTALLDIR)/usr/lib/pppd/2.4.5
	$(STRIP) $(INSTALLDIR)/usr/lib/pppd/2.4.5/openl2tp.so	
	#cp -f $(SRCDIR)/pppd/plugins/pppoatm/pppoatm.so $(INSTALLDIR)/usr/lib/pppd/2.4.5
	#$(STRIP) $(INSTALLDIR)/usr/lib/pppd/2.4.5/pppoatm.so
