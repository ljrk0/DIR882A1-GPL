TOPDIR=../
include $(TOPDIR)Rules.mak

LIBZLIB_SRC=lua-5.1.5
PKG_SOURCE:=lua-5.1.5.tar.gz
DIRS := ${LIBZLIB_SRC}

LIB_SHARED_FULLNAME=liblua.so.5.1.5
LIB_SHARED=liblua.so

TARGET_CROSS := $(CROSS_COMPILE)
TARGET_CC := $(CC)
TARGET_CXX := $(CXX)
TARGET_ASFLAGS += -Os -Wall -DLUA_USE_LINUX -pipe -DLINUX -fpic -std=gnu99
TARGET_CFLAGS += -Os -Wall -DLUA_USE_LINUX -pipe -DLINUX -fpic -std=gnu99

TARGET_CXXFLAGS += $(TARGET_CPPFLAGS)

TARGET_CONFIGURE_OPTS = \
  AR="$(TARGET_CROSS)ar rcu" \
  AS="$(TARGET_CC) -c $(TARGET_ASFLAGS)" \
  LD=$(TARGET_CROSS)ld \
  NM=$(TARGET_CROSS)nm \
  CC=$(TARGET_CC) \
  GCC=$(TARGET_CC) \
  CXX=$(TARGET_CXX) \
  RANLIB=$(TARGET_CROSS)ranlib \
  STRIP=$(TARGET_CROSS)strip \
  OBJCOPY=$(TARGET_CROSS)objcopy \
  OBJDUMP=$(TARGET_CROSS)objdump \
  SIZE=$(TARGET_CROSS)size

CONFIGURE_VARS = \
                $(TARGET_CONFIGURE_OPTS) \
                CFLAGS="$(TARGET_CFLAGS)" \
                CXXFLAGS="$(TARGET_CXXFLAGS)" \
                CPPFLAGS="$(TARGET_CPPFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS)" 

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

all:  Makefile
	if [ ! -x $(LIBZLIB_SRC)/src/liblua.so ]; then \
	( \
	 $(MAKE) -C $(LIBZLIB_SRC) $(CONFIGURE_VARS) MYCFLAGS="-DLUA_USE_LINUX " MYLDFLAGS="-DLUA_USE_LINUX " MYLIBS="-Wl,-E -ldl " PKG_VERSION=5.1.5 linux \
	) || exit 1; \
	fi

Makefile:
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  tar -xvf $(PKG_SOURCE); \
	fi
	
	if [ ! -d $(LIBZLIB_SRC) ]; then\
	  echo "tar \"$(PKG_SOURCE)\" not exists! "; \
	  exit 1;  \
	fi;

# the is a proxy section --YY
all-recursive: Makefile
	make -C lua-5.1.5 all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -C lua-5.1.5 all-am

# the is a proxy section --YY
clean-am: Makefile
	make -C lua-5.1.5 clean-am

clean: Makefile
	make -C lua-5.1.5 clean

romfs:

shared: all
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIB_SHARED)*
	$(INSTALL) -m 644 lua-5.1.5/src/$(LIB_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIB_SHARED)


