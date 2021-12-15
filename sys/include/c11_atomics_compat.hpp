/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_c11_atomics_cpp_compat C++ compatibility with C11 atomics
 * @ingroup     cpp
 * @brief       C++ compatibility of default C11 atomics types
 *
 * This module provides opaque `typedef`s for each standard C11 atomic type with
 * the same size and alignment requirements. The contents of types are not
 * accessible to C++ code, but this allows `struct`s to be defined with the
 * same memory layout as the C version, so that they can be allocated from C++.
 *
 * @{
 *
 * @file
 * @brief       C++ compatibility of default C11 atomics types
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#pragma once

#include "c11_atomics_compat_cpu.hpp"

/**
 * @brief   Initialize an atomic variable
 *
 * Usage (same syntax as the real C11 atomic initializer):
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * atomic_int foo = ATOMIC_VAR_INIT(42);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifndef ATOMIC_VAR_INIT
#define ATOMIC_VAR_INIT(x) { x }
#endif

/**
  * @brief Type with the same alignment and size as `atomic_bool`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_BOOL_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_bool;

/**
  * @brief Type with the same alignment and size as `atomic_char`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_CHAR_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_char;

/**
  * @brief Type with the same alignment and size as `atomic_schar`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_SCHAR_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_schar;

/**
  * @brief Type with the same alignment and size as `atomic_uchar`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UCHAR_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uchar;

/**
  * @brief Type with the same alignment and size as `atomic_short`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_SHORT_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_short;

/**
  * @brief Type with the same alignment and size as `atomic_ushort`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_USHORT_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_ushort;

/**
  * @brief Type with the same alignment and size as `atomic_int`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int;

/**
  * @brief Type with the same alignment and size as `atomic_uint`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint;

/**
  * @brief Type with the same alignment and size as `atomic_long`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_LONG_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_long;

/**
  * @brief Type with the same alignment and size as `atomic_ulong`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_ULONG_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_ulong;

/**
  * @brief Type with the same alignment and size as `atomic_llong`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_LLONG_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_llong;

/**
  * @brief Type with the same alignment and size as `atomic_ullong`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_ULLONG_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_ullong;

/**
  * @brief Type with the same alignment and size as `atomic_int_least8_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_LEAST8_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_least8_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_least8_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_LEAST8_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_least8_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_least16_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_LEAST16_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_least16_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_least16_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_LEAST16_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_least16_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_least32_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_LEAST32_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_least32_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_least32_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_LEAST32_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_least32_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_least64_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_LEAST64_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_least64_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_least64_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_LEAST64_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_least64_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_fast8_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_FAST8_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_fast8_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_fast8_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_FAST8_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_fast8_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_fast16_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_FAST16_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_fast16_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_fast16_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_FAST16_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_fast16_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_fast32_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_FAST32_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_fast32_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_fast32_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_FAST32_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_fast32_t;

/**
  * @brief Type with the same alignment and size as `atomic_int_fast64_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INT_FAST64_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_int_fast64_t;

/**
  * @brief Type with the same alignment and size as `atomic_uint_fast64_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINT_FAST64_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uint_fast64_t;

/**
  * @brief Type with the same alignment and size as `atomic_intptr_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INTPTR_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_intptr_t;

/**
  * @brief Type with the same alignment and size as `atomic_uintptr_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINTPTR_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uintptr_t;

/**
  * @brief Type with the same alignment and size as `atomic_size_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_SIZE_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_size_t;

/**
  * @brief Type with the same alignment and size as `atomic_intmax_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_INTMAX_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_intmax_t;

/**
  * @brief Type with the same alignment and size as `atomic_uintmax_t`
 */
typedef struct {
    /**
     * @brief Opaque content placeholder
     *
     * Access only for initializing from C++
     */
    ATOMIC_UINTMAX_T_SAME_SIZED_TYPE do_not_access_from_cpp;
} atomic_uintmax_t;

/** @} */
