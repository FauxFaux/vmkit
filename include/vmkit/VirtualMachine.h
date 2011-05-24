//===--------- VirtualMachine.h - Registering a VM ------------------------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Pierre et Marie Curie 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MVM_VIRTUALMACHINE_H
#define MVM_VIRTUALMACHINE_H

#include "vmkit/Allocator.h"
#include "vmkit/CollectionRV.h"
#include "vmkit/Locks.h"

#include <cassert>
#include <map>

namespace vmkit {

/// VirtualMachine - This class is the root of virtual machine classes. It
/// defines what a VM should be.
///
class VirtualMachine : public vmkit::PermanentObject {
private:
	friend class VMKit;
	VirtualMachine(vmkit::BumpPtrAllocator &Alloc) : allocator(Alloc) {}

protected:
  VirtualMachine(vmkit::BumpPtrAllocator &Alloc, vmkit::VMKit* vmk);

public:
  virtual ~VirtualMachine();

  /// allocator - Bump pointer allocator to allocate permanent memory
  /// related to this VM.
  ///
  vmkit::BumpPtrAllocator& allocator;

	/// vmkit - a pointer to vmkit that contains information on all the vms
	///
	vmkit::VMKit* vmkit;

	/// vmID - id of the vm
	size_t vmID;

//===----------------------------------------------------------------------===//
// (1) thread-related methods.
//===----------------------------------------------------------------------===//
	/// buildVMThreadData - allocate a java thread for the underlying mutator. Called when the java thread is a foreign thread.
	///
	virtual VMThreadData* buildVMThreadData(Thread* mut) { return new VMThreadData(this, mut); }

	/// runApplicationImpl - code executed after a runApplication in a vmkit thread
	///
	virtual void runApplicationImpl(int argc, char** argv) {}

	/// runApplication - launch runApplicationImpl in a vmkit thread. The vmData is not allocated.
	///
	void runApplication(int argc, char** argv);

	/// runApplication - launch starter in a vmkit thread. The vmData is not allocated.
	///
	void runApplication(void (*starter)(VirtualMachine* vm, int argc, char** argv), int argc, char** argv);
  
//===----------------------------------------------------------------------===//
// (2) GC-related methods.
//===----------------------------------------------------------------------===//
  /// startCollection - Preliminary code before starting a GC.
  ///
  virtual void startCollection() {}
  
  /// endCollection - Code after running a GC.
  ///
  virtual void endCollection() {}

  /// finalizeObject - invoke the finalizer of a java object
  ///
	virtual void finalizeObject(vmkit::gc* obj) = 0;

  /// getReferentPtr - return the referent of a reference
  ///
	virtual vmkit::gc** getReferent(vmkit::gc* ref) = 0;

  /// setReferentPtr - set the referent of a reference
  ///
	virtual void setReferent(vmkit::gc* ref, vmkit::gc* val) = 0;

  /// enqueueReference - enqueue the reference
  ///
	virtual bool enqueueReference(vmkit::gc* _obj) = 0;

  /// tracer - Trace this virtual machine's GC-objects. 
	///    Called once by vm. If you have GC-objects in a thread specific data, redefine the tracer of your VMThreadData.
  ///
  virtual void tracer(uintptr_t closure) {}

  /// getObjectSize - Get the size of this object. Used by copying collectors.
  ///
  virtual size_t getObjectSize(gc* object) = 0;

  /// getObjectTypeName - Get the type of this object. Used by the GC for
  /// debugging purposes.
  ///
  virtual const char* getObjectTypeName(gc* object) { return "An object"; }
};

} // end namespace vmkit
#endif // MVM_VIRTUALMACHINE_H
