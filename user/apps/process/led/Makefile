#
SHELL:=/bin/bash
#
INSTALL    = install
LN         = ln
RM         = rm -f
#
NAME	= led
#memwatch.o 
DEPEND_FILES	= crypt.o main.o
#				  

CFLAG = -O0 -Wall -g 

DESTDIR =
PREFIX = /usr/local

MAJOR = 1
MINOR = 9
REVISION = 3
LIB = libled.so

#CC = gcc
DEBUG = -g
OPTIM = -O3
#CFLAGS += $(DEBUG) $(OPTIM) -Wall -Wformat-security -Wno-format-zero-length -I$(ROOTDIR)/user/apps/include 

CFLAGS += -DLINUX26 -D__CONFIG_IPV6__
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/apps/include -I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS += -fPIC
LDFLAGS	+= -L$(ROOTDIR)/user/apps/rcm -L$(ROOTDIR)/user/apps/notify_rc 
LDFLAGS += -lnvram -lrcm -lnotifyrc -L$(ROOTDIR)/user/openssl-1.0.2j/ -lssl -lcrypto


LIBPATH += -L.
LIBS = -lcrypt

OBJS := ledmain.o ledfun.o libled.o

all: install 

$(LIB): libled.o
	$(CC) $(CFLAG) -o $(LIB).$(MAJOR).$(MINOR).$(REVISION) $^ $(LDFLAGS) $(LIBS)
	-rm -f $(LIB) $(LIB).$(MAJOR).$(MINOR)
	ln -s $(LIB).$(MAJOR).$(MINOR).$(REVISION) $(LIB).$(MAJOR).$(MINOR)
	ln -s $(LIB).$(MAJOR).$(MINOR) $(LIB)
	
#%.o: %.c
#	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -o $@ -c $<

libled.o: libled.h

romfs: install

led: ledmain.o ledfun.o libled.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ ledmain.o ledfun.o libled.o -L. $(LDFLAGS)
#	$(CC) $(CFLAG) -o ledtest.o ledtest.c $(LDFLAGS) $(LIBS)
#	$(CC) $(CPPFLAGS) $(CFLAGS) -o led ledtest.o libled.o 
	
clean:
	rm -f *.o $(LIB)* led ledtest

install:led 
	install -d $(ROOTDIR)/romfs/usr/sbin
	install led $(ROOTDIR)/romfs/usr/sbin
	$(STRIP) $(ROOTDIR)/romfs/usr/sbin/led

