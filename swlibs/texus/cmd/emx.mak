# Makefile for Glide
# build all
#
	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS=waco.obj

texus.exe: waco.obj

texus.exe: $(OBJS)
	$(LINK_EXE) texus.exe $(OBJS) $(BUILD_ROOT_SWLIBS)/lib/texus.lib
	cp texus.exe $(BUILD_ROOT_SWLIBS)/bin

waco.obj: waco.c

