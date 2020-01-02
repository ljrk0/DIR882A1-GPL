#
SHELL:=/bin/bash
#
INSTALL    = install
LN         = ln
RM         = rm -f
#
NAME	= cli
TELNET  = clitelnet
TEST    = test
#memwatch.o 
DEPEND_FILES	= crypt.o main.o
#				  

CFLAG = -O0 -Wall -g 

DESTDIR =
PREFIX = /usr/local

MAJOR = 1
MINOR = 9
REVISION = 3
LIB = libcli.so

#CC = gcc
DEBUG = -g
OPTIM = -O3
CFLAGS += $(DEBUG) $(OPTIM) -Wall -Wformat-security -Wno-format-zero-length
LDFLAGS += -shared -Wl,-soname,$(LIB).$(MAJOR).$(MINOR)
LIBPATH += -L.
LIBS = -lcrypt

all: $(LIB) $(NAME) $(TELNET) $(TEST)

$(LIB): libcli.o
	$(CC) $(CFLAG) -o $(LIB).$(MAJOR).$(MINOR).$(REVISION) $^ $(LDFLAGS) $(LIBS)
	-rm -f $(LIB) $(LIB).$(MAJOR).$(MINOR)
	ln -s $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(LIB).$(MAJOR).$(MINOR)
	ln -s $(LIB).$(MAJOR).$(MINOR) $(LIB)
	
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -o $@ -c $<

libcli.o: libcli.h

romfs:
	$(ROMFSINST) $(NAME) /usr/bin/$(NAME)
	$(INSTALL) -m 644 $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(ROOTDIR)/romfs/lib
	$(LN) -sf $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(ROOTDIR)/romfs/lib/$(LIB).$(MAJOR).$(MINOR) 
	$(LN) -sf $(LIB).$(MAJOR).$(MINOR) $(ROOTDIR)/romfs/lib/$(LIB) 
	
	
test: test.o $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lcli

cli: clitest.o $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lcli

clitelnet: clitelnet.o $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lcli

cli.exe: clitest.c libcli.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< libcli.o -lws2_32

clean:
	rm -f *.o $(LIB)* cli clitest

install: $(LIB)
	install -d $(DESTDIR)$(PREFIX)/include $(DESTDIR)$(PREFIX)/lib
	install -m 0644 libcli.h $(DESTDIR)$(PREFIX)/include
	install -m 0755 $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(DESTDIR)$(PREFIX)/lib
	cd $(DESTDIR)$(PREFIX)/lib && \
	    ln -s $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(LIB).$(MAJOR).$(MINOR) && \
	    ln -s $(LIB).$(MAJOR).$(MINOR) $(LIB)

rpm:
	mkdir libcli-$(MAJOR).$(MINOR).$(REVISION)
	cp -R *.c *.h Makefile Doc README *.spec libcli-$(MAJOR).$(MINOR).$(REVISION)
	tar zcvf libcli-$(MAJOR).$(MINOR).$(REVISION).tar.gz --exclude CVS --exclude *.tar.gz libcli-$(MAJOR).$(MINOR).$(REVISION)
	rm -rf libcli-$(MAJOR).$(MINOR).$(REVISION)
	rpm -ta libcli-$(MAJOR).$(MINOR).$(REVISION).tar.gz --clean
