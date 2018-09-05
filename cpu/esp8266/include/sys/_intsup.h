/*
 * Copyright (c) 2004, 2005 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

/**
 * @brief Modified _intsup.h for compilation with RIOT OS
 * Source: /path/to/newlib-xtensa/xtensa-lx106-elf/include/sys
 * Changes: __INT32 (see below)
 */

#ifndef SYS__INTSUP_H
#define SYS__INTSUP_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/* additional header guard to avoid conflicts if original file is included later */
#ifndef _SYS__INTSUP_H
#define _SYS__INTSUP_H

#include <sys/features.h>

#if __GNUC_PREREQ (3, 2)
/* gcc > 3.2 implicitly defines the values we are interested */
#define __STDINT_EXP(x) __##x##__
#else
#define __STDINT_EXP(x) x
#include <limits.h>
#endif

/* Determine how intptr_t and intN_t fastN_t and leastN_t are defined by gcc
   for this target.  This is used to determine the correct printf() constant in
   inttypes.h and other  constants in stdint.h.
   So we end up with
   ?(signed|unsigned) char == 0
   ?(signed|unsigned) short == 1
   ?(signed|unsigned) int == 2
   ?(signed|unsigned) short int == 3
   ?(signed|unsigned) long == 4
   ?(signed|unsigned) long int == 6
   ?(signed|unsigned) long long == 8
   ?(signed|unsigned) long long int == 10
 */
#pragma push_macro("signed")
#pragma push_macro("unsigned")
#pragma push_macro("char")
#pragma push_macro("short")
#pragma push_macro("__int20")
#pragma push_macro("int")
#pragma push_macro("long")
#undef signed
#undef unsigned
#undef char
#undef short
#undef int
#undef __int20
#undef long
#define signed +0
#define unsigned +0
#define char +0
#define short +1
#define __int20 +2
#define int +2
#define long +4
#if (__INTPTR_TYPE__ == 8 || __INTPTR_TYPE__ == 10)
#define _INTPTR_EQ_LONGLONG
#elif (__INTPTR_TYPE__ == 4 || __INTPTR_TYPE__ == 6)
#define _INTPTR_EQ_LONG
/* Note - the tests for _INTPTR_EQ_INT and _INTPTR_EQ_SHORT are currently
   redundant as the values are not used.  But one day they may be needed
   and so the tests remain.  */
#elif __INTPTR_TYPE__ == 2
#define _INTPTR_EQ_INT
#elif (__INTPTR_TYPE__ == 1 || __INTPTR_TYPE__ == 3)
#define _INTPTR_EQ_SHORT
#else
#error "Unable to determine type definition of intptr_t"
#endif
#if (__INT32_TYPE__ == 4 || __INT32_TYPE__ == 6)
#define _INT32_EQ_LONG
#elif __INT32_TYPE__ == 2
/* Nothing to define because int32_t is safe to print as an int. */
#else
#error "Unable to determine type definition of int32_t"
#endif

#if (__INT8_TYPE__ == 0)
#define __INT8 "hh"
#elif (__INT8_TYPE__ == 1 || __INT8_TYPE__ == 3)
#define __INT8 "h"
#elif (__INT8_TYPE__ == 2)
#define __INT8
#elif (__INT8_TYPE__ == 4 || __INT8_TYPE__ == 6)
#define __INT8 "l"
#elif (__INT8_TYPE__ == 8 || __INT8_TYPE__ == 10)
#define __INT8 "ll"
#endif
#if (__INT16_TYPE__ == 1 || __INT16_TYPE__ == 3)
#define __INT16 "h"
#elif (__INT16_TYPE__ == 2)
#define __INT16
#elif (__INT16_TYPE__ == 4 || __INT16_TYPE__ == 6)
#define __INT16 "l"
#elif (__INT16_TYPE__ == 8 || __INT16_TYPE__ == 10)
#define __INT16 "ll"
#endif
#if (__INT32_TYPE__ == 2)
#define __INT32
#elif (__INT32_TYPE__ == 4 || __INT32_TYPE__ == 6)
/**
 * Definition of __INT32 had to be changed since to avoid format warnings/
 * errors since xtensa-lx106-elf-gcc defines *__INT32_TYPE__* as *long int*
 * while newlib defines *int32_t* as *signed int*. PRI*32 there throw format
 * warnings/errors.
 */
