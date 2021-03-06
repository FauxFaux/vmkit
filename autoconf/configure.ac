dnl === configure.ac --------------------------------------------------------===
dnl                     The vmkit project
dnl
dnl This file is distributed under the University of Illinois Open Source
dnl License. See LICENSE.TXT for details.
dnl
dnl===-----------------------------------------------------------------------===
dnl This is the VMKit configuration script. It is processed by the autoconf
dnl program to produce a script named configure. This script contains the
dnl configuration checks that VMKit needs in order to support multiple platforms.
dnl This file is composed of 10 sections per the recommended organization of
dnl autoconf input defined in the autoconf documentation. As this file evolves,
dnl please keep the various types of checks within their sections. The sections
dnl are as follows:
dnl
dnl SECTION 1: Initialization & Setup
dnl SECTION 2: Architecture, target, and host checks
dnl SECTION 3: Command line arguments for the configure script.
dnl SECTION 4: Check for programs we need and that they are the right version
dnl SECTION 5: Check for libraries
dnl SECTION 6: Check for header files
dnl SECTION 7: Check for types and structures
dnl SECTION 8: Check for specific functions needed
dnl SECTION 9: Additional checks, variables, etc.
dnl SECTION 10: Specify the output files and generate it
dnl
dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 1: Initialization & Setup
dnl===
dnl===-----------------------------------------------------------------------===
dnl Initialize autoconf and define the package name, version number and
dnl email address for reporting bugs.
AC_INIT([vmkit],[0.32svn],[vmkit2-development@lists.gforge.inria.fr])

dnl Provide a copyright substitution and ensure the copyright notice is included
dnl in the output of --version option of the generated configure script.
AC_SUBST(VMKIT_COPYRIGHT,["Copyright (c) 2003-2013 Universite Pierre et Marie Curie."])
AC_COPYRIGHT([Copyright (c) 2003-2013 Universite Pierre et Marie Curie.])

dnl Indicate that we require autoconf 2.59 or later. Ths is needed because we
dnl use some autoconf macros only available in 2.59.
AC_PREREQ(2.59)

dnl Verify that the source directory is valid. This makes sure that we are
dnl configuring VMKit and not some other package (it validates --srcdir argument)
AC_CONFIG_SRCDIR([lib/vmkit/Runtime/Object.cpp])


dnl Quit if the source directory has already been configured.
dnl NOTE: This relies upon undocumented autoconf behavior.
if test ${srcdir} != "." ; then
  if test -f ${srcdir}/include/vmkit/config.h ; then
    AC_MSG_ERROR([Already configured in ${srcdir}])
  fi
fi

dnl Place all of the extra autoconf files into the config subdirectory. Tell
dnl various tools where the m4 autoconf macros are.
AC_CONFIG_AUX_DIR([./autoconf])


dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 2: Architecture, target, and host checks
dnl===
dnl===-----------------------------------------------------------------------===

dnl Check the target for which we're compiling and the host that will do the
dnl compilations. This will tell us which LLVM compiler will be used for
dnl compiling SSA into object code. This needs to be done early because
dnl following tests depend on it.
AC_CANONICAL_TARGET

