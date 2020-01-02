all: $(EXEC)

romfs:

ifeq ($(CONFIG_DEFAULTS_MEDIATEK_MT7623), y)
	ln -sf auto_wan_pt_mt7623 auto_wan_pt
endif
	$(ROMFSINST) /bin/auto_wan_pt

