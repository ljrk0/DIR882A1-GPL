COMMON_DIR = ../../01.common
OPENSSL_DIR = ../05.openssl
OPENSSL_SOURCE_DIR = ../../../openssl-1.0.2j

ifeq ($(PRC_CROSS_COMPILE),)
   XML_LIB_PATH=$(COMMON_DIR)/dm_lib/libxml2/gcc_lib
else
	XML_LIB_PATH=$(COMMON_DIR)/dm_lib/libxml2/mips_lib
endif

CC    := $(PRC_CROSS_COMPILE)gcc
AR    := $(PRC_CROSS_COMPILE)ar
link  := $(PRC_CROSS_COMPILE)gcc
STRIP := $(PRC_CROSS_COMPILE)strip

TIME = `date -u +%Y-%m-%dT%H:%M:%SZ`
DM_SRV_BUILDS := -D BUILD_TIME="\"$(TIME)\""

CFLAGS := -g -Wall -Wshadow -Wcast-qual
ifndef COMPILE_VERSION
COMPILE_VERSION := undefined
endif
PRO_COMPILE_MODE = -DBRCM_DMSRV_REVISION="\"$(COMPILE_VERSION)\""

PRO_OBJS_PATH := ./obj
PRO_LIBS_PATH := ./lib
PRO_SRC_PATH  := ./src
DM_SRV_BIN    := ./lib/dmsrv

DM_SRV_INC = -I./inc \
             -I$(COMMON_DIR)/include/ \
             -I$(COMMON_DIR)/osa/inc/ \
             -I$(COMMON_DIR)/dev_lib/inc/ \
             -I$(COMMON_DIR)/perview_log/inc/ \
             -I$(COMMON_DIR)/debug_lib/inc/  \
             -I$(COMMON_DIR)/dm_lib/external_type \
             -I$(COMMON_DIR)/dm_lib/common/inc/  \
             -I$(OPENSSL_SOURCE_DIR)/include \
             -I$(OPENSSL_DIR)/04.libopenssl/inc

DM_SRV_LDFLAG = -L$(COMMON_DIR)/dm_lib/lib -ldm -lDbConv
#DM_SRV_LDFLAG += -L$(COMMON_DIR)/perview_log/lib -lperview
DM_SRV_LDFLAG += -L$(COMMON_DIR)/osa/lib -losa
#DM_SRV_LDFLAG += -L$(COMMON_DIR)/dev_lib/lib -ldevs
DM_SRV_LDFLAG += -L$(XML_LIB_PATH) -lxml2
DM_SRV_LDFLAG += -L ../../../openssl-1.0.2j -lcrypto 
DM_SRV_LDFLAG += -L$(OPENSSL_DIR)/04.libopenssl/lib -lopenssl
DM_SRV_LDFLAG += -lpthread -lrt -ldl -lm

DM_SRV_BASE = dmsrv_monitor.c   \
              dmsrv_listener.c  \
              dmsrv_common.c

DM_SRV_SRC := $(addprefix $(PRO_SRC_PATH)/, $(DM_SRV_BASE))
DM_SRV_OBJ := $(addprefix $(PRO_OBJS_PATH)/,$(patsubst %.c,%.o,$(notdir $(DM_SRV_SRC))))

all : $(DM_SRV_BIN)

# command ruler
$(DM_SRV_BIN): $(DM_SRV_OBJ)
	$(CC) $(CFLAGS) -rdynamic -o $(DM_SRV_BIN) $(DM_SRV_OBJ) $(DM_SRV_LDFLAG)

$(PRO_OBJS_PATH)/%.o : $(PRO_SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(DM_SRV_INC) -c $< -o $@  $(PRO_COMPILE_MODE)

.PHONY:clean
clean:
	$(RM) -rf $(DM_SRV_BIN)
	$(RM) -rf $(DM_SRV_OBJ)

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR dmsrv"
	$(RM) -rf $(PRO_SRC_PATH)/*.c
	$(RM) -rf $(DM_SRV_BIN)
	$(RM) -rf $(DM_SRV_OBJ)
