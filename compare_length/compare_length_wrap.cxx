/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.2.0
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

// source: compare_length.i


extern
#ifdef __cplusplus
  "C"
#endif
  void cgo_panic__compare_length_f57c2e54145f5e02(const char*);
static void _swig_gopanic(const char *p) {
  cgo_panic__compare_length_f57c2e54145f5e02(p);
}



#define SWIG_VERSION 0x040200
#define SWIGGO
#define SWIGMODULE compare_length
/* -----------------------------------------------------------------------------
 *  This section contains generic SWIG labels for method/variable
 *  declarations/attributes, and other compiler dependent labels.
 * ----------------------------------------------------------------------------- */

/* template workaround for compilers that cannot correctly implement the C++ standard */
#ifndef SWIGTEMPLATEDISAMBIGUATOR
# if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x560)
#  define SWIGTEMPLATEDISAMBIGUATOR template
# elif defined(__HP_aCC)
/* Needed even with `aCC -AA' when `aCC -V' reports HP ANSI C++ B3910B A.03.55 */
/* If we find a maximum version that requires this, the test would be __HP_aCC <= 35500 for A.03.55 */
#  define SWIGTEMPLATEDISAMBIGUATOR template
# else
#  define SWIGTEMPLATEDISAMBIGUATOR
# endif
#endif

/* inline attribute */
#ifndef SWIGINLINE
# if defined(__cplusplus) || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#   define SWIGINLINE inline
# else
#   define SWIGINLINE
# endif
#endif

/* attribute recognised by some compilers to avoid 'unused' warnings */
#ifndef SWIGUNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define SWIGUNUSED __attribute__ ((__unused__))
#   else
#     define SWIGUNUSED
#   endif
# elif defined(__ICC)
#   define SWIGUNUSED __attribute__ ((__unused__))
# else
#   define SWIGUNUSED
# endif
#endif

#ifndef SWIG_MSC_UNSUPPRESS_4505
# if defined(_MSC_VER)
#   pragma warning(disable : 4505) /* unreferenced local function has been removed */
# endif
#endif

#ifndef SWIGUNUSEDPARM
# ifdef __cplusplus
#   define SWIGUNUSEDPARM(p)
# else
#   define SWIGUNUSEDPARM(p) p SWIGUNUSED
# endif
#endif

/* internal SWIG method */
#ifndef SWIGINTERN
# define SWIGINTERN static SWIGUNUSED
#endif

/* internal inline SWIG method */
#ifndef SWIGINTERNINLINE
# define SWIGINTERNINLINE SWIGINTERN SWIGINLINE
#endif

/* exporting methods */
#if defined(__GNUC__)
#  if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#    ifndef GCC_HASCLASSVISIBILITY
#      define GCC_HASCLASSVISIBILITY
#    endif
#  endif
#endif

#ifndef SWIGEXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define SWIGEXPORT
#   else
#     define SWIGEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define SWIGEXPORT __attribute__ ((visibility("default")))
#   else
#     define SWIGEXPORT
#   endif
# endif
#endif

/* calling conventions for Windows */
#ifndef SWIGSTDCALL
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   define SWIGSTDCALL __stdcall
# else
#   define SWIGSTDCALL
# endif
#endif

/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE
#endif

/* Deal with Microsoft's attempt at deprecating methods in the standard C++ library */
#if !defined(SWIG_NO_SCL_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_SCL_SECURE_NO_DEPRECATE)
# define _SCL_SECURE_NO_DEPRECATE
#endif

/* Deal with Apple's deprecated 'AssertMacros.h' from Carbon-framework */
#if defined(__APPLE__) && !defined(__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES)
# define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#endif

/* Intel's compiler complains if a variable which was never initialised is
 * cast to void, which is a common idiom which we use to indicate that we
 * are aware a variable isn't used.  So we just silence that warning.
 * See: https://github.com/swig/swig/issues/192 for more discussion.
 */
#ifdef __INTEL_COMPILER
# pragma warning disable 592
#endif

#if __cplusplus >=201103L
# define SWIG_NULLPTR nullptr
#else
# define SWIG_NULLPTR NULL
#endif 


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>



typedef long long intgo;
typedef unsigned long long uintgo;


# if !defined(__clang__) && (defined(__i386__) || defined(__x86_64__))
#   define SWIGSTRUCTPACKED __attribute__((__packed__, __gcc_struct__))
# else
#   define SWIGSTRUCTPACKED __attribute__((__packed__))
# endif



