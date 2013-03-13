# Makefile for Glide
# build all
#
	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJ1=detect.obj
OBJ2=pcirw.obj
OBJ3=ptltest.obj

pcitools: detect.exe pcirw.exe ptltest.exe

detect.exe: $(OBJ1)
	$(LINK_EXE) detect.exe $(OBJ1) $(BUILD_ROOT_SWLIBS)/lib/fxpci.lib
	cp detect.exe $(BUILD_ROOT_SWLIBS)/bin

pcirw.exe: $(OBJ2)
	$(LINK_EXE) pcirw.exe $(OBJ2) $(BUILD_ROOT_SWLIBS)/lib/fxpci.lib
	cp pcirw.exe $(BUILD_ROOT_SWLIBS)/bin

ptltest.exe: $(OBJ3)
	$(LINK_EXE) ptltest.exe $(OBJ3) $(BUILD_ROOT_SWLIBS)/lib/fxpci.lib
	cp ptltest.exe $(BUILD_ROOT_SWLIBS)/bin

detect.obj: detect.c

pcirw.obj: pcirw.c

ptltest.obj: ptltest.c
