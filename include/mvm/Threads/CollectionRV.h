//===---------- CollectionRV.h - Rendez-vous for garbage collection -------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _MVM_COLLECTIONRV_H_
#define _MVM_COLLECTIONRV_H_

#include "mvm/Threads/Cond.h"
#include "mvm/Threads/Locks.h"
#include "mvm/Threads/Thread.h"

namespace mvm {

class CollectionRV {
public:
  /// _lockRV - Lock for synchronization.
  LockNormal _lockRV;         
  
  /// condEndRV - Condition for unlocking other tasks (write protect).
  Cond condEndRV;

  /// collectionCond - Condition for unblocking the initator.
  Cond condInitiator;

  /// nbJoined - Number of threads that joined the rendezvous.
  unsigned nbJoined;
  
public: 
  CollectionRV() {
    nbJoined = 0;
  }

  void lockRV() { _lockRV.lock(); }
  void unlockRV() { _lockRV.unlock(); }

  void waitEndOfRV();
  void waitRV();
  
  void startRV() {
    mvm::Thread::get()->inRV = true;
    lockRV();
  }

  void cancelRV() {
    unlockRV();
    mvm::Thread::get()->inRV = false;
  }
  
  void another_mark();

  virtual void finishRV() = 0;
  virtual void synchronize() = 0;

  virtual void join() = 0;
  virtual void joinAfterUncooperative(void* SP) = 0;
  virtual void joinBeforeUncooperative() = 0;

  /// prepareForJoin - for uncooperative gc, prepare the SIGGC handler
	virtual void prepareForJoin() = 0;
};

class CooperativeCollectionRV : public CollectionRV {
public: 
  void finishRV();
  void synchronize();

  void join();
  void joinAfterUncooperative(void* SP);
  void joinBeforeUncooperative();
	void prepareForJoin();
};

class UncooperativeCollectionRV : public CollectionRV {
public: 
  void finishRV();
  void synchronize();

  void join();
  void joinAfterUncooperative(void* SP);
  void joinBeforeUncooperative();
  void prepareForJoin();
};


}

#endif