dnl Determine the platform type and cache its value. This helps us configure
dnl the System library to the correct build platform.
AC_CACHE_CHECK([type of operating system we're going to host on],
               [vmkit_cv_os_type],
[case $host in
  *-*-aix*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="AIX"
    vmkit_cv_platform_type="Unix" ;;
  *-*-irix*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="IRIX"
    vmkit_cv_platform_type="Unix" ;;
  *-*-cygwin*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="Cygwin"
    vmkit_cv_platform_type="Unix" ;;
  *-*-darwin*)
    DYLIB_EXTENSION="dylib"
		SHLIBEXT=".dylib"
    SHOPT="-fno-common -Wl,-flat_namespace -Wl,-undefined,suppress"
    vmkit_cv_os_type="Darwin"
    vmkit_cv_platform_type="Unix" ;;
  *-*-freebsd*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="FreeBSD"
    vmkit_cv_platform_type="Unix" ;;
  *-*-openbsd*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="OpenBSD"
    vmkit_cv_platform_type="Unix" ;;
  *-*-netbsd*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="NetBSD"
    vmkit_cv_platform_type="Unix" ;;
  *-*-hpux*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="HP-UX"
    vmkit_cv_platform_type="Unix" ;;
  *-*-interix*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="Interix"
    vmkit_cv_platform_type="Unix" ;;
  *-*-linux*)
    DYLIB_EXTENSION="so"
    SHLIBEXT=".so"
    LDOPT="-Wl,-export-dynamic"
    vmkit_cv_os_type="Linux"
    vmkit_cv_platform_type="Unix" ;;
  *-*-solaris*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="SunOS"
    vmkit_cv_platform_type="Unix" ;;
  *-*-win32*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="Win32"
    vmkit_cv_platform_type="Win32" ;;
  *-*-mingw*)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="MingW"
    vmkit_cv_platform_type="Win32" ;;
  *)
    AC_MSG_ERROR([Good luck porting vmkit to your host!])
    vmkit_cv_os_type="Unknown"
    vmkit_cv_platform_type="Unknown" ;;
esac])

dnl Make sure we aren't attempting to configure for an unknown system

AC_SUBST([DYLIB_EXTENSION])
AC_SUBST([SHOPT])
AC_SUBST([LDOPT])
AC_SUBST([SHLIBEXT])
AC_SUBST([EXEEXT])

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 3: Command line arguments for the configure script.
dnl===
dnl===-----------------------------------------------------------------------===

dnl **************************************************************************
dnl Architecture
dnl **************************************************************************

case $target_cpu in
  powerpc) 
    AC_DEFINE([WITH_64], [0], [Building on a platform without 64bit cas])
    AC_SUBST([WITH_64], [0]);;
  *) 
    AC_DEFINE([WITH_64], [1], [Building on a platform with 64bit cas])
    AC_SUBST([WITH_64], [1]);;
esac

dnl **************************************************************************
dnl optimization and debug
dnl **************************************************************************
AC_ARG_ENABLE(optimized,
              AS_HELP_STRING([--enable-optimized],
                             [Build with all optimization flag enable (default is yes)]),,
                             enable_optimized=yes)
case "$enable_optimized" in
  yes) AC_SUBST(OPTIMIZED,[1]) ;;
  no)  AC_SUBST(OPTIMIZED,[0]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-optimized. Use "yes" or "no"]) ;;
esac

AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],
                             [Build with debug flags (default is no)]),,
                             enable_debug=no)
case "$enable_debug" in
  yes) AC_SUBST(DEBUG,[1]) ;;
  no)  AC_SUBST(DEBUG,[0]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-debug. Use "yes" or "no"]) ;;
esac

AC_ARG_ENABLE(assert,
              AS_HELP_STRING([--enable-assert],
                             [Build with assert flags (default is yes)]),,
                             enable_assert=yes)

case "$enable_assert" in
  yes) AC_SUBST(ASSERT,[1]) ;;
  no)  AC_SUBST(ASSERT,[0]) ;;
  *) AC_MSG_ERROR([Invalid setting for --enable-assert. Use "yes" or "no"]) ;;
esac

if test "$enable_optimized" = "yes"; then
   VMKIT_BUILD_NAME=Release
   if test "$enable_debug" = "yes"; then
      VMKIT_BUILD_NAME="$VMKIT_BUILD_NAME"+Debug
   fi 
else
   if test "$enable_debug" = "yes"; then
      VMKIT_BUILD_NAME=Debug
   else
      VMKIT_BUILD_NAME=Unoptimized
   fi
fi

if test "$enable_assert" = "yes"; then
   VMKIT_BUILD_NAME="$VMKIT_BUILD_NAME"+Asserts
fi

AC_SUBST([VMKIT_BUILD_NAME], ["$VMKIT_BUILD_NAME"])

dnl **************************************************************************
dnl llvm
dnl **************************************************************************
AC_ARG_WITH(llvm-config-path,
       [AS_HELP_STRING(--with-llvm-config-path=path,
           [llvm-config path (use default path)])],
       [[LLVM_CONFIG=$with_llvm_config_path]],
       [[LLVM_CONFIG="llvm-config"]]
)

