//===----------- VMKitGC.h - Garbage Collection Interface -------------------===//
//
//                     The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


#ifndef VMKIT_MMTK_GC_H
#define VMKIT_MMTK_GC_H

#include <cstdlib>

#define gc_allocator std::allocator

namespace vmkit {

class GCVirtualTable : public CommonVirtualTable {
public:
  uintptr_t specializedTracers[1];
  
  static uint32_t numberOfBaseFunctions() {
    return numberOfCommonEntries() + 1;
  }

  static uint32_t numberOfSpecializedTracers() {
    return 1;
  }
};

extern "C" void* gcmallocUnresolved(uint32_t sz, VirtualTable* VT);

class collectable : public gcRoot {
public:

  size_t objectSize() const {
    abort();
    return 0;
  }

  void* operator new(size_t sz, VirtualTable *VT) {
    return gcmallocUnresolved(sz, VT);
  }

};
  
class Collector {
public:
  static int verbose;

  static bool isLive(gc* ptr, uintptr_t closure) __attribute__ ((always_inline)); 
  static void scanObject(void** ptr, uintptr_t closure) __attribute__ ((always_inline));
  static void markAndTrace(void* source, void* ptr, uintptr_t closure) __attribute__ ((always_inline));
  static void markAndTraceRoot(void* ptr, uintptr_t closure) __attribute__ ((always_inline));
  static gc*  retainForFinalize(gc* val, uintptr_t closure) __attribute__ ((always_inline));
  static gc*  retainReferent(gc* val, uintptr_t closure) __attribute__ ((always_inline));
  static gc*  getForwardedFinalizable(gc* val, uintptr_t closure) __attribute__ ((always_inline));
  static gc*  getForwardedReference(gc* val, uintptr_t closure) __attribute__ ((always_inline));
  static gc*  getForwardedReferent(gc* val, uintptr_t closure) __attribute__ ((always_inline));
  static void objectReferenceWriteBarrier(gc* ref, gc** slot, gc* value) __attribute__ ((always_inline));
  static void objectReferenceArrayWriteBarrier(gc* ref, gc** slot, gc* value) __attribute__ ((always_inline));
  static void objectReferenceNonHeapWriteBarrier(gc** slot, gc* value) __attribute__ ((always_inline));
  static bool objectReferenceTryCASBarrier(gc* ref, gc** slot, gc* old, gc* value) __attribute__ ((always_inline));
  static bool needsWriteBarrier() __attribute__ ((always_inline));
  static bool needsNonHeapWriteBarrier() __attribute__ ((always_inline));

  static void collect();
  
  static void initialise();
  
  static int getMaxMemory() {
    return 0;
  }
  
  static int getFreeMemory() {
    return 0;
  }
  
  static int getTotalMemory() {
    return 0;
  }

  void setMaxMemory(size_t sz){
  }

  void setMinMemory(size_t sz){
  }

  static void* begOf(gc*);
};

}
#endif
