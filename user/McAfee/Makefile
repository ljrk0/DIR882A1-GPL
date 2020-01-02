NVDIR = $(ROOTDIR)/lib/libnvram
#CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/lib/libevent-2.0.22-stable/include -I$(ROOTDIR)/user/openssl-1.0.2j/include
CFLAGS += -I$(NVDIR)
LDFLAGS += -L$(NVDIR) -lnvram
#LDEVENT = -L$(ROOTDIR)/user/openssl-1.0.2j/ -L$(ROOTDIR)/lib/libevent-2.0.22-stable/.libs -levent_core -lpthread
#CFLAGS +=  -fPIC
#SHARE_LIB = libmdb.so
#EXE = mcAfee
#OBJS = mcAfee.o


all: mngcli 

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
mngcli: mngcli.o
	$(CC) -o $@ mngcli.o $(LDFLAGS)
%.o: .%.depend
.%.depend: %.c
	$(CC) $(CFLAGS) -M $< > $@
	
-include $(OBJS:%.o=.%.depend)
romfs:
	$(ROMFSINST) /bin/mngcli
	mkdir -p $(ROOTDIR)/romfs/lib/McAfee
	mkdir -p $(ROOTDIR)/romfs/etc/McAfee
	mkdir -p $(ROOTDIR)/romfs/etc/McAfee/shqw
	
clean:
	rm -f *.o mngcli .*.depend *.so
