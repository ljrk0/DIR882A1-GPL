DIRS=sbd nb 
INSTALL  = install
romfs_dir = $(ROOTDIR)/romfs
meshd_dirs = $(romfs_dir)/etc_ro/AZ3/script

.PHONY : all nb_vfy sbd clean 

ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-2660")
CFLAG += -DPRODUCT_DIR2660
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
CFLAG += -DPRODUCT_DIR853_A2
CFLAG += -DWIFI_DBDC_SUPPORT
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
CFLAG += -DPRODUCT_DIR853_A1
CFLAG += -DWIFI_DBDC_SUPPORT
else ifeq ("$(CONFIG_PRODUCT)", "DIR_1360")
CFLAG += -DPRODUCT_DIR1360
CFLAG += -DWIFI_DBDC_SUPPORT
endif

all : 
	set -e; for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	rm -rf nb/nb_vfy
	rm -rf sbd/libapsond_vendor_sbapi.so
	
romfs:
	#lib
ifneq ($(CONFIG_OOKLA_SUPPORT), y)
	$(INSTALL) -m 755   lib/libgcc_s.so.1 $(romfs_dir)/lib/
endif
	$(INSTALL) -m 755   lib/libapson_mqtt_api.so $(romfs_dir)/lib/
	$(INSTALL) -m 755   lib/libapson_nbapi.so $(romfs_dir)/lib/
	$(INSTALL) -m 755   lib/libapsond_sbapi.so $(romfs_dir)/lib/
	#$(INSTALL) -m 755 	lib/libapsond_vendor_sbapi.so $(romfs_dir)/lib/
	$(INSTALL) -m 755   lib/libapsonpb.so  $(romfs_dir)/lib/
	$(INSTALL) -m 755   lib/libazlog.so  $(romfs_dir)/lib/
	$(INSTALL) -m 755   lib/libapsonpb.so  $(romfs_dir)/lib/
	$(INSTALL) -m 755   lib/libcares.so.2.0.0  $(romfs_dir)/lib/
	ln -sf   lib/libcares.so.2.0.0  $(romfs_dir)/lib/libcares.so.2
	
	mkdir -p $(romfs_dir)/lib/modules/3.10.14+/
	$(INSTALL) -m 755	lib/modules/3.10.14+/meshm.ko $(romfs_dir)/lib/modules/3.10.14+/
	
	#usr
	$(INSTALL) -m 755 usr/bin/acmd $(romfs_dir)/usr/bin/
	$(INSTALL) -m 755 usr/bin/apsond $(romfs_dir)/usr/bin/
	$(INSTALL) -m 755 usr/bin/arpd $(romfs_dir)/usr/bin/
	$(INSTALL) -m 755 usr/bin/meshd $(romfs_dir)/usr/bin/
	$(INSTALL) -m 755 usr/bin/meshdctrl $(romfs_dir)/usr/bin/
	$(INSTALL) -m 755 usr/bin/sbd $(romfs_dir)/usr/bin/

	#mosquitto
	#cp -rf usr/sbin/* $(ROOTDIR)/romfs/usr/sbin/
	#cp -rf   lib/libapsonpb.so  $(ROOTDIR)/romfs$(romfs_dir)/lib/libmosquitto.so.1
	
	#etc
	mkdir -p $(romfs_dir)/etc_ro/AZ3/script/
	$(INSTALL) -m 755 etc/AZ3/script/meshd_start_ap.sh $(romfs_dir)/etc_ro/AZ3/script/
	$(INSTALL) -m 755 etc/AZ3/script/meshd_start_apcli.sh $(romfs_dir)/etc_ro/AZ3/script/

	#mkdir -p $(romfs_dir)/etc_ro/config/
	#$(INSTALL) -m 755  etc/config/apson  $(romfs_dir)/etc_ro/config/
	#$(INSTALL) -m 755  etc/config/apson.ap  $(romfs_dir)/etc_ro/config/
	
	#nb
	$(INSTALL) -m 755 nb/nb_vfy $(romfs_dir)/usr/bin/
	
	#sbd
	$(INSTALL) -m 755 sbd/libapsond_vendor_sbapi.so $(romfs_dir)/lib/
	
	#setup.sh
	$(INSTALL) -m 755 supermesh_setup.sh $(romfs_dir)/usr/sbin/
	
