#
# pc monitor Makefile
#
#

CFLAGS += -DLINUX26
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS +=  -fPIC
LDFLAGS += -lnvram




all:pc_daemon


pc_daemon:pc_daemon.c 
	$(CC) -o $@ $^ $(CFLAGS)

romfs: 

clean:
	rm -f *~ pc_daemon

	

