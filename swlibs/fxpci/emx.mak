# Makefile for Glide
# build all
#
fxpci: pcilib pcitools

pcilib: emx.mak
	@make -C pcilib -f emx.mak

pcitools: emx.mak
	@make -C pcitools -f emx.mak