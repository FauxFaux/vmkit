//===---------------- main.cc - VMKit Garbage Collector ---------------------===//
//
//                              VMKit
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "vmkit/GC.h"
#include "vmkit/Thread.h"
#include <stdio.h>

void destr(gc *me, size_t sz) {
 	printf("Destroy %p\n", (void*)me);
}

void trace(gc *me, size_t sz) {
	// printf("Trace %p\n", (void*)me);
}

void marker(void*) {
	// printf("Marker...\n");
}

int main(int argc, char **argv) {
  vmkit::Collector::initialise();
#ifdef MULTIPLE_GC
  vmkit::Thread::get()->GC->destroy();
#else
  vmkit::Collector::destroy();
#endif
  return 0;
}

