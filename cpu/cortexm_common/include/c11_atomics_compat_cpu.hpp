#pragma once

#if defined(__clang__)
#include "c11_atomics_compat_cpu/llvm.hpp"
#elif defined(__GNUC__)
#include "c11_atomics_compat_cpu/gcc.hpp"
#else
#error "No support for C11 atomics compatibility with C++ for your compiler"
#endif
