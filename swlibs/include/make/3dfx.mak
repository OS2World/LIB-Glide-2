#
# THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
# PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
# TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
# INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
# DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
# THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
# EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
# FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
# 
# USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
# RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
# TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
# AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
# SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
# THE UNITED STATES.  
# 
# COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
#
# $Revision: 1.2 $
# $Date: 1997/06/27 06:58:08 $

#
# Set the important directories in the environment
#
export BUILD_ROOT_SWLIBS = $(TOPDIR)/swlibs
export BUILD_ROOT_SST1 = $(TOPDIR)/sst1

#
# determine the OS type
#
OS=$(shell $(BUILD_ROOT_SWLIBS)/include/make/ostype)
ifeq ($(OS),) 
        echo "$OS not defined"
endif

#
# Make include file, kept as simple as possible, defines/setups the following
#	1) global CC flags
#	2) global LD flags
#	3) default rule (all)
#	4) make depend rules
#	5) file removal rules
#	6) recursive rules

#--------------------------------------------------------------------------
# configuration variables for the compiler environment

#	FX_COMPILER must be set to either MICROSOFT or WATCOM
#	DEBUG must be set to enable debugging flags for cc and link
#
CPP            = $(CC) -E -c
ASM_LIST_FLAGS = -s
CDEBUG         = -g
CNODEBUG       = -O
LDEBUG         = -g
LNODEBUG       =
GCDEFS         = -DENDB -DX11 -DGDBG_INFO_ON -DGLIDE_DEBUG
GLDOPTS	       = -L$(BUILD_ROOT_SWLIBS)/lib -L$(BUILD_ROOT_SST1)/lib
LINK           = $(CC)

ifneq ($(DOH3D),)
H3DOPT	       = -DH3D
endif

ifeq "$(OS)" "sunos"
GCINCS         = -I -I- -I/usr/local/include/gcc -I$(BUILD_ROOT_SWLIBS)/include -I/usr/include
GCOPTS         = -ansi -Wall
endif
ifeq "$(OS)" "solaris"
GCINCS         = -I -I- -I/usr/local/include/gcc -I$(BUILD_ROOT_SWLIBS)/include
GCOPTS         = -Wall
endif
ifeq "$(OS)" "hpux"
GCINCS         = -I -I- -I/usr/local/include/gcc -I$(BUILD_ROOT_SWLIBS)/include
GCOPTS         = -Wall
endif
ifeq "$(OS)" "Linux"
GCINCS	       = -I. -I$(BUILD_ROOT_SWLIBS)/include -I$(BUILD_ROOT_SST1)/include
GCOPTS	       = -Wall
# Compiler switches
ifeq ($(CC),gcc)
GCCARCH	       = -m486
else
ifeq ($(CC),egcs)
GCCARGH        = -mpentium
else
GCCARCH        =
endif
endif
CNODEBUG       = -O6 $(GCCARCH) -fomit-frame-pointer -funroll-loops \
	-fexpensive-optimizations -ffast-math

CDEBUG	       = -g -O
# Profiling
#CDEBUG	       = -pg -g -O
#GCDEFS	       =
endif

# if we are not debugging then replace debug flags with nodebug flags

# DEBUG = xx

ifndef DEBUG
GCDEFS   = 
CDEBUG   = $(CNODEBUG)
LDEBUG   = $(LNODEBUG)
endif

ifeq ($(XLOCATION),)
XLOCATION = /usr/X11R6
endif

#--------------------------------------------------------------------------
# build up CFLAGS from global, local, and variable flags
#	each of which has includes, defines, and options
#
GCFLAGS	= $(GCINCS) $(GCDEFS) $(GCOPTS) $(H3DOPT)
LCFLAGS	= $(LCINCS) $(LCDEFS) $(LCOPTS)
VCFLAGS	= $(VCINCS) $(VCDEFS) $(VCOPTS)

CFLAGS	= $(CDEBUG) $(GCFLAGS) $(LCFLAGS) $(VCFLAGS)

#--------------------------------------------------------------------------
# build up global linker flags (LDFLAGS) and libraries (LDLIBS)
#	note that local libs are before global libs
#
GLDLIBS	= -lm
LDFLAGS	= $(LDEBUG) $(GLDOPTS) $(LLDOPTS)
LDLIBS  = $(LLDLIBS) $(GLDLIBS)

#--------------------------------------------------------------------------
# configure OS commands
#

ifeq "$(OS)" "sunos"
AR      = /usr/5bin/ar crsl
ECHO	= /usr/5bin/echo
INSTALL = install
endif
ifeq "$(OS)" "solaris"
AR      = /usr/ccs/bin/ar crsl
ECHO	= /usr/5bin/echo
INSTALL = /usr/ucb/install
endif
ifeq "$(OS)" "hpux"
AR      = /bin/ar crsl
ECHO	= /bin/echo
INSTALL = /usr/local/bin/install
endif
ifeq "$(OS)" "Linux"
AR      = /usr/bin/ar crsl
ECHO	= /bin/echo
INSTALL = /usr/bin/install
endif

DATE	= date
RM      = rm

#--------------------------------------------------------------------------
#
# a default rule, serves 2 purposes
#	1) keeps us from typing "make install" all the time
#	2) allows this file to be included first, without clobber becoming
#		the default rule
default: all

all: incs libs bins

