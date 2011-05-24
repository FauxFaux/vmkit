#include "vmkit/VMKit.h"
#include "vmkit/VirtualMachine.h"
#include "vmkit/SystemThreads.h"
#include "vmkit/GC.h"
#include "vmkit/JIT.h"

using namespace vmkit;

#if 0
#define dprintf(...) do { printf("[%p] vmkit: ", (void*)vmkit::Thread::get()); printf(__VA_ARGS__); } while(0)
#else
#define dprintf(...)
#endif

static SpinLock initedLock;
static bool     inited = false;

VMKit::VMKit(vmkit::BumpPtrAllocator &Alloc) : allocator(Alloc) {
	initialise();

	vms          = 0;
	vmsArraySize = 0;

	numberOfRunningThreads = 0;
	nonDaemonThreads = 0;
	doExit = 0;
}

void VMKit::initialise(llvm::CodeGenOpt::Level level, llvm::Module* TheModule, llvm::TargetMachine* TheTarget) {
	initedLock.lock();
	if(!inited) {
		inited = true;
		vmkit::MvmModule::initialise(level, TheModule, TheTarget);
		vmkit::Collector::initialise();
	}
	initedLock.unlock();
}

void VMKit::scanWeakReferencesQueue(uintptr_t closure) {
	if(referenceThread)
		referenceThread->WeakReferencesQueue.scan(referenceThread, closure);
}
  
void VMKit::scanSoftReferencesQueue(uintptr_t closure) {
	if(referenceThread)
		referenceThread->SoftReferencesQueue.scan(referenceThread, closure);
}
  
void VMKit::scanPhantomReferencesQueue(uintptr_t closure) {
	if(referenceThread)
		referenceThread->PhantomReferencesQueue.scan(referenceThread, closure);
}

void VMKit::scanFinalizationQueue(uintptr_t closure) {
	if(finalizerThread)
		finalizerThread->scanFinalizationQueue(closure);
}

FinalizerThread* VMKit::getAndAllocateFinalizerThread() {
	if(!finalizerThread) {
		vmkitLock();
		if(!finalizerThread) {
			finalizerThread = new FinalizerThread(this);
			finalizerThread->start((void (*)(vmkit::Thread*))FinalizerThread::finalizerStart);
		}
		vmkitUnlock();
	}
	return finalizerThread;
}

ReferenceThread* VMKit::getAndAllocateReferenceThread() {
	if(!referenceThread) {
		vmkitLock();
		if(!referenceThread) {
			referenceThread = new ReferenceThread(this);
			referenceThread->start((void (*)(vmkit::Thread*))ReferenceThread::enqueueStart);
		}
		vmkitUnlock();
	}
	return referenceThread;
}

void VMKit::addFinalizationCandidate(vmkit::gc* object) {
  llvm_gcroot(object, 0);
  getAndAllocateFinalizerThread()->addFinalizationCandidate(object);
}

void VMKit::addWeakReference(vmkit::gc* ref) {
  llvm_gcroot(ref, 0);
	getAndAllocateReferenceThread()->addWeakReference(ref);
}
  
void VMKit::addSoftReference(vmkit::gc* ref) {
  llvm_gcroot(ref, 0);
	getAndAllocateReferenceThread()->addSoftReference(ref);
}
  
void VMKit::addPhantomReference(vmkit::gc* ref) {
	llvm_gcroot(ref, 0);
	getAndAllocateReferenceThread()->addPhantomReference(ref);
}

void VMKit::tracer(uintptr_t closure) {
	// don't have to take the vmkitLock, already taken by the rendezvous.
	for(size_t i=0; i<vmsArraySize; i++)
		if(vms[i])
			vms[i]->tracer(closure);
}

bool VMKit::startCollection() {
	// do not take the lock here because if a gc is currently running, it could call enterUncooperativeCode 
	// which will execute the gc and we will therefore recall the gc just behind. Stupid because the previous one
	// should have freed some memory
  rendezvous.startRV();

  if (rendezvous.getInitiator() != NULL) {
    rendezvous.cancelRV();
    rendezvous.join();
    return 0;
  } else {
		dprintf("Start collection\n");
		// Lock thread lock, so that we can traverse the vm and thread lists safely. This will be released on finishRV.
		vmkitLock();

		if(finalizerThread)
			finalizerThread->FinalizationQueueLock.acquire();
		if(referenceThread) {
			referenceThread->ToEnqueueLock.acquire();
			referenceThread->SoftReferencesQueue.acquire();
			referenceThread->WeakReferencesQueue.acquire();
			referenceThread->PhantomReferencesQueue.acquire();
		}

		// call first startCollection on each vm to avoid deadlock. 
		// indeed, a vm could want to execute applicative code
		for(size_t i=0; i<vmsArraySize; i++)
			if(vms[i])
				vms[i]->startCollection();

    rendezvous.synchronize();

		return 1;
	}
}

