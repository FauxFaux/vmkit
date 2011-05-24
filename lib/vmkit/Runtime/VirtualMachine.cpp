#include "vmkit/VirtualMachine.h"
#include "vmkit/VMKit.h"
#include "MutatorThread.h"

using namespace vmkit;

VirtualMachine::VirtualMachine(vmkit::BumpPtrAllocator &Alloc, vmkit::VMKit* vmk) :	allocator(Alloc) {
	vmkit = vmk;
	vmID = vmkit->addVM(this);
}

VirtualMachine::~VirtualMachine() {
	vmkit->removeVM(vmID);
}

class LauncherThread : public MutatorThread {
public:
 	void          (*realStart)(VirtualMachine*, int, char**);
 	VirtualMachine* vm;
 	int             argc;
 	char**          argv;

 	LauncherThread(VMKit* vmkit, void (*s)(VirtualMachine*, int, char**), VirtualMachine* v, int ac, char** av) : MutatorThread(vmkit) {
 		realStart = s;
 		vm = v;
 		argc = ac;
 		argv = av;
 	}

 	static void launch(LauncherThread* th) {
 		if(th->realStart)
 			th->realStart(th->vm, th->argc, th->argv);
 		else
 			th->vm->runApplicationImpl(th->argc, th->argv);
 	}
};

void VirtualMachine::runApplication(void (*starter)(VirtualMachine*, int, char**), int argc, char **argv) {
	(new LauncherThread(vmkit, starter, this, argc, argv))->start((void (*)(Thread*))LauncherThread::launch);
}

void VirtualMachine::runApplication(int argc, char** argv) {
	runApplication(0, argc, argv);
}
