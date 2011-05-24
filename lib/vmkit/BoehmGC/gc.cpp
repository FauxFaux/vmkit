//===------------ gc.cc - Boehm GC Garbage Collector ----------------------===//
//
//                              VMKit
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "vmkit/GC.h"
#include "vmkit/Thread.h"

using namespace vmkit;

void Collector::inject_my_thread(vmkit::Thread* th) {
  GC_init();
}

void Collector::initialise() {
  GC_INIT();
}

extern "C" gc* gcmalloc(size_t sz, VirtualTable* VT) {
  return (gc*)gc::operator new(sz, VT);
}
