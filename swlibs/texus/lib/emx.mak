DEFS  = -DGLIDE_HARDWARE

	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

OBJS = texuslib.obj clamp.obj read.obj resample.obj mipmap.obj quantize.obj \
       ncc.obj nccnnet.obj pal256.obj dequant.obj view.obj util.obj diffuse.obj \
       write.obj tga.obj 3df.obj ppm.obj rgt.obj

texus: texus.lib

texus.lib: $(OBJS)
	emxomfar r texus.lib $(OBJS)
	cp texus.lib $(BUILD_ROOT_SWLIBS)/lib


