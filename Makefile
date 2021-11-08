###########################################################################
#
# Makefile -- Top level uClinux makefile.
#
# Copyright (c) 2001-2004, SnapGear (www.snapgear.com)
# Copyright (c) 2001, Lineo
#

VERSIONPKG = 3.2.0
VERSIONSTR = $(CONFIG_VENDOR)/$(CONFIG_PRODUCT) Version $(VERSIONPKG)
#STRIPTOOL="$CONFIG_CROSS_COMPILER_PATH"/mipsel-linux-sstrip
############################################################################
#
# Lets work out what the user wants, and if they have configured us yet
#

ifeq (.config,$(wildcard .config))
include .config
-include config/.config


#=======================
# MT7623 Setting
#=======================
ifeq ($(CONFIG_DEFAULTS_MEDIATEK_MT7623),y)

ifeq ($(CONFIG_LINUXDIR),linux-3.4.x)
ifeq ($(CONFIG_DEFSETTING_4M32M),y)
export ARCH_MTK_PLATFORM=mt7623
export MTK_PLATFORM=mt7623
export TARGET_PRODUCT=mt7623_fpga_ldvt
else
export ARCH_MTK_PLATFORM=mt8127
export MTK_PLATFORM=mt8127
export TARGET_PRODUCT=mt8127_tablet_p1_v1
endif
endif

UCLIBC_PLUS_LIB = uClibc++-0.2.4
UCLIBC_PLUS_LIB_ONLY = uClibc++-0.2.4_only
UCLIBC_APP = user
UCLIBC_APP_ONLY = user_only
BUSYBOX_APP = user/busybox

#=======================
# MT7622 Setting
#=======================
else
ifeq ($(CONFIG_DEFAULTS_MEDIATEK_MT7622),y)
export MTK_PLATFORM=mt6735m
export TARGET_BUILD_VARIANT=eng

UCLIBC_PLUS_LIB =		#uClibc++ doesn't support CA53
UCLIBC_PLUS_LIB_ONLY =		#uClibc++ doesn't support CA53
UCLIBC_APP =			#uClibc doesn't support CA53
UCLIBC_APP_ONLY =		#uClibc++ doesn't support CA53
BUSYBOX_APP = user2/busybox-1.23.0

#=======================
# other chipsets Setting
#=======================
else
UCLIBC_PLUS_LIB = uClibc++
UCLIBC_PLUS_LIB_ONLY = uClibc++_only
UCLIBC_APP = user
UCLIBC_APP_ONLY = user_only
BUSYBOX_APP = user/busybox
endif
endif

#=======================
# library setting
#=======================
ifeq ($(CONFIG_UCLIBC_0_9_33_2),y)
UCLIBC_LIB = uClibc-0.9.33.2
UCLIBC_LIB_ONLY = uClibc-0.9.33.2_only
LIB = lib
LIB_ONLY = lib_only
endif

ifeq ($(CONFIG_UCLIBC_0_9_28),y)
#uClibc-0.9.28 and other libs are in the same directory
UCLIBC_LIB = lib	
UCLIBC_LIB_ONLY =
LIB = lib		
LIB_ONLY = lib_only		
endif

ifeq ($(CONFIG_UCLIBC_NONE),y)
UCLIBC_LIB =
UCLIBC_LIB_ONLY =
#current libraries cannot be compiled by aarch64 toolchain
LIB =	
LIB_ONLY =
endif

ifeq ($(CONFIG_HAVE_GLIBC_APP),y)
GLIBC_APP = user2
GLIBC_APP_ONLY = user2_only
endif
#------------------------------

#changed by Steven Liu
all:cmake install_kernel_headers $(UCLIBC_PLUS_LIB_ONLY) $(UCLIBC_LIB_ONLY) $(LIB_ONLY) modules $(UCLIBC_APP_ONLY) $(GLIBC_APP_ONLY) romfs linux image

else
all: config_error
endif

############################################################################
#
# Get the core stuff worked out
#

LINUXDIR = $(CONFIG_LINUXDIR)
LIBCDIR  = $(CONFIG_LIBCDIR)
ROOTDIR  = $(shell pwd)
PATH	 := $(PATH):$(ROOTDIR)/tools
HOSTCC   = cc
IMAGEDIR = $(ROOTDIR)/images
ROMFSDIR = $(ROOTDIR)/romfs
ROMFSINST= romfs-inst.sh
SCRIPTSDIR = $(ROOTDIR)/config/scripts
ifneq ($(shell hostname | grep worker)$(shell hostname | grep builder), )
TFTPDIR    = /tftpboot
else
TFTPDIR    = $(ROOTDIR)
#TFTPDIR    = $(ROOTDIR)/../tools/single_img/MT7621
endif
BUILD_START_STRING ?= $(shell date "+%a, %d %b %Y %T %z")

# generate and use kernel headers (for uclibc library)
ifeq ($(CONFIG_LINUXDIR),linux-3.10.54.x)
KERNEL_HEADERS = kernel_headers
endif

ifeq ($(CONFIG_LINUXDIR),linux-3.10.14.x)
KERNEL_HEADERS = kernel_headers
endif
ifeq ($(CONFIG_LINUXDIR),linux-3.10.20.x)
KERNEL_HEADERS = kernel_headers
endif
ifeq ($(CONFIG_LINUXDIR),linux-3.4.x)
KERNEL_HEADERS = kernel_headers
endif
#
# For linux-2.6 kernel, it doesn't know VPATH parameter in Makefile.
# That means object files will be generated at source directory.
#
# When we want to built both AP and STA driver in root filesystem, we MUST disable compiler's -j option to 
# avoid different CPU build WiFi driver at the same time.
#
# by Steven
#
-include $(LINUXDIR)/.config

ifneq ($(CONFIG_RT2860V2_AP),)
 ifneq ($(CONFIG_RT2860V2_STA),)
  HOST_NCPU := 1   # AP & STA driver
 else
  HOST_NCPU := $(shell if [ -f /proc/cpuinfo ]; then n=`grep -c processor /proc/cpuinfo`; if [ $$n -gt 1 ];then echo 2; else echo $$n; fi; else echo 1; fi)  
 endif
else
  HOST_NCPU := $(shell if [ -f /proc/cpuinfo ]; then n=`grep -c processor /proc/cpuinfo`; if [ $$n -gt 1 ];then echo 2; else echo $$n; fi; else echo 1; fi)  
