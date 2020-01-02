#
# wl btn monitor Makefile
#
# by T&W sudh
#

CFLAGS += -DLINUX26
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS +=  -fPIC
LDFLAGS += -lnvram


OBJS := btn_main.o btn_gpio.o

all: btn_monitor

clean:
	rm -f *.o *~ btn_monitor

romfs: all


btn_monitor: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
