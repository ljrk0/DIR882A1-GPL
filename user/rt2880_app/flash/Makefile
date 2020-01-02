ifeq ($(CONFIG_FLASH_TOOL), y)
EXEC += flash
endif
ifeq ($(CONFIG_ETHMAC_TOOL), y)
EXEC += eth_mac
ifeq ($(CONFIG_ARCH_MT7623), y)
EXEC += bt_mac
endif
endif

all: $(EXEC) 

romfs:
	$(ROMFSINST) -e CONFIG_FLASH_TOOL /bin/flash
	$(ROMFSINST) -e CONFIG_ETHMAC_TOOL /bin/eth_mac
	$(ROMFSINST) -e CONFIG_ARCH_MT7623 /bin/bt_mac


