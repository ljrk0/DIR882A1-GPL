
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

all: $(EXEC)
	

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@.$$$$ 2>/dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(OBJS:.o=.d)

clean:
	-rm -f $(EXEC) $(OBJS) *.o *.d
ifeq ("$(CONFIG_VENDOR)", "Mediatek")
	-rm -f mtk/*.d
endif

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
