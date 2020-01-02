
SRC := confuse-3.2.2
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
		tar -xf $(SRC).tar.gz; \
	fi

$(SRC)/Makefile:

	cd $(SRC) \
	autoreconf -vif && \
	./configure \
		--prefix=$(DIR)/$(SRC)/install \
		--host=mipsel-linux

clean:

	make -C $(SRC) clean


distclean:

	rm -rf $(SRC)

romfs:

	$(ROMFSINST) $(SRC)/install/lib/libconfuse.so.2.0.0 /lib/libconfuse.so.2.0.0
	$(ROMFSINST) -s libconfuse.so.2.0.0 /lib/libconfuse.so.2
	$(ROMFSINST) -s libconfuse.so.2.0.0 /lib/libconfuse.so

