
EXEC := fota
OBJS := fota.o fota_json.o fota_curl.o fota_config.o

ifeq ("$(CONFIG_VENDOR)", "Mediatek")
	OBJS += mtk/fota_platform_dependence.o
endif

ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-882")
	CFLAGS += -DPRODUCT_DIR882
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-878")
	CFLAGS += -DPRODUCT_DIR878
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
	CFLAGS += -DPRODUCT_DIR853_A2
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
	CFLAGS += -DPRODUCT_DIR853_A1
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1360")
	CFLAGS += -DPRODUCT_DIR1360
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1935")
	CFLAGS += -DPRODUCT_DIR1935
endif

ifeq ($(CONFIG_USE_NAND_FLASH), y)
	CFLAGS += -DCONFIG_USE_NAND_FLASH
endif

CFLAGS += -I$(ROOTDIR)/lib/json-c/json-c-0.11/
CFLAGS += -I$(ROOTDIR)/user/curl-7.57.0/curl-curl-7_57_0/include/
CFLAGS += -I$(ROOTDIR)/lib/libnvram
CFLAGS += -I$(ROOTDIR)/user/apps/notify_rc
CFLAGS += -I../include
CFLAGS += -I$(ROOTDIR)/user/apps/include
CFLAGS += -Wall

LDFLAGS += -ljson-c
LDFLAGS += -lz
LDFLAGS += -L$(ROOTDIR)/user/openssl-1.0.2j/ -lcrypto -lssl
LDFLAGS += -L$(ROOTDIR)/user/curl-7.57.0/curl-curl-7_57_0/lib/.libs/ -lcurl
LDFLAGS += -L../common/ -lfota_common -lfota_api
LDFLAGS += -lnotifyrc
LDFLAGS	+= -L$(ROOTDIR)/user/apps/rcm -lrcm
LDFLAGS += -lnvram	

.PHONY: all
all: $(EXEC)
	

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@.$$$$ 2>/dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(OBJS:.o=.d)

.PHONY: clean
clean:
	-rm -f $(EXEC) $(OBJS) *.o *.d
ifeq ("$(CONFIG_VENDOR)", "Mediatek")
	-rm -f mtk/*.d
endif

.PHONY: romfs
romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
