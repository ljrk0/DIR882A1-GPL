#
#	Copyright (C) 2007 Delta Networks Inc.
#

CFLAGS += -I$(ROOTDIR)/user/apps/rcm
LDFLAGS += -L$(ROOTDIR)/user/apps/rcm -L$(ROOTDIR)/user/openssl-1.0.2j/
#-L$(ROOTDIR)/user/WRS/build.WRS/tmpdist/lib -lwrscfg
LDFLAGS += -lnvram -lrcm -lssl -lcrypto

EXEC = dhcpmon
SRCS = dhcpmon.c

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)
	@echo "make $@ finished on `date`"



clean:
	rm -f *.o $(EXEC)
	
romfs:
	$(ROMFSINST) /sbin/$(EXEC)
