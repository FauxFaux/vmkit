##===- lib/VMKit/Makefile ------------------------------------*- Makefile -*-===##
# 
#                     The vmkit project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../..

include $(LEVEL)/Makefile.config

EXTRA_DIST = BoehmGC GCMmap2 MMTk

ifeq ($(GCLIB), GCMmap2)
	DIRS = Allocator
endif

DIRS += CommonThread $(GCLIB) Runtime Compiler StaticGCPass JITGCPass

include $(LEVEL)/Makefile.common
