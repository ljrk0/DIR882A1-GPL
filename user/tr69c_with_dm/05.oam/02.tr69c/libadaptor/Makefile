include ../build/makePara.mk

LIBADAPTOR_OBJS_PATH := ../build/objs
LIBADAPTOR_SRC_PATH := ./src
LIBADAPTOR_DM_PATH := ../../../01.common/dm_lib
LIBADAPTOR_LIB := ../build/libs/libadaptor.a

LIBADAPTOR_INC := -I ./inc -I ./main -I ./bcmLibIF -I ./SOAPParser -I ./webproto 
LIBADAPTOR_INC += -I ../include -I ../../../01.common/osa/inc
LIBADAPTOR_INC += -I $(LIBADAPTOR_DM_PATH)/common/inc
LIBADAPTOR_INC += -I $(LIBADAPTOR_DM_PATH)/db_device/inc
LIBADAPTOR_INC += -I $(LIBADAPTOR_DM_PATH)/external_type
LIBADAPTOR_INC += -I $(LIBADAPTOR_DM_PATH)/db_internal/inc
LIBADAPTOR_INC += -I $(LIBADAPTOR_DM_PATH)/db_cbs/tr069
LIBADAPTOR_INC += -I $(LIBADAPTOR_DM_PATH)/libxml2/include
LIBADAPTOR_INC += -I ../../../01.common/perview_log/inc
LIBADAPTOR_INC += -I ../../../01.common/debug_lib/inc
LIBADAPTOR_INC += -I ../../../01.common/dev_lib/inc
LIBADAPTOR_INC += -I ../../../01.common/softdog/inc
LIBADAPTOR_INC += -I ../../../01.common/include
LIBADAPTOR_INC += -I ../libtr69c/bcmLibIF
LIBADAPTOR_INC += -I ../libtr69c/inc
LIBADAPTOR_INC += -I ../libtr69c/main
LIBADAPTOR_INC += -I ../libtr69c/SOAPParser
LIBADAPTOR_INC += -I ../libtr69c/webproto
LIBADAPTOR_INC += -I ../../01.oam/ema/inc
LIBADAPTOR_INC += -I ../../01.oam/oam_agent/inc
LIBADAPTOR_INC += -I ../../05.openssl/01.include
LIBADAPTOR_INC += -I ../../05.openssl/04.libopenssl/inc

LIBADAPTOR_SRC = $(wildcard $(LIBADAPTOR_SRC_PATH)/*.c)
LIBADAPTOR_OBJ = $(patsubst $(LIBADAPTOR_SRC_PATH)/%,$(LIBADAPTOR_OBJS_PATH)/%,$(patsubst %.c,%.o,$(LIBADAPTOR_SRC)))

#target
$(LIBADAPTOR_LIB):$(LIBADAPTOR_OBJ)
	$(AR) -r $(LIBADAPTOR_LIB) $(LIBADAPTOR_OBJ)

$(LIBADAPTOR_OBJ):$(LIBADAPTOR_OBJS_PATH)/%.o:$(LIBADAPTOR_SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(LIBADAPTOR_INC) -pthread -c $< -o $@

.PHONY:clean
clean:
	$(RM) $(LIBADAPTOR_OBJ)
	$(RM) $(LIBADAPTOR_LIB)

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR libadaptor"
	rm -rf $(LIBADAPTOR_SRC)