endif
#--

#uncomment this line if you want to use single cpu to compiile your code.
HOST_NCPU := 1

LINUX_CONFIG  = $(ROOTDIR)/$(LINUXDIR)/.config
CONFIG_CONFIG = $(ROOTDIR)/config/.config
BUSYBOX_CONFIG = $(ROOTDIR)/$(BUSYBOX_APP)/.config
MODULES_CONFIG = $(ROOTDIR)/modules/.config


CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

ifeq (config.arch,$(wildcard config.arch))
ifeq ($(filter %_default, $(MAKECMDGOALS)),)
include config.arch
ARCH_CONFIG = $(ROOTDIR)/config.arch
export ARCH_CONFIG
endif
endif

# May use a different compiler for the kernel
KERNEL_CROSS_COMPILE ?= $(CROSS_COMPILE)
#KERNEL_CROSS_COMPILE = /opt/timesys/toolchains/mipsisa32r2el-linux/bin/mipsel-linux-
ifneq ($(SUBARCH),)
# Using UML, so make the kernel and non-kernel with different ARCHs
MAKEARCH = $(MAKE) ARCH=$(SUBARCH) CROSS_COMPILE=$(CROSS_COMPILE)
MAKEARCH_KERNEL = $(MAKE) ARCH=$(ARCH) SUBARCH=$(SUBARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
else
MAKEARCH = $(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
MAKEARCH_KERNEL = $(MAKE)  ARCH=$(ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
endif
DIRS    =  $(ROOTDIR)/vendors include $(UCLIBC_PLUS_LIB) $(UCLIBC_APP) $(GLIBC_APP) $(UCLIBC_LIB) $(LIB)

export VENDOR PRODUCT ROOTDIR LINUXDIR HOSTCC CONFIG_SHELL
export CONFIG_CONFIG BUSYBOX_CONFIG LINUX_CONFIG MODULES_CONFIG ROMFSDIR SCRIPTSDIR
export VERSIONPKG VERSIONSTR ROMFSINST PATH IMAGEDIR RELFILES TFTPDIR
export BUILD_START_STRING
export HOST_NCPU

.PHONY: ucfront
ucfront: tools/ucfront/*.c
	$(MAKE) -C tools/ucfront
	ln -sf $(ROOTDIR)/tools/ucfront/ucfront tools/ucfront-gcc
	ln -sf $(ROOTDIR)/tools/ucfront/ucfront tools/ucfront-g++
	ln -sf $(ROOTDIR)/tools/ucfront/ucfront-ld tools/ucfront-ld

.PHONY: cksum
cksum: tools/sg-cksum/*.c
	$(MAKE) -C tools/sg-cksum
	ln -sf $(ROOTDIR)/tools/sg-cksum/cksum tools/cksum

############################################################################

#
# Config stuff,  we recall ourselves to load the new config.arch before
# running the kernel and other config scripts
#

.PHONY: config.tk config.in
cmake:
	cd user/cmake/;\
	$(MAKE);     

config.in:
	@chmod u+x config/mkconfig
	config/mkconfig > config.in

config.tk: config.in
	$(MAKE) -C $(SCRIPTSDIR) tkparse
	ARCH=dummy $(SCRIPTSDIR)/tkparse < config.in > config.tmp
	@if [ -f /usr/local/bin/wish ];	then \
		echo '#!'"/usr/local/bin/wish -f" > config.tk; \
	else \
		echo '#!'"/usr/bin/wish -f" > config.tk; \
	fi
	cat $(SCRIPTSDIR)/header.tk >> ./config.tk
	cat config.tmp >> config.tk
	rm -f config.tmp
	echo "set defaults \"/dev/null\"" >> config.tk
	echo "set help_file \"config/Configure.help\"" >> config.tk
	cat $(SCRIPTSDIR)/tail.tk >> config.tk
	chmod 755 config.tk

.PHONY: xconfig
xconfig: config.tk
	@wish -f config.tk
	@if [ ! -f .config ]; then \
		echo; \
		echo "You have not saved your config, please re-run make config"; \
		echo; \
		exit 1; \
	 fi
	@chmod u+x config/setconfig
	@config/setconfig defaults
	@if egrep "^CONFIG_DEFAULTS_KERNEL=y" .config > /dev/null; then \
		$(MAKE) linux_xconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_MODULES=y" .config > /dev/null; then \
		$(MAKE) modules_xconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_VENDOR=y" .config > /dev/null; then \
		$(MAKE) config_xconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_BUSYBOX=y" .config > /dev/null; then \
		$(MAKE) -C $(BUSYBOX_APP) menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC=y" .config > /dev/null; then \
		$(MAKE) -C $(UCLIBC_LIB) menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC_PLUS_LIB_PLUS=y" .config > /dev/null; then \
		$(MAKE) -C $(UCLIBC_PLUS_LIB) menuconfig; \
	 fi
	@config/setconfig final

.PHONY: config
config: config.in
	@HELP_FILE=config/Configure.help \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Configure config.in
	@chmod u+x config/setconfig
	@config/setconfig defaults
	@if egrep "^CONFIG_DEFAULTS_KERNEL=y" .config > /dev/null; then \
		$(MAKE) linux_config; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_MODULES=y" .config > /dev/null; then \
		$(MAKE) modules_config; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_VENDOR=y" .config > /dev/null; then \
		$(MAKE) config_config; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_BUSYBOX=y" .config > /dev/null; then \
		$(MAKE) -C $(BUSYBOX_APP) menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC=y" .config > /dev/null; then \
		$(MAKE) -C $(UCLIBC_LIB) menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC_PLUS_LIB_PLUS=y" .config > /dev/null; then \
		$(MAKE) -C $(UCLIBC_PLUS_LIB) menuconfig; \
	 fi
	@config/setconfig final

.PHONY: menuconfig
menuconfig: config.in
	$(MAKE) -C $(SCRIPTSDIR)/lxdialog all
	@HELP_FILE=config/Configure.help \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Menuconfig config.in
	@if [ ! -f .config ]; then \
		echo; \
		echo "You have not saved your config, please re-run make config"; \
		echo; \
		exit 1; \
	 fi

	@chmod u+x config/setconfig
	@config/setconfig defaults
	@if egrep "^CONFIG_DEFAULTS_KERNEL=y" .config > /dev/null; then \
		$(MAKE) linux_menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_MODULES=y" .config > /dev/null; then \
		$(MAKE) modules_menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_VENDOR=y" .config > /dev/null; then \
		$(MAKE) config_menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_BUSYBOX=y" .config > /dev/null; then \
		$(MAKE) -C $(BUSYBOX_APP) menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC=y" .config > /dev/null; then \
		$(MAKE) -C $(UCLIBC_LIB) menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC_PLUS_LIB_PLUS=y" .config > /dev/null; then \
		$(MAKE) -C $(UCLIBC_PLUS_LIB) menuconfig; \
	 fi
	@config/setconfig final

.PHONY: oldconfig
oldconfig: config.in
	@HELP_FILE=config/Configure.help \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Configure -d config.in
	@$(MAKE) oldconfig_linux
	@$(MAKE) oldconfig_modules
	@$(MAKE) oldconfig_config
	@chmod u+x config/setconfig
	@config/setconfig final

.PHONY: modules
modules:
	. $(LINUXDIR)/.config; if [ "$$CONFIG_MODULES" = "y" ]; then \
		[ -d $(LINUXDIR)/modules ] || mkdir $(LINUXDIR)/modules; \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) modules; \
	fi

.PHONY: modules_install
modules_install:
	. $(LINUXDIR)/.config; if [ "$$CONFIG_MODULES" = "y" ]; then \
		[ -d $(ROMFSDIR)/lib/modules ] || mkdir -p $(ROMFSDIR)/lib/modules; \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) INSTALL_MOD_PATH=$(ROMFSDIR) DEPMOD="../$(BUSYBOX_APP)/examples/depmod.pl" modules_install; \
		rm -f $(ROMFSDIR)/lib/modules/*/build; \
		rm -f $(ROMFSDIR)/lib/modules/*/source; \
		find $(ROMFSDIR)/lib/modules -type f -name "*o" | xargs -r $(STRIP) -R .comment -R .note -g --strip-unneeded; \
	fi
	
#CONFIG_PRODUCT_AC2600 = MT7621_AC2600
CONFIG_PRODUCT_AC2600 = DIR_882_VLAN
CONFIG_PRODUCT_AC2600_LINUXDIR = linux-3.10.14.x

DIR_882_VLAN:
	#$(warning $(LINUXDIR))
	#$(warning $(CONFIG_PRODUCT))
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/product.config                          .config
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/8M_128M_config.busybox-3.10.14.x        user/busybox/.config
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/8M_128M_config.linux-3.10.14.x          $(CONFIG_PRODUCT_AC2600_LINUXDIR)/.config
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/8M_128M_config.uclibc-3.10.14.x         lib/.config
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/8M_128M_config.uclibc0933-3.10.14.x     uClibc-0.9.33.2/.config
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/8M_128M_config.vendor-3.10.14.x         config/.config
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/autoconf_config.h                       config/autoconf.h
	cp vendors/Mediatek/$(CONFIG_PRODUCT_AC2600)/config/autoconf_main.h                         autoconf.h
	make menuconfig

	
linux_xconfig:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) xconfig
linux_menuconfig:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) menuconfig
linux_config:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) config
modules_xconfig:
	[ ! -d modules ] || $(MAKEARCH) -C modules xconfig
modules_menuconfig:
	[ ! -d modules ] || $(MAKEARCH) -C modules menuconfig
modules_config:
	[ ! -d modules ] || $(MAKEARCH) -C modules config
modules_clean:
	-[ ! -d modules ] || $(MAKEARCH) -C modules clean
config_xconfig:
	$(MAKEARCH) -C config xconfig
config_menuconfig:
	$(MAKEARCH) -C config menuconfig
config_config:
	$(MAKEARCH) -C config config
oldconfig_config:
	$(MAKEARCH) -C config oldconfig
oldconfig_modules:
	[ ! -d modules ] || $(MAKEARCH) -C modules oldconfig
oldconfig_linux:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) oldconfig

############################################################################
#
# normal make targets
#

.PHONY: romfs
romfs: romfs.subdirs modules_install romfs.post

.PHONY: romfs.subdirs
romfs.subdirs:
	for dir in vendors $(DIRS) ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir romfs || exit 1 ; done

.PHONY: romfs.post
romfs.post:
	for dir in $(UCLIBC_APP) $(GLIBC_APP) vendors ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir romfs.post || exit 1 ; done
	-find $(ROMFSDIR)/. -name CVS | xargs -r rm -rf

.PHONY: image
image:
	@rm -rf $(ROOTDIR)/linux-3.10.14.x/kernel/config_data.gz
	[ -d $(IMAGEDIR) ] || mkdir $(IMAGEDIR)
	$(MAKEARCH) -C vendors image
	cp $(IMAGEDIR)/$(USER)_uImage DIR882_uImage.img
#	make -f $(TFTPDIR)/Makefile.16M

.PHONY: release
release:
	make -C release release

%_fullrelease:
	@echo "This target no longer works"
	@echo "Do a make -C release $@"
	exit 1
#
# fancy target that allows a vendor to have other top level
# make targets,  for example "make vendor_flash" will run the
# vendor_flash target in the vendors directory
#

vendor_%:
	$(MAKEARCH) -C vendors $@

.PHONY: linux
linux linux%_only:
	@if [ $(LINUXDIR) = linux-2.4.x -a ! -f $(LINUXDIR)/.depend ] ; then \
		echo "ERROR: you need to do a 'make dep' first" ; \
		exit 1 ; \
	fi
	$(MAKEARCH_KERNEL) -j$(HOST_NCPU) -C $(LINUXDIR) $(LINUXTARGET) || exit 1
	if [ -f $(LINUXDIR)/vmlinux ]; then \
		ln -f $(LINUXDIR)/vmlinux $(LINUXDIR)/linux ; \
	fi

.PHONY: sparse
sparse:
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) C=1 $(LINUXTARGET) || exit 1

.PHONY: sparseall
sparseall:
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) C=2 $(LINUXTARGET) || exit 1

.PHONY: subdirs
subdirs: $(LIB) $(UCLIBC_LIB) $(UCLIBC_PLUS_LIB) linux $(MODULES)
	for dir in $(DIRS) ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir || exit 1 ; done	

.PHONY: install_kernel_headers
install_kernel_headers:
	@if [ $(LINUXDIR) = linux-3.10.14.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) headers_install INSTALL_HDR_PATH=$(ROOTDIR)/$(KERNEL_HEADERS) ; \
	fi
	@if [ $(LINUXDIR) = linux-3.10.20.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) headers_install INSTALL_HDR_PATH=$(ROOTDIR)/$(KERNEL_HEADERS) ; \
	fi
	@if [ $(LINUXDIR) = linux-3.10.54.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) headers_install INSTALL_HDR_PATH=$(ROOTDIR)/$(KERNEL_HEADERS) ; \
	fi
	
	@if [ $(LINUXDIR) = linux-3.4.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) headers_install INSTALL_HDR_PATH=$(ROOTDIR)/$(KERNEL_HEADERS) ; \
	fi

dep:
	@if [ ! -f $(LINUXDIR)/.config ] ; then \
		echo "ERROR: you need to do a 'make config' first" ; \
		exit 1 ; \
	fi
	
	@if [ $(LINUXDIR) = linux-2.6.21.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare ; \
	fi
	
	@if [ $(LINUXDIR) = linux-2.6.36MT.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare ; \
		rm -fr $(LINUXDIR)/include/asm; \
		rm -fr $(LINUXDIR)/include/linux/autoconf.h; \
		rm -fr $(LINUXDIR)/arch/mips/include/asm/rt2880; \
		ln -sf ../arch/mips/include/asm $(LINUXDIR)/include/asm; \
		ln -sf ../../include/generated/autoconf.h $(LINUXDIR)/include/linux/autoconf.h; \
		ln -sf ./mach-ralink $(LINUXDIR)/arch/mips/include/asm/rt2880; \
	fi
	
	@if [ $(LINUXDIR) = linux-2.6.36.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare ; \
		rm -fr $(LINUXDIR)/include/asm; \
		rm -fr $(LINUXDIR)/arch/mips/include/asm/rt2880; \
		rm -fr $(LINUXDIR)/include/linux/autoconf.h; \
		ln -sf ../arch/mips/include/asm $(LINUXDIR)/include/asm; \
		ln -sf ./mach-ralink $(LINUXDIR)/arch/mips/include/asm/rt2880; \
		ln -sf ../../include/generated/autoconf.h $(LINUXDIR)/include/linux/autoconf.h; \
	fi

	@if [ $(LINUXDIR) = linux-3.10.14.x -o $(LINUXDIR) = linux-3.10.20.x -o $(LINUXDIR) = linux-3.10.54.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare ; \
		rm -fr $(LINUXDIR)/include/uapi/linux/autoconf.h; \
		rm -fr $(LINUXDIR)/include/linux/autoconf.h; \
		rm -fr $(LINUXDIR)/arch/mips/include/uapi/asm/rt2880; \
		rm -fr $(LINUXDIR)/arch/mips/include/asm/rt2880; \
		ln -sf ./mach-ralink $(LINUXDIR)/arch/mips/include/asm/rt2880; \
		ln -sf ../../asm/mach-ralink $(LINUXDIR)/arch/mips/include/uapi/asm/rt2880; \
		ln -sf ../../generated/autoconf.h $(LINUXDIR)/include/uapi/linux/autoconf.h; \
		ln -sf ../generated/autoconf.h $(LINUXDIR)/include/linux/autoconf.h; \
	fi

	@if [ $(LINUXDIR) = linux-3.4.x ] ; then \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare ; \
	fi
	
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) dep

symbolic_link:
	echo "Linux Version: $(LINUXDIR)"
	@if [ $(LINUXDIR) = linux-2.6.36MT.x ] ; then \
		rm -fr $(LINUXDIR)/arch/mips/ralink/nvram.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/nvram.h; \
		rm -fr $(LINUXDIR)/drivers/net/rt_rdm; \
		rm -fr $(LINUXDIR)/drivers/net/raeth; \
		rm -fr $(LINUXDIR)/drivers/net/nfc; \
		rm -fr $(LINUXDIR)/drivers/char/i2c_drv.c; \
		rm -fr $(LINUXDIR)/drivers/char/i2c_drv.h; \
		rm -fr $(LINUXDIR)/net/nat/hw_nat; \
		rm -fr $(LINUXDIR)/drivers/char/gdma_test.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		rm -fr $(LINUXDIR)/drivers/char/i2s; \
		rm -fr $(LINUXDIR)/drivers/mtd/ralink/ralink_nand.c; \
		rm -fr $(LINUXDIR)/drivers/mtd/ralink/ralink_nand.h; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_wdt.c; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_wdt.h; \
		rm -fr $(LINUXDIR)/drivers/net/cryptoDriver; \
		rm -fr $(LINUXDIR)/include/net/mtk_esp.h; \
		rm -fr $(LINUXDIR)/net/ipv4/mtk_esp4.c; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RTPCI_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3090_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5392_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5592_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3593_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/MT7610_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3572_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5572_ap; \
		ln -sf ../../../../linux-2.6.36.x/arch/mips/ralink/slt $(LINUXDIR)/arch/mips/ralink/slt; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.c $(LINUXDIR)/arch/mips/ralink/nvram.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.h $(LINUXDIR)/arch/mips/ralink/nvram.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/rt_rdm $(LINUXDIR)/drivers/net/rt_rdm; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/raeth $(LINUXDIR)/drivers/net/raeth; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/nfc $(LINUXDIR)/drivers/net/nfc; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2c_drv.c $(LINUXDIR)/drivers/char/i2c_drv.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2c_drv.h $(LINUXDIR)/drivers/char/i2c_drv.h; \
		ln -sf ../../../linux-2.6.21.x/net/nat/hw_nat $(LINUXDIR)/net/nat/hw_nat; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/gdma_test.c $(LINUXDIR)/drivers/char/gdma_test.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.c $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.h $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2s $(LINUXDIR)/drivers/char/i2s; \
		ln -sf ../../../../linux-2.6.21.x/drivers/mtd/ralink/ralink_nand.c $(LINUXDIR)/drivers/mtd/ralink/ralink_nand.c; \
		ln -sf ../../../../linux-2.6.21.x/drivers/mtd/ralink/ralink_nand.h $(LINUXDIR)/drivers/mtd/ralink/ralink_nand.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_wdt.c $(LINUXDIR)/drivers/watchdog/ralink_wdt.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_wdt.h $(LINUXDIR)/drivers/watchdog/ralink_wdt.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/net/cryptoDriver $(LINUXDIR)/drivers/net/cryptoDriver; \
		ln -sf ../../../linux-2.6.36.x/include/net/mtk_esp.h $(LINUXDIR)/include/net/mtk_esp.h; \
		ln -sf ../../../linux-2.6.36.x/net/ipv4/mtk_esp4.c $(LINUXDIR)/net/ipv4/mtk_esp4.c; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RTPCI_ap $(LINUXDIR)/drivers/net/wireless/RTPCI_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3090_ap $(LINUXDIR)/drivers/net/wireless/RT3090_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5392_ap $(LINUXDIR)/drivers/net/wireless/RT5392_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5592_ap $(LINUXDIR)/drivers/net/wireless/RT5592_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3593_ap $(LINUXDIR)/drivers/net/wireless/RT3593_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/MT7610_ap $(LINUXDIR)/drivers/net/wireless/MT7610_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3572_ap $(LINUXDIR)/drivers/net/wireless/RT3572_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5572_ap $(LINUXDIR)/drivers/net/wireless/RT5572_ap; \
	fi
	
	@if [ $(LINUXDIR) = linux-2.6.36.x ] ; then \
		rm -fr $(LINUXDIR)/arch/mips/ralink/ralink_wdt.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/rt_timer.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/rt_timer.h; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/nvram.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/nvram.h; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/hash_utils.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/hash_utils.h; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/list_utils.h; \
		rm -fr $(LINUXDIR)/drivers/usb/dwc_otg; \
		rm -fr $(LINUXDIR)/drivers/net/raeth; \
		rm -fr $(LINUXDIR)/drivers/net/nfc; \
		rm -fr $(LINUXDIR)/drivers/net/rt_rdm; \
		rm -fr $(LINUXDIR)/drivers/mtd/ralink; \
		rm -fr $(LINUXDIR)/drivers/mtd/maps/ralink-flash.c; \
		rm -fr $(LINUXDIR)/drivers/mtd/maps/ralink-flash.h; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gpio.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gpio.h; \
		rm -fr $(LINUXDIR)/drivers/char/gdma_test.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		rm -fr $(LINUXDIR)/drivers/char/i2c_drv.c; \
		rm -fr $(LINUXDIR)/drivers/char/i2c_drv.h; \
		rm -fr $(LINUXDIR)/drivers/char/spi_drv.c; \
		rm -fr $(LINUXDIR)/drivers/char/spi_drv.h; \
		rm -fr $(LINUXDIR)/drivers/char/pcm; \
		rm -fr $(LINUXDIR)/drivers/char/i2s; \
		rm -fr $(LINUXDIR)/drivers/char/vtss.h; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_wdt.c; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_wdt.h; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_period.c; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/rt2860v2; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/rt2860v2_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/rt2860v2_sta; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RTPCI_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3090_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5392_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5592_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3593_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/MT7610_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3572_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5572_ap; \
		rm -fr $(LINUXDIR)/net/nat/hw_nat; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/ralink_wdt.c $(LINUXDIR)/arch/mips/ralink/ralink_wdt.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/rt_timer.c $(LINUXDIR)/arch/mips/ralink/rt_timer.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/rt_timer.h $(LINUXDIR)/arch/mips/ralink/rt_timer.h; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.c $(LINUXDIR)/arch/mips/ralink/nvram.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.h $(LINUXDIR)/arch/mips/ralink/nvram.h; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/hash_utils.c $(LINUXDIR)/arch/mips/ralink/hash_utils.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/hash_utils.h $(LINUXDIR)/arch/mips/ralink/hash_utils.h; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/list_utils.h $(LINUXDIR)/arch/mips/ralink/list_utils.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/usb/dwc_otg $(LINUXDIR)/drivers/usb/dwc_otg; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/raeth $(LINUXDIR)/drivers/net/raeth; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/nfc $(LINUXDIR)/drivers/net/nfc; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/rt_rdm $(LINUXDIR)/drivers/net/rt_rdm; \
		ln -sf ../../../linux-2.6.21.x/drivers/mtd/ralink $(LINUXDIR)/drivers/mtd/ralink; \
		ln -sf ../../../../linux-2.6.21.x/drivers/mtd/maps/ralink-flash.c $(LINUXDIR)/drivers/mtd/maps/ralink-flash.c; \
		ln -sf ../../../../linux-2.6.21.x/drivers/mtd/maps/ralink-flash.h $(LINUXDIR)/drivers/mtd/maps/ralink-flash.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gpio.c $(LINUXDIR)/drivers/char/ralink_gpio.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gpio.h $(LINUXDIR)/drivers/char/ralink_gpio.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/gdma_test.c $(LINUXDIR)/drivers/char/gdma_test.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.c $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.h $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2c_drv.c $(LINUXDIR)/drivers/char/i2c_drv.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2c_drv.h $(LINUXDIR)/drivers/char/i2c_drv.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/spi_drv.c $(LINUXDIR)/drivers/char/spi_drv.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/spi_drv.h $(LINUXDIR)/drivers/char/spi_drv.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/pcm $(LINUXDIR)/drivers/char/pcm; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2s $(LINUXDIR)/drivers/char/i2s; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/vtss.h $(LINUXDIR)/drivers/char/vtss.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_wdt.c $(LINUXDIR)/drivers/watchdog/ralink_wdt.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_wdt.h $(LINUXDIR)/drivers/watchdog/ralink_wdt.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_period.c $(LINUXDIR)/drivers/watchdog/ralink_period.c; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/rt2860v2 $(LINUXDIR)/drivers/net/wireless/rt2860v2; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/rt2860v2_ap $(LINUXDIR)/drivers/net/wireless/rt2860v2_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/rt2860v2_sta $(LINUXDIR)/drivers/net/wireless/rt2860v2_sta; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RTPCI_ap $(LINUXDIR)/drivers/net/wireless/RTPCI_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3090_ap $(LINUXDIR)/drivers/net/wireless/RT3090_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5392_ap $(LINUXDIR)/drivers/net/wireless/RT5392_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5592_ap $(LINUXDIR)/drivers/net/wireless/RT5592_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3593_ap $(LINUXDIR)/drivers/net/wireless/RT3593_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/MT7610_ap $(LINUXDIR)/drivers/net/wireless/MT7610_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3572_ap $(LINUXDIR)/drivers/net/wireless/RT3572_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5572_ap $(LINUXDIR)/drivers/net/wireless/RT5572_ap; \
		ln -sf ../../../linux-2.6.21.x/net/nat/hw_nat $(LINUXDIR)/net/nat/hw_nat; \
	fi

	@if [ $(LINUXDIR) = linux-3.10.14.x -o $(LINUXDIR) = linux-3.10.20.x ] ; then \
		rm -fr $(LINUXDIR)/arch/mips/ralink/ralink_wdt.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/rt_timer.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/rt_timer.h; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/time.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/nvram.c; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/nvram.h; \
		rm -fr $(LINUXDIR)/arch/mips/ralink/pci.c; \
		rm -fr $(LINUXDIR)/drivers/usb/dwc_otg; \
		rm -fr $(LINUXDIR)/drivers/net/raeth; \
		rm -fr $(LINUXDIR)/drivers/net/nfc; \
		rm -fr $(LINUXDIR)/drivers/net/rt_rdm; \
		rm -fr $(LINUXDIR)/drivers/mtd/ralink; \
		rm -fr $(LINUXDIR)/drivers/mtd/maps/ralink-flash.c; \
		rm -fr $(LINUXDIR)/drivers/mtd/maps/ralink-flash.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/bmt.c; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/bmt.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/mt6575_nand.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/mt6575_typedefs.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/mtk_nand.c; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/mtk_nand.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/nand_def.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/nand_device_list.h; \
		rm -fr $(LINUXDIR)/drivers/mtd/nand/partition.h; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gpio.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gpio.h; \
		rm -fr $(LINUXDIR)/drivers/char/gdma_test.c; \
		rm -fr $(LINUXDIR)/drivers/char/hs_dma_test.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		rm -fr $(LINUXDIR)/drivers/char/hs_dma.c; \
		rm -fr $(LINUXDIR)/drivers/char/hs_dma.h; \
		rm -fr $(LINUXDIR)/drivers/char/i2c_drv.c; \
		rm -fr $(LINUXDIR)/drivers/char/i2c_drv.h; \
		rm -fr $(LINUXDIR)/drivers/char/spi_drv.c; \
		rm -fr $(LINUXDIR)/drivers/char/spi_drv.h; \
		rm -fr $(LINUXDIR)/drivers/char/pcm; \
		rm -fr $(LINUXDIR)/drivers/char/i2s; \
		rm -fr $(LINUXDIR)/drivers/crypto/aes_engine; \
		rm -fr $(LINUXDIR)/drivers/crypto/cryptodev; \
		rm -fr $(LINUXDIR)/drivers/char/vtss.h; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_wdt.c; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_wdt.h; \
		rm -fr $(LINUXDIR)/drivers/watchdog/ralink_period.c; \
		rm -fr $(LINUXDIR)/drivers/dma/rt_dma.c; \
		rm -fr $(LINUXDIR)/drivers/dma/rt_dma.h; \
		rm -fr $(LINUXDIR)/drivers/dma/rt_hsdma.h; \
		rm -fr $(LINUXDIR)/drivers/dma/rt_hsdma.c; \
		rm -fr $(LINUXDIR)/drivers/mmc/host/mtk-mmc; \
		rm -fr $(LINUXDIR)/drivers/net/eip93_drivers; \
		rm -fr $(LINUXDIR)/drivers/net/cryptoDriver; \
		rm -fr $(LINUXDIR)/include/net/mtk_esp.h; \
		rm -fr $(LINUXDIR)/net/ipv4/mtk_esp4.c; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/rt2860v2; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/rt2860v2_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/rt2860v2_sta; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RTPCI_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3090_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5392_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5592_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3593_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/MT7610_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT3572_ap; \
		rm -fr $(LINUXDIR)/drivers/net/wireless/RT5572_ap; \
		rm -fr $(LINUXDIR)/net/nat/hw_nat; \
		rm -fr $(LINUXDIR)/drivers/char/spdif; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/ralink_wdt.c $(LINUXDIR)/arch/mips/ralink/ralink_wdt.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/rt_timer.c $(LINUXDIR)/arch/mips/ralink/rt_timer.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/rt_timer.h $(LINUXDIR)/arch/mips/ralink/rt_timer.h; \
		ln -sf ../../../../linux-2.6.36.x/arch/mips/ralink/time.c $(LINUXDIR)/arch/mips/ralink/time.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.c $(LINUXDIR)/arch/mips/ralink/nvram.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.h $(LINUXDIR)/arch/mips/ralink/nvram.h; \
		ln -sf ../../../../linux-2.6.36.x/arch/mips/ralink/pci.c $(LINUXDIR)/arch/mips/ralink/pci.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/usb/dwc_otg $(LINUXDIR)/drivers/usb/dwc_otg; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/raeth $(LINUXDIR)/drivers/net/raeth; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/nfc $(LINUXDIR)/drivers/net/nfc; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/rt_rdm $(LINUXDIR)/drivers/net/rt_rdm; \
		ln -sf ../../../linux-2.6.21.x/drivers/mtd/ralink $(LINUXDIR)/drivers/mtd/ralink; \
		ln -sf ../../../../linux-2.6.21.x/drivers/mtd/maps/ralink-flash.c $(LINUXDIR)/drivers/mtd/maps/ralink-flash.c; \
		ln -sf ../../../../linux-2.6.21.x/drivers/mtd/maps/ralink-flash.h $(LINUXDIR)/drivers/mtd/maps/ralink-flash.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/bmt.c $(LINUXDIR)/drivers/mtd/nand/bmt.c; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/bmt.h $(LINUXDIR)/drivers/mtd/nand/bmt.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/mt6575_nand.h $(LINUXDIR)/drivers/mtd/nand/mt6575_nand.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/mt6575_typedefs.h $(LINUXDIR)/drivers/mtd/nand/mt6575_typedefs.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/mtk_nand.c $(LINUXDIR)/drivers/mtd/nand/mtk_nand.c; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/mtk_nand.h $(LINUXDIR)/drivers/mtd/nand/mtk_nand.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/nand_def.h $(LINUXDIR)/drivers/mtd/nand/nand_def.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/nand_device_list.h $(LINUXDIR)/drivers/mtd/nand/nand_device_list.h; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mtd/nand/partition.h $(LINUXDIR)/drivers/mtd/nand/partition.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gpio.c $(LINUXDIR)/drivers/char/ralink_gpio.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gpio.h $(LINUXDIR)/drivers/char/ralink_gpio.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/gdma_test.c $(LINUXDIR)/drivers/char/gdma_test.c; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/hs_dma_test.c $(LINUXDIR)/drivers/char/hs_dma_test.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.c $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.h $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/hs_dma.c $(LINUXDIR)/drivers/char/hs_dma.c; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/hs_dma.h $(LINUXDIR)/drivers/char/hs_dma.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2c_drv.c $(LINUXDIR)/drivers/char/i2c_drv.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2c_drv.h $(LINUXDIR)/drivers/char/i2c_drv.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/spi_drv.c $(LINUXDIR)/drivers/char/spi_drv.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/spi_drv.h $(LINUXDIR)/drivers/char/spi_drv.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/dma/rt_dma.c $(LINUXDIR)/drivers/dma/rt_dma.c; \
		ln -sf ../../../linux-2.6.36.x/drivers/dma/rt_dma.h $(LINUXDIR)/drivers/dma/rt_dma.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/dma/rt_hsdma.h $(LINUXDIR)/drivers/dma/rt_hsdma.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/dma/rt_hsdma.h $(LINUXDIR)/drivers/dma/rt_hsdma.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/pcm $(LINUXDIR)/drivers/char/pcm; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2s $(LINUXDIR)/drivers/char/i2s; \
		ln -sf ../../../linux-2.6.36.x/sound/soc/mtk $(LINUXDIR)/sound/soc/mtk; \
		ln -sf ../../../linux-2.6.36.x/drivers/crypto/aes_engine $(LINUXDIR)/drivers/crypto/aes_engine; \
		ln -sf ../../../linux-2.6.36.x/drivers/crypto/cryptodev $(LINUXDIR)/drivers/crypto/cryptodev; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/vtss.h $(LINUXDIR)/drivers/char/vtss.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_wdt.c $(LINUXDIR)/drivers/watchdog/ralink_wdt.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_wdt.h $(LINUXDIR)/drivers/watchdog/ralink_wdt.h; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/watchdog/ralink_period.c $(LINUXDIR)/drivers/watchdog/ralink_period.c; \
		ln -sf ../../../../linux-2.6.36.x/drivers/mmc/host/mtk-mmc $(LINUXDIR)/drivers/mmc/host/mtk-mmc; \
		ln -sf ../../../linux-2.6.36.x/drivers/net/eip93_drivers $(LINUXDIR)/drivers/net/eip93_drivers; \
		ln -sf ../../../linux-2.6.36.x/drivers/net/cryptoDriver $(LINUXDIR)/drivers/net/cryptoDriver; \
		ln -sf ../../../linux-2.6.36.x/include/net/mtk_esp.h $(LINUXDIR)/include/net/mtk_esp.h; \
		ln -sf ../../../linux-2.6.36.x/net/ipv4/mtk_esp4.c $(LINUXDIR)/net/ipv4/mtk_esp4.c; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/rt2860v2 $(LINUXDIR)/drivers/net/wireless/rt2860v2; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/rt2860v2_ap $(LINUXDIR)/drivers/net/wireless/rt2860v2_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/rt2860v2_sta $(LINUXDIR)/drivers/net/wireless/rt2860v2_sta; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RTPCI_ap $(LINUXDIR)/drivers/net/wireless/RTPCI_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3090_ap $(LINUXDIR)/drivers/net/wireless/RT3090_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5392_ap $(LINUXDIR)/drivers/net/wireless/RT5392_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5592_ap $(LINUXDIR)/drivers/net/wireless/RT5592_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3593_ap $(LINUXDIR)/drivers/net/wireless/RT3593_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/MT7610_ap $(LINUXDIR)/drivers/net/wireless/MT7610_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT3572_ap $(LINUXDIR)/drivers/net/wireless/RT3572_ap; \
		ln -sf ../../../../linux-2.6.21.x/drivers/net/wireless/RT5572_ap $(LINUXDIR)/drivers/net/wireless/RT5572_ap; \
		mkdir -p $(LINUXDIR)/net/nat; \
		ln -sf ../../../linux-2.6.21.x/net/nat/hw_nat $(LINUXDIR)/net/nat/hw_nat; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/spdif $(LINUXDIR)/drivers/char/spdif; \
	fi

	@if [ $(LINUXDIR) = linux-3.10.20.x ] ; then \
		unlink $(LINUXDIR)/drivers/mtd/nand/mt6575_nand.h; \
		unlink $(LINUXDIR)/drivers/mtd/nand/mt6575_typedefs.h; \
		unlink $(LINUXDIR)/drivers/mtd/nand/mtk_nand.c; \
		unlink $(LINUXDIR)/drivers/mtd/nand/mtk_nand.h; \
		unlink $(LINUXDIR)/drivers/mtd/nand/partition.h; \
		unlink $(LINUXDIR)/drivers/mtd/nand/nand_def.h; \
		unlink $(LINUXDIR)/drivers/mtd/nand/nand_device_list.h; \
		unlink $(LINUXDIR)/drivers/net/eip93_drivers; \
		unlink $(LINUXDIR)/drivers/net/cryptoDriver; \
		rm -fr $(LINUXDIR)/arch/arm/mach-mt7623/nvram.c; \
		rm -fr $(LINUXDIR)/arch/arm/mach-mt7623/nvram.h; \
		rm -fr $(LINUXDIR)/drivers/net/ethernet/broadcom/tg3-3.133d; \
		ln -sf ../../../../../linux-3.10.14.x/drivers/net/ethernet/broadcom/tg3-3.133d $(LINUXDIR)/drivers/net/ethernet/broadcom/; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.c $(LINUXDIR)/arch/arm/mach-mt7623/nvram.c; \
		ln -sf ../../../../linux-2.6.21.x/arch/mips/rt2880/nvram.h $(LINUXDIR)/arch/arm/mach-mt7623/nvram.h; \
	fi

	@if [ $(LINUXDIR) = linux-3.4.x ] ; then \
                rm -fr $(LINUXDIR)/mediatek/platform; \
                rm -fr $(LINUXDIR)/mediatek/kernel; \
		rm -fr $(LINUXDIR)/net/nat/hw_nat; \
		rm -fr $(LINUXDIR)/drivers/net/raeth; \
		rm -fr $(LINUXDIR)/drivers/net/rt_rdm; \
		rm -fr $(LINUXDIR)/drivers/char/pcm; \
		rm -fr $(LINUXDIR)/drivers/char/i2s; \
		rm -fr $(LINUXDIR)/drivers/char/gdma_test.c; \
		rm -fr $(LINUXDIR)/drivers/char/hs_dma_test.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		rm -fr $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		rm -fr $(LINUXDIR)/drivers/char/hs_dma.c; \
		rm -fr $(LINUXDIR)/drivers/char/hs_dma.h; \
		rm -fr $(LINUXDIR)/drivers/char/mem_test.c; \
		rm -fr $(LINUXDIR)/drivers/net/ethernet/broadcom/tg3-3.133d; \
		rm -fr kernel; \
                ln -sf ../../mediatek/platform $(LINUXDIR)/mediatek/platform; \
                ln -sf ../../mediatek/kernel $(LINUXDIR)/mediatek/kernel; \
		ln -sf ../../../linux-2.6.21.x/net/nat/hw_nat $(LINUXDIR)/net/nat/hw_nat; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/raeth $(LINUXDIR)/drivers/net/raeth; \
		ln -sf ../../../linux-2.6.21.x/drivers/net/rt_rdm $(LINUXDIR)/drivers/net/rt_rdm; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/pcm $(LINUXDIR)/drivers/char/pcm; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/i2s $(LINUXDIR)/drivers/char/i2s; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/gdma_test.c $(LINUXDIR)/drivers/char/gdma_test.c; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/hs_dma_test.c $(LINUXDIR)/drivers/char/hs_dma_test.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.c $(LINUXDIR)/drivers/char/ralink_gdma.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gdma.h $(LINUXDIR)/drivers/char/ralink_gdma.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/hs_dma.c $(LINUXDIR)/drivers/char/hs_dma.c; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/hs_dma.h $(LINUXDIR)/drivers/char/hs_dma.h; \
		ln -sf ../../../linux-2.6.36.x/drivers/char/mem_test.c $(LINUXDIR)/drivers/char/mem_test.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gpio.c $(LINUXDIR)/drivers/char/ralink_gpio.c; \
		ln -sf ../../../linux-2.6.21.x/drivers/char/ralink_gpio.h $(LINUXDIR)/drivers/char/ralink_gpio.h; \
		ln -sf linux-3.4.x kernel; \
		ln -sf ../../../../../linux-3.10.14.x/drivers/net/ethernet/broadcom/tg3-3.133d $(LINUXDIR)/drivers/net/ethernet/broadcom/; \
	fi

# This one removes all executables from the tree and forces their relinking
.PHONY: relink
relink:
	find $(UCLIBC_APP) $(GLIBC_APP) prop vendors -type f -name '*.gdb' | sed 's/^\(.*\)\.gdb/\1 \1.gdb/' | xargs rm -f

clean: modules_clean
	for dir in $(LINUXDIR) $(DIRS); do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir clean ; done
	if [ -f $(LINUXDIR)/System.map ]; then \
		rm -f $(LINUXDIR)/System.map; \
		rm -f $(LINUXDIR)/vmlinux*; \
	fi
	rm -f $(IMAGEDIR)/*
	rm -f config.tk
	rm -f $(LINUXDIR)/linux
	rm -rf $(LINUXDIR)/net/ipsec/alg/libaes $(LINUXDIR)/net/ipsec/alg/perlasm
	rm -f $(LINUXDIR)/arch/mips/ramdisk/*.gz
	@if [ $(LINUXDIR) = linux-3.10.14.x -o $(LINUXDIR) = linux-3.10.20.x  -o $(LINUXDIR) = linux-3.10.54.x ] ; then \
		rm -rf $(KERNEL_HEADERS); \
	fi


real_clean mrproper: clean
	-$(MAKEARCH_KERNEL) -C $(LINUXDIR) mrproper
	-$(MAKEARCH) -C config clean
	-$(MAKEARCH) -C $(LIB) distclean
	-$(MAKEARCH) -C $(BUSYBOX_APP)) mrproper
	make -C $(UCLIBC_APP)/cmake distclean
	rm -rf config.in config.arch config.tk images
	rm -f modules/config.tk
	rm -rf .config .config.old .oldconfig autoconf.h

distclean: mrproper
	-$(MAKEARCH_KERNEL) -C $(LINUXDIR) distclean
	-rm -f user/tinylogin/applet_source_list user/tinylogin/config.h

%_only:
	rm -fr $(ROOTDIR)/uClibc-0.9.33.2/include/config/autoconf.h
	@case "$(@)" in \
	*/*) d=`expr $(@) : '\([^/]*\)/.*'`; \
	     t=`expr $(@) : '[^/]*/\(.*\)'`; \
	     $(MAKEARCH) -C $$d $$t;; \
	*)   $(MAKEARCH) -C $(@:_only=);; \
	esac

%_clean:
	@case "$(@)" in \
	*/*) d=`expr $(@) : '\([^/]*\)/.*'`; \
	     t=`expr $(@) : '[^/]*/\(.*\)'`; \
	     $(MAKEARCH) -C $$d $$t;; \
	*)   $(MAKEARCH) -C $(@:_clean=) clean;; \
	esac

%_default:
	@if [ ! -f "vendors/$(@:_default=)/config.device" ]; then \
		echo "vendors/$(@:_default=)/config.device must exist first"; \
		exit 1; \
	 fi
	-make clean > /dev/null 2>&1
	cp vendors/$(@:_default=)/config.device .config
	chmod u+x config/setconfig
	yes "" | config/setconfig defaults
	config/setconfig final
	make dep
	make

config_error:
	@echo "*************************************************"
	@echo "You have not run make config."
	@echo "The build sequence for this source tree is:"
	@echo "1. 'make config' or 'make xconfig'"
	@echo "2. 'make dep'"
	@echo "3. 'make'"
	@echo "*************************************************"
	@exit 1

prune: ucfront
	@for i in `ls -d linux-* | grep -v $(LINUXDIR)`; do \
		rm -fr $$i; \
	done
	$(MAKE) -C $(UCLIBC_APP) prune
	$(MAKE) -C $(GLIBC_APP) prune
	$(MAKE) -C vendors prune

dist-prep:
	-find $(ROOTDIR) -name 'Makefile*.bin' | while read t; do \
		$(MAKEARCH) -C `dirname $$t` -f `basename $$t` $@; \
	 done

############################################################################
