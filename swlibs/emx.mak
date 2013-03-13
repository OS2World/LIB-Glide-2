# Makefile for Glide
# build all
#
swlibs: fxpci fxmisc s3remap texus

fxpci: emx.mak
	@make -C fxpci -f emx.mak

fxmisc: emx.mak
	@make -C fxmisc -f emx.mak

s3remap: emx.mak
	@make -C s3remap -f emx.mak

texus: emx.mak
	@make -C texus -f emx.mak