typedef struct { char *p; intgo n; } _gostring_;
typedef struct { void* array; intgo len; intgo cap; } _goslice_;



#ifdef __cplusplus
#include <utility>
/* SwigValueWrapper is described in swig.swg */
template<typename T> class SwigValueWrapper {
  struct SwigSmartPointer {
    T *ptr;
    SwigSmartPointer(T *p) : ptr(p) { }
    ~SwigSmartPointer() { delete ptr; }
    SwigSmartPointer& operator=(SwigSmartPointer& rhs) { T* oldptr = ptr; ptr = 0; delete oldptr; ptr = rhs.ptr; rhs.ptr = 0; return *this; }
    void reset(T *p) { T* oldptr = ptr; ptr = 0; delete oldptr; ptr = p; }
  } pointer;
  SwigValueWrapper& operator=(const SwigValueWrapper<T>& rhs);
  SwigValueWrapper(const SwigValueWrapper<T>& rhs);
public:
  SwigValueWrapper() : pointer(0) { }
  SwigValueWrapper& operator=(const T& t) { SwigSmartPointer tmp(new T(t)); pointer = tmp; return *this; }
#if __cplusplus >=201103L
  SwigValueWrapper& operator=(T&& t) { SwigSmartPointer tmp(new T(std::move(t))); pointer = tmp; return *this; }
  operator T&&() const { return std::move(*pointer.ptr); }
#else
  operator T&() const { return *pointer.ptr; }
#endif
  T *operator&() const { return pointer.ptr; }
  static void reset(SwigValueWrapper& t, T *p) { t.pointer.reset(p); }
};

/*
 * SwigValueInit() is a generic initialisation solution as the following approach:
 * 
 *       T c_result = T();
 * 
 * doesn't compile for all types for example:
 * 
 *       unsigned int c_result = unsigned int();
 */
template <typename T> T SwigValueInit() {
  return T();
}

#if __cplusplus >=201103L
# define SWIG_STD_MOVE(OBJ) std::move(OBJ)
#else
# define SWIG_STD_MOVE(OBJ) OBJ
#endif

#endif


static void Swig_free(void* p) {
  free(p);
}

static void* Swig_malloc(int c) {
  return malloc(c);
}


#include "compare_length.h"


#include <vector>
#include <stdexcept>

SWIGINTERN std::vector< int >::const_reference std_vector_Sl_int_Sg__get(std::vector< int > *self,int i){
                int size = int(self->size());
                if (i>=0 && i<size)
                    return (*self)[i];
                else
                    throw std::out_of_range("vector index out of range");
            }
SWIGINTERN void std_vector_Sl_int_Sg__set(std::vector< int > *self,int i,std::vector< int >::value_type const &val){
                int size = int(self->size());
                if (i>=0 && i<size)
                    (*self)[i] = val;
                else
                    throw std::out_of_range("vector index out of range");
            }
#ifdef __cplusplus
extern "C" {
#endif

void _wrap_Swig_free_compare_length_f57c2e54145f5e02(void *_swig_go_0) {
  void *arg1 = (void *) 0 ;
  
  arg1 = *(void **)&_swig_go_0; 
  
  Swig_free(arg1);
  
}


void *_wrap_Swig_malloc_compare_length_f57c2e54145f5e02(intgo _swig_go_0) {
  int arg1 ;
  void *result = 0 ;
  void *_swig_go_result;
  
  arg1 = (int)_swig_go_0; 
  
  result = (void *)Swig_malloc(arg1);
  *(void **)&_swig_go_result = (void *)result; 
  return _swig_go_result;
}


std::vector< int > *_wrap_new_VecInt__SWIG_0_compare_length_f57c2e54145f5e02() {
  std::vector< int > *result = 0 ;
  std::vector< int > *_swig_go_result;
  
  
  result = (std::vector< int > *)new std::vector< int >();
  *(std::vector< int > **)&_swig_go_result = (std::vector< int > *)result; 
  return _swig_go_result;
}


std::vector< int > *_wrap_new_VecInt__SWIG_1_compare_length_f57c2e54145f5e02(long long _swig_go_0) {
  std::vector< int >::size_type arg1 ;
  std::vector< int > *result = 0 ;
  std::vector< int > *_swig_go_result;
  
  arg1 = (size_t)_swig_go_0; 
  
  result = (std::vector< int > *)new std::vector< int >(arg1);
  *(std::vector< int > **)&_swig_go_result = (std::vector< int > *)result; 
  return _swig_go_result;
}


std::vector< int > *_wrap_new_VecInt__SWIG_2_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0) {
  std::vector< int > *arg1 = 0 ;
  std::vector< int > *result = 0 ;
  std::vector< int > *_swig_go_result;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  
  result = (std::vector< int > *)new std::vector< int >((std::vector< int > const &)*arg1);
  *(std::vector< int > **)&_swig_go_result = (std::vector< int > *)result; 
  return _swig_go_result;
}


