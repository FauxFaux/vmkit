//===-- ClasspathReflect.h - Internal representation of core system classes --//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef JNJVM_CLASSPATH_REFLECT_H
#define JNJVM_CLASSPATH_REFLECT_H

#include "VmkitGC.h"

#include "JavaClass.h"
#include "JavaObject.h"
#include "JavaThread.h"

extern "C" j3::JavaObject* internalFillInStackTrace(j3::JavaObject*);

namespace j3 {

class JavaObjectClass : public JavaObject {
private:
  JavaObject* signers;
  JavaObject* pd;
  UserCommonClass* vmdata;
  JavaObject* constructor;

public:
  
  static UserCommonClass* getClass(JavaObjectClass* cl) __attribute__ ((always_inline)) {
    llvm_gcroot(cl, 0);
    return cl->vmdata;
  }

  static void setClass(JavaObjectClass* cl, UserCommonClass* vmdata) {
    llvm_gcroot(cl, 0);
    cl->vmdata = vmdata;
  }

  static void setProtectionDomain(JavaObjectClass* cl, JavaObject* pd) {
    llvm_gcroot(cl, 0);
    llvm_gcroot(pd, 0);
    vmkit::Collector::objectReferenceWriteBarrier(
        (gc*)cl, (gc**)&(cl->pd), (gc*)pd);
  }
  
  static JavaObject* getProtectionDomain(JavaObjectClass* cl) {
    llvm_gcroot(cl, 0);
    return cl->pd;
  }

  static void staticTracer(JavaObjectClass* obj, word_t closure) {
    llvm_gcroot(obj, 0);
    vmkit::Collector::markAndTrace(obj, &obj->pd, closure);
    vmkit::Collector::markAndTrace(obj, &obj->signers, closure);
    vmkit::Collector::markAndTrace(obj, &obj->constructor, closure);
    if (obj->vmdata) {
      JavaObject** Obj = obj->vmdata->classLoader->getJavaClassLoaderPtr();
      if (*Obj) vmkit::Collector::markAndTraceRoot(obj, Obj, closure);
    }
  }

  static ArrayObject* getDeclaredAnnotations(JavaObjectClass* Cl);
  static ArrayObject* getDeclaredClasses(JavaObjectClass* Cl, bool publicOnly);
  static ArrayObject* getDeclaredConstructors(JavaObjectClass* Cl, bool publicOnly);
  static ArrayObject* getDeclaredFields(JavaObjectClass* Cl, bool publicOnly);
  static ArrayObject* getDeclaredMethods(JavaObjectClass* Cl, bool publicOnly);
  static ArrayObject* getInterfaces(JavaObjectClass* Cl);
  static JavaObject* getDeclaringClass(JavaObjectClass* Cl);
  static int getModifiers(JavaObjectClass* Cl);
  static JavaObject* getEnclosingClass(JavaObjectClass* Cl);
  static bool isAnonymousClass(JavaObjectClass* Cl);
};

class JavaObjectVMField : public JavaObject {
private:
	JavaObjectClass* declaringClass;
	JavaObject* name;
	uint32 slot;
	// others
public:
	static void staticTracer(JavaObjectVMField* obj, word_t closure) {
	    llvm_gcroot(obj, 0);
		vmkit::Collector::markAndTrace(obj, &obj->name, closure);
		vmkit::Collector::markAndTrace(obj, &obj->declaringClass, closure);
	}

	static JavaField* getInternalField(JavaObjectVMField* self) {
		llvm_gcroot(self, 0);
		UserCommonClass* cls = JavaObjectClass::getClass(self->declaringClass);
		return &(cls->asClass()->virtualFields[self->slot]);
	}

	static UserClass* getClass(JavaObjectVMField* self) __attribute__ ((always_inline)) {
		llvm_gcroot(self, 0);
		UserCommonClass* cls = JavaObjectClass::getClass(self->declaringClass);
		return cls->asClass();
	}
};

class JavaObjectField : public JavaObject {
private:
  uint8 flag;
  JavaObject* p;
  JavaObjectVMField* vmField;

public:

  static void staticTracer(JavaObjectField* obj, word_t closure) {
    llvm_gcroot(obj, 0);
    vmkit::Collector::markAndTrace(obj, &obj->p, closure);
    vmkit::Collector::markAndTrace(obj, &obj->vmField, closure);
  }

  static JavaField* getInternalField(JavaObjectField* self) {
    llvm_gcroot(self, 0);
    return JavaObjectVMField::getInternalField(self->vmField);
  }

  static UserClass* getClass(JavaObjectField* self) __attribute__ ((always_inline)) {
    llvm_gcroot(self, 0);
    return JavaObjectVMField::getClass(self->vmField);
  }

  static JavaObjectField* createFromInternalField(JavaField* field, int i);
};

class JavaObjectVMMethod : public JavaObject {
public:
	JavaObjectClass* declaringClass;
	JavaString* name;
	uint32 slot;
public:
  	static void staticTracer(JavaObjectVMMethod* obj, word_t closure) {
	    llvm_gcroot(obj, 0);
  		vmkit::Collector::markAndTrace(obj, &obj->name, closure);
  		vmkit::Collector::markAndTrace(obj, &obj->declaringClass, closure);
	}

	static JavaMethod* getInternalMethod(JavaObjectVMMethod* self);

