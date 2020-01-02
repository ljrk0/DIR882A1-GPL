ifndef PRC_CROSS_COMPILE
$(warning  build-tool-gcc-compile)
LIB_OPENSSL_DEP_LDLIB := -L../03.gcc_libs -lcrypto
else
$(warning  build-tool-cross-compile)
#LIB_OPENSSL_DEP_LDLIB := -L../02.mips_libs -lcrypto
LIB_OPENSSL_DEP_LDLIB := -L../../../../openssl-1.0.2j -lcrypto
endif

CC := $(PRC_CROSS_COMPILE)gcc
AR := $(PRC_CROSS_COMPILE)ar
link := $(PRC_CROSS_COMPILE)gcc
STRIP := $(PRC_CROSS_COMPILE)strip

PRO_OBJS_PATH := ./obj
PRO_LIB_PATH := ./lib

#LIB_OPENSSL_INCLUDES := -I ./inc -I ../01.include
LIB_OPENSSL_INCLUDES := -I ./inc -I ../../../../openssl-1.0.2j -I ../../../../openssl-1.0.2j/apps  -I ../../../../openssl-1.0.2j/include
LIB_OPENSSL_CFLAGS := $(CFLAGS) -fPIC
LIB_OPENSSL_LIB_CFLAGS :=  -O -fPIC -shared -o

LIB_OPENSSL_LIB := $(PRO_LIB_PATH)/libopenssl.so

LIB_OPENSSL_SRC_PATH = ./src

LIB_OPENSSL_SRC := $(wildcard $(LIB_OPENSSL_SRC_PATH)/*.c)
LIB_OPENSSL_OBJS := $(patsubst $(LIB_OPENSSL_SRC_PATH)/%,$(PRO_OBJS_PATH)/%,$(patsubst %.c,%.o,$(LIB_OPENSSL_SRC)))

$(LIB_OPENSSL_LIB):$(LIB_OPENSSL_OBJS)
	$(CC) $(LIB_OPENSSL_LIB_CFLAGS) $(LIB_OPENSSL_LIB) $(LIB_OPENSSL_OBJS) $(LIB_OPENSSL_DEP_LDLIB)

$(LIB_OPENSSL_OBJS):$(PRO_OBJS_PATH)/%.o:$(LIB_OPENSSL_SRC_PATH)/%.c
	$(CC) $(LIB_OPENSSL_CFLAGS) $(LIB_OPENSSL_INCLUDES) $(OAM_BUILDS) -pthread -c $< -o $@

.PHONY:all clean
all:$(LIB_OPENSSL_LIB)

clean:
	$(RM) $(LIB_OPENSSL_OBJS)
	$(RM) $(LIB_OPENSSL_LIB)

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR oam_libopenssl"
	$(RM) $(LIB_OPENSSL_SRC_PATH)/*
	$(RM) ./lib/*