long long _wrap_VecInt_size_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  std::vector< int >::size_type result;
  long long _swig_go_result;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  
  result = ((std::vector< int > const *)arg1)->size();
  _swig_go_result = result; 
  return _swig_go_result;
}


long long _wrap_VecInt_capacity_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  std::vector< int >::size_type result;
  long long _swig_go_result;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  
  result = ((std::vector< int > const *)arg1)->capacity();
  _swig_go_result = result; 
  return _swig_go_result;
}


void _wrap_VecInt_reserve_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0, long long _swig_go_1) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  std::vector< int >::size_type arg2 ;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  arg2 = (size_t)_swig_go_1; 
  
  (arg1)->reserve(arg2);
  
}


bool _wrap_VecInt_isEmpty_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  bool result;
  bool _swig_go_result;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  
  result = (bool)((std::vector< int > const *)arg1)->empty();
  _swig_go_result = result; 
  return _swig_go_result;
}


void _wrap_VecInt_clear_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  
  (arg1)->clear();
  
}


void _wrap_VecInt_add_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0, intgo _swig_go_1) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  std::vector< int >::value_type *arg2 = 0 ;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  arg2 = (std::vector< int >::value_type *)&_swig_go_1; 
  
  (arg1)->push_back((std::vector< int >::value_type const &)*arg2);
  
  
}


intgo _wrap_VecInt_get_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0, intgo _swig_go_1) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  int arg2 ;
  std::vector< int >::value_type *result = 0 ;
  intgo _swig_go_result;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  arg2 = (int)_swig_go_1; 
  
  try {
    result = (std::vector< int >::value_type *) &std_vector_Sl_int_Sg__get(arg1,arg2);
  } catch(std::out_of_range &_e) {
    (void)_e;
    _swig_gopanic("C++ std::out_of_range exception thrown");
    
  }
  _swig_go_result = (int)*result; 
  return _swig_go_result;
}


void _wrap_VecInt_set_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0, intgo _swig_go_1, intgo _swig_go_2) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  int arg2 ;
  std::vector< int >::value_type *arg3 = 0 ;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  arg2 = (int)_swig_go_1; 
  arg3 = (std::vector< int >::value_type *)&_swig_go_2; 
  
  try {
    std_vector_Sl_int_Sg__set(arg1,arg2,(int const &)*arg3);
  } catch(std::out_of_range &_e) {
    (void)_e;
    _swig_gopanic("C++ std::out_of_range exception thrown");
    
  }
  
  
}


void _wrap_delete_VecInt_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0) {
  std::vector< int > *arg1 = (std::vector< int > *) 0 ;
  
  arg1 = *(std::vector< int > **)&_swig_go_0; 
  
  delete arg1;
  
}


intgo _wrap_compare_compare_length_f57c2e54145f5e02(std::vector< int > *_swig_go_0, std::vector< int > *_swig_go_1) {
  std::vector< int > arg1 ;
  std::vector< int > arg2 ;
  std::vector< int > const *argp1 ;
  std::vector< int > const *argp2 ;
  int result;
  intgo _swig_go_result;
  
  
  argp1 = (std::vector< int > *)_swig_go_0;
  if (argp1 == NULL) {
    _swig_gopanic("Attempt to dereference null std::vector< int > const");
  }
  arg1 = (std::vector< int >)*argp1;
  
  
  argp2 = (std::vector< int > *)_swig_go_1;
  if (argp2 == NULL) {
    _swig_gopanic("Attempt to dereference null std::vector< int > const");
  }
  arg2 = (std::vector< int >)*argp2;
  
  
  result = (int)compare(SWIG_STD_MOVE(arg1),SWIG_STD_MOVE(arg2));
  _swig_go_result = result; 
  return _swig_go_result;
}


#ifdef __cplusplus
}
#endif