	static UserClass* getClass(JavaObjectVMMethod* self) __attribute__ ((always_inline)) {
		llvm_gcroot(self, 0);
		UserCommonClass* cls = JavaObjectClass::getClass(self->declaringClass);
		return cls->asClass();
	}
};

class JavaObjectMethod : public JavaObject {
private:
  uint8 flag;
  JavaObject* p;
  JavaObjectVMMethod* vmMethod;

public:
  
  static void staticTracer(JavaObjectMethod* obj, word_t closure) {
    llvm_gcroot(obj, 0);
    vmkit::Collector::markAndTrace(obj, &obj->p, closure);
    vmkit::Collector::markAndTrace(obj, &obj->vmMethod, closure);
  }
  
  static JavaMethod* getInternalMethod(JavaObjectMethod* self);
  
  static UserClass* getClass(JavaObjectMethod* self) __attribute__ ((always_inline)) {
    llvm_gcroot(self, 0);
    return JavaObjectVMMethod::getClass(self->vmMethod);
  }

  static JavaObjectMethod* createFromInternalMethod(JavaMethod* meth, int i);
};


class JavaObjectVMConstructor : public JavaObject {
private:
  JavaObjectClass* declaringClass;
  uint32 slot;

public:
  static void staticTracer(JavaObjectVMConstructor* obj, word_t closure) {
    llvm_gcroot(obj, 0);
    vmkit::Collector::markAndTrace(obj, &obj->declaringClass, closure);
  }

  static JavaMethod* getInternalMethod(JavaObjectVMConstructor* self);

  static UserClass* getClass(JavaObjectVMConstructor* self) __attribute__ ((always_inline)) {
    llvm_gcroot(self, 0);
    UserCommonClass* cls = JavaObjectClass::getClass(self->declaringClass);
    return cls->asClass();
  }
};

class JavaObjectConstructor : public JavaObject {
private:
  uint8 flag;
  JavaObject* p;
  JavaObjectVMConstructor* vmCons;

public:
  static void staticTracer(JavaObjectConstructor* obj, word_t closure) {
    llvm_gcroot(obj, 0);
    vmkit::Collector::markAndTrace(obj, &obj->p, closure);
    vmkit::Collector::markAndTrace(obj, &obj->vmCons, closure);
  }
  
  static JavaMethod* getInternalMethod(JavaObjectConstructor* self);
  
  static UserClass* getClass(JavaObjectConstructor* self);

  static JavaObjectConstructor* createFromInternalConstructor(JavaMethod* cons, int i);
};

class JavaObjectVMThread : public JavaObject {
private:
  JavaObject* thread;
  bool running;
  JavaThread* vmdata;

public:
  static void staticTracer(JavaObjectVMThread* obj, word_t closure) {
    llvm_gcroot(obj, 0);
    vmkit::Collector::markAndTrace(obj, &obj->thread, closure);
  }

  static void setVmdata(JavaObjectVMThread* vmthread,
                        JavaThread* internal_thread) {
    llvm_gcroot(vmthread, 0);
    vmthread->vmdata = internal_thread;
  }

  static JavaThread* getVmdata(JavaObjectVMThread* vmthread) {return vmthread->vmdata;}

  friend std::ostream& operator << (std::ostream&, JavaObjectVMThread&);
};


class JavaObjectThrowable : public JavaObject {
private:
  JavaObject* detailedMessage;
  JavaObject* cause;
  JavaObject* stackTrace;
  JavaObject* vmState;

public:

  static void setDetailedMessage(JavaObjectThrowable* self, JavaObject* obj) {
    llvm_gcroot(self, 0);
    llvm_gcroot(obj, 0);
    vmkit::Collector::objectReferenceWriteBarrier(
        (gc*)self, (gc**)&(self->detailedMessage), (gc*)obj);
  }

  static void fillInStackTrace(JavaObjectThrowable* self) {
    JavaObject* stackTrace = NULL;
    llvm_gcroot(self, 0);
    llvm_gcroot(stackTrace, 0);

    stackTrace = internalFillInStackTrace(self);
    vmkit::Collector::objectReferenceWriteBarrier(
        (gc*)self, (gc**)&(self->vmState), (gc*)stackTrace);

    vmkit::Collector::objectReferenceWriteBarrier(
        (gc*)self, (gc**)&(self->cause), (gc*)self);

    self->stackTrace = NULL;
  }
};

class JavaObjectReference : public JavaObject {
private:
  JavaObject* referent;
  JavaObject* queue;
  JavaObject* nextOnQueue;

public:
  static void init(JavaObjectReference* self, JavaObject* r, JavaObject* q) {
    llvm_gcroot(self, 0);
    llvm_gcroot(r, 0);
    llvm_gcroot(q, 0);
    vmkit::Collector::objectReferenceWriteBarrier(
        (gc*)self, (gc**)&(self->referent), (gc*)r);
    vmkit::Collector::objectReferenceWriteBarrier(
        (gc*)self, (gc**)&(self->queue), (gc*)q);
  }

  static JavaObject** getReferentPtr(JavaObjectReference* self) {
    llvm_gcroot(self, 0);
    return &(self->referent);
  }

  static void setReferent(JavaObjectReference* self, JavaObject* r) {
    llvm_gcroot(self, 0);
    llvm_gcroot(r, 0);
    // No write barrier: this is only called by the GC.
    self->referent = r;
  }
};

}

#endif
