##===- tools/vmjc/Makefile --------------------------------*- Makefile -*-===##
# 
#                     The VMKit project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../..

include $(LEVEL)/Makefile.config

EXTRA_DIST = libvmjc

TOOLNAME = vmjc


ifeq ($(WITH_LLVM_GCC), 1)
	ifeq ($(GCLIB), GCMmap2)
		Allocator=Allocator.bc
	endif

  MODULESNAME = vmkit
  USEDMODULES = j3.bc Classpath.bc J3Compiler.bc $(Allocator) \
		CommonThread.bc VMKit.bc VMKitCompiler.bc $(GCLIB).bc

  BUILT_SOURCES = vmkit.s
  SOURCES = vmkit.s $(notdir $(wildcard $(PROJ_SRC_DIR)/*.cpp))

else
	ifeq ($(GCLIB), GCMmap2)
		Allocator=Allocator.a
	endif

  USEDLIBS = j3.a Classpath.a j3.a J3Compiler.a $(Allocator) \
	     VMKit.a VMKitCompiler.a $(GCLIB).a Allocator.a CommonThread.a
endif

LINK_COMPONENTS = jit nativecodegen scalaropts instrumentation ipa ipo bitwriter bitreader asmparser linker

include $(LEVEL)/Makefile.common
