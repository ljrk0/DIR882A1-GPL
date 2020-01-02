#
# rssi Makefile
#
# by T&W sudh
#
CFLAGS += -DLINUX26
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS +=  -fPIC
LDFLAGS += -lnvram


OBJS := rssi.o

all: preclean rssi

preclean:
	$(shell touch *.c *.h 2> /dev/null)
	
clean:
	rm -f *.o *~ rssi

romfs:


rssi: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