dnl **************************************************************************
dnl clang
dnl **************************************************************************
AC_ARG_WITH(clang-path,
       [AS_HELP_STRING(--with-clang-path=path,
           [clang path (use default path)])],
       [[CLANG_PATH=$with_clang_path]],
       [[CLANG_PATH="`$LLVM_CONFIG --bindir`"]]
)
AC_SUBST([CLANG_PATH])

dnl **************************************************************************
dnl GC type 
dnl **************************************************************************
AC_ARG_WITH(mmtk-plan,
       [AS_HELP_STRING(--with-mmtk-plan=something,
           [MMTk plan type ('org.mmtk.plan.marksweep.MS')])],
       [[MMTK_PLAN=$with_mmtk_plan]],
       [[MMTK_PLAN=org.mmtk.plan.marksweep.MS]]
)

GC_FLAGS="-I\$(PROJ_SRC_ROOT)/lib/vmkit/MMTk"

AC_SUBST([GC_FLAGS])
AC_SUBST([MMTK_PLAN])

dnl **************************************************************************
dnl GNU CLASSPATH installation prefix
dnl **************************************************************************

classpathversion=0.97.2;

AC_ARG_WITH(gnu-classpath-libs,
       [AS_HELP_STRING(--with-gnu-classpath-libs=something,
           [GNU CLASSPATH libraries (default is /usr/lib/classpath)])],
       [[classpathlibs=$with_gnu_classpath_libs]],
       [[classpathlibs=/usr/lib/classpath]]
)

AC_ARG_WITH(gnu-classpath-glibj,
       [AS_HELP_STRING(--with-gnu-classpath-glibj,
           [Build J3 with GNU Classpath install (default is '/usr/share/classpath/glibj.zip')])],
       [[classpathglibj=$with_gnu_classpath_glibj]],
       [[classpathglibj=/usr/share/classpath/glibj.zip]]
)

AC_SUBST([classpathglibj])
AC_SUBST([classpathlibs])
AC_SUBST([classpathversion])

dnl **************************************************************************
dnl OpenJDK Paths
dnl **************************************************************************

AC_ARG_WITH(openjdk-path,
       [AS_HELP_STRING(--with-openjdk-path,
           [Build J3 with OpenJDK JRE install (default is '/usr/lib/java/')])],
       [[openjdkpath=$with_openjdk_path]],
       [[openjdkpath=/usr/lib/java/]]
)

AC_SUBST([openjdkpath])


dnl **************************************************************************
dnl Pick Classpath or OpenJDK.  Also, light sanity checks on the chosen one.
dnl **************************************************************************

AC_ARG_WITH(classpath-impl,
       [AS_HELP_STRING(--with-classpath-impl,
           [Build J3 with the specified classpath implementation (default is gnuclasspath)])],
       [[classpathimpl=$with_classpath_impl]],
       [[classpathimpl=gnuclasspath]]
)

