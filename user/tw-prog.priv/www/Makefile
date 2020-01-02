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
PRODUCT=$(shell echo $(CONFIG_CUSTOM_PRODUCT) | sed s/-//g)
.PHONY: all build clean distclean
all: romfs

romfs: 
	$(RM) $(ROOTDIR)/romfs/etc_ro/lighttpd/www/*
	$(ROMFSINST) web /etc_ro/lighttpd/www/web
	echo "-------------------$(PRODUCT)------------------------"
	$(CP) web/config/deviceinfo/$(PRODUCT).js $(ROOTDIR)/romfs/etc_ro/lighttpd/www/web/config/deviceinfo.js
#	$(ROMFSINST) ../prog-cgi/src/fwupload.cgi /etc_ro/lighttpd/www/web/fwupload.cgi
#	$(ROMFSINST) ../prog-cgi/src/dlcfg.cgi /etc_ro/lighttpd/www/web/dlcfg.cgi
	$(ROMFSINST) ../prog-cgi/src/dllog.cgi /etc_ro/lighttpd/www/web/dllog.cgi	
#	$(ROMFSINST) ../prog-cgi/src/seama.cgi /etc_ro/lighttpd/www/web/seama.cgi
	$(ROMFSINST) ../prog-cgi/src/wireless.cgi /etc_ro/lighttpd/www/web/wireless.cgi
#	$(ROMFSINST) ../prog-cgi/src/dlquickvpnsettings.cgi /etc_ro/lighttpd/www/web/dlquickvpnsettings.cgi
	
clean:
	echo "--$(RM) $(TOPDIR)etc_ro/lighttpd/www/*--"
	$(RM) $(TOPDIR)etc_ro/lighttpd/www/*

