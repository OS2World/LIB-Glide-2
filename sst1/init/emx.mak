# Makefile for Glide
# build all
#
# local defines, options, includes

ifdef DEBUG
DEFS   = -DGDBG_INFO_ON
endif 

ifeq ($(FX_GLIDE_HW),SST96)
INITHW   =      SST96
DEFS     += -DSST96
else
INITHW   =      SST1
DEFS     += -DSST1
endif

# Turn on the alternate fifo handling code
ifeq ($(FX_SST96_PAGE_FIFO),1)
DEFS          += -DSST96_ALT_FIFO_WRAP
endif

	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

all: initvg init.lib

initvg: emx.mak
	@make -C initvg -f emx.mak

OBJS = init.obj vgdrvr.obj vg96drvr.obj h3drvr.obj

init.lib: $(OBJS)
	emxomfar r libinit.lib $(OBJS)
	cp libinit.lib $(BUILD_ROOT_SST1)/lib
