//===-------- CollectionRV.cpp - Rendez-vous for garbage collection -------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <signal.h>
#include "vmkit/VirtualMachine.h"
#include "vmkit/CollectionRV.h"
#include "vmkit/VMKit.h"
#include "vmkit/GC.h"

#include "debug.h"

#if 0
#define dprintf(...) do { printf("[%p] CollectionRV: ", (void*)vmkit::Thread::get()); printf(__VA_ARGS__); } while(0)
#else
#define dprintf(...)
#endif

using namespace vmkit;

void CollectionRV::beginSynchronize() {
  assert(initiator == NULL);
  initiator = vmkit::Thread::get();
	nbJoined = 0;
}

void CollectionRV::endSynchronize() {
  initiator = NULL;
}

void CollectionRV::another_mark() {
	VMKit *vmkit = vmkit::Thread::get()->vmkit;
  assert(th->getLastSP() != NULL);
  assert(nbJoined < vmkit->NumberOfThreads);
  nbJoined++;
	dprintf("another_mark: %d %d\n", nbJoined, vmkit->numberOfRunningThreads);
  if (nbJoined == vmkit->numberOfRunningThreads) {
    condInitiator.broadcast();
  }
}

void CollectionRV::waitEndOfRV() {
  vmkit::Thread* th = vmkit::Thread::get();
  assert(th->getLastSP() != NULL);

  while (th->doYield) {
    condEndRV.wait(&_lockRV);
  }
}

void CollectionRV::waitRV() {
  // Add myself.
  nbJoined++;

  while (nbJoined != vmkit::Thread::get()->vmkit->numberOfRunningThreads) {
		dprintf("wait...\n");
    condInitiator.wait(&_lockRV);
		dprintf("wake up!!! %d %d\n", nbJoined, vmkit::Thread::get()->vmkit->numberOfRunningThreads);
  } 
}

void CooperativeCollectionRV::synchronize() {
	dprintf("synchronize\n");

  vmkit::Thread* self = vmkit::Thread::get();
  // Lock thread lock, so that we can traverse the thread list safely. This will
  // be released on finishRV.
	vmkit::VMKit* vmkit = self->vmkit;

	beginSynchronize();

	for(Thread* cur=vmkit->runningThreads.next(); cur!=&vmkit->runningThreads; cur=cur->next()) { 
    cur->doYield = true;
    assert(!cur->joinedRV);
  }

  // The CAS is not necessary but it does a memory barrier. 
  __sync_bool_compare_and_swap(&(self->joinedRV), false, true);

  // Lookup currently blocked threads.
	for(Thread* cur=vmkit->runningThreads.next(); cur!=&vmkit->runningThreads; cur=cur->next()) {
		if(cur->getLastSP() && cur != self) {
      nbJoined++;
      cur->joinedRV = true;
    }
  }

  // And wait for other threads to finish.
  waitRV();

  // Unlock, so that threads in uncooperative code that go back to cooperative
  // code can set back their lastSP.
  unlockRV();
}


#if defined(__MACH__)
# define SIGGC  SIGXCPU
#else
# define SIGGC  SIGPWR
#endif

void UncooperativeCollectionRV::synchronize() { 
	dprintf("synchronize()\n");

  vmkit::Thread* self = vmkit::Thread::get();
	vmkit::VMKit* vmkit = self->vmkit;

	beginSynchronize();

	for(Thread* cur=vmkit->runningThreads.next(); cur!=&vmkit->runningThreads; cur=cur->next()) { 
		if(cur!=self) {
			int res;
			res = cur->kill(SIGGC);
			assert(!res && "Error on kill");
		}
	}
  
  // And wait for other threads to finish.
	dprintf("wait()\n");
  waitRV();

	dprintf("unlock()\n");
  // Unlock, so that threads in uncooperative code that go back to cooperative
  // code can set back their lastSP.
  unlockRV();
}


