# Makefile for Glide
# build all
#
glide: src tests

src: emx.mak
	@make -C src -f emx.mak

tests: emx.mak
	@make -C tests -f emx.mak

