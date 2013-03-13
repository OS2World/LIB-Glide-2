# Makefile for Glide
# build all
#
# local defines, options, includes

DEFS	 = -DGDBG_INFO_ON -DINIT_LINUX

#ifeq "$(FX_SST96_PAGE_FIFO)" != "1"
#DEFS          = $(DEFS) -DSST96_ALT_FIFO_WRAP
#!endif

	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS = sst1init.obj info.obj print.obj parse.obj gamma.obj util.obj \
                      sli.obj video.obj dac.obj gdebug.obj


initvg: initvg.lib

initvg.lib: $(OBJS)
	emxomfar r initvg.lib $(OBJS)
	cp initvg.lib $(BUILD_ROOT_SST1)/lib