void UncooperativeCollectionRV::join() {
	dprintf("join()\n");
  vmkit::Thread* th = vmkit::Thread::get();
  th->inRV = true;

	dprintf("lock()\n");
  lockRV();
	dprintf("locked()\n");
  void* old = th->getLastSP();
  th->setLastSP(FRAME_PTR());
	dprintf("another()\n");
  another_mark();
	dprintf("waitEnd()\n");
  waitEndOfRV();
  th->setLastSP(old);
  unlockRV();

  th->inRV = false;
}

void CooperativeCollectionRV::join() {
  vmkit::Thread* th = vmkit::Thread::get();
  assert(th->doYield && "No yield");
  assert((th->getLastSP() == NULL) && "SP present in cooperative code");

  th->inRV = true;
  
  lockRV();
  th->setLastSP(FRAME_PTR());
  th->joinedRV = true;
  another_mark();
  waitEndOfRV();
  th->setLastSP(0);
  unlockRV();
  
  th->inRV = false;
}

void CooperativeCollectionRV::joinBeforeUncooperative() {
  vmkit::Thread* th = vmkit::Thread::get();
  assert((th->getLastSP() != NULL) &&
         "SP not set before entering uncooperative code");

  th->inRV = true;
  
  lockRV();
  if (th->doYield) {
    if (!th->joinedRV) {
      th->joinedRV = true;
      another_mark();
    }
    waitEndOfRV();
  }
  unlockRV();

  th->inRV = false;
}

void CooperativeCollectionRV::joinAfterUncooperative(void* SP) {
  vmkit::Thread* th = vmkit::Thread::get();
  assert((th->getLastSP() == NULL) &&
         "SP set after entering uncooperative code");

  th->inRV = true;

  lockRV();
  if (th->doYield) {
    th->setLastSP(SP);
    if (!th->joinedRV) {
      th->joinedRV = true;
      another_mark();
    }
    waitEndOfRV();
    th->setLastSP(NULL);
  }
  unlockRV();

  th->inRV = false;
}

extern "C" void conditionalSafePoint() {
  vmkit::Thread* th = vmkit::Thread::get();
  th->vmkit->rendezvous.join();
}

void CooperativeCollectionRV::finishRV() {
  lockRV();
  
	vmkit::VMKit* vmkit = vmkit::Thread::get()->vmkit;

  for(vmkit::Thread* cur=vmkit->runningThreads.next(); cur!=&vmkit->runningThreads; cur=cur->next()) {
    assert(cur->doYield && "Inconsistent state");
    assert(cur->joinedRV && "Inconsistent state");
    cur->doYield = false;
    cur->joinedRV = false;
  }
	
  assert(nbJoined == vmkit->NumberOfThreads && "Inconsistent state");

  condEndRV.broadcast();
	endSynchronize();
  unlockRV();
  vmkit::Thread::get()->inRV = false;
}

void CooperativeCollectionRV::prepareForJoin() {
	/// nothing to do
	dprintf("prepareForJoin()\n");
}

void UncooperativeCollectionRV::finishRV() {
	dprintf("finishRV()\n");
  lockRV();
	dprintf("locked()\n");
  vmkit::Thread* th = vmkit::Thread::get();
  assert(nbJoined == th->vmkit->NumberOfThreads && "Inconsistent state");

	dprintf("broadcast()\n");
  condEndRV.broadcast();
	endSynchronize();
  unlockRV();
  th->inRV = false;
}

void UncooperativeCollectionRV::joinAfterUncooperative(void* SP) {
  UNREACHABLE();
}

void UncooperativeCollectionRV::joinBeforeUncooperative() {
  UNREACHABLE();
}

static void siggcHandler(int) {
  vmkit::Thread* th = vmkit::Thread::get();
  th->vmkit->rendezvous.join();
}

void UncooperativeCollectionRV::prepareForJoin() {
	dprintf("set SIGGC()\n");
  // Set the SIGGC handler for uncooperative rendezvous.
  struct sigaction sa;
  sigset_t mask;
  sigaction(SIGGC, 0, &sa);
  sigfillset(&mask);
  sa.sa_mask = mask;
  sa.sa_handler = siggcHandler;
  sa.sa_flags |= SA_RESTART;
  sigaction(SIGGC, &sa, NULL);
  
  if (initiator != 0) {
    // In uncooperative mode, we may have missed a signal.
    join();
  }
}
