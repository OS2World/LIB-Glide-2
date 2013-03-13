# Makefile for Glide
# build all
#
swlibs: lib cmd

lib: emx.mak
	@make -C lib -f emx.mak

cmd: emx.mak
	@make -C cmd -f emx.mak

