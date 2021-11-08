
EXEC := time_client
OBJS := main.o tz_config.o tz_curl.o tz_json.o

CFLAGS += -I$(ROOTDIR)/lib/json-c/json-c-0.11/
CFLAGS += -I$(ROOTDIR)/user/curl-7.57.0/curl-curl-7_57_0/include/
CFLAGS += -I../fota/include
CFLAGS += -Wall

LDFLAGS += -ljson-c
LDFLAGS += -lz
LDFLAGS += -L$(ROOTDIR)/user/openssl-1.0.2j/ -lcrypto -lssl
LDFLAGS += -L$(ROOTDIR)/user/curl-7.57.0/curl-curl-7_57_0/lib/.libs/ -lcurl
LDFLAGS += -L../fota/common/ -lfota_common -lfota_api


.PHONY: romfs
romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)
