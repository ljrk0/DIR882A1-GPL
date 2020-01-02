#
# protest Makefile
#
# by T&W sudh
#
CFLAGS += -DLINUX26
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS +=  -fPIC
LDFLAGS += -L$(ROOTDIR)/user/apps/rcm -L$(ROOTDIR)/user/openssl-1.0.2j/
LDFLAGS += -lnvram -lrcm -lssl -lcrypto


OBJS := protest.o

all: preclean protest

preclean:
	$(shell touch *.c *.h 2> /dev/null)
	
clean:
	rm -f *.o *~ protest

romfs:
	$(ROMFSINST) /bin/protest

protest: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
