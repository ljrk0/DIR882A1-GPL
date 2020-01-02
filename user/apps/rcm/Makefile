#
# tw rc Makefile
# by T&W sudh
# 2013-3-19 
#

CFLAGS += -DLINUX26 -D__CONFIG_IPV6__ -DSUPPORT_IPV6_AUTO_DETECTION 
ifeq ($(CONFIG_USER_STUPID_FTPD), y)
CFLAGS += -D__CONFIG_FTP__
endif 
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include -I$(ROOTDIR)/config -I$(ROOTDIR)/user/apps/rcm -I$(ROOTDIR)/user/openssl-1.0.2j/include
CFLAGS +=  -fPIC
LDFLAGS += -lnvram -L$(ROOTDIR)/user/openssl-1.0.2j/ -lssl -lcrypto

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

OBJS += ../ifaddrs.o
OBJS += ../misc.o
OBJS += ../common.o
OBJS += ../shutils.o
OBJS += ../files.o
OBJS += ../process.o
OBJS += public/public.o
OBJS += antiattack/antiattack.o
OBJS += dns/dns.o
OBJS += sntp/sntp.o
OBJS += lan/lan.o
OBJS += devinfo/devinfo.o
OBJS += acl/acl.o
OBJS += alg/alg.o
OBJS += sroute/sroute.o
OBJS += ftpd/base64.o
OBJS += ftpd/ftpd.o
OBJS += urlfilter/urlfilter.o
OBJS += services/services.o
OBJS += trigger/trigger.o
OBJS += parentcontrol/parentcontrol.o
OBJS += genpasswd/des.o
OBJS += genpasswd/md5.o
OBJS += genpasswd/crypt.o
OBJS += miniupnpd/miniupnpd.o
OBJS += usbwan/at_cmd.o
OBJS += usbwan/usbwan.o
OBJS += dmz/dmz.o
OBJS += portforward/portforward.o
OBJS += virtualserver/virtualserver.o
OBJS += remotemange/remotemange.o
OBJS += ipfilter/ipfilter.o
OBJS += macfilter/macfilter.o
OBJS += firewallfilter/firewallfilter.o
OBJS += qos/qos.o

all: librcm.so

librcm.so: $(OBJS)
	$(LD) -shared -o $@ $^
#	cp -rf librcm.so $(ROOTDIR)/lib/lib
	
%.o: .%.depend
.%.depend: %.c
	$(CC) $(CFLAGS) -M $< > $@
	
romfs:
	#$(RM) $(ROOTDIR)/lib/lib/librcm.so
	$(ROMFSINST) librcm.so /lib/librcm.so

	
clean: 
	$(Q)rm -f $(OBJS)
	$(Q)rm -f *.o *.so
	$(Q)find ./ -name librcm.so | xargs rm -rf	
	
