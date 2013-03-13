# Makefile for Glide
# build all
#
sst1: init glide

init: emx.mak
	@make -C init -f emx.mak

glide: emx.mak
	@make -C glide -f emx.mak


