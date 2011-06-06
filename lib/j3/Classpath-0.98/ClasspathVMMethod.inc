//===- ClasspathMethod.cpp ------------------------------------------------===//
//===------------- GNU classpath java/lang/reflect/Method -----------------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "types.h"

#include "Classpath.h"
#include "ClasspathReflect.h"
#include "JavaArray.h"
#include "JavaClass.h"
#include "JavaConstantPool.h"
#include "JavaObject.h"
#include "JavaTypes.h"
#include "JavaThread.h"
#include "JavaUpcalls.h"
#include "Jnjvm.h"
#include "JnjvmClassLoader.h"
#include "Reader.h"

#if 0
using namespace vmkit;
#define dprintf(...) do { printf("ClasspathVMMethod: "); printf(__VA_ARGS__); } while(0)
#else
#define dprintf(...)
#endif

using namespace j3;

extern "C" {


JNIEXPORT JavaObject* JNICALL Java_java_lang_reflect_VMMethod_getAnnotation(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* VMMeth, JavaObjectClass* annotationClass) {
  JavaObject* res = 0;
  JavaObject* newHashMap = 0;
  llvm_gcroot(res, 0);
  llvm_gcroot(newHashMap, 0);
  llvm_gcroot(VMMeth, 0);
  llvm_gcroot(annotationClass, 0);

  BEGIN_NATIVE_EXCEPTION(0)

  Jnjvm* vm = JavaThread::get()->getJVM();
  Classpath* upcalls = vm->upcalls;

  dprintf("*************VMMethod_getAnnotation*************\n");
  if (annotationClass) {
    /* Retrieving annotationClass classname */
    UserClass* clazz = JavaObjectClass::getClass(annotationClass)->asClass();
    dprintf("searched annotation:%s\n\n", PrintBuffer(clazz->name).cString());

    /* Retrieving VMMethod.class attributes list */
    UserClass* compilingClass = JavaObjectVMMethod::getClass(VMMeth);
    JavaMethod* method = JavaObjectVMMethod::getInternalMethod(VMMeth);
    Attribut* annotationsAtt =
      method->lookupAttribut(Attribut::annotationsAttribut);
    dprintf("JavaMethod:%s\n", PrintBuffer(method->name).cString());

    if (annotationsAtt) {
      Reader reader(annotationsAtt, compilingClass->bytes);
      AnnotationReader AR(reader, compilingClass);
      uint16 numAnnotations = reader.readU2();

      for (uint16 i = 0; i < numAnnotations; ++i) {
	uint16 typeIndex = reader.readU2();
	const UTF8* annoType = compilingClass->ctpInfo->UTF8At(typeIndex);

	// Remove the L and ; in the name annotation type (L.....;)
	// for the comparison.
	if (clazz->name->equals(annoType->elements+1, annoType->size-2)) {
	  newHashMap = AR.createAnnotationMapValues();
	  break;

	} else {
	  AR.readAnnotationElementValues();
	}
      } // end for

      if (newHashMap) {
	dprintf("Annotation creation\n");
	res = upcalls->createAnnotation->invokeJavaObjectStatic(upcalls->newAnnotationHandler, &annotationClass, &newHashMap);
      }

    }
  } else {
    vm->nullPointerException();
  }

  END_NATIVE_EXCEPTION

  return res;
}


JNIEXPORT jint JNICALL Java_java_lang_reflect_VMMethod_getModifiersInternal(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* Meth) { 
  
  jint res = 0;
  llvm_gcroot(Meth, 0);

  BEGIN_NATIVE_EXCEPTION(0)
  
  JavaMethod* meth = JavaObjectVMMethod::getInternalMethod(Meth); 
  res = meth->access;

  END_NATIVE_EXCEPTION

  return res;
}

JNIEXPORT JavaObject* JNICALL Java_java_lang_reflect_VMMethod_getReturnType(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* Meth) {

  JavaObject* res = 0;
  llvm_gcroot(Meth, 0);
  llvm_gcroot(res, 0);

  BEGIN_NATIVE_EXCEPTION(0)

  UserClass* cl = JavaObjectVMMethod::getClass(Meth);
  JavaMethod* meth = JavaObjectVMMethod::getInternalMethod(Meth); 
  JnjvmClassLoader* loader = cl->classLoader;
  res = meth->getReturnType(loader);

  END_NATIVE_EXCEPTION

  return res;
}


JNIEXPORT JavaObject* JNICALL Java_java_lang_reflect_VMMethod_getParameterTypes(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* Meth) {

  JavaObject* res = 0;
  llvm_gcroot(Meth, 0);
  llvm_gcroot(res, 0);

  BEGIN_NATIVE_EXCEPTION(0)

  UserClass* cl = JavaObjectVMMethod::getClass(Meth);
  JavaMethod* meth = JavaObjectVMMethod::getInternalMethod(Meth);
  JnjvmClassLoader* loader = cl->classLoader;
  
  res = meth->getParameterTypes(loader);

  END_NATIVE_EXCEPTION

  return res;
}

JavaObject* proceedMethod(JavaObjectVMMethod* Meth, JavaObject* obj,
                          ArrayObject* args, JavaObject* Cl, jint index) 
  __attribute__((noinline));

JavaObject* proceedMethod(JavaObjectVMMethod* Meth, JavaObject* obj,
                          ArrayObject* args, JavaObject* Cl, jint index) {

  JavaObject* res = 0;
  JavaObject* jexc = 0;
	vmkit::gc* exc = 0;
  
  llvm_gcroot(res, 0);
  llvm_gcroot(Meth, 0);
  llvm_gcroot(obj, 0);
  llvm_gcroot(args, 0);
  llvm_gcroot(Cl, 0);
  llvm_gcroot(exc, 0);
  llvm_gcroot(jexc, 0);

  Jnjvm* vm = JavaThread::get()->getJVM();

  JavaMethod* meth = JavaObjectVMMethod::getInternalMethod(Meth);
  
  sint32 nbArgs = args ? ArrayObject::getSize(args) : 0;
  Signdef* sign = meth->getSignature();
  sint32 size = sign->nbArguments;

  vmkit::ThreadAllocator allocator;
  jvalue* buf = size ?
      (jvalue*)allocator.Allocate(size * sizeof(jvalue)) : NULL;
  
  if (nbArgs == size) {
    UserCommonClass* _cl = UserCommonClass::resolvedImplClass(Cl, false);
    UserClass* cl = (UserClass*)_cl;
    
    if (isVirtual(meth->access)) {
      verifyNull(obj);
      UserCommonClass* objCl = JavaObject::getClass(obj);
      if (!(objCl->isAssignableFrom(cl))) {
        vm->illegalArgumentException("<this> is not a valid type");
      }

      if (isInterface(cl->access)) {
        cl->initialiseClass();
        UserClass* methodCl = 0;
        UserClass* lookup = objCl->isArray() ? objCl->super : objCl->asClass();
        meth = lookup->lookupMethod(meth->name, meth->type, false, true,
                                    &methodCl);
      }
    } else {
      cl->initialiseClass();
    }
    
    JavaObject** ptr = ArrayObject::getElements(args);
    Typedef* const* arguments = sign->getArgumentsType();
    for (sint32 i = 0; i < size; ++i) {
      JavaObject::decapsulePrimitive(ptr[i], &buf[i], arguments[i]);
      if (!arguments[i]->isPrimitive()) {
        buf[i].l = reinterpret_cast<jobject>(&ptr[i]);
      }
    }
    

#define RUN_METH(TYPE, VAR)																							\
    TRY {																																\
      if (isVirtual(meth->access)) {																		\
        if (isPublic(meth->access) && !isFinal(meth->access) &&					\
            !isFinal(meth->classDef->access)) {													\
          VAR = meth->invoke##TYPE##VirtualBuf(cl, obj, buf);						\
        } else {																												\
          VAR = meth->invoke##TYPE##SpecialBuf(cl, obj, buf);						\
				}																																\
			} else {																													\
				VAR = meth->invoke##TYPE##StaticBuf(cl, buf);										\
			}																																	\
		} CATCH {																														\
      exc = vmkit::Thread::get()->getPendingException();									\
    } END_CATCH;																												\
																																				\
    if (exc) {																													\
			vmkit::Thread* mut = vmkit::Thread::get();														\
			jexc = Jnjvm::asJavaException(exc);																\
      if (jexc && JavaObject::getClass(jexc)->isAssignableFrom(					\
																															 vm->upcalls->newException)) { \
        mut->clearPendingException();																		\
				JavaThread::j3Thread(mut)->getJVM()->invocationTargetException(jexc); \
      } else {																													\
        mut->throwIt();																									\
      }																																	\
      return NULL;																											\
    }
    
    Typedef* retType = sign->getReturnType();
    if (retType->isPrimitive()) {
      PrimitiveTypedef* prim = (PrimitiveTypedef*)retType;
      if (prim->isVoid()) {
        res = 0;
        uint32 val = 0;
        RUN_METH(Int, val);
      } else if (prim->isBool()) {
        uint32 val = 0;
        RUN_METH(Int, val);
        res = vm->upcalls->boolClass->doNew();
        vm->upcalls->boolValue->setInstanceInt8Field(res, val);
      } else if (prim->isByte()) {
        uint32 val = 0;
        RUN_METH(Int, val);
        res = vm->upcalls->byteClass->doNew();
        vm->upcalls->byteValue->setInstanceInt8Field(res, val);
      } else if (prim->isChar()) {
        uint32 val = 0;
        RUN_METH(Int, val);
        res = vm->upcalls->charClass->doNew();
        vm->upcalls->charValue->setInstanceInt16Field(res, val);
      } else if (prim->isShort()) {
        uint32 val = 0;
        RUN_METH(Int, val);
        res = vm->upcalls->shortClass->doNew();
        vm->upcalls->shortValue->setInstanceInt16Field(res, val);
      } else if (prim->isInt()) {
        uint32 val = 0;
        RUN_METH(Int, val);
        res = vm->upcalls->intClass->doNew();
        vm->upcalls->intValue->setInstanceInt32Field(res, val);
      } else if (prim->isLong()) {
        sint64 val = 0;
        RUN_METH(Long, val);
        res = vm->upcalls->longClass->doNew();
        vm->upcalls->longValue->setInstanceLongField(res, val);
      } else if (prim->isFloat()) {
        float val = 0;
        RUN_METH(Float, val);
        res = vm->upcalls->floatClass->doNew();
        vm->upcalls->floatValue->setInstanceFloatField(res, val);
      } else if (prim->isDouble()) {
        double val = 0;
        RUN_METH(Double, val);
        res = vm->upcalls->doubleClass->doNew();
        vm->upcalls->doubleValue->setInstanceDoubleField(res, val);
      }
    } else {
      RUN_METH(JavaObject, res);
    } 
  } else {
    vm->illegalArgumentException("wrong number of arguments");
    return NULL;
  }

  return res;
}

#undef RUN_METH

JNIEXPORT JavaObject* JNICALL Java_java_lang_reflect_VMMethod_invoke(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* VMMeth, JavaObject* obj, ArrayObject* args) {
  JavaObject* Cl = 0;
  JavaObject* res = 0;
  jint index;
  llvm_gcroot(res, 0);
  llvm_gcroot(VMMeth, 0);
  llvm_gcroot(obj, 0);
  llvm_gcroot(args, 0);
  llvm_gcroot(Cl, 0);

  BEGIN_NATIVE_EXCEPTION(0)

  Classpath* upcalls = JavaThread::get()->getJVM()->upcalls;
  Cl = upcalls->vmmethodClass->getInstanceObjectField(VMMeth);
  index = upcalls->vmmethodSlot->getInstanceInt32Field(VMMeth);
  res = proceedMethod(VMMeth, obj, args, Cl, index);
  
  END_NATIVE_EXCEPTION

  return res;
}


JNIEXPORT ArrayObject* JNICALL Java_java_lang_reflect_VMMethod_getExceptionTypes(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* Meth) {

  ArrayObject* res = 0;
  llvm_gcroot(Meth, 0);
  llvm_gcroot(res, 0);

  BEGIN_NATIVE_EXCEPTION(0)

  verifyNull(Meth);
  UserClass* cl = JavaObjectVMMethod::getClass(Meth);
  JavaMethod* meth = JavaObjectVMMethod::getInternalMethod(Meth);
  JnjvmClassLoader* loader = cl->classLoader;
  res = meth->getExceptionTypes(loader);

  END_NATIVE_EXCEPTION

  return res;
}

JNIEXPORT JavaObject* JNICALL Java_java_lang_reflect_Method_getSignature(
#ifdef NATIVE_JNI
JNIEnv *env, 
#endif
JavaObjectVMMethod* Meth) {

  JavaObject* result = 0;
  llvm_gcroot(Meth, 0);
  llvm_gcroot(result, 0);

  BEGIN_NATIVE_EXCEPTION(0)

  verifyNull(Meth);
  JavaMethod* meth = JavaObjectVMMethod::getInternalMethod(Meth);
  Jnjvm* vm = JavaThread::get()->getJVM();
  result = vm->internalUTF8ToStr(meth->type);
  
  END_NATIVE_EXCEPTION

  return result;
}

}
