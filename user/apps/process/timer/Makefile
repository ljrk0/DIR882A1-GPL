#
# timer Makefile
#
# by T&W sudh
#
CFLAGS += -DLINUX26 -D__CONFIG_IPV6__
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS += -fPIC
LDFLAGS	+= -L$(ROOTDIR)/user/apps/rcm -L$(ROOTDIR)/user/apps/notify_rc 
LDFLAGS += -lnvram -lrcm -lnotifyrc -L$(ROOTDIR)/user/openssl-1.0.2j/ -lssl -lcrypto


OBJS := timer.o linux_timer.o

#vpath %.c $(TOP)/shared $(SRCBASE)/shared

all: install
	#cp timer $(ROOTDIR)/romfs/usr/sbin
	#$(STRIP) $(ROOTDIR)/romfs/usr/sbin/timer
	
clean:
	rm -f *.o *~ timer


install:timer 
	install -d $(ROOTDIR)/romfs/usr/sbin
	install timer $(ROOTDIR)/romfs/usr/sbin
	$(STRIP) $(ROOTDIR)/romfs/usr/sbin/timer

timer: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
	
romfs: all

