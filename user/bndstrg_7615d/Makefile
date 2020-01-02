NVDIR = $(ROOTDIR)/lib/libnvram
LIBNVRAM = $(ROOTDIR)/lib/lib/libnvram-0.9.28.so
LDFLAGS += -lpthread -lnvram
BNDSTRG_EXEC = bndstrg
CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
UCONF_H	= $(ROOTDIR)/config/autoconf.h
#PLATFORM = x86
PLATFORM = mipsel

ifeq ($(PLATFORM),x86)
CROSS_COMPILE=""
endif

ifeq ($(PLATFORM),mipsel)
CROSS_COMPILE = mipsel-linux-
endif

CFLAGS += -O2 -Wall -g -lrt -I$(NVDIR) -DNVRAM_LIB_SUPPORT
ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
CFLAGS += -DPRODUCT_DIR853_A2
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
CFLAGS += -DPRODUCT_DIR853_A1
endif
CC = $(CROSS_COMPILE)gcc

BNDSTRG_OBJS = driver_wext.o eloop.o os_internal.o main.o util.o debug.o netlink.o
#ctrl_iface_unix.o
BNDSTRG_OBJS += bndstrg.o

all: $(BNDSTRG_EXEC)

$(BNDSTRG_EXEC): $(BNDSTRG_OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) $(LIBNVRAM) -o $(BNDSTRG_EXEC) $(BNDSTRG_OBJS)

bndstrg.o: bndstrg.c $(CONF_H) $(UCONF_H) 
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c bndstrg.c
romfs_dir = $(ROOTDIR)/romfs

romfs_dirs = etc_ro/wifi
   
romfs:
	[ -d $(romfs_dir)/$$i ] || mkdir -p $(romfs_dir)
	for i in $(romfs_dirs); do \
		[ -d $(romfs_dir)/$$i ] || mkdir -p $(romfs_dir)/$$i; \
	done 
	$(ROMFSINST) /bin/$(BNDSTRG_EXEC)

clean:
	rm -f *.o $(BNDSTRG_EXEC)
