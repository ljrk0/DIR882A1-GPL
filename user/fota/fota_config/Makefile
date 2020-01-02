
EXEC := fota_config

ifeq ("$(CONFIG_VENDOR)", "Mediatek")
	OBJS := mtk/fota_generate_config.o
endif

ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-882")
	CFLAGS += -DPRODUCT_DIR882
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-878")
	CFLAGS += -DPRODUCT_DIR878
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-867")
	CFLAGS += -DPRODUCT_DIR867
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
	CFLAGS += -DPRODUCT_DIR853_A2
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
	CFLAGS += -DPRODUCT_DIR853_A1
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1360")
	CFLAGS += -DPRODUCT_DIR1360
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1935")
	CFLAGS += -DPRODUCT_DIR1935
endif

CFLAGS += -I$(ROOTDIR)/lib/libnvram
CFLAGS += -I../include
CFLAGS += -Wall

LDFLAGS += -lnvram
LDFLAGS += -L../common/ -lfota_common

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
