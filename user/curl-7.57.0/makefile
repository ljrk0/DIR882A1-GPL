CONFOPTS= --host=mipsel-linux


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
	cp lib/.libs/libcurl.so* $(ROOTDIR)/romfs/lib/
	$(ROMFSINST) src/.libs/curl /sbin/curl
