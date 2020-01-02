#
# tw rc Makefile
# by T&W sudh
# 2013-3-19 
#
CFLAGS += -DLINUX26
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS +=  -fPIC
LDFLAGS += -lnvram

OBJS += notify_rc.o

all: libnotifyrc.so

libnotifyrc.so: $(OBJS)
	$(LD) -shared -o $@ $^
	cp -rf libnotifyrc.so $(ROOTDIR)/lib/lib

%.o: .%.depend
.%.depend: %.c
	$(CC) $(CFLAGS) -M $< > $@

romfs:
	$(ROMFSINST) libnotifyrc.so /lib/libnotifyrc.so
	
clean: 
	$(Q)rm -f $(OBJS)
	$(Q)rm -f *.o *.so

	