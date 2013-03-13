# Makefile for Glide
# build all
#
	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS=fx64.obj fxglob.obj fximg.obj fxos.obj linutil.obj

fxmisc: fxmisc.lib

fxmisc.lib: $(OBJS)
	emxomfar r fxmisc.lib $(OBJS)
	cp fxmisc.lib $(BUILD_ROOT_SWLIBS)/lib
# 	ar s fxpci.a

#fxos2.obj: fxos2.c\
#	cio.h

