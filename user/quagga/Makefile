
SRC := quagga-1.1.1
TOOL_CHAIN_DIR := $(CONFIG_CROSS_COMPILER_PATH)
INSTALL_DIR := $(ROMFSDIR)

all: $(SRC) $(SRC)/Makefile

	make -C $(SRC)

$(SRC):

	@if [ ! -e $(SRC) ]; then \
		echo "Untarring original $(SRC) source"; \
		tar -xf $(SRC).tar.gz; \
	fi

$(SRC)/Makefile:

	cd $(SRC) \
 \
	PATH=$(TOOL_CHAIN_DIR):$(PATH) \
	autoreconf -vif && \
	./configure \
		--host=mipsel-buildroot-linux-gnu \
		--disable-vtysh \
		--disable-doc \
		--disable-bgpd \
		--disable-ripngd \
		--disable-ospfd \
		--disable-ospf6d \
		--disable-watchquagga \
		--disable-isisd \
		--disable-pimd \
		--disable-bgp-announce \
		--disable-ospfapi \
		--disable-ospfclient \
		--disable-rtadv

clean:

	make -C $(SRC) clean

	rm -f $(INSTALL_DIR)/lib/libzebra.so.1
	rm -f $(INSTALL_DIR)/lib/libzebra.so.1.0.0
	rm -f $(INSTALL_DIR)/sbin/zebra
	rm -f $(INSTALL_DIR)/sbin/ripd

distclean:

	rm -rf $(SRC)

romfs:

	$(ROMFSINST) $(SRC)/lib/.libs/libzebra.so.1.0.0 /lib/libzebra.so.1.0.0
	$(ROMFSINST) -s libzebra.so.1.0.0 /lib/libzebra.so.1

	$(ROMFSINST) $(SRC)/zebra/.libs/zebra /sbin/zebra

	$(ROMFSINST) $(SRC)/ripd/.libs/ripd /sbin/ripd