case "${classpathimpl}" in
  gnuclasspath)
    AC_MSG_NOTICE(Validating GNU Classpath installation...)
    AC_CHECK_FILES([${classpathlibs}]
                   [${classpathlibs}/libjavaio.$DYLIB_EXTENSION]
                   [${classpathlibs}/libjavalang.$DYLIB_EXTENSION]
                   [${classpathlibs}/libjavalangreflect.$DYLIB_EXTENSION]
                   [${classpathlibs}/libjavanet.$DYLIB_EXTENSION]
                   [${classpathlibs}/libjavanio.$DYLIB_EXTENSION]
                   [${classpathlibs}/libjavautil.$DYLIB_EXTENSION],,
      AC_MSG_ERROR([[Invalid GNU Classpath library path, can't find required libraries]]))
    AC_CHECK_FILE([${classpathglibj}],,
      AC_MSG_ERROR([[Invalid glibj.zip location]]))
      ;;
  openjdk)
    AC_MSG_NOTICE(Validating OpenJDK installation...)
    AC_CHECK_FILES([${openjdkpath}]
                   [${openjdkpath}/jre/lib/rt.jar],,
      AC_MSG_ERROR([[Invalid OpenJDK JDK path, can't find required jar files!]]))
      ;;
  *)
    AC_MSG_ERROR([Invalid --with-classpath-impl "${classpathimpl}".  Must be one of "gnuclasspath" or "openjdk"])
      ;;
esac

AC_SUBST(classpathimpl)

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 4: Check for programs we need and that they are the right version
dnl===
dnl===-----------------------------------------------------------------------===

dnl Check for compilation tools

AC_PROG_NM
AC_SUBST(NM)

dnl Check for the tools that the makefiles require
AC_PATH_PROG(LD,    [ld],    [ld])
AC_PATH_PROG(AR,    [ar],    [ar])
AC_PROG_GREP
AC_PROG_MKDIR_P
AC_PATH_PROG(MV,    [mv],    [mv])
AC_PATH_PROG(RM,    [rm],    [rm])
AC_PATH_PROG(CP,    [cp],    [cp])
AC_PATH_PROG(TOUCH, [touch], [touch])
AC_PROG_RANLIB

AC_PATH_PROG(ANT, [ant])
AC_PATH_PROG(JAVAC, [javac], [javac])
AC_PATH_PROG(ZIP, [zip], [zip])

AC_PATH_PROG(LLVM_CONFIG, [llvm-config],,[`AS_DIRNAME(["$LLVM_CONFIG_PATH"])`:$PATH])

if test -z ${ANT}; then
    AC_MSG_ERROR([Unable to find ant, please put ant tool in your path])
fi

if test -z ${ZIP}; then
    AC_MSG_ERROR([Unable to find zip, please put zip in your path])
fi

if test -z ${LLVM_CONFIG}; then
    AC_MSG_ERROR([Unable to find llvm-config, please specify its path])
fi

LLVM_RTTI=$(if $LLVM_CONFIG --cxxflags|grep "\-fno-rtti" > /dev/null 2>&1;then echo -fno-rtti;fi)

AC_SUBST([LLVM_CONFIG])
AC_SUBST([LLVM_RTTI])

dnl Find the install program
AC_PROG_INSTALL

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 5: Check for libraries
dnl===
dnl===-----------------------------------------------------------------------===

AC_CHECK_LIB(z, inflate, [], \
  [AC_MSG_ERROR([You need to install the zlib package (z).])]
)
  

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 6: Check for header files
dnl===
dnl===-----------------------------------------------------------------------===

AC_CHECK_HEADER([zlib.h], [], \
  AC_MSG_ERROR([You need to install the zlib devel package (zlib.h).])
)
  
dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 7: Check for types and structures
dnl===
dnl===-----------------------------------------------------------------------===

AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_CHECK_TYPES([int64_t],,AC_MSG_ERROR([Type int64_t required but not found]))
AC_CHECK_TYPES([uint64_t],,
         AC_CHECK_TYPES([u_int64_t],,
         AC_MSG_ERROR([Type uint64_t or u_int64_t required but not found])))

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 8: Check for specific functions needed
dnl===
dnl===-----------------------------------------------------------------------===

AC_CHECK_FUNCS([setjmp longjmp])


dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 9: Additional checks, variables, etc.
dnl===
dnl===-----------------------------------------------------------------------===

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 10: Specify the output files and generate it
dnl===
dnl===-----------------------------------------------------------------------===

dnl Configure header files
dnl WARNING: dnl If you add or remove any of the following config headers, then
dnl you MUST also update Makefile.rules so that the variable FilesToConfig
dnl contains the same list of files as AC_CONFIG_HEADERS below. This ensures the
dnl files can be updated automatically when their *.in sources change.
AC_CONFIG_HEADERS([include/vmkit/config.h])

dnl Verify that the source directory is valid
AC_CONFIG_SRCDIR(["Makefile.common.in"])

dnl Configure a common Makefile
AC_CONFIG_FILES(Makefile.common)
AC_CONFIG_FILES([lib/j3/ClassLib/Classpath.h])
AC_CONFIG_FILES([tools/llcj/LinkPaths.h])
AC_CONFIG_FILES([mmtk/java/src/org/j3/config/Selected.java])
AC_CONFIG_FILES([mmtk/java/build.xml])

AC_SUBST([ac_config_files])
AC_SUBST([ac_config_headers])

AC_OUTPUT
