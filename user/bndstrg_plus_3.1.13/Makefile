ifeq ($(CONFIG_SUPPORT_OPENWRT), y)
EXE:=bndstrg2

#ROAMING SUPPORT adds "BS Pre-Steering" bypass to stations already connected in the ESS.
#ESS participant APs send periodic(default 10sec) broadcast to sync list of stations
#in ESS. A station entry is timed out if no update received from any ESS AP for
#RoamingEntryTimeout(default is 120sec)
#use BS conf file to configure: EthIface(default "br0"), EthPort(default 2107),
#IappSendTimer(default 10sec) and RoamingEntryTimeout(default 120sec)
#CFLAGS += -DROAMING_SUPPORT

#WPS Support adds support for connecting wps station while bandsteering on
CFLAGS += -DWPS_SUPPORT
#CFLAGS += -DVENDOR_FEATURE7_SUPPORT
#CFLAGS += -DVENDOR_FEATURE8_SUPPORT

EXTRA_CFLAGS += -O2 -Wall -g -Werror -lrt $(CFLAGS)

LDFLAGS += -lpthread

OBJS =	driver_wext.o eloop.o os_internal.o main.o util.o debug.o netlink.o bndstrg.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

clean:
	-@rm -f *~ *.o $(EXE) *.d

$(OBJS): %.o : %.c
	$(CC) $(EXTRA_CFLAGS) -c $< -o $@

else #CONFIG_SUPPORT_OPENWRT

NVDIR = $(ROOTDIR)/lib/libnvram
LIBNVRAM = $(ROOTDIR)/lib/lib/libnvram-0.9.28.so
LDFLAGS += -lpthread
BNDSTRG_EXEC = bndstrg2
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

CFLAGS += -O2 -Wall -Werror -g -lrt -I$(NVDIR)
CFLAGS += -DVENDOR_IOS_SUPPORT
#CFLAGS += -DBNDSTRG_NVRAM_SUPPORT
#CFLAGS += -DVENDOR_FEATURE7_SUPPORT
#CFLAGS += -DVENDOR_FEATURE8_SUPPORT
#CFLAGS += -DNVRAM_LIB_SUPPORT
#CFLAGS += -DBTM_DEBUG_SUPPORT
#WPS Support adds support for connecting wps station while bandsteering on
CFLAGS += -DWPS_SUPPORT

#ROAMING SUPPORT adds "BS Pre-Steering" bypass to stations already connected in the ESS.
#ESS participant APs send periodic(default 10sec) broadcast to sync list of stations
#in ESS. A station entry is timed out if no update received from any ESS AP for
#RoamingEntryTimeout(default is 120sec)
#use BS conf file to configure: EthIface(default "br0"), EthPort(default 2107),
#IappSendTimer(default 10sec) and RoamingEntryTimeout(default 120sec)
#CFLAGS += -DROAMING_SUPPORT

CC = $(CROSS_COMPILE)gcc

BNDSTRG_OBJS = driver_wext.o eloop.o os_internal.o main.o util.o debug.o netlink.o
#ctrl_iface_unix.o
BNDSTRG_OBJS += bndstrg.o

all: $(BNDSTRG_EXEC)

$(BNDSTRG_EXEC): $(BNDSTRG_OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $(BNDSTRG_EXEC) $(BNDSTRG_OBJS)

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
	$(ROMFSINST) /etc_ro/bndstrg.conf

clean:
	rm -f *.o $(BNDSTRG_EXEC)

endif #CONFIG_SUPPORT_OPENWRT
