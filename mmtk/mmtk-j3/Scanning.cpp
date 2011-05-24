//===-------- Scanning.cpp - Implementation of the Scanning class  --------===//
//
//                              The VMKit project
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "debug.h"
#include "vmkit/VirtualMachine.h"
#include "MMTkObject.h"
#include "vmkit/GC.h"
#include "vmkit/VMKit.h"

namespace mmtk {

extern "C" void Java_org_j3_mmtk_Scanning_computeThreadRoots__Lorg_mmtk_plan_TraceLocal_2 (MMTkObject* Scanning, MMTkObject* TL) {
  // When entering this function, all threads are waiting on the rendezvous to
  // finish.
  vmkit::VMKit* vmkit = vmkit::Thread::get()->vmkit;
  vmkit::Thread* tcur;
  
	for(tcur=vmkit->runningThreads.next(); tcur!=&vmkit->runningThreads; tcur=tcur->next()) {
    tcur->scanStack(reinterpret_cast<uintptr_t>(TL));
  }
}

extern "C" void Java_org_j3_mmtk_Scanning_computeGlobalRoots__Lorg_mmtk_plan_TraceLocal_2 (MMTkObject* Scanning, MMTkObject* TL) { 
  vmkit::Thread::get()->vmkit->tracer(reinterpret_cast<uintptr_t>(TL));

	vmkit::VMKit* vmkit = vmkit::Thread::get()->vmkit;
  vmkit::Thread* tcur;
  
	for(tcur=vmkit->runningThreads.next(); tcur!=&vmkit->runningThreads; tcur=tcur->next()) {
    tcur->tracer(reinterpret_cast<uintptr_t>(TL));
  }
}

extern "C" void Java_org_j3_mmtk_Scanning_computeStaticRoots__Lorg_mmtk_plan_TraceLocal_2 (MMTkObject* Scanning, MMTkObject* TL) {
  // Nothing to do.
}

extern "C" void Java_org_j3_mmtk_Scanning_resetThreadCounter__ (MMTkObject* Scanning) {
  // Nothing to do.
}

extern "C" void Java_org_j3_mmtk_Scanning_specializedScanObject__ILorg_mmtk_plan_TransitiveClosure_2Lorg_vmmagic_unboxed_ObjectReference_2 (MMTkObject* Scanning, uint32_t id, MMTkObject* TC, vmkit::gc* obj) ALWAYS_INLINE;

extern "C" void Java_org_j3_mmtk_Scanning_specializedScanObject__ILorg_mmtk_plan_TransitiveClosure_2Lorg_vmmagic_unboxed_ObjectReference_2 (MMTkObject* Scanning, uint32_t id, MMTkObject* TC, vmkit::gc* obj) {
  assert(obj && "No object to trace");
  assert(obj->getVirtualTable() && "No virtual table");
  assert(obj->getVirtualTable()->tracer && "No tracer in VT");
  obj->tracer(reinterpret_cast<uintptr_t>(TC));
}

extern "C" void Java_org_j3_mmtk_Scanning_preCopyGCInstances__Lorg_mmtk_plan_TraceLocal_2 (MMTkObject* Scanning, MMTkObject* TL) {
  // Nothing to do, there are no GC objects on which the GC depends.
}

extern "C" void Java_org_j3_mmtk_Scanning_scanObject__Lorg_mmtk_plan_TransitiveClosure_2Lorg_vmmagic_unboxed_ObjectReference_2 (
    MMTkObject* Scanning, uintptr_t TC, vmkit::gc* obj) {
  assert(obj && "No object to trace");
  assert(obj->getVirtualTable() && "No virtual table");
  assert(obj->getVirtualTable()->tracer && "No tracer in VT");
  obj->tracer(TC);
}

extern "C" void Java_org_j3_mmtk_Scanning_precopyChildren__Lorg_mmtk_plan_TraceLocal_2Lorg_vmmagic_unboxed_ObjectReference_2 (
    MMTkObject* Scanning, MMTkObject TL, uintptr_t ref) { UNIMPLEMENTED(); }

}
