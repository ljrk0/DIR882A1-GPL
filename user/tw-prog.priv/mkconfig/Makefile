#
SHELL:=/bin/bash
#

RM = -rm
CFLAG = -O0 -Wall -g
CFLAGS += $(CFLAG)

EXEC = mkconfig
DEPEND_FILES	= mkconfig.o md5.o

all: $(EXEC) 

$(EXEC): $(DEPEND_FILES)
	$(CC) $(CFLAG) -o $@ $(DEPEND_FILES) 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

mkconfig: mkconfig.o md5.o
	$(CC) $(LDFLAGS) -o $@ $^ 
clean:
	-rm -f $(EXEC) *.o

romfs:
	$(ROMFSINST) $(EXEC) /bin/$(EXEC)


