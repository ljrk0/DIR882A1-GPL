
EXEC := fota_output

ifeq ("$(CONFIG_VENDOR)", "Mediatek")
	OBJS := mtk/fota_output.o
endif

CFLAGS += -I$(ROOTDIR)/lib/libnvram
CFLAGS += -I$(ROOTDIR)/user/apps/notify_rc
CFLAGS += -I../include
CFLAGS += -Wall

LDFLAGS += -lnvram
LDFLAGS += -lnotifyrc
LDFLAGS += -L../common/ -lfota_common

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
