all: $(EXEC)

romfs:
ifeq ($(CONFIG_DEFAULTS_MEDIATEK_MT7621), y)
ifeq ($(CONFIG_RALINKAPP_HWQOS), y)
	ln -sf qos_run_hw_mt7621 qos_run
else
ifeq ($(CONFIG_RALINKAPP_SWQOS), y)
	ln -sf qos_run_sw_mt7621 qos_run
endif
	$(ROMFSINST) /bin/qos_run
endif
endif