void VMKit::endCollection() {
	dprintf("End collection\n");

	rendezvous.finishRV();

	for(size_t i=0; i<vmsArraySize; i++)
		if(vms[i])
			vms[i]->endCollection();

	if(finalizerThread) {
		finalizerThread->FinalizationQueueLock.release();
		finalizerThread->FinalizationCond.broadcast();
	}

	if(referenceThread) {
		referenceThread->ToEnqueueLock.release();
		referenceThread->SoftReferencesQueue.release();
		referenceThread->WeakReferencesQueue.release();
		referenceThread->PhantomReferencesQueue.release();
		referenceThread->EnqueueCond.broadcast();
	}

	vmkitUnlock();
}

size_t VMKit::addVM(VirtualMachine* vm) {
	dprintf("add vm: %p\n", (void*)vm);
	vmkitLock();

	for(size_t i=0; i<vmsArraySize; i++)
		if(!vms[i]) {
			vms[i] = vm;
			vmkitUnlock();
			return i;
		}

	int res = vmsArraySize;
	vmsArraySize = vmsArraySize ? (vmsArraySize<<1) : 1;
	// reallocate the vms
	VirtualMachine **newVms = new VirtualMachine*[vmsArraySize];

	memcpy(newVms, vms, res*sizeof(VirtualMachine*));
	memset(newVms + res*sizeof(VirtualMachine*), 0, (vmsArraySize-res)*sizeof(VirtualMachine*));
	newVms[res] = vm;

	VirtualMachine **oldVms = vms;
	vms = newVms; // vms must always contain valid data
	delete[] oldVms;
	
 	// reallocate the allVMDatas
 	for(Thread* cur=preparedThreads.next(); cur!=&preparedThreads; cur=cur->next()) {
		cur->reallocAllVmsData(res, vmsArraySize);
	}

 	for(Thread* cur=runningThreads.next(); cur!=&runningThreads; cur=cur->next()) {
		cur->reallocAllVmsData(res, vmsArraySize);
	}

	vmkitUnlock();

	return res;
}

void VMKit::removeVM(size_t id) {
	dprintf("remove vm: %d %p\n", id, (void*)vms[id]);
	// I think that we only should call this function when all the thread data are released
	vms[id] = 0;
}

void VMKit::registerPreparedThread(vmkit::Thread* th) {
	dprintf("Register prepared thread: %p\n", (void*)th);
	vmkitLock();
	th->appendTo(&preparedThreads);
	th->reallocAllVmsData(0, vmsArraySize);
	vmkitUnlock();
}
  
void VMKit::unregisterPreparedThread(vmkit::Thread* th) {
	dprintf("Unregister prepared thread: %p\n", (void*)th);
	vmkitLock();
	th->remove();
	size_t n = vmsArraySize;
	vmkitUnlock();

	for(size_t i=0; i<n; i++)
		if(th->allVmsData[i])
			delete th->allVmsData[i];
	delete th->allVmsData;
	th->allVmsData = 0;
}

void VMKit::notifyThreadStart(vmkit::Thread* th) {
	dprintf("Register running thread: %p\n", (void*)th);
	vmkitLock();
	numberOfRunningThreads++;
	th->remove();
	th->appendTo(&runningThreads);
	vmkitUnlock();
}
  
void VMKit::notifyThreadQuit(vmkit::Thread* th) {
	dprintf("Unregister running thread: %p\n", (void*)th);
	vmkitLock();
	numberOfRunningThreads--;
	th->remove();
	th->appendTo(&preparedThreads);
	vmkitUnlock();
}

void VMKit::leaveNonDaemonMode(vmkit::Thread* th) {
	vmkitLock();
	dprintf("Leave non daemon mode: %p\n", (void*)th);
	--nonDaemonThreads;
	if (nonDaemonThreads == 0) th->vmkit->exit();
	vmkitUnlock();
}

void VMKit::enterNonDaemonMode(vmkit::Thread* th) {
	vmkitLock();
	dprintf("Enter non daemon mode: %p\n", (void*)th);
	++nonDaemonThreads;
	vmkitUnlock();
}

#include <sys/mman.h>

void VMKit::freeThread(vmkit::Thread* th) {
	vmkitLock();

	while(exitingThread)
		nonDaemonVar.wait(&_vmkitLock);

	exitingThread = th;
	nonDaemonVar.broadcast();
	vmkitUnlock();
}

void VMKit::waitNonDaemonThreads() {
	vmkitLock();

	while (!doExit) {
		if (exitingThread == NULL) {
			nonDaemonVar.wait(&_vmkitLock);
		} else {
			vmkit::MutatorThread* th = (vmkit::MutatorThread*)exitingThread;
			exitingThread = NULL;
			delete th;
			nonDaemonVar.broadcast();
		}
	}

	vmkitUnlock();
}

void VMKit::exit() {
	dprintf("exit\n");
	doExit = true;
	vmkitLock();
	nonDaemonVar.signal();
	vmkitUnlock();
}
