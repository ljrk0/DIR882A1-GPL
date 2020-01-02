EXEC = stad
SRCS = stad.c

CFLAGS += -I$(ROOTDIR)/user/apps/rcm -I$(ROOTDIR)/user/apps/include/
LDFLAGS += -L$(ROOTDIR)/user/apps/rcm -L$(ROOTDIR)/user/openssl-1.0.2j/
LDFLAGS += -lnvram -lrcm -lssl -lcrypto

ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-882")
CFLAGS += -DPRODUCT_DIR882
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-878") 
CFLAGS += -DPRODUCT_DIR878
else ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-867") 
CFLAGS += -DPRODUCT_DIR867
endif

ifeq ("$(CONFIG_PRODUCT)", "DIR_2660")
CFLAGS += -DPRODUCT_DIR2660
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
CFLAGS += -DPRODUCT_DIR853_A2
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
CFLAGS += -DPRODUCT_DIR853_A1
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1360")
CFLAGS += -DPRODUCT_DIR1360
endif

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $@.c $(LDFLAGS)

clean:
	-rm -f $(EXEC) *.o

romfs:
	$(ROMFSINST) /sbin/$(EXEC)

