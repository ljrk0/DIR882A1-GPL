
EXEC := libfota_common.so libfota_api.so
COMMON_OBJS := fota_common.o fota_crypto.o
API_OBJS := fota_api.o fota_api_curl.o fota_api_json.o

CFLAGS += -I$(ROOTDIR)/lib/json-c/json-c-0.11/
CFLAGS += -I$(ROOTDIR)/user/curl-7.57.0/curl-curl-7_57_0/include/
CFLAGS += -fPIC
CFLAGS += -I../include
CFLAGS += -Wall

API_LDFLAGS += $(LDFLAGS)
API_LDFLAGS += -ljson-c
API_LDFLAGS += -lz
API_LDFLAGS += -L$(ROOTDIR)/user/openssl-1.0.2j/ -lcrypto -lssl
API_LDFLAGS += -L$(ROOTDIR)/user/curl-7.57.0/curl-curl-7_57_0/lib/.libs/ -lcurl

.PHONY: all
all: $(EXEC)
	

libfota_common.so: $(COMMON_OBJS)
	$(LD) -shared -o $@ $^ $(LDFLAGS)

libfota_api.so: $(API_OBJS)
	$(LD) -shared -o $@ $^ $(API_LDFLAGS)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@.$$$$ 2>/dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(COMMON_OBJS:.o=.d)
sinclude $(API_OBJS:.o=.d)

.PHONY: clean
clean:
	-rm -f $(EXEC) $(COMMON_OBJS) $(API_OBJS) *.o *.d

.PHONY: romfs
romfs:
	$(ROMFSINST) libfota_common.so /lib/libfota_common.so
	$(ROMFSINST) libfota_api.so /lib/libfota_api.so
