.PHONY: all romfs clean

#
# Include architecture specific build rules.
#
ifndef ROOTDIR
ROOTDIR=..
endif
TRDMDIR  = $(shell pwd)
LIBXML2DIR = 01.common/dm_lib/libxml2
DMLIBDIR = 01.common/dm_lib
OSADIR = 01.common/osa
LIBOPENSSLCDIR = 05.oam/05.openssl/04.libopenssl
TR69CDIR = 05.oam/02.tr69c
DMSRVDIR = 05.oam/07.dm_srv
DBEXE = DbStart
SRCS = DbStart.c

FLAGS = -I$(TRDMDIR)/01.common/include -I$(TRDMDIR)/$(DMLIBDIR)/common/inc -I$(TRDMDIR)/$(OSADIR)/inc
LDFLAGS += -L$(TRDMDIR)/$(DMLIBDIR)/lib -ldm -L$(TRDMDIR)/$(OSADIR)/lib -losa -L$(TRDMDIR)/$(LIBXML2DIR)/mips_lib -lxml2 -lpthread -ldl -lrt -lm


UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(BUSYBOX_CONFIG)
-include $(ARCH_CONFIG)
-include $(MODULES_CONFIG)

export ROOTDIR
export CROSS_COMPILE
export LINUXDIR
export TRDMDIR
#
# must run the vendor build first
#

dir_x =
dir_y = 

dir_x+= 01.common/dm_lib/_script/tool
dir_x+= $(LIBXML2DIR)
#dir_x+= 05.oam/05.openssl/00.source
dir_y+= 01.common/dm_lib/db_device
dir_y+= 01.common/dm_lib/db_internal
dir_y+= 01.common/dm_lib/common
dir_y+= 05.oam/05.openssl/04.libopenssl
dir_y+= 01.common/dm_lib/external_type
dir_y+= 05.oam/02.tr69c/build
dir_y+= 05.oam/07.dm_srv
	
all:
	for i in $(dir_x) ; do make -C $$i $@ || exit $?; done
	for i in $(dir_y) ; do make KERNEL_PATH=$(ROOTDIR)/$(LINUXDIR) COMPILE_VERSION= L1Version=LTE_L1 DUTYPE=NULL PRC_CROSS_COMPILE=$(CROSS_COMPILE) -C $$i $@ || exit $?; done
	$(CC) $(SRCS) -o $(DBEXE) $(FLAGS) $(LDFLAGS)
	
#cd 01.common/dm_lib/libxml2 && ./CompileLibxml2.sh $(ROOTDIR)/$(LINUXDIR) '' LTE_L1 $(CROSS_COMPILE)


clean:
	-for i in $(dir_y) ; do \
			[ ! -d $$i ] || $(MAKE) -C $$i clean ; \
	done

romfs:
#	$(ROMFSINST) $(LIBXML2DIR)/results/bin/xmlcatalog /bin/xmlcatalog
#	$(ROMFSINST) $(LIBXML2DIR)/results/bin/xmllint /bin/xmllint
#	$(ROMFSINST) $(LIBXML2DIR)/results/lib/libxml2.so.2.9.2 /lib/libxml2.so.2.9.2
#	$(ROMFSINST) -s libxml2.so.2.9.2 /lib/libxml2.so
	$(ROMFSINST) $(LIBXML2DIR)/mips_lib/libxml2.a /lib/libxml2.a
	$(ROMFSINST) $(DMLIBDIR)/lib/libDbConv.so /lib/libDbConv.so
	$(ROMFSINST) $(DMLIBDIR)/lib/libDbConv.a /lib/libDbConv.a
	$(ROMFSINST) $(DMLIBDIR)/lib/libDeviceDb.so /lib/libDeviceDb.so
	$(ROMFSINST) $(DMLIBDIR)/lib/libDeviceDb.a /lib/libDeviceDb.a
	$(ROMFSINST) $(DMLIBDIR)/lib/libInternalDb.so /lib/libInternalDb.so
	$(ROMFSINST) $(DMLIBDIR)/lib/libInternalDb.a /lib/libInternalDb.a
	$(ROMFSINST) $(DMLIBDIR)/lib/libdm.so /lib/libdm.so
	$(ROMFSINST) $(DMLIBDIR)/lib/libdm.a /lib/libdm.a
	$(ROMFSINST) $(DMLIBDIR)/lib/libTr069DbCbs.so /lib/libTr069DbCbs.so
	$(ROMFSINST) $(DMLIBDIR)/lib/libTr069DbCbs.a /lib/libTr069DbCbs.a
	$(ROMFSINST) $(OSADIR)/lib/libosa.a /lib/libosa.a
	$(ROMFSINST) $(OSADIR)/lib/libosa.so /lib/libosa.so
	$(ROMFSINST) $(LIBOPENSSLCDIR)/lib/libopenssl.so /lib/libopenssl.so
	$(ROMFSINST) $(TR69CDIR)/build/libs/libadaptor.a /lib/libadaptor.a
	$(ROMFSINST) $(TR69CDIR)/build/libs/libtr69c.a /lib/libtr69c.a
	$(ROMFSINST) $(TR69CDIR)/build/tr69c /bin/tr69c
	$(ROMFSINST) $(DMSRVDIR)/lib/dmsrv /bin/dmsrv
	$(ROMFSINST) DbStart /bin/DbStart
