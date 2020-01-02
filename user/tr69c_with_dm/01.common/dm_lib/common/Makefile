#simulator need this lib, if compiling simulator ,do NOT cross-compile
ifndef PRC_CROSS_COMPILE
ifdef CN_BUILDS
PRC_CROSS_COMPILE := 
else
PRC_CROSS_COMPILE := /opt/buildroot-gcc463/usr/bin/mipsel-linux-
endif
endif

CC := $(PRC_CROSS_COMPILE)gcc
AR := $(PRC_CROSS_COMPILE)ar
link := $(PRC_CROSS_COMPILE)gcc
STRIP := $(PRC_CROSS_COMPILE)strip

#ifndef RESULT_PATH
RESULT_PATH := ../lib
#endif
OBJ_PATH := ../obj

DM_INC_PATH = ./inc
DM_SRCS_PATH = ./src
LIBXML2_INC_PATH = ../libxml2/include

OSA_INCLUDES = ../../osa/inc
COM_INCLUDES = ../../include
DBG_INCLUDES = ../../debug_lib/inc
DMALLOC_INCLUDES = ../../dmalloc/

DM_INCLUDES := -I$(DM_INC_PATH) -I$(OSA_INCLUDES) -I$(COM_INCLUDES) -I$(LIBXML2_INC_PATH) -I$(DBG_INCLUDES) -I $(DMALLOC_INCLUDES)


DM_LIB = $(RESULT_PATH)/libdm.so
DM_STATIC_LIB = $(RESULT_PATH)/libdm.a

DM_OBJ_CFLAGS := -g -Wall -Wshadow -Wcast-qual -fPIC
DM_LIB_CFLAGS := -g -Wall -Wshadow -Wcast-qual -O -shared -o


OBJ_PATH_DMALLOC=../obj_dmalloc/
DM_LIB_DMALLOC = $(RESULT_PATH)/libdm_dmalloc.so
DM_LIB_STATIC_DMALLOC = $(RESULT_PATH)/libdm_dmalloc.a
DM_OBJ_CFLAGS_DMALLOC := $(DM_OBJ_CFLAGS) -DDMALLOC_CHECK_ENABLED



DM_OBJS := $(OBJ_PATH)/dm_api.o \
						$(OBJ_PATH)/dm_common.o \
						$(OBJ_PATH)/dm_hash.o \
						$(OBJ_PATH)/dm_trace.o \
						$(OBJ_PATH)/dm_xml.o \
						$(OBJ_PATH)/dm_sock_msg.o \
						$(OBJ_PATH)/dm_gsad.o \
						$(OBJ_PATH)/dm_srv.o

DM_OBJS_DMALLOC := $(OBJ_PATH_DMALLOC)/dm_api.o \
						$(OBJ_PATH_DMALLOC)/dm_common.o \
						$(OBJ_PATH_DMALLOC)/dm_hash.o \
						$(OBJ_PATH_DMALLOC)/dm_trace.o \
						$(OBJ_PATH_DMALLOC)/dm_xml.o \
						$(OBJ_PATH_DMALLOC)/dm_sock_msg.o \
						$(OBJ_PATH_DMALLOC)/dm_gsad.o \
						$(OBJ_PATH_DMALLOC)/dm_srv.o

ifdef BRCM_DMALLOC_ENABLED 
all : $(DM_LIB) $(DM_STATIC_LIB) $(DM_LIB_DMALLOC) $(DM_LIB_STATIC_DMALLOC)
else
all : $(DM_LIB) $(DM_STATIC_LIB)
endif

$(DM_LIB) : $(DM_OBJS)
	$(CC) $(DM_LIB_CFLAGS) $(DM_LIB) $(DM_OBJS)

$(DM_STATIC_LIB) : $(DM_OBJS)
	$(AR) -r $(DM_STATIC_LIB) $(DM_OBJS)

$(DM_LIB_DMALLOC) : $(DM_OBJS_DMALLOC)
	$(CC) $(DM_LIB_CFLAGS) $(DM_LIB_DMALLOC) $(DM_OBJS_DMALLOC)

$(DM_LIB_STATIC_DMALLOC) : $(DM_OBJS_DMALLOC)
	$(AR) -r $(DM_LIB_STATIC_DMALLOC) $(DM_OBJS_DMALLOC)

$(OBJ_PATH)/dm_api.o: ./src/dm_api.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH)/dm_common.o: ./src/dm_common.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH)/dm_hash.o: ./src/dm_hash.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH)/dm_trace.o: ./src/dm_trace.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH)/dm_xml.o: ./src/dm_xml.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@

$(OBJ_PATH)/dm_sock_msg.o: ./src/dm_sock_msg.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@

$(OBJ_PATH)/dm_gsad.o: ./src/dm_gsad.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@

$(OBJ_PATH)/dm_srv.o: ./src/dm_srv.c
	$(CC) $(DM_OBJ_CFLAGS) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH_DMALLOC)/dm_api.o: ./src/dm_api.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH_DMALLOC)/dm_common.o: ./src/dm_common.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH_DMALLOC)/dm_hash.o: ./src/dm_hash.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH_DMALLOC)/dm_trace.o: ./src/dm_trace.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@
	
$(OBJ_PATH_DMALLOC)/dm_xml.o: ./src/dm_xml.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@

$(OBJ_PATH_DMALLOC)/dm_sock_msg.o: ./src/dm_sock_msg.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@

$(OBJ_PATH_DMALLOC)/dm_gsad.o: ./src/dm_gsad.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@

$(OBJ_PATH_DMALLOC)/dm_srv.o: ./src/dm_srv.c
	$(CC) $(DM_OBJ_CFLAGS_DMALLOC) $(DM_INCLUDES) -pthread -c $< -o $@
	
.PHONY:clean
clean:
	$(RM) $(DM_LIB) $(DM_STATIC_LIB) $(DM_OBJS) $(DM_LIB_DMALLOC) $(DM_LIB_STATIC_DMALLOC) $(DM_OBJS_DMALLOC)

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR db_common"
	@$(RM) -rf ./src/*
	@$(RM) -rf ../obj/*

