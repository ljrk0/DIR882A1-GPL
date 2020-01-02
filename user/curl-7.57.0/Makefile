
SRC := curl-curl-7_57_0
PATH := $(CONFIG_CROSS_COMPILER_PATH):$(PATH)
PKG_CONFIG_LIBDIR := $(ROOTDIR)/user/openssl-1.0.2j/
LDFLAGS += -L$(ROOTDIR)/user/openssl-1.0.2j/
CFLAGS += -I$(ROOTDIR)/user/openssl-1.0.2j/include/

all: $(SRC)

	@if [ ! -e $(SRC)/Makefile ] ; then \
		cd $(SRC) \
		&& PATH=$(PATH) autoreconf -vif \
		&& PATH=$(PATH) CC=$(CC) PKG_CONFIG_LIBDIR=$(PKG_CONFIG_LIBDIR) ./configure --host=mipsel-linux --target=mipsel-linux \
		--with-ssl=$(ROOTDIR)/user/openssl-1.0.2j/; \
	fi

	make -C $(SRC)

$(SRC):

	@if [ ! -e $(SRC) ] ; then \
		echo "unzip curl source code package."; \
		unzip -q $(SRC).zip; \
	fi

romfs:
	$(ROMFSINST) $(SRC)/lib/.libs/libcurl.so.4.5.0 /lib/libcurl.so.4.5.0
	$(ROMFSINST) -s libcurl.so.4.5.0 /lib/libcurl.so.4
	$(ROMFSINST) -s libcurl.so.4.5.0 /lib/libcurl.so
	$(ROMFSINST) $(SRC)/src/.libs/curl /bin/curl
