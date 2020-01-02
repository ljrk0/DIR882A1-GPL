PLT ?= mipsel
EXE = radvd
USRSBINDIR=$(ROOTDIR)/romfs/usr/sbin

INSTALL  = install

ifeq ($(CONFIG_IPV6_CE_ROUTER_TEST_DEBUG), y)
	CFLAGS += -D__CONFIG_IPV6_CE_ROUTER_TEST_DEBUG__
endif

all: 
	[ ! -d radvd-1.8 ] || [ -f radvd-1.8/Makefile ] || ( \
	    cd radvd-1.8 \
	    && touch configure && touch Makefile.in && touch config.h.in \
	    && CC=$(CC) ./configure --host=$(PLT)-linux \
	)
	+$(if $(wildcard radvd-1.8/), \
	    $(MAKE) -C radvd-1.8 CC=${CC} \
	   , \
	    @true \
	  )

romfs:
	$(INSTALL) radvd-1.8/$(EXE) $(USRSBINDIR)
	
	
clean:
	[ ! -f radvd-1.8/Makefile ] || $(MAKE) -C radvd-1.8 distclean	