CONFOPTS= --host=mipsel-linux


all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)  --enable-static --disable-shared

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:
	$(ROMFSINST) iptables/xtables-multi /bin/xtables-multi
	$(RM) -rf $(ROOTDIR)/romfs/bin/iptables
	ln -s /bin/xtables-multi $(ROOTDIR)/romfs/bin/iptables
	$(RM) -rf $(ROOTDIR)/romfs/bin/ip6tables
	ln -s /bin/xtables-multi $(ROOTDIR)/romfs/bin/ip6tables
	$(RM) -rf $(ROOTDIR)/romfs/bin/iptables-save
	ln -s /bin/xtables-multi $(ROOTDIR)/romfs/bin/iptables-save
	$(RM) -rf $(ROOTDIR)/romfs/bin/ip6tables-save
	ln -s /bin/xtables-multi $(ROOTDIR)/romfs/bin/ip6tables-save
	$(RM) -rf $(ROOTDIR)/romfs/bin/iptables-restore
	ln -s /bin/xtables-multi $(ROOTDIR)/romfs/bin/iptables-restore
	$(RM) -rf $(ROOTDIR)/romfs/bin/ip6tables-restore
	ln -s /bin/xtables-multi $(ROOTDIR)/romfs/bin/ip6tables-restore
	
	#$(ROMFSINST) libiptc/.libs/libip4tc.so.0 /lib/libip4tc.so.0
	#$(ROMFSINST) libiptc/.libs/libip6tc.so.0 /lib/libip6tc.so.0
	#$(ROMFSINST) libxtables/.libs/libxtables.so.10 /lib/libxtables.so.10
ifeq ("$(CONFIG_RALINKAPP_SWQOS)", "y")
	$(ROMFSINST) -d /etc_ro/l7-protocols
endif
ifeq ("$(CONFIG_RALINKAPP_HWQOS)", "y")
	$(ROMFSINST) -d /etc_ro/l7-protocols
endif
