#include "mvm/SystemThreads.h"
#include "mvm/GC.h"
#include "mvm/VirtualMachine.h"

using namespace vmkit;

ReferenceThread::ReferenceThread(vmkit::VMKit* vmkit) :
	MutatorThread(vmkit),
	WeakReferencesQueue(ReferenceQueue::WEAK),
	SoftReferencesQueue(ReferenceQueue::SOFT), 
	PhantomReferencesQueue(ReferenceQueue::PHANTOM) {

  ToEnqueue = new vmkit::gc*[INITIAL_QUEUE_SIZE];
  ToEnqueueLength = INITIAL_QUEUE_SIZE;
  ToEnqueueIndex = 0;

	setDaemon();
}

void ReferenceThread::addWeakReference(vmkit::gc* ref) {
	llvm_gcroot(ref, 0);
	WeakReferencesQueue.addReference(ref);
}
  
void ReferenceThread::addSoftReference(vmkit::gc* ref) {
	llvm_gcroot(ref, 0);
	SoftReferencesQueue.addReference(ref);
}
  
void ReferenceThread::addPhantomReference(vmkit::gc* ref) {
	llvm_gcroot(ref, 0);
	PhantomReferencesQueue.addReference(ref);
}

vmkit::gc** getReferent(vmkit::gc* obj) {
  llvm_gcroot(obj, 0);
	vmkit::VirtualMachine* vm = obj->getVirtualTable()->vm;
	vmkit::Thread::get()->attach(vm);
	return vm->getReferent(obj);
}

void setReferent(vmkit::gc* obj, vmkit::gc* val) {
  llvm_gcroot(obj, 0);
  llvm_gcroot(val, 0);
	vmkit::VirtualMachine* vm = obj->getVirtualTable()->vm;
	vmkit::Thread::get()->attach(vm);
	vm->setReferent(obj, val);
}
 
void invokeEnqueue(vmkit::gc* obj) {
  llvm_gcroot(obj, 0);
  TRY {
		vmkit::VirtualMachine* vm = obj->getVirtualTable()->vm;
		vmkit::Thread::get()->attach(vm);
		
    vm->enqueueReference(obj);
  } IGNORE;
  vmkit::Thread::get()->clearPendingException();
}

void ReferenceThread::enqueueStart(ReferenceThread* th) {
	vmkit::gc* res = NULL;
  llvm_gcroot(res, 0);

  while (true) {
    th->EnqueueLock.lock();
    while (th->ToEnqueueIndex == 0) {
      th->EnqueueCond.wait(&th->EnqueueLock);
    }
    th->EnqueueLock.unlock();

    while (true) {
      th->ToEnqueueLock.acquire();
      if (th->ToEnqueueIndex != 0) {
        res = th->ToEnqueue[th->ToEnqueueIndex - 1];
        --th->ToEnqueueIndex;
      }
      th->ToEnqueueLock.release();
      if (!res) break;

      invokeEnqueue(res);
      res = NULL;
    }
  }
}


void ReferenceThread::addToEnqueue(vmkit::gc* obj) {
  llvm_gcroot(obj, 0);
  if (ToEnqueueIndex >= ToEnqueueLength) {
    uint32 newLength = ToEnqueueLength * GROW_FACTOR;
		vmkit::gc** newQueue = new vmkit::gc*[newLength];
    if (!newQueue) {
      fprintf(stderr, "I don't know how to handle reference overflow yet!\n");
      abort();
    }   
    for (uint32 i = 0; i < ToEnqueueLength; ++i) {
      newQueue[i] = ToEnqueue[i];
    }   
    delete[] ToEnqueue;
    ToEnqueue = newQueue;
    ToEnqueueLength = newLength;
  }
  ToEnqueue[ToEnqueueIndex++] = obj;
}

void ReferenceQueue::addReference(vmkit::gc* ref) {
	llvm_gcroot(ref, 0);
	QueueLock.acquire();
	if (CurrentIndex >= QueueLength) {
		uint32 newLength = QueueLength * GROW_FACTOR;
		vmkit::gc** newQueue = new vmkit::gc*[newLength];
		memset(newQueue, 0, newLength * sizeof(vmkit::gc*));
		if (!newQueue) {
			fprintf(stderr, "I don't know how to handle reference overflow yet!\n");
			abort();
		}
		for (uint32 i = 0; i < QueueLength; ++i) newQueue[i] = References[i];
		delete[] References;
		References = newQueue;
		QueueLength = newLength;
	}
	References[CurrentIndex++] = ref;
	QueueLock.release();
}

vmkit::gc* ReferenceQueue::processReference(vmkit::gc* reference, ReferenceThread* th, uintptr_t closure) {
  if (!vmkit::Collector::isLive(reference, closure)) {
    setReferent(reference, 0);
    return NULL;
  }

	vmkit::gc* referent = *(getReferent(reference));

  if (!referent) {
    return NULL;
  }

  if (semantics == SOFT) {
    // TODO: are we are out of memory? Consider that we always are for now.
    if (false) {
      vmkit::Collector::retainReferent(referent, closure);
    }
  } else if (semantics == PHANTOM) {
    // Nothing to do.
  }

	vmkit::gc* newReference =
      vmkit::Collector::getForwardedReference(reference, closure);
  if (vmkit::Collector::isLive(referent, closure)) {
		vmkit::gc* newReferent = vmkit::Collector::getForwardedReferent(referent, closure);
    setReferent(newReference, newReferent);
    return newReference;
  } else {
    setReferent(newReference, 0);
    th->addToEnqueue(newReference);
    return NULL;
  }
}


