#
# log downloader Makefile
#
#



include $(TOP)/.config


CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include
#CFLAGS	+= -g -DDEBUG
#CFLAGS	+= -s
LDFLAGS	+= -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -L$(TOP)/libbcm -L$(INSTALLDIR)/libbcm/usr/lib -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib 
LDFLAGS += -lnvram -lbcm -lshared
#LDFLAGS += -L$(TOP)/libbcmcrypto -L$(INSTALLDIR)/libbcmcrypto/usr/lib -lbcmcrypto
LDFLAGS += $(EXTRA_LDFLAGS)


OBJS := downloader.o downloader_main.o

all:downloader


downloader:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


install:   
	install -d $(INSTALLDIR)/downloader/usr/sbin
	install downloader $(INSTALLDIR)/downloader/usr/sbin
	$(STRIP) $(INSTALLDIR)/downloader/usr/sbin/downloader

	  
clean:
	rm -f *~ downloader *.o

