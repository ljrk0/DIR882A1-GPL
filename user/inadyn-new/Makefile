
SRC := inadyn-2.5
TOOL_CHAIN_DIR := $(CONFIG_CROSS_COMPILER_PATH)
INSTALL_DIR := $(ROMFSDIR)
DIR  = $(shell pwd)
#CFLAGS=
#LDFLAGS=
#export CFLAGS LDFLAGS

all: $(SRC) $(SRC)/Makefile

	make -C $(SRC)
	make -C $(SRC) install

$(SRC):

	@if [ ! -e $(SRC) ]; then \
		echo "Untarring original $(SRC) source"; \
		tar -xf $(SRC).tar; \
		cp -rf append-myip.patch $(SRC); \
	fi

$(SRC)/Makefile:

	cd $(SRC) \
	autoreconf -vif && \
	./configure \
		--prefix=$(DIR)/$(SRC)/install \
		--disable-ssl \
		--host=mipsel-linux \
		PKG_CONFIG_PATH=$(DIR)/../confuse/confuse-3.2.2/install/lib/pkgconfig && \
	patch -p1 < append-myip.patch

clean:

	make -C $(SRC) clean

distclean:

	rm -rf $(SRC)

romfs:

	$(ROMFSINST) $(SRC)/install/sbin/inadyn /sbin/inadyn

