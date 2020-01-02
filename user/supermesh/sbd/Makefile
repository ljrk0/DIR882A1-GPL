ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-2660")
CFLAG += -DPRODUCT_DIR2660
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
CFLAG += -DPRODUCT_DIR853_A2
CFLAG += -DWIFI_DBDC_SUPPORT
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
CFLAG += -DPRODUCT_DIR853_A1
CFLAG += -DWIFI_DBDC_SUPPORT
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1360")
CFLAG += -DPRODUCT_DIR1360
CFLAG += -DWIFI_DBDC_SUPPORT
endif

libapsond_vendor_sbapi.so:
	$(CC) $(CFLAGS) -fPIC -shared -o $@ apson_sb_vendor_api.c  -I$(ROOTDIR)/$(LINUXDIR)/include -I$(ROOTDIR)/lib/libnvram -L$(ROOTDIR)/lib/libnvram -lnvram  -I$(ROOTDIR)/user/apps/notify_rc -L$(ROOTDIR)/user/apps/notify_rc -lnotifyrc
