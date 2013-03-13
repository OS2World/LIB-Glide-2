# Makefile for Glide
# build all
#
TOPDIR := $(shell pwd)
export TOPDIR
export BUILD_ROOT_SWLIBS = $(TOPDIR)/swlibs
export BUILD_ROOT_SST1 = $(TOPDIR)/sst1

all: swlibs sst1

swlibs: swlibs/emx.mak
	@make -C swlibs -f emx.mak

sst1:   sst1/emx.mak
	@make -C sst1 -f emx.mak

clean: 
	@$(RM) $(BUILD_ROOT_SWLIBS)/fxpci/pcitools/*.obj
	@$(RM) $(BUILD_ROOT_SWLIBS)/fxpci/pcilib/*.lib
	@$(RM) $(BUILD_ROOT_SWLIBS)/fxpci/pcilib/*.obj
	@$(RM) $(BUILD_ROOT_SWLIBS)/fxpci/pcitools/*.exe
	@$(RM) $(BUILD_ROOT_SWLIBS)/fxmisc/*.obj
	@$(RM) $(BUILD_ROOT_SWLIBS)/fxmisc/*.lib
	@$(RM) $(BUILD_ROOT_SWLIBS)/lib/*.lib
	@$(RM) $(BUILD_ROOT_SWLIBS)/bin/*.exe
	@$(RM) $(BUILD_ROOT_SWLIBS)/s3remap/*.exe
	@$(RM) $(BUILD_ROOT_SWLIBS)/s3remap/*.obj
	@$(RM) $(BUILD_ROOT_SWLIBS)/texus/lib/*.obj
	@$(RM) $(BUILD_ROOT_SWLIBS)/texus/lib/*.lib
	@$(RM) $(BUILD_ROOT_SWLIBS)/texus/cmd/*.obj
	@$(RM) $(BUILD_ROOT_SWLIBS)/texus/cmd/*.exe
	@$(RM) $(BUILD_ROOT_SST1)/bin/*.dll
	@$(RM) $(BUILD_ROOT_SST1)/lib/*.lib
	@$(RM) $(BUILD_ROOT_SST1)/init/*.lib
	@$(RM) $(BUILD_ROOT_SST1)/init/*.obj
	@$(RM) $(BUILD_ROOT_SST1)/init/initvg/*.obj
	@$(RM) $(BUILD_ROOT_SST1)/init/initvg/*.lib
	@$(RM) $(BUILD_ROOT_SST1)/glide/src/*.dll
	@$(RM) $(BUILD_ROOT_SST1)/glide/src/*.lib
	@$(RM) $(BUILD_ROOT_SST1)/glide/src/*.obj
	@$(RM) $(BUILD_ROOT_SST1)/glide/tests/*.obj
	@$(RM) $(BUILD_ROOT_SST1)/glide/tests/*.lib
	@$(RM) $(BUILD_ROOT_SST1)/glide/tests/*.exe