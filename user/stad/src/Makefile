EXEC = stad
SRCS = stad.c

CFLAGS += -I$(ROOTDIR)/user/apps/rcm
LDFLAGS += -L$(ROOTDIR)/user/apps/rcm -L$(ROOTDIR)/user/openssl-1.0.2j/
LDFLAGS += -lnvram -lrcm -lssl -lcrypto

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $@.c $(LDFLAGS)

clean:
	-rm -f $(EXEC) *.o

romfs:
	$(ROMFSINST) /sbin/$(EXEC)

