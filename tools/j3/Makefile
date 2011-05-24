##===- tools/jnjvm/Makefile --------------------------------*- Makefile -*-===##
# 
#                     The vmkit project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../..

include $(LEVEL)/Makefile.config

TOOLNAME = j3

ifeq ($(WITH_LLVM_GCC), 1)
	ifeq ($(GCLIB), GCMmap2)
		Allocator=Allocator.bc
	endif

  MODULESNAME = j3
  USEDMODULES = j3.bc Classpath.bc J3Compiler.bc $(Allocator) CommonThread.bc \
		VMKit.bc VMKitCompiler.bc

  ifeq ($(GC_MMTK), 1)
    USEDMODULES += FinalMMTk.bc InlineMMTk.bc
  else
    USEDMODULES += $(GCLIB).bc
  endif

  BUILT_SOURCES = j3.s
  SOURCES = j3.s $(notdir $(wildcard $(PROJ_SRC_DIR)/*.cpp))

else

	ifeq ($(GCLIB), GCMmap2)
		Allocator=Allocator.a
	endif

	USEDLIBS = j3.a Classpath.a j3.a J3Compiler.a $(Allocator) \
	   VMKit.a VMKitCompiler.a $(GCLIB).a $(Allocator) CommonThread.a

endif

LINK_COMPONENTS = jit nativecodegen scalaropts instrumentation ipa ipo bitreader asmparser linker


include $(LEVEL)/Makefile.common
