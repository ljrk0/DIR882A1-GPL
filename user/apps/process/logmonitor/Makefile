#
# log monitor Makefile
#
#


CFLAGS += -DLINUX26
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS +=  -fPIC
LDFLAGS += -lnvram


OBJS := logmonitor.o

all:logmonitor


logmonitor:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


romfs:   


	  
clean:
	rm -f *~ logmonitor *.o

