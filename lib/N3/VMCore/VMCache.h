//===--------- VMCache.h - Inline cache for virtual calls -----------------===//
//
//                              N3
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef N3_VM_CACHE_H
#define N3_VM_CACHE_H

#include "vmkit/Object.h"
#include "vmkit/PrintBuffer.h"
#include "vmkit/Threads/Locks.h"

#include "llvm/DerivedTypes.h"

#include "types.h"

namespace n3 {

class Assembly;
class Enveloppe;
class VMClass;

class CacheNode : public vmkit::PermanentObject {
public:
  virtual void print(vmkit::PrintBuffer* buf) const;

  void* methPtr;
  VMClass* lastCible;
  CacheNode* next;
  Enveloppe* enveloppe;
  bool box;
  
  static const llvm::Type* llvmType;

  static CacheNode* allocate(vmkit::BumpPtrAllocator &allocator);
};

class Enveloppe : public vmkit::PermanentObject {
public:
  virtual void print(vmkit::PrintBuffer* buf) const;
  
  CacheNode *firstCache;
  vmkit::Lock* cacheLock;
  VMMethod* originalMethod;

  static const llvm::Type* llvmType;

  static Enveloppe* allocate(vmkit::BumpPtrAllocator &allocator, VMMethod* orig);

};

} // end namespace n3

#endif
