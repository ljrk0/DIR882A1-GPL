CROSS_TOOL=mipsel-buildroot-linux-uclibc
CC=/opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc

.PHONY: all build clean distclean

all: configure build

configure:
	cd source; \
	./configure CC=$(CC) CFLAGS='-I$(ROOTDIR)/user/openssl-1.0.2j/include' LDFLAGS='-L$(ROOTDIR)/user/openssl-1.0.2j' LIBS='-lcrypto -lssl' \
		--target=$(CROSS_TOOL) \
		--host=$(CROSS_TOOL) \
		--with-ssl=openssl
build:
	cd source; \
	make

romfs:
	$(ROMFSINST) -p 775 source/src/wget /usr/bin/wgets

clean:
	$(MAKE) -C source clean
