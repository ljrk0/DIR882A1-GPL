CC    := $(PRC_CROSS_COMPILE)gcc
AR    := $(PRC_CROSS_COMPILE)ar
link  := $(PRC_CROSS_COMPILE)gcc
STRIP := $(PRC_CROSS_COMPILE)strip

CFLAGS := $(CFLAGS) -g -fPIC

JSON_LIB_PATH = ./
JSON_LIB      := $(JSON_LIB_PATH)/libjson.a
JSON_SRC_PATH := .
JSON_OBJ_PATH := $(JSON_LIB_PATH)

JSON_SRCS = $(CJSON_SRC_PATH)/json.c
JSON_OBJS := $(addprefix $(JSON_OBJ_PATH)/,$(patsubst %.c,%.o,$(notdir $(JSON_SRCS))))

# command ruler
all : $(JSON_LIB)

$(JSON_LIB): $(JSON_OBJS)
	$(AR) -r $(JSON_LIB) $(JSON_OBJS)

$(JSON_OBJ_PATH)/%.o : $(JSON_SRC_PATH)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY:clean
clean:
	$(RM) -rf $(JSON_LIB)
	$(RM) -rf $(JSON_OBJS)
	$(RM) -rf testJson

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR json lib"
	$(RM) -rf $(JSON_SRC_PATH)/*.c
	$(RM) -rf $(JSON_LIB)
	$(RM) -rf $(JSON_OBJS)
	$(RM) -rf testJson

.PHONY:test
test : $(JSON_LIB)
	$(CC) $(CFLAGS) test.c $(JSON_LIB) -lm -o testJson
