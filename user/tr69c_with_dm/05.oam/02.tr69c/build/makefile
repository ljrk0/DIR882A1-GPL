include ./makePara.mk

export CC
export AR
export link 
export CFLAGS
export TR69_BUILDS
export LIB_PATH
export OBJ_PATH


ifeq ($(PRC_CROSS_COMPILE),)
XML_LIB_PATH=../../../01.common/dm_lib/libxml2/gcc_lib
else
XML_LIB_PATH=../../../01.common/dm_lib/libxml2/mips_lib
export PRC_CROSS_COMPILE
endif

TR69C_BIN = ./tr69c
TR69C_LDFLAG = -L ./libs -ltr69c -ladaptor 
TR69C_LDFLAG += -L ../../../01.common/dm_lib/lib -ldm -lTr069DbCbs 
#TR69C_LDFLAG += -L ../../../01.common/perview_log/lib -lperview 
#TR69C_LDFLAG += -L ../../../01.common/softdog/lib -lsoftdog 
#TR69C_LDFLAG += -L ../../../01.common/debug_lib/lib -ldebugrpt
#TR69C_LDFLAG += -L ../../../01.common/dev_lib/lib -ldevs
TR69C_LDFLAG += -L $(XML_LIB_PATH) -lxml2 
TR69C_LDFLAG += -L ../../../01.common/osa/lib -losa
TR69C_LDFLAG += -lpthread -lm -lrt -ldl

ifdef USE_CYASSL
TR69C_LDFLAG += -L /home/cyassl/ -lcyassl
endif

ifdef USE_OPENSSL
ifeq ($(PRC_CROSS_COMPILE),)
TR69C_LDFLAG += -L ../../05.openssl/03.gcc_libs -lssl -lcrypto
else
#TR69C_LDFLAG += -L ../../05.openssl/02.mips_libs -lssl -lcrypto
TR69C_LDFLAG += -L ../../../../openssl-1.0.2j -lssl -lcrypto
endif
TR69C_LDFLAG += -L ../../05.openssl/04.libopenssl/lib -lopenssl
endif

#subdir
ifndef TOOL_COMPILE
SUBDIRS :=  ../../05.openssl/04.libopenssl \
           ../../../01.common/softdog \
           ../../../01.common/dm_lib/common \
	    ../../../01.common/dm_lib/db_cbs/tr069 \
           ../../../01.common/perview_log \
	   ../../../01.common/debug_lib \
           ../../../01.common/osa
endif
SUBDIRS += ../libtr69c ../libadaptor
        
ifndef TOOL_COMPILE
SUBDIRS_CLEAN := ../../05.openssl/04.libopenssl \
           ../../../01.common/dm_lib/common \
	   ../../../01.common/dm_lib/db_cbs/tr069 \
	   ../../../01.common/softdog \
           ../../../01.common/perview_log \
	   ../../../01.common/debug_lib \
          ../../../01.common/osa 
endif
SUBDIRS_CLEAN += ../libtr69c ../libadaptor

SUBDIRS_CODE_CLEAN := ../libtr69c ../libadaptor

#call sub makefiles to clean
define make_subdir
 @for subdir in $(SUBDIRS) ; do \
 (cd $$subdir && make $1) \
 done;
endef

define clean_subdir
 @for subdir in $(SUBDIRS_CLEAN) ; do \
 (cd $$subdir && make $1) \
 done;
endef

define code_clean_subdir
 @for subdir in $(SUBDIRS_CODE_CLEAN) ; do \
 (cd $$subdir && make $1) \
 done;
endef

.PHONY:all
all:
	$(call make_subdir)
	$(CC) -g -o $(TR69C_BIN) $(TR69C_LDFLAG)
	
.PHONY:clean
clean:
	@$(call clean_subdir, clean) 
	@$(RM) $(TR69C_BIN)


.PHONY:code_clean
code_clean:
	@$(call code_clean_subdir, code_clean) 
	rm -rf ./objs/*
	rm -rf ./libs/*.a


