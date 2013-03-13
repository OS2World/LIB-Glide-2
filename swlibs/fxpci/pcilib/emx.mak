# Makefile for Glide
# build all
#
	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS=fxos2.obj

fxpci: fxpci.lib

fxpci.lib: $(OBJS)
	emxomfar r fxpci.lib $(OBJS)
	cp fxpci.lib $(BUILD_ROOT_SWLIBS)/lib


