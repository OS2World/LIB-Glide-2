# Makefile for Glide
# build all
#
	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS=fxremap.obj

fxremap.exe: fxremap.obj

fxremap.exe: $(OBJS)
	$(LINK_EXE) fxremap.exe $(OBJS) $(BUILD_ROOT_SWLIBS)/lib/fxpci.lib
	cp fxremap.exe $(BUILD_ROOT_SWLIBS)/bin

fxremap.obj: fxremap.c

