//===-------- ReferenceProcessor.cpp --------------------------------------===//
//===-------- Implementation of the Selected class  -----------------------===//
//
//                              The VMKit project
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "debug.h"
#include "mvm/VirtualMachine.h"
#include "mvm/VMKit.h"
#include "mvm/SystemThreads.h"
#include "MMTkObject.h"

namespace mmtk {

extern "C" void Java_org_j3_mmtk_ReferenceProcessor_scan__Lorg_mmtk_plan_TraceLocal_2Z (MMTkReferenceProcessor* RP, uintptr_t TL, uint8_t nursery) {
  vmkit::Thread* th = vmkit::Thread::get();
  uint32_t val = RP->ordinal;

  if (val == 0) {
    th->vmkit->scanSoftReferencesQueue(TL);
  } else if (val == 1) {
    th->vmkit->scanWeakReferencesQueue(TL);
  } else {
    assert(val == 2);
    th->vmkit->scanPhantomReferencesQueue(TL);
  }
}

extern "C" void Java_org_j3_mmtk_ReferenceProcessor_forward__Lorg_mmtk_plan_TraceLocal_2Z (MMTkReferenceProcessor* RP, uintptr_t TL, uint8_t nursery) { UNIMPLEMENTED(); }
extern "C" void Java_org_j3_mmtk_ReferenceProcessor_clear__ (MMTkReferenceProcessor* RP) { UNIMPLEMENTED(); }
extern "C" void Java_org_j3_mmtk_ReferenceProcessor_countWaitingReferences__ (MMTkReferenceProcessor* RP) { UNIMPLEMENTED(); }

}
