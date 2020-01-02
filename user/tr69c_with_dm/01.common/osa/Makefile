ifndef PRC_CROSS_COMPILE
ifdef CN_BUILDS
PRC_CROSS_COMPILE := 
else
PRC_CROSS_COMPILE := /opt/buildroot-gcc463/usr/bin/mipsel-linux-
endif
endif
CC := $(PRC_CROSS_COMPILE)gcc
AR := $(PRC_CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip
link := $(PRC_CROSS_COMPILE)gcc

OSA_OBJS_PATH := ./obj
OSA_SRC_PATH := ./src

#ifndef RESULT_PATH
RESULT_PATH := ./lib/
#endif
OSA_LIB := $(RESULT_PATH)/libosa.so
OSA_STATIC_LIB := $(RESULT_PATH)/libosa.a

OSA_LIB_CFLAGS := -g -Wall -Wshadow -Wcast-qual -O -shared -o
OSA_OBJ_CFLAGS := -g -Wall -Wshadow -Wcast-qual -fPIC
INCLUDES_OSA := -I ./inc -I ../include -I ../perview_log/inc -I ../dmalloc

OSA_OBJS_PATH_DMALLOC=./obj_dmalloc/

OSA_LIB_DMALLOC = $(RESULT_PATH)/libosa_dmalloc.so
OSA_STATIC_LIB_DMALLOC := $(RESULT_PATH)/libosa_dmalloc.a
OSA_OBJ_CFLAGS_DMALLOC := $(OSA_OBJ_CFLAGS) -DDMALLOC_CHECK_ENABLED

OSA_OBJS := $(OSA_OBJS_PATH)/osa_debug.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_event.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_resource.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_mem.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_msg_queue.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_mutex.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_sem.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_shm.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_thread.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_thread_pool.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_timer.o
OSA_OBJS += $(OSA_OBJS_PATH)/osa_getaddrinfo.o

OSA_OBJS_DMALLOC := $(OSA_OBJS_PATH_DMALLOC)/osa_debug.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_event.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_resource.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_mem.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_msg_queue.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_mutex.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_sem.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_shm.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_thread.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_thread_pool.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_timer.o
OSA_OBJS_DMALLOC += $(OSA_OBJS_PATH_DMALLOC)/osa_getaddrinfo.o


#target
ifdef BRCM_DMALLOC_ENABLED 
all : $(OSA_LIB) $(OSA_STATIC_LIB) $(OSA_LIB_DMALLOC) $(OSA_STATIC_LIB_DMALLOC)
else
all : $(OSA_LIB) $(OSA_STATIC_LIB)
endif

$(OSA_LIB):$(OSA_OBJS)
	$(CC) $(OSA_LIB_CFLAGS) $(OSA_LIB) $(OSA_OBJS)

$(OSA_STATIC_LIB):$(OSA_OBJS)
	$(AR) -r $(OSA_STATIC_LIB) $(OSA_OBJS)

$(OSA_LIB_DMALLOC):$(OSA_OBJS_DMALLOC)
	$(CC) $(OSA_LIB_CFLAGS) $(OSA_LIB_DMALLOC) $(OSA_OBJS_DMALLOC)

$(OSA_STATIC_LIB_DMALLOC):$(OSA_OBJS_DMALLOC)
	$(AR) -r $(OSA_STATIC_LIB_DMALLOC) $(OSA_OBJS_DMALLOC)

$(OSA_OBJS_PATH)/osa_event.o: 	$(OSA_SRC_PATH)/osa_event.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_event.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH)/osa_resource.o: 	$(OSA_SRC_PATH)/osa_resource.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_resource.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH)/osa_debug.o: 	$(OSA_SRC_PATH)/osa_debug.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH)/osa_mem.o: 	$(OSA_SRC_PATH)/osa_mem.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_mem.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_msg_queue.o: 	$(OSA_SRC_PATH)/osa_msg_queue.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_msg_queue.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_mutex.o: 	$(OSA_SRC_PATH)/osa_mutex.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_mutex.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_sem.o: 	$(OSA_SRC_PATH)/osa_sem.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_sem.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_shm.o: 	$(OSA_SRC_PATH)/osa_shm.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_shm.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_thread.o: 	$(OSA_SRC_PATH)/osa_thread.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_thread.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_thread_pool.o: 	$(OSA_SRC_PATH)/osa_thread_pool.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_thread_pool.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH)/osa_timer.o: 	$(OSA_SRC_PATH)/osa_timer.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_timer.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH)/osa_getaddrinfo.o: 	$(OSA_SRC_PATH)/osa_getaddrinfo.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_getaddrinfo.h
	$(CC) $(OSA_OBJ_CFLAGS) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH_DMALLOC)/osa_event.o: 	$(OSA_SRC_PATH)/osa_event.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_event.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH_DMALLOC)/osa_resource.o: 	$(OSA_SRC_PATH)/osa_resource.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_resource.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH_DMALLOC)/osa_debug.o: 	$(OSA_SRC_PATH)/osa_debug.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH_DMALLOC)/osa_mem.o: 	$(OSA_SRC_PATH)/osa_mem.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_mem.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_msg_queue.o: 	$(OSA_SRC_PATH)/osa_msg_queue.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_msg_queue.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_mutex.o: 	$(OSA_SRC_PATH)/osa_mutex.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_mutex.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_sem.o: 	$(OSA_SRC_PATH)/osa_sem.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_sem.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_shm.o: 	$(OSA_SRC_PATH)/osa_shm.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_shm.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_thread.o: 	$(OSA_SRC_PATH)/osa_thread.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_thread.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_thread_pool.o: 	$(OSA_SRC_PATH)/osa_thread_pool.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_thread_pool.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@
	
$(OSA_OBJS_PATH_DMALLOC)/osa_timer.o: 	$(OSA_SRC_PATH)/osa_timer.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_timer.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@

$(OSA_OBJS_PATH_DMALLOC)/osa_getaddrinfo.o: 	$(OSA_SRC_PATH)/osa_getaddrinfo.c \
								../include/common_type_def.h \
								./inc/osa.h \
								./inc/osa_debug.h \
								./inc/osa_getaddrinfo.h
	$(CC) $(OSA_OBJ_CFLAGS_DMALLOC) $(INCLUDES_OSA) -c $< -o $@

.PHONY:clean
clean:
	$(RM) $(OSA_LIB) $(OSA_STATIC_LIB) $(OSA_OBJS) $(OSA_LIB_DMALLOC) $(OSA_STATIC_LIB_DMALLOC) $(OSA_OBJS_DMALLOC)


.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR osa"
	$(RM) ./src/*
	$(RM) ./obj/*