OBJECTS	= $(CFILES:.c=.o) $(LIBOBJS)

#--------------------------------------------------------------------------
# rules for INCS, LIBS, and BINS , the three major targets
#

$(THISDIR)incs: $(HEADERS)
ifdef HEADERS
ifdef INSTALL_DESTINATION
	$(INSTALL) -d $(INSTALL_DESTINATION)/include
	$(INSTALL) -m 444 $(HEADERS) $(INSTALL_DESTINATION)/include
else
	@echo INSTALL_DESTINATION not defined, not installing HEADERS
endif
endif

#--------------------------------------------------------------------------
# rules for LIBRARIES
#	NOTE: we supply a default rule for making a library
ifdef LIBRARIES
LIBPARTS = $(OBJECTS)

$(LIBRARIES): $(LIBPARTS)
	$(AR) $*.a $(LIBPARTS)

$(SHARED_LIBRARY): $(LIBPARTS) $(SUBLIBRARIES)
	$(LINK) $(LDFLAGS) -shared -o $(SHARED_LIBRARY) \
		-Xlinker --whole-archive \
		$(LIBRARIES) $(SUBLIBRARIES) \
		-Xlinker --no-whole-archive \
		$(LINKLIBRARIES)

$(THISDIR)libs: $(LIBRARIES) $(SHARED_LIBRARY)
ifdef INSTALL_DESTINATION
	$(INSTALL) -d $(INSTALL_DESTINATION)/lib
	$(INSTALL) -m 444 $(LIBRARIES) $(INSTALL_DESTINATION)/lib
ifneq "$(SHARED_LIBRARY)" ""
	$(INSTALL) -m 444 $(SHARED_LIBRARY) $(INSTALL_DESTINATION)/lib
endif
else
	@echo INSTALL_DESTINATION not defined, not installing LIBRARIES
endif
else
$(THISDIR)libs:
endif

#--------------------------------------------------------------------------
# rules for BINS
#	NOTE: calling makefile must define rules for making programs
#	or define SIMPLE_EXE or MULTI_EXE
ifdef PROGRAM
$(PROGRAM): $(OBJECTS)
	$(LINK) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS) 
endif

ifdef PROGRAMS
$(PROGRAMS): % : %.o $(LIBOBJS)
	$(LINK) $(CFLAGS) -o $@ $@.o $(LDFLAGS) $(LDLIBS) 
endif

$(THISDIR)bins: $(PROGRAM) $(PROGRAMS)
ifneq "$(PROGRAM)" "$(PROGRAMS)"
ifdef INSTALL_DESTINATION
	$(INSTALL) -d $(INSTALL_DESTINATION)/bin
	$(INSTALL) -m 555 $(PROGRAM) $(PROGRAMS) $(INSTALL_DESTINATION)/bin
else
	@echo INSTALL_DESTINATION not defined, not installing PROGRAMS
endif
endif

TARGETS += $(LIBRARIES) $(SHARED_LIBRARY) $(PROGRAM) $(PROGRAMS)

#--------------------------------------------------------------------------
# MKDEPFILE is the name of the dependency database
#
MKDEPFILE = makedep

#--------------------------------------------------------------------------
# File removal rules: there are four
#	- neat removes dirt
#	- clean removes intermediates and neat (dirt)
#	- clobber removes targets and clean (intermediates and dirt)
#	- rmtargets removes targets only
#

#--------------------------------------------------------------------------
# DIRT definitions
#
GDIRT	= *~ *.cod *.bak *.pdb *.ilk *.map *.sym *.err *.i stderr.out core
DIRT	= $(GDIRT) $(LDIRT)
JUNK	= __junk__

$(THISDIR)clobber: $(THISDIR)clean $(THISDIR)rmtargets
	$(RM) -f $(MKDEPFILE) $(JUNK)

$(THISDIR)clean: $(THISDIR)neat
	$(RM) -f $(OBJECTS) $(JUNK)

$(THISDIR)neat:
	$(RM) -f $(DIRT) $(JUNK)

$(THISDIR)rmtargets:
	$(RM) -f $(TARGETS) $(JUNK)

.SUFFIXES: .cod .i .bat .sh

.c.cod:
	$(CC) $(CFLAGS) $(ASM_LIST_FLAGS) $*.c

.c.i:
	$(CPP) $(CFLAGS) $*.c > $@

.bat.sh:
	awk -f ../bat2sh.awk $*.bat > $@
	@chmod +x $@

#--------------------------------------------------------------------------
# Include a makedepend file if necessary.  Don't know if this
# works.
#
#if EXISTS ($(MKDEPFILE))
#include $(MKDEPFILE)
#endif

ifdef CFILES
$(OBJECTS): $(HEADERS)
endif

#--------------------------------------------------------------------------
# RECURSIVE rules
#	A recursive makefile should set SUBDIRS and THISDIR
#	setting THISDIR prefixes all the common targets with $(THISDIR)
#	and enables the recursive definitions of them below
#	SUBDIRS1 is used when SUBDIRS exceeds 6 (DOS args only go up to %9)
ifdef THISDIR

# Recursive targets and rules (decend into each subdirectory)
RETARGETS= clobber clean neat rmtargets depend incs libs bins

$(RETARGETS): % : $(THISDIR)%
	@for d in ${SUBDIRS} ;\
	do \
		${MAKE} -f makefile.unix -C $$d $@;\
	done
endif
