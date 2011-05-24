//===------ ActivePlan.cpp - Implementation of the ActivePlan class  ------===//
//
//                              The VMKit project
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "debug.h"
#include "vmkit/VirtualMachine.h"
#include "vmkit/VMKit.h"
#include "MMTkObject.h"
#include "MutatorThread.h"

namespace mmtk {

extern "C" MMTkObject* Java_org_j3_mmtk_ActivePlan_getNextMutator__(MMTkActivePlan* A) {
  assert(A && "No active plan");

	vmkit::CircularBase<vmkit::Thread>* mut = A->current;

	if(!mut)
		mut = &vmkit::Thread::get()->vmkit->runningThreads;

	mut = mut->next();

	if(mut == &vmkit::Thread::get()->vmkit->runningThreads) {
		A->current = NULL;
		return NULL;
	}
	
	A->current = (vmkit::MutatorThread*)mut;

  if (A->current->MutatorContext == 0) {
    return Java_org_j3_mmtk_ActivePlan_getNextMutator__(A);
  }
  return (MMTkObject*)A->current->MutatorContext;
}

extern "C" void Java_org_j3_mmtk_ActivePlan_resetMutatorIterator__(MMTkActivePlan* A) {
  A->current = NULL;
}

extern "C" void Java_org_j3_mmtk_ActivePlan_collectorCount__ (MMTkActivePlan* A) {
  UNIMPLEMENTED();
}

}
