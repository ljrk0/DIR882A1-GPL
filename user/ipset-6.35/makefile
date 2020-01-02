CONFOPTS= --host=mipsel-linux LDFLAGS=-L$(ROOTDIR)/lib/libmnl/libmnl-1.0.4/src/.libs/ CFLAGS=-I$(ROOTDIR)/lib/libmnl/libmnl-1.0.4/include libmnl_LIBS="$(ROOTDIR)/lib/libmnl/libmnl-1.0.4/src/.libs/ -static -lmnl" libmnl_CFLAGS=$(ROOTDIR)/lib/libmnl/libmnl-1.0.4/ --with-kbuild=$(ROOTDIR)/$(LINUXDIR) --with-ksource=$(ROOTDIR)/$(LINUXDIR)


all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)

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
	$(ROMFSINST) src/ipset /sbin/ipset
