# Makefile for Glide
# build all
#
# local defines, options, includes

DEFS		= -DSST96
ifeq ($(FX_SST96_ALT_FIFO),1)
DEFS		+= -DSST96_ALT_FIFO_WRAP
endif

	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS = init96.obj lindrvr.obj initmcrx.obj


init96: init96.lib

init96.lib: $(OBJS)
	emxomfar r init96.lib $(OBJS)
	cp init96.lib $(BUILD_ROOT_SST1)/lib
