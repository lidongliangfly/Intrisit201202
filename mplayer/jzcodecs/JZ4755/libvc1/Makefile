
#include config.mak
include ../../../configure.mak
include ../../../config.mak
SRCS_DIR = .
REL_DIR =./rel

SRC_PATH = ../../../

JZC_VC1_MACRO	= jzsoc_vc1/jzvc1/vc1_jzmacro.h
MINI_OS_DIR = ~/minios_137/minios/mtv_50

CFLAGS += -fPIC -DPIC -imacros $(JZC_VC1_MACRO) -mlong-calls -I$(SRC_PATH)/libswscale -I$(SRC_PATH)/libavcodec -I./jzsoc_vc1
SRCS_VD_VC1 = $(SRCS_DIR)/vc1.c  $(SRCS_DIR)/vc1dsp.c 
OBJS_VD_VC1 = $(addsuffix .o, $(basename $(SRCS_VD_VC1)) )

OBJS = $(OBJS_VD_VC1)

EXTRALIBS := -L$(BUILD_ROOT)/libavutil -lavutil$(BUILDSUF) $(EXTRALIBS)

NAME=vc1dec
LIBVERSION=$(LAVCVERSION)
LIBMAJOR=$(LAVCMAJOR)
#LIB=$(NAME).a


STATIC_OBJS := $(OBJS)
VPATH = $(SRC_PATH_BARE)/lib$(NAME)

MIDS_VD_VC1 = $(OBJS_VD_VC1:.o=.s)
ASMS_VD_VC1 = $(OBJS_VD_VC1:.o=.mid)


CFLAGS += -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
          -D_ISOC9X_SOURCE -I$(BUILD_ROOT) -I$(SRC_PATH) \
          -I$(SRC_PATH)/libavutil $(OPTFLAGS)


EXT_BIN_DIR := ./jzsoc_vc1/jzvc1/
EXT_BIN_NAME := vc1_aux_task.bin

EXT_BIN = $(join $(EXT_BIN_DIR),$(EXT_BIN_NAME))


all: $(REL_DIR) $(LIB) Makefile #$(EXT_BIN)
	@test -d $(REL_DIR)
	#@cp $(EXT_BIN) $(REL_DIR) -f
	cp $(LIB) $(REL_DIR)

$(LIB): $(STATIC_OBJS)
	rm -f $@
	$(AR) rc $@ $^ 
	$(RANLIB) $@

$(EXT_BIN):$(EXT_BIN_DIR)$(EXT_BIN_NAME:.bin=.c)
	@for d in $(EXT_BIN_DIR);do make -C $$d;done

$(REL_DIR):
	test -d $(REL_DIR) || mkdir $(REL_DIR)
$(OBJS_VD_VC1):%.o:%.s
	$(CC) $(CFLAGS) -c -o $@ $<
$(MIDS_VD_VC1):%.s:%.mid
	mxu_as $< > $@
$(ASMS_VD_VC1):%.mid:%.c
	$(CC) $(CFLAGS) -S -o $@ $<

distclean:
clean::
	rm -f *.o *~ *.a *.lib *.so *.so.* *.dylib *.dll *.s *.mid\
	      *.def *.dll.a *.exp
	rm -fr $(REL_DIR)
	@for d in $(EXT_BIN_DIR);do make -C $$d  clean;done
