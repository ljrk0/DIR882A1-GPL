#SRCDIR=openswan-2.6.38
SRCDIR=openswan-2.6.49

.PHONY: all build clean

all: build


build:
	cd $(SRCDIR); \
	make clean all

clean:
	cd $(SRCDIR); \
	make clean; \
	rm -rf OBJ.linux.mips

romfs:
	cd $(SRCDIR); \
	make romfs
