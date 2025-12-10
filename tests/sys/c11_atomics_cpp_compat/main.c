/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Compile test to verify values in c11_atomics_compat_cpu.hpp
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* This file contains only #defines, so it is fully compatible with C */
#include "c11_atomics_compat_cpu.hpp"

int main(void)
{
    _Static_assert(ATOMIC_BOOL_SIZE == sizeof(_Atomic(bool)), "Size of type must match define");
    _Static_assert(ATOMIC_CHAR_SIZE == sizeof(_Atomic(char)), "Size of type must match define");
    _Static_assert(ATOMIC_SCHAR_SIZE == sizeof(_Atomic(signed char)), "Size of type must match define");
    _Static_assert(ATOMIC_UCHAR_SIZE == sizeof(_Atomic(unsigned char)), "Size of type must match define");
    _Static_assert(ATOMIC_SHORT_SIZE == sizeof(_Atomic(short)), "Size of type must match define");
    _Static_assert(ATOMIC_USHORT_SIZE == sizeof(_Atomic(unsigned short)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_SIZE == sizeof(_Atomic(int)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_SIZE == sizeof(_Atomic(unsigned int)), "Size of type must match define");
    _Static_assert(ATOMIC_LONG_SIZE == sizeof(_Atomic(long)), "Size of type must match define");
    _Static_assert(ATOMIC_ULONG_SIZE == sizeof(_Atomic(unsigned long)), "Size of type must match define");
    _Static_assert(ATOMIC_LLONG_SIZE == sizeof(_Atomic(long long)), "Size of type must match define");
    _Static_assert(ATOMIC_ULLONG_SIZE == sizeof(_Atomic(unsigned long long)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST8_T_SIZE == sizeof(_Atomic(int_least8_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST8_T_SIZE == sizeof(_Atomic(uint_least8_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST16_T_SIZE == sizeof(_Atomic(int_least16_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST16_T_SIZE == sizeof(_Atomic(uint_least16_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST32_T_SIZE == sizeof(_Atomic(int_least32_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST32_T_SIZE == sizeof(_Atomic(uint_least32_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST64_T_SIZE == sizeof(_Atomic(int_least64_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST64_T_SIZE == sizeof(_Atomic(uint_least64_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST8_T_SIZE == sizeof(_Atomic(int_fast8_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST8_T_SIZE == sizeof(_Atomic(uint_fast8_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST16_T_SIZE == sizeof(_Atomic(int_fast16_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST16_T_SIZE == sizeof(_Atomic(uint_fast16_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST32_T_SIZE == sizeof(_Atomic(int_fast32_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST32_T_SIZE == sizeof(_Atomic(uint_fast32_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST64_T_SIZE == sizeof(_Atomic(int_fast64_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST64_T_SIZE == sizeof(_Atomic(uint_fast64_t)), "Size of type must match define");
    _Static_assert(ATOMIC_INTPTR_T_SIZE == sizeof(_Atomic(intptr_t)), "Size of type must match define");
    _Static_assert(ATOMIC_UINTPTR_T_SIZE == sizeof(_Atomic(uintptr_t)), "Size of type must match define");
    _Static_assert(ATOMIC_SIZE_T_SIZE == sizeof(_Atomic(size_t)), "Size of type must match define");

    _Static_assert(ATOMIC_BOOL_SIZE == sizeof(ATOMIC_BOOL_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_CHAR_SIZE == sizeof(ATOMIC_CHAR_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_SCHAR_SIZE == sizeof(ATOMIC_SCHAR_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UCHAR_SIZE == sizeof(ATOMIC_UCHAR_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_SHORT_SIZE == sizeof(ATOMIC_SHORT_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_USHORT_SIZE == sizeof(ATOMIC_USHORT_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_SIZE == sizeof(ATOMIC_INT_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_SIZE == sizeof(ATOMIC_UINT_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_LONG_SIZE == sizeof(ATOMIC_LONG_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_ULONG_SIZE == sizeof(ATOMIC_ULONG_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_LLONG_SIZE == sizeof(ATOMIC_LLONG_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_ULLONG_SIZE == sizeof(ATOMIC_ULLONG_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST8_T_SIZE == sizeof(ATOMIC_INT_LEAST8_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST8_T_SIZE == sizeof(ATOMIC_UINT_LEAST8_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST16_T_SIZE == sizeof(ATOMIC_INT_LEAST16_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST16_T_SIZE == sizeof(ATOMIC_UINT_LEAST16_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST32_T_SIZE == sizeof(ATOMIC_INT_LEAST32_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST32_T_SIZE == sizeof(ATOMIC_UINT_LEAST32_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_LEAST64_T_SIZE == sizeof(ATOMIC_INT_LEAST64_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_LEAST64_T_SIZE == sizeof(ATOMIC_UINT_LEAST64_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST8_T_SIZE == sizeof(ATOMIC_INT_FAST8_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST8_T_SIZE == sizeof(ATOMIC_UINT_FAST8_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST16_T_SIZE == sizeof(ATOMIC_INT_FAST16_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST16_T_SIZE == sizeof(ATOMIC_UINT_FAST16_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST32_T_SIZE == sizeof(ATOMIC_INT_FAST32_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST32_T_SIZE == sizeof(ATOMIC_UINT_FAST32_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INT_FAST64_T_SIZE == sizeof(ATOMIC_INT_FAST64_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINT_FAST64_T_SIZE == sizeof(ATOMIC_UINT_FAST64_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_INTPTR_T_SIZE == sizeof(ATOMIC_INTPTR_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_UINTPTR_T_SIZE == sizeof(ATOMIC_UINTPTR_T_SAME_SIZED_TYPE), "Size of type must match define");
    _Static_assert(ATOMIC_SIZE_T_SIZE == sizeof(ATOMIC_SIZE_T_SAME_SIZED_TYPE), "Size of type must match define");

    return 0;
}
