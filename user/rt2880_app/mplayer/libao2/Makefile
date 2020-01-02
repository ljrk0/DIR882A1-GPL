include ../config.mak

LIBNAME_MPLAYER = libao2.a

SRCS_MPLAYER = audio_out.c \
               ao_null.c \
               ao_oss.c \
               ao_pcm.c \
               $(AO_SRCS) \

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char/i2s
#ao_mpegpes.c
include ../mpcommon.mak
