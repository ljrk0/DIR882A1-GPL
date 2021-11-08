#SRCDIR=dnsmasq-2.40
#SRCDIR=dnsmasq-2.76
#SRCDIR=dnsmasq-2.78
SRCDIR=dnsmasq-2.79

.PHONY: all build clean

all: build


build:
	cd $(SRCDIR); \
	make all

clean:
	cd $(SRCDIR); \
	make clean

romfs:
	cd $(SRCDIR); \
	make romfs