#if 0
#define __INT32 "l"
#else
#define __INT32
#endif
#elif (__INT32_TYPE__ == 8 || __INT32_TYPE__ == 10)
#define __INT32 "ll"
#endif
#if (__INT64_TYPE__ == 2)
#define __INT64
#elif (__INT64_TYPE__ == 4 || __INT64_TYPE__ == 6)
#define __INT64 "l"
#elif (__INT64_TYPE__ == 8 || __INT64_TYPE__ == 10)
#define __INT64 "ll"
#endif
#if (__INT_FAST8_TYPE__ == 0)
#define __FAST8 "hh"
#elif (__INT_FAST8_TYPE__ == 1 || __INT_FAST8_TYPE__ == 3)
#define __FAST8 "h"
#elif (__INT_FAST8_TYPE__ == 2)
#define __FAST8
#elif (__INT_FAST8_TYPE__ == 4 || __INT_FAST8_TYPE__ == 6)
#define __FAST8 "l"
#elif (__INT_FAST8_TYPE__ == 8 || __INT_FAST8_TYPE__ == 10)
#define __FAST8 "ll"
#endif
#if (__INT_FAST16_TYPE__ == 1 || __INT_FAST16_TYPE__ == 3)
#define __FAST16 "h"
#elif (__INT_FAST16_TYPE__ == 2)
#define __FAST16
#elif (__INT_FAST16_TYPE__ == 4 || __INT_FAST16_TYPE__ == 6)
#define __FAST16 "l"
#elif (__INT_FAST16_TYPE__ == 8 || __INT_FAST16_TYPE__ == 10)
#define __FAST16 "ll"
#endif
#if (__INT_FAST32_TYPE__ == 2)
#define __FAST32
#elif (__INT_FAST32_TYPE__ == 4 || __INT_FAST32_TYPE__ == 6)
#define __FAST32 "l"
#elif (__INT_FAST32_TYPE__ == 8 || __INT_FAST32_TYPE__ == 10)
#define __FAST32 "ll"
#endif
#if (__INT_FAST64_TYPE__ == 2)
#define __FAST64
#elif (__INT_FAST64_TYPE__ == 4 || __INT_FAST64_TYPE__ == 6)
#define __FAST64 "l"
#elif (__INT_FAST64_TYPE__ == 8 || __INT_FAST64_TYPE__ == 10)
#define __FAST64 "ll"
#endif

#if (__INT_LEAST8_TYPE__ == 0)
#define __LEAST8 "hh"
#elif (__INT_LEAST8_TYPE__ == 1 || __INT_LEAST8_TYPE__ == 3)
#define __LEAST8 "h"
#elif (__INT_LEAST8_TYPE__ == 2)
#define __LEAST8
#elif (__INT_LEAST8_TYPE__ == 4 || __INT_LEAST8_TYPE__ == 6)
#define __LEAST8 "l"
#elif (__INT_LEAST8_TYPE__ == 8 || __INT_LEAST8_TYPE__ == 10)
#define __LEAST8 "ll"
#endif
#if (__INT_LEAST16_TYPE__ == 1 || __INT_LEAST16_TYPE__ == 3)
#define __LEAST16 "h"
#elif (__INT_LEAST16_TYPE__ == 2)
#define __LEAST16
#elif (__INT_LEAST16_TYPE__ == 4 || __INT_LEAST16_TYPE__ == 6)
#define __LEAST16 "l"
#elif (__INT_LEAST16_TYPE__ == 8 || __INT_LEAST16_TYPE__ == 10)
#define __LEAST16 "ll"
#endif
#if (__INT_LEAST32_TYPE__ == 2)
#define __LEAST32
#elif (__INT_LEAST32_TYPE__ == 4 || __INT_LEAST32_TYPE__ == 6)
#define __LEAST32 "l"
#elif (__INT_LEAST32_TYPE__ == 8 || __INT_LEAST32_TYPE__ == 10)
#define __LEAST32 "ll"
#endif
#if (__INT_LEAST64_TYPE__ == 2)
#define __LEAST64
#elif (__INT_LEAST64_TYPE__ == 4 || __INT_LEAST64_TYPE__ == 6)
#define __LEAST64 "l"
#elif (__INT_LEAST64_TYPE__ == 8 || __INT_LEAST64_TYPE__ == 10)
#define __LEAST64 "ll"
#endif
#undef signed
#undef unsigned
#undef char
#undef short
#undef int
#undef long
#pragma pop_macro("signed")
#pragma pop_macro("unsigned")
#pragma pop_macro("char")
#pragma pop_macro("short")
#pragma pop_macro("__int20")
#pragma pop_macro("int")
#pragma pop_macro("long")

#endif /* _SYS__INTSUP_H */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SYS__INTSUP_H */
