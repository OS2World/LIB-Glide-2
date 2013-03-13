# Makefile for Glide
# build all
#
# local defines, options, includes
#
IMPLIB = emximp -o
#
DEFS	   = -DGLIDE_LIB
DEFS       += -DSST1 -DGLIDE_USE_C_TRISETUP
#DEFS          +=       -DGLIDE_SPLASH
#DEFS          +=       -DGROUP_WRITE
#DEFS          +=       -DGLIDE_SANITY_SIZE
#DEFS          +=       -DGLIDE_SANITY_ASSERT

ASM_DEFINES = -DGLIDE_HARDWARE=1 
#ifneq ($(DEBUG),)
#ASM_DEFINES += -DGLIDE_DEBUG=1
#else
ASM_DEFINES += -DGLIDE_DEBUG=0
#endif

LIBS = $(BUILD_ROOT_SST1)/lib/libinit.lib  \
       $(BUILD_ROOT_SST1)/lib/initvg.lib   \
       $(BUILD_ROOT_SWLIBS)/lib/fxpci.lib  \
       $(BUILD_ROOT_SWLIBS)/lib/fxmisc.lib 


	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS          =	gsplash.obj g3df.obj  gu.obj guclip.obj gpci.obj gump.obj\
                        diglide.obj disst.obj ditex.obj gbanner.obj gerror.obj\
                        gmovie.obj digutex.obj ddgump.obj gaa.obj gdraw.obj\
                        gglide.obj glfb.obj gsst.obj gtex.obj gtexdl.obj\
                        gutex.obj gxdraw.obj initterm.obj \
			cpudetect.obj

all: fxgasm.exe glide2x.dll 

fxgasm.exe: fxgasm.obj
	$(LINK_EXE) fxgasm.exe fxgasm.obj

glide2x.dll: $(OBJS)
	$(LINK_DLL) glide2x.dll -lemxio $(OBJS) glide2x.def $(LIBS)
	$(IMPLIB) glide2x.lib glide2x.def
	cp glide2x.dll $(BUILD_ROOT_SST1)/bin
	cp glide2x.lib $(BUILD_ROOT_SST1)/lib

# .align 16  :-((( EMX only can 1,2,4
#xdraw.obj : fxgasm.h xdraw.s
# gasp xdraw.s $(ASM_DEFINES) | as -V -Qy -o xdraw.o