void ReferenceQueue::scan(ReferenceThread* th, uintptr_t closure) {
  uint32 NewIndex = 0;

  for (uint32 i = 0; i < CurrentIndex; ++i) {
		vmkit::gc* obj = References[i];
		vmkit::gc* res = processReference(obj, th, closure);
    if (res) References[NewIndex++] = res;
  }

  CurrentIndex = NewIndex;
}

void ReferenceThread::tracer(uintptr_t closure) {
  for (uint32 i = 0; i < ToEnqueueIndex; ++i) {
    vmkit::Collector::markAndTraceRoot(ToEnqueue + i, closure);
  } 
	MutatorThread::tracer(closure);
}


FinalizerThread::FinalizerThread(VMKit* vmkit) : MutatorThread(vmkit) {
  FinalizationQueue = new vmkit::gc*[INITIAL_QUEUE_SIZE];
  QueueLength = INITIAL_QUEUE_SIZE;
  CurrentIndex = 0;

  ToBeFinalized = new vmkit::gc*[INITIAL_QUEUE_SIZE];
  ToBeFinalizedLength = INITIAL_QUEUE_SIZE;
  CurrentFinalizedIndex = 0;

	setDaemon();
}

void FinalizerThread::growFinalizationQueue() {
  if (CurrentIndex >= QueueLength) {
    uint32 newLength = QueueLength * GROW_FACTOR;
		vmkit::gc** newQueue = new vmkit::gc*[newLength];
    if (!newQueue) {
      fprintf(stderr, "I don't know how to handle finalizer overflows yet!\n");
      abort();
    }
    for (uint32 i = 0; i < QueueLength; ++i) newQueue[i] = FinalizationQueue[i];
    delete[] FinalizationQueue;
    FinalizationQueue = newQueue;
    QueueLength = newLength;
  }
}

void FinalizerThread::growToBeFinalizedQueue() {
  if (CurrentFinalizedIndex >= ToBeFinalizedLength) {
    uint32 newLength = ToBeFinalizedLength * GROW_FACTOR;
		vmkit::gc** newQueue = new vmkit::gc*[newLength];
    if (!newQueue) {
      fprintf(stderr, "I don't know how to handle finalizer overflows yet!\n");
      abort();
    }
    for (uint32 i = 0; i < ToBeFinalizedLength; ++i) newQueue[i] = ToBeFinalized[i];
    delete[] ToBeFinalized;
    ToBeFinalized = newQueue;
    ToBeFinalizedLength = newLength;
  }
}


void FinalizerThread::addFinalizationCandidate(vmkit::gc* obj) {
  llvm_gcroot(obj, 0);
  FinalizationQueueLock.acquire();
 
  if (CurrentIndex >= QueueLength) {
    growFinalizationQueue();
  }
  
  FinalizationQueue[CurrentIndex++] = obj;
  FinalizationQueueLock.release();
}

void FinalizerThread::scanFinalizationQueue(uintptr_t closure) {
  uint32 NewIndex = 0;
  for (uint32 i = 0; i < CurrentIndex; ++i) {
		vmkit::gc* obj = FinalizationQueue[i];

    if (!vmkit::Collector::isLive(obj, closure)) {
      obj = vmkit::Collector::retainForFinalize(FinalizationQueue[i], closure);
      
      if (CurrentFinalizedIndex >= ToBeFinalizedLength)
        growToBeFinalizedQueue();
      
      /* Add to object table */
      ToBeFinalized[CurrentFinalizedIndex++] = obj;
    } else {
      FinalizationQueue[NewIndex++] =
        vmkit::Collector::getForwardedFinalizable(obj, closure);
    }
  }
  CurrentIndex = NewIndex;
}

typedef void (*destructor_t)(void*);

void invokeFinalize(vmkit::gc* obj) {
  llvm_gcroot(obj, 0);
  TRY {
		llvm_gcroot(obj, 0);
		VirtualMachine* vm = obj->getVirtualTable()->vm;
		vmkit::Thread::get()->attach(vm);
		vm->finalizeObject(obj);
  } IGNORE;
  vmkit::Thread::get()->clearPendingException();
}

void FinalizerThread::finalizerStart(FinalizerThread* th) {
	vmkit::gc* res = NULL;
  llvm_gcroot(res, 0);

  while (true) {
    th->FinalizationLock.lock();
    while (th->CurrentFinalizedIndex == 0) {
      th->FinalizationCond.wait(&th->FinalizationLock);
    }
    th->FinalizationLock.unlock();

    while (true) {
      th->FinalizationQueueLock.acquire();
      if (th->CurrentFinalizedIndex != 0) {
        res = th->ToBeFinalized[th->CurrentFinalizedIndex - 1];
        --th->CurrentFinalizedIndex;
      }
      th->FinalizationQueueLock.release();
      if (!res) break;

			vmkit::VirtualTable* VT = res->getVirtualTable();
			ASSERT(VT->vm);
      if (VT->operatorDelete) {
        destructor_t dest = (destructor_t)VT->destructor;
        dest(res);
      } else {
        invokeFinalize(res);
      }
      res = NULL;
    }
  }
}

void FinalizerThread::tracer(uintptr_t closure) {
  for (uint32 i = 0; i < CurrentFinalizedIndex; ++i) {
    vmkit::Collector::markAndTraceRoot(ToBeFinalized + i, closure);
  }
	MutatorThread::tracer(closure);
}
