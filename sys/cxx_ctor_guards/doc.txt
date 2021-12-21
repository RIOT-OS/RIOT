/**
@defgroup   sys_cxx_ctor_guards     C++ constructor guards for static instances
@ingroup    cpp
@brief      C++ constructor guards for thread-safe initialization of static
            instances

@warning    This implementation is likely only compatible with `g++`

# Introduction

The libstd++ ABI requires implementations of the following functions:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
int __cxa_guard_acquire(__guard *g);
void __cxa_guard_release(__guard *g);
void __cxa_guard_abort(__guard *g);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions are not intended to be ever used by the programmer, instead
the C++ compiler will emit code calling them if statically allocated class
instances are used. In a multi-threaded environment special care needs to be
taken to prevent race conditions while initializing and using such instances.
This modules provides them.

# Usage

This module is intended to be used by platforms that want to provide C++
support, but the used standard C++ library does not provide these guards. In
this case, adding this module will do the trick. The programmer / user should
never interact with any of the functions.

Note that on some platforms the type `__guard` is defined differently from the
"generic" definition, most notably ARM. For those platforms a header named
`cxx_ctor_guards_arch.h` needs to be created containing the correct `typedef`
and the preprocessor macro `CXX_CTOR_GUARDS_CUSTOM_TYPE` needs to be defined.

# Implementation

This implementation provides the C++ ctor guards as defined by the libstd++ ABI
used in g++. It will likely not be compatible with other implementations of
libstd++.

The libstd++ ABI expects the functions to be implemented as C functions. Most
implementations will put the code into C++ files and wrap everything into an
`extern "C" {...}`. This implementation will just use a plain C file for less
boilerplate.

The implementation intentionally makes only use of a single byte of the
`__guard` type. This should result in the implementation being usable on any
platform, regardless of the actual size of `__guard`.
 */
