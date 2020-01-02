#
SHELL:=/bin/bash
#
INSTALL    = install
LN         = ln
RM         = rm -f
#
NAME	= cmd
TELNET  = clitelnet
TEST    = test
#memwatch.o 
DEPEND_FILES	= cli/cli_cmdtree.o cli/cli_cmdreg.o cli/cli_interpret.o cli/cli_io.o cli/cli_lib.o \
          cli/cli_stub.o app/cli_syscmd.o cli/cli_term.o cli_main.o cli/cli_init.o
#				  

CFLAG = -O0 -Wall -g 

DESTDIR =
PREFIX = /usr/local

MAJOR = 1
MINOR = 9
REVISION = 3

#CC = gcc
DEBUG = -g
OPTIM = -O3
CFLAGS += $(DEBUG) $(OPTIM) -Wall -Wformat-security -Wno-format-zero-length -I./
LDFLAGS += -shared -Wl,-soname,$(LIB).$(MAJOR).$(MINOR)
LIBPATH += -L.
LIBS = -lcrypt

all: $(NAME) $(DEPEND_FILES)

$(LIB): libcli.o
	$(CC) $(CFLAG) -o $(LIB).$(MAJOR).$(MINOR).$(REVISION) $^ $(LDFLAGS) $(LIBS)
	
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -o $@ -c $<

romfs:
	$(ROMFSINST) $(NAME) /usr/bin/$(NAME)
	$(INSTALL) -m 644 $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(ROOTDIR)/romfs/lib
	$(LN) -sf $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(ROOTDIR)/romfs/lib/$(LIB).$(MAJOR).$(MINOR) 
	$(LN) -sf $(LIB).$(MAJOR).$(MINOR) $(ROOTDIR)/romfs/lib/$(LIB) 
	
	
test: test.o $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lcli

$(NAME): $(DEPEND_FILES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ -L. -I./ 

clitelnet: clitelnet.o $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -L. -lcli

cli.exe: clitest.c libcli.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< libcli.o -lws2_32

clean:
	rm -f *.o $(DEPEND_FILES)* clitest

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
