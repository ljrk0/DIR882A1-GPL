include ../build/makePara.mk

LIBTR69C_OBJS_PATH := ../build/objs
LIBTR69C_MAIN_SRC_PATH := ./main
LIBTR69C_BCMLIBIF_SRC_PATH := ./bcmLibIF
LIBTR69C_SOAPPARSER_SRC_PATH := ./SOAPParser
LIBTR69C_WEBPROTO_SRC_PATH := ./webproto
LIBTR69C_DM_PATH := ../../../01.common/dm_lib
LIBTR69C_LIB := ../build/libs/libtr69c.a

LIBTR69C_INC := -I ./inc -I ./main -I ./bcmLibIF -I ./SOAPParser -I ./webproto 
LIBTR69C_INC += -I ../include -I ../../../01.common/osa/inc -I ../libadaptor/inc
LIBTR69C_INC += -I $(LIBTR69C_DM_PATH)/common/inc
LIBTR69C_INC += -I $(LIBTR69C_DM_PATH)/db_device/inc
LIBTR69C_INC += -I $(LIBTR69C_DM_PATH)/external_type
LIBTR69C_INC += -I $(LIBTR69C_DM_PATH)/db_internal/inc
LIBTR69C_INC += -I $(LIBTR69C_DM_PATH)/libxml2/include
LIBTR69C_INC += -I ../../../01.common/perview_log/inc
LIBTR69C_INC += -I ../../../01.common/debug_lib/inc
LIBTR69C_INC += -I ../../../01.common/softdog/inc
LIBTR69C_INC += -I ../../../01.common/include
LIBTR69C_INC += -I ../../01.oam/ema/inc
LIBTR69C_INC += -I ../../01.oam/oam_agent/inc
#LIBTR69C_INC += -I ../../05.openssl/01.include
LIBTR69C_INC += -I ../../../../openssl-1.0.2j/include

LIBTR69C_MAIN_SRC = $(wildcard $(LIBTR69C_MAIN_SRC_PATH)/*.c)
LIBTR69C_MAIN_OBJ = $(patsubst $(LIBTR69C_MAIN_SRC_PATH)/%,$(LIBTR69C_OBJS_PATH)/%,$(patsubst %.c,%.o,$(LIBTR69C_MAIN_SRC)))

LIBTR69C_BCMLIBIF_SRC = $(wildcard $(LIBTR69C_BCMLIBIF_SRC_PATH)/*.c)
LIBTR69C_BCMLIBIF_OBJ = $(patsubst $(LIBTR69C_BCMLIBIF_SRC_PATH)/%,$(LIBTR69C_OBJS_PATH)/%,$(patsubst %.c,%.o,$(LIBTR69C_BCMLIBIF_SRC)))

LIBTR69C_SOAPPARSER_SRC = $(wildcard $(LIBTR69C_SOAPPARSER_SRC_PATH)/*.c)
LIBTR69C_SOAPPARSER_OBJ = $(patsubst $(LIBTR69C_SOAPPARSER_SRC_PATH)/%,$(LIBTR69C_OBJS_PATH)/%,$(patsubst %.c,%.o,$(LIBTR69C_SOAPPARSER_SRC)))

LIBTR69C_WEBPROTO_SRC = $(wildcard $(LIBTR69C_WEBPROTO_SRC_PATH)/*.c)
LIBTR69C_WEBPROTO_OBJ = $(patsubst $(LIBTR69C_WEBPROTO_SRC_PATH)/%,$(LIBTR69C_OBJS_PATH)/%,$(patsubst %.c,%.o,$(LIBTR69C_WEBPROTO_SRC)))


#target
$(LIBTR69C_LIB):$(LIBTR69C_MAIN_OBJ) $(LIBTR69C_BCMLIBIF_OBJ) $(LIBTR69C_SOAPPARSER_OBJ) $(LIBTR69C_WEBPROTO_OBJ)
	$(AR) -r $(LIBTR69C_LIB) $(LIBTR69C_MAIN_OBJ) $(LIBTR69C_BCMLIBIF_OBJ) $(LIBTR69C_SOAPPARSER_OBJ) $(LIBTR69C_WEBPROTO_OBJ)

$(LIBTR69C_MAIN_OBJ):$(LIBTR69C_OBJS_PATH)/%.o:$(LIBTR69C_MAIN_SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(LIBTR69C_INC) -pthread -c $< -o $@

$(LIBTR69C_BCMLIBIF_OBJ):$(LIBTR69C_OBJS_PATH)/%.o:$(LIBTR69C_BCMLIBIF_SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(LIBTR69C_INC) -pthread -c $< -o $@

$(LIBTR69C_SOAPPARSER_OBJ):$(LIBTR69C_OBJS_PATH)/%.o:$(LIBTR69C_SOAPPARSER_SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(LIBTR69C_INC) -pthread -c $< -o $@

$(LIBTR69C_WEBPROTO_OBJ):$(LIBTR69C_OBJS_PATH)/%.o:$(LIBTR69C_WEBPROTO_SRC_PATH)/%.c
	$(CC) $(CFLAGS) $(LIBTR69C_INC) -pthread -c $< -o $@

.PHONY:clean
clean:
	$(RM) $(LIBTR69C_MAIN_OBJ) $(LIBTR69C_BCMLIBIF_OBJ) $(LIBTR69C_SOAPPARSER_OBJ) $(LIBTR69C_WEBPROTO_OBJ)
	$(RM) $(LIBTR69C_LIB)

.PHONY:code_clean
code_clean:
	echo "CODE CLEAN FOR libtr69c"
	rm -rf $(LIBTR69C_MAIN_SRC) $(LIBTR69C_BCMLIBIF_SRC) $(LIBTR69C_SOAPPARSER_SRC) $(LIBTR69C_WEBPROTO_SRC)
