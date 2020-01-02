CC    := $(PRC_CROSS_COMPILE)gcc
AR    := $(PRC_CROSS_COMPILE)ar
link  := $(PRC_CROSS_COMPILE)gcc
STRIP := $(PRC_CROSS_COMPILE)strip

CFLAGS := $(CFLAGS) -g -Wall -Wshadow -Wcast-qual -fPIC
LDLAGS := $(LDLAGS) -g -Wall -Wshadow -Wcast-qual -O -shared -o

DMJSON_LIB = ../lib/DmJsonAddon.so
DMJSON_BASE_SRCS := dmJson_main.c  dmJson_comm.c dmJson_Json.c

COMMON_DIR  = ../..
JSON_LIB_PATH=./jsonlib
JSON_LIB=$(JSON_LIB_PATH)/libjson.a

ifeq ($(PRC_CROSS_COMPILE),)
   XML_LIB_PATH=$(COMMON_DIR)/dm_lib/libxml2/gcc_lib
else
   XML_LIB_PATH=$(COMMON_DIR)/dm_lib/libxml2/mips_lib
endif

DMJSON_INC = -I./inc -I./jsonlib/ \
             -I$(COMMON_DIR)/include/ \
             -I$(COMMON_DIR)/osa/inc/ \
             -I$(COMMON_DIR)/dm_lib/external_type \
             -I$(COMMON_DIR)/dm_lib/common/inc/  \
             -I$(COMMON_DIR)/dm_lib/libxml2/include

DMJSON_LDFLAG = -L$(COMMON_DIR)/dm_lib/lib -ldm -lDbConv
DMJSON_LDFLAG += -L$(COMMON_DIR)/osa/lib -losa
DMJSON_LDFLAG += -lm

DMJSON_SRC_PATH := ./src
DMJSON_OBJ_PATH := ./obj

DMJSON_SRCS := $(addprefix $(DMJSON_SRC_PATH)/, $(DMJSON_BASE_SRCS))
DMJSON_OBJS := $(addprefix $(DMJSON_OBJ_PATH)/,$(patsubst %.c,%.o,$(notdir $(DMJSON_SRCS))))

# command ruler
all : $(DMJSON_LIB)

$(DMJSON_LIB): $(DMJSON_OBJS) $(JSON_LIB)
	$(CC) $(LDLAGS) $(DMJSON_LIB) $(DMJSON_OBJS) $(JSON_LIB)

$(DMJSON_OBJ_PATH)/%.o : $(DMJSON_SRC_PATH)/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $(DMJSON_INC) -c $< -o $@ 

$(JSON_LIB):
	@cd $(JSON_LIB_PATH); make clean; make all PRC_CROSS_COMPILE=$(PRC_CROSS_COMPILE)

.PHONY:clean
clean:
	$(RM) -rf $(DMJSON_LIB)
	$(RM) -rf $(DMJSON_OBJS)
	$(RM) -rf $(JSON_LIB)
	$(RM) -rf dmjson

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR DmJson"
	$(RM) -rf $(CJSON_SRC_PATH)/*.c
	$(RM) -rf $(DMJSON_LIB)
	$(RM) -rf $(DMJSON_OBJS)
	$(RM) -rf $(JSON_LIB)
	$(RM) -rf dmjson

.PHONY:test
test : $(DMJSON_LIB)
	$(CC) $(CFLAGS) $(DMJSON_INC) $(DMJSON_SRC_PATH)/dmJson_test.c $(DMJSON_LIB) $(DMJSON_LDFLAG) -o dmjson \
	-I$(COMMON_DIR)/perview_log/inc  -L$(COMMON_DIR)/perview_log/lib  -lperview \
	-L$(XML_LIB_PATH) -lxml2 -lpthread -ldl -lrt -lm
