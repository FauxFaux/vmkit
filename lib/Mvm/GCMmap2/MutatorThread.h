//===--------- MutatorThread.h - Thread for GC ----------------------------===//
//
//                     The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


#ifndef MVM_MUTATOR_THREAD_H
#define MVM_MUTATOR_THREAD_H

#include "vmkit/Threads/Thread.h"

namespace vmkit {

class MutatorThread : public vmkit::Thread {
public:
  MutatorThread(VMKit* vmkit) : vmkit::Thread(vmkit) {}
};

}

#endif
