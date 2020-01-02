CROSS_TOOL=mipsel-buildroot-linux-uclibc
CC = /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
SRCDIR=inadyn-mt.v.02.28.10

ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-882")
CFLAGS += -DPRODUCT_DIR882
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-878") 
CFLAGS += -DPRODUCT_DIR878
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-867") 
CFLAGS += -DPRODUCT_DIR867
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2") 
CFLAGS += -DPRODUCT_DIR853_A2
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1") 
CFLAGS += -DPRODUCT_DIR853_A1
endif

.PHONY: all build clean

all: clean init build

init:
	cd $(SRCDIR); \
	chmod 755 ./configure; \
	./configure CC=$(CC) \
		--build=i686-linux \
		--target=$(CROSS_TOOL) \
		--host=$(CROSS_TOOL) \
		-disable-sound \
build:
	cd $(SRCDIR); \
	make all

clean:
	cd $(SRCDIR); \
	make clean ;\
	rm -rf src/.deps \
	rm -f src/config.h src/Makefile Makefile aclocal.m4 config.log man/Makefile

romfs:
	$(ROMFSINST)   $(SRCDIR)/src/inadyn-mt /bin