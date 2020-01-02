.PHONY: all build clean 

TARGET = nl_server

CFLAGS += -Wall

all: build
build:
	$(CC) -o $(TARGET) $(CFLAGS) nbns_llmnr_server.c

clean:
	rm -f *.o $(TARGET)

romfs:
	$(ROMFSINST) /usr/bin/$(TARGET)
