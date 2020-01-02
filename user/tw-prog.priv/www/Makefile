#
# Linux NVRAM Makefile
#
# Copyright (C) 2012, Broadcom Corporation. All Rights Reserved.
# 
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# $Id: Makefile 271576 2011-07-12 20:31:22Z $
#

-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(BUSYBOX_CONFIG)
-include $(ARCH_CONFIG)
-include $(MODULES_CONFIG)

CP:=cp -fpR
RM:=rm -rf
ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
PRODUCT=$(shell echo $(CONFIG_PRODUCT) | sed s/-//g)
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
PRODUCT=$(shell echo $(CONFIG_PRODUCT) | sed s/-//g)
else
PRODUCT=$(shell echo $(CONFIG_CUSTOM_PRODUCT) | sed s/-//g)
endif
.PHONY: all build clean distclean
all: romfs

romfs: 
	$(RM) $(ROOTDIR)/romfs/etc_ro/lighttpd/www/*
	echo "-------------------$(PRODUCT)------------------------"
ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_882_PARSE_2), y)
	$(ROMFSINST) web_PARSE_2 /etc_ro/lighttpd/www/web
	$(CP) web_PARSE_2/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_882_VLAN), y)
	$(ROMFSINST) web_VLAN /etc_ro/lighttpd/www/web
	$(CP) web_VLAN/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_878_VLAN), y)
	$(ROMFSINST) web_VLAN /etc_ro/lighttpd/www/web
	$(CP) web_VLAN/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_867), y)
	$(ROMFSINST) web_867 /etc_ro/lighttpd/www/web
	$(CP) web_867/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_1935), y)
	$(ROMFSINST) web_1935 /etc_ro/lighttpd/www/web
	$(CP) web_1935/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_853_A1), y)
	$(ROMFSINST) web_853_A1 /etc_ro/lighttpd/www/web
	$(CP) web_853_A1/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_853_A2), y)
	$(ROMFSINST) web_853_A2 /etc_ro/lighttpd/www/web
	$(CP) web_853_A2/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_1360), y)
	$(ROMFSINST) web_1360 /etc_ro/lighttpd/www/web
	$(CP) web_1360/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_878_CA), y)
	$(ROMFSINST) web_878_CA /etc_ro/lighttpd/www/web
	$(CP) web_878_CA/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
else
	$(ROMFSINST) web /etc_ro/lighttpd/www/web
	$(CP) web/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
endif

ifeq ($(CONFIG_SUPPORT_WIFI_CERT), y)
	$(CP) web_wifi_cert/SOAPWLanRadio.js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/js/SOAP/SOAPWLanRadio.js
	$(CP) web_wifi_cert/WiFi.html $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/WiFi.html
	$(CP) web_wifi_cert/GetCurrPinConnectStatus.xml $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/hnap/GetCurrPinConnectStatus.xml
	$(CP) web_wifi_cert/GetDeviceInputPin.xml $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/hnap/GetDeviceInputPin.xml
	$(CP) web_wifi_cert/GetDevicePin.xml $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/hnap/GetDevicePin.xml
	$(CP) web_wifi_cert/GetCurrPbcConnectStatus.xml $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/hnap/GetCurrPbcConnectStatus.xml
	$(CP) web_wifi_cert/SetStartPbc.xml $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/hnap/SetStartPbc.xml
	$(CP) web_wifi_cert/SetDeviceInputPin.xml $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/hnap/SetDeviceInputPin.xml
endif

#	$(ROMFSINST) ../prog-cgi/src/fwupload.cgi /etc_ro/lighttpd/www/web/fwupload.cgi
	$(ROMFSINST) ../prog-cgi/src/dlcfg.cgi /etc_ro/lighttpd/www/web/HNAP1/dlcfg.cgi
	$(ROMFSINST) ../prog-cgi/src/dllog.cgi /etc_ro/lighttpd/www/web/HNAP1/dllog.cgi	
#	$(ROMFSINST) ../prog-cgi/src/seama.cgi /etc_ro/lighttpd/www/web/seama.cgi
	$(ROMFSINST) ../prog-cgi/src/wireless.cgi /etc_ro/lighttpd/www/web/wireless.cgi
	$(ROMFSINST) ../prog-cgi/src/dlquickvpnsettings.cgi /etc_ro/lighttpd/www/web/HNAP1/dlquickvpnsettings.cgi
	
clean:
	echo "--$(RM) $(TOPDIR)etc_ro/lighttpd/www/*--"
	$(RM) $(TOPDIR)etc_ro/lighttpd/www/*

