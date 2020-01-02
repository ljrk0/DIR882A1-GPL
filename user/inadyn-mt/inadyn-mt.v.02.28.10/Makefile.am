## Process this file with automake to produce Makefile.in

EXTRA_DIST = bin lang extra prj readme.html NOTICE

SUBDIRS = src man
DIST_SUBDIRS = src man

#ACLOCAL_AMFLAGS = -I /usr/share/aclocal -I /usr/local/share/aclocal -I ./m4
ACLOCAL_AMFLAGS = -I ./m4

DISTCHECK_CONFIGURE_FLAGS = --enable-gcc-werror

install-data-hook: 

if ! IS_INADYN_MT_DIR

	mkdir $(INSTALL_PREFIX)/inadyn-mt
endif

if ! IS_INADYN_MT_LANG_DIR

	mkdir $(INSTALL_PREFIX)/inadyn-mt/lang

	cp ./lang/en.lng $(INSTALL_PREFIX)/inadyn-mt/lang
endif

if ! IS_INADYN_MT_EXTRA_DIR

	mkdir $(INSTALL_PREFIX)/inadyn-mt/extra

	mkdir $(INSTALL_PREFIX)/inadyn-mt/extra/wav

	cp ./extra/wav/alarm.wav $(INSTALL_PREFIX)/inadyn-mt/extra/wav
	cp ./extra/wav/bird-calls.wav $(INSTALL_PREFIX)/inadyn-mt/extra/wav

	cp ./extra/servers_additional.cfg $(INSTALL_PREFIX)/inadyn-mt/extra
endif

uninstall-hook:

if ! IS_INADYN_MT_DIR

	rm -rf $(INSTALL_PREFIX)/inadyn-mt
else

if ! IS_INADYN_MT_LANG_DIR

	rm -rf $(INSTALL_PREFIX)/inadyn-mt/lang

else

if ! IS_INADYN_MT_EXTRA_DIR

	rm -rf $(INSTALL_PREFIX)/inadyn-mt/extra
endif
endif
endif

test: check-recursive

