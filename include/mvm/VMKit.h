#ifndef _VMKIT_H_
#define _VMKIT_H_

#include "mvm/Allocator.h"
#include "mvm/Threads/CollectionRV.h"
#include "mvm/VirtualMachine.h"

#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class Module;
  class TargetMachine;
}

namespace vmkit {
class MethodInfo;
class VMKit;
class gc;
class FinalizerThread;
class ReferenceThread;

class FunctionMap {
public:
  /// Functions - Map of applicative methods to function pointers. This map is
  /// used when walking the stack so that VMKit knows which applicative method
  /// is executing on the stack.
  ///
  std::map<void*, MethodInfo*> Functions;

  /// FunctionMapLock - Spin lock to protect the Functions map.
  ///
  vmkit::SpinLock FunctionMapLock;

  /// IPToMethodInfo - Map a code start instruction instruction to the MethodInfo.
  ///
  MethodInfo* IPToMethodInfo(void* ip);

  /// addMethodInfo - A new instruction pointer in the function map.
  ///
  void addMethodInfo(MethodInfo* meth, void* ip);

  /// removeMethodInfos - Remove all MethodInfo owned by the given owner.
  void removeMethodInfos(void* owner);

  FunctionMap();
};

class VMKit : public vmkit::PermanentObject {
public:
  /// allocator - Bump pointer allocator to allocate permanent memory of VMKit
  vmkit::BumpPtrAllocator& allocator;

	// initialise - initialise vmkit. If never called, will be called by the first constructor of vmkit
	static void initialise(llvm::CodeGenOpt::Level = llvm::CodeGenOpt::Default,
                         llvm::Module* TheModule = 0,
                         llvm::TargetMachine* TheTarget = 0);

  VMKit(vmkit::BumpPtrAllocator &Alloc);

	LockRecursive                _vmkitLock;

	void vmkitLock() { _vmkitLock.lock(); }
	void vmkitUnlock() { _vmkitLock.unlock(); }

	/// ------------------------------------------------- ///
	/// ---             vm managment                  --- ///
	/// ------------------------------------------------- ///
	// vms - the list of vms. 
	//       synchronized with vmkitLock
	VirtualMachine**             vms;
	size_t                       vmsArraySize;

	size_t addVM(VirtualMachine* vm);
	void   removeVM(size_t id);

	/// ------------------------------------------------- ///
	/// ---             thread managment              --- ///
	/// ------------------------------------------------- ///
  /// preparedThreads - the list of prepared threads, they are not yet running.
	///                   synchronized with vmkitLock
  ///
	CircularBase<Thread>         preparedThreads;

  /// runningThreads - the list of running threads
	///                  synchronize with vmkitLock
  ///
	CircularBase<Thread>         runningThreads;

  /// doExit - Should the VM exit now?
  bool doExit;

  /// exitingThread - Thread that is currently exiting. Used by the thread
  /// manager to free the resources (stack) used by a thread.
  vmkit::Thread* exitingThread;

  /// numberOfRunningThreads - Number of threads that are running in the system
  /// threads.
  //
  uint32 numberOfRunningThreads;

  /// nonDaemonThreads - Number of threads in the system that are not daemon
  /// threads.
  //
  uint32 nonDaemonThreads;

  /// nonDaemonVar - Condition variable to wake up the initial thread when it
  /// waits for other non-daemon threads to end. The non-daemon thread that
  /// decrements the nonDaemonThreads variable to zero wakes up the initial
  /// thread.
  ///
  vmkit::Cond nonDaemonVar;
  
  /// leave - A thread calls this function when it leaves the thread system.
  ///
  void leaveNonDaemonMode(vmkit::Thread* th);

  /// enter - A thread calls this function when it enters the thread system.
  ///
  void enterNonDaemonMode(vmkit::Thread* th);

public:
	void registerPreparedThread(vmkit::Thread* th);  
	void unregisterPreparedThread(vmkit::Thread* th);

	void notifyThreadStart(vmkit::Thread* th);  
	void notifyThreadQuit(vmkit::Thread* th);

	void freeThread(vmkit::Thread* th);

	void waitNonDaemonThreads();

  /// exit - Exit this vmkit.
  void exit();

	/// ------------------------------------------------- ///
	/// ---             memory managment              --- ///
	/// ------------------------------------------------- ///
  /// rendezvous - The rendezvous implementation for garbage collection.
  ///
#ifdef WITH_LLVM_GCC
  CooperativeCollectionRV      rendezvous;
#else
  UncooperativeCollectionRV    rendezvous;
#endif

private:
  /// enqueueThread - The thread that finalizes references.
  ///
	FinalizerThread*             finalizerThread;
  
  /// enqueueThread - The thread that enqueues references.
  ///
  ReferenceThread*             referenceThread;

  /// getAndAllocateFinalizerThread - get the finalizer thread and allocate it if it does not exist
  ///
	FinalizerThread*             getAndAllocateFinalizerThread();

  /// getAndAllocateReferenceThread - get the reference thread and allocate it if it does not exist
  ///
	ReferenceThread*             getAndAllocateReferenceThread();

public:
  /// addWeakReference - Add a weak reference to the queue.
  ///
  void addWeakReference(vmkit::gc* ref);
  
  /// addSoftReference - Add a weak reference to the queue.
  ///
  void addSoftReference(vmkit::gc* ref);
  
  /// addPhantomReference - Add a weak reference to the queue.
  ///
  void addPhantomReference(vmkit::gc* ref);

  /// addFinalizationCandidate - Add an object to the queue of objects with
  /// a finalization method.
  ///
  void addFinalizationCandidate(gc* object);

  /// scanFinalizationQueue - Scan objets with a finalized method and schedule
  /// them for finalization if they are not live.
  /// 
  void scanFinalizationQueue(uintptr_t closure);
  
  /// scanWeakReferencesQueue - Scan all weak references. Called by the GC
  /// before scanning the finalization queue.
  /// 
  void scanWeakReferencesQueue(uintptr_t closure);
  
  /// scanSoftReferencesQueue - Scan all soft references. Called by the GC
  /// before scanning the finalization queue.
  ///
  void scanSoftReferencesQueue(uintptr_t closure);
  
  /// scanPhantomReferencesQueue - Scan all phantom references. Called by the GC
  /// after the finalization queue.
  ///
  void scanPhantomReferencesQueue(uintptr_t closure);

	bool startCollection(); // 1 ok, begin collection, 0 do not start collection
	void endCollection();

  void tracer(uintptr_t closure);

	/// ------------------------------------------------- ///
	/// ---    backtrace related methods              --- ///
	/// ------------------------------------------------- ///
  /// FunctionsCache - cache of compiled functions
	//  
  FunctionMap FunctionsCache;

  MethodInfo* IPToMethodInfo(void* ip) {
    return FunctionsCache.IPToMethodInfo(ip);
  }

  void removeMethodInfos(void* owner) {
    FunctionsCache.removeMethodInfos(owner);
  }
};

}

#endif
