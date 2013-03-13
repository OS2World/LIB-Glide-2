# Makefile for Glide
# build all
#
# local defines, options, includes
	include $(BUILD_ROOT_SWLIBS)/include/make/3dfx.emx

GLIDELIB=$(BUILD_ROOT_SST1)/lib/glide2x.lib

all: tlib.lib display.exe test00.exe test01.exe test02.exe test03.exe  \
     test04.exe test05.exe test06.exe test07.exe test08.exe test09.exe \
     test10.exe test11.exe test12.exe test13.exe test14.exe test15.exe \
     test16.exe test17.exe test18.exe test19.exe test20.exe test21.exe \
     test22.exe test23.exe test24.exe test25.exe test26.exe test27.exe \
     test28.exe test29.exe h3dtst01.exe h3dtst02.exe

tlib.lib: tlib.obj plib.obj
	emxomfar r tlib.lib tlib.obj plib.obj

display.exe: display.obj 
	$(LINK_EXE) display.exe display.obj tlib.lib -l$(GLIDELIB)

test00.exe: test00.obj
	$(LINK_EXE) test00.exe test00.obj tlib.lib -l$(GLIDELIB)

test01.exe: test01.obj
	$(LINK_EXE) test01.exe test01.obj tlib.lib -l$(GLIDELIB)

test02.exe: test02.obj
	$(LINK_EXE) test02.exe test02.obj tlib.lib -l$(GLIDELIB)

test03.exe: test03.obj
	$(LINK_EXE) test03.exe test03.obj tlib.lib -l$(GLIDELIB)

test04.exe: test04.obj
	$(LINK_EXE) test04.exe test04.obj tlib.lib -l$(GLIDELIB)

test05.exe: test05.obj
	$(LINK_EXE) test05.exe test05.obj tlib.lib -l$(GLIDELIB)

test06.exe: test06.obj
	$(LINK_EXE) test06.exe test06.obj tlib.lib -l$(GLIDELIB)

test07.exe: test07.obj
	$(LINK_EXE) test07.exe test07.obj tlib.lib -l$(GLIDELIB)

test08.exe: test08.obj
	$(LINK_EXE) test08.exe test08.obj tlib.lib -l$(GLIDELIB)

test09.exe: test09.obj
	$(LINK_EXE) test09.exe test09.obj tlib.lib -l$(GLIDELIB)

test10.exe: test10.obj
	$(LINK_EXE) test10.exe test10.obj tlib.lib -l$(GLIDELIB)

test11.exe: test11.obj
	$(LINK_EXE) test11.exe test11.obj tlib.lib -l$(GLIDELIB)

test12.exe: test12.obj
	$(LINK_EXE) test12.exe test12.obj tlib.lib -l$(GLIDELIB)

test13.exe: test13.obj
	$(LINK_EXE) test13.exe test13.obj tlib.lib -l$(GLIDELIB)

test14.exe: test14.obj
	$(LINK_EXE) test14.exe test14.obj tlib.lib -l$(GLIDELIB)

test15.exe: test15.obj
	$(LINK_EXE) test15.exe test15.obj tlib.lib -l$(GLIDELIB)

test16.exe: test16.obj
	$(LINK_EXE) test16.exe test16.obj tlib.lib -l$(GLIDELIB)

test17.exe: test17.obj
	$(LINK_EXE) test17.exe test17.obj tlib.lib -l$(GLIDELIB)

test18.exe: test18.obj
	$(LINK_EXE) test18.exe test18.obj tlib.lib -l$(GLIDELIB)

test19.exe: test19.obj
	$(LINK_EXE) test19.exe test19.obj tlib.lib -l$(GLIDELIB)

test20.exe: test20.obj
	$(LINK_EXE) test20.exe test20.obj tlib.lib -l$(GLIDELIB)

test21.exe: test21.obj
	$(LINK_EXE) test21.exe test21.obj tlib.lib -l$(GLIDELIB)

test22.exe: test22.obj
	$(LINK_EXE) test22.exe test22.obj tlib.lib -l$(GLIDELIB)

test23.exe: test23.obj
	$(LINK_EXE) test23.exe test23.obj tlib.lib -l$(GLIDELIB)

test24.exe: test24.obj
	$(LINK_EXE) test24.exe test24.obj tlib.lib -l$(GLIDELIB)

test25.exe: test25.obj
	$(LINK_EXE) test25.exe test25.obj tlib.lib -l$(GLIDELIB)

test26.exe: test26.obj
	$(LINK_EXE) test26.exe test26.obj tlib.lib -l$(GLIDELIB)

test27.exe: test27.obj
	$(LINK_EXE) test27.exe test27.obj tlib.lib -l$(GLIDELIB)

test28.exe: test28.obj
	$(LINK_EXE) test28.exe test28.obj tlib.lib -l$(GLIDELIB)

test29.exe: test29.obj
	$(LINK_EXE) test29.exe test29.obj tlib.lib -l$(GLIDELIB)

h3dtst01.exe: h3dtst01.obj
	$(LINK_EXE) h3dtst01.exe h3dtst01.obj tlib.lib -l$(GLIDELIB)

h3dtst02.exe: h3dtst02.obj
	$(LINK_EXE) h3dtst02.exe h3dtst02.obj tlib.lib -l$(GLIDELIB)