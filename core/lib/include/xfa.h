/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     core_util
 * @brief       Cross File Arrays
 * @{
 *
 * This macro, in combination with an entry in the linker scripts, allows the
 * definition of constant arrays to be spread over multiple C compilation
 * units. These arrays are called "cross-file arrays" or short xfa.
 *
 * @experimental This API is considered experimental and will probably change
 *               without notice!
 *
 * @file
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <inttypes.h>
#include <stdalign.h>

#include "compiler_hints.h"

/*
 * Unfortunately, current gcc trips over accessing XFA's because of their
 * zero-size start/end array that are used of symbol markers, with an "array
 * index out of bounds" warning.  So until a solution for that is found, we
 * need to disable array bounds checks for files using XFAs.
 */
#ifndef DOXYGEN
_Pragma("GCC diagnostic ignored \"-Warray-bounds\"")
#endif

/**
 * @brief helper macro for other XFA_* macros
 *
 * @internal
 */
#define _XFA(type, name, prio) \
    NO_SANITIZE_ARRAY \
    __attribute__((used, section(".xfa." #name "." #prio))) alignas(type) type

/**
 * @brief helper macro for other XFA_* macros
 *
 * @internal
 */
#define _XFA_CONST(type, name, prio) \
    NO_SANITIZE_ARRAY \
    __attribute__((used, section(".roxfa." #name "." #prio))) alignas(type) type

/**
 * @brief Define a read-only cross-file array
 *
 * This macro defines the symbols necessary to use XFA_START() and XFA_END().
 * It needs to be part of one single compilation unit.
 *
 * The pragmas prevent these errors:
 *
 *     error: ISO C forbids empty initializer braces
 *     error: ISO C forbids zero-size array ‘xfatest_const_end’
 *
 * @param[in]   type    type of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_INIT_CONST(type, name) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    _XFA_CONST(type, name, 0_) name [0] = {}; \
    _XFA_CONST(type, name, 9_) name ## _end [0] = {}; \
    _Pragma("GCC diagnostic pop") \
    extern const unsigned __xfa_dummy

/**
 * @brief Define a writable cross-file array
 *
 * This macro defines the symbols necessary to use XFA_START() and XFA_END().
 * It needs to be part of one single compilation unit.
 *
 * The pragmas prevent these errors:
 *
 *     error: ISO C forbids empty initializer braces
 *     error: ISO C forbids zero-size array ‘xfatest_end’
 *
 * @param[in]   type    type of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_INIT(type, name) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    _XFA(type, name, 0_) name [0] = {}; \
    _XFA(type, name, 9_) name ## _end [0] = {}; \
    _Pragma("GCC diagnostic pop") \
    extern const unsigned __xfa_dummy

/**
 * @brief Declare an external read-only cross-file array
 *
 * This macro defines the symbols necessary to use XFA_START() and XFA_END().
 * Think of this as XFA_INIT() but with "extern" keyword.
 * It is supposed to be used in compilation units where the cross file array is
 * being accessed, but not defined using XFA_INIT.
 *
 * @param[in]   type    type of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_USE_CONST(type, name) \
    XFA_USE(type, name)

/**
 * @brief Declare an external writable cross-file array
 *
 * This macro defines the symbols necessary to use XFA_START() and XFA_END().
 * Think of this as XFA_INIT() but with "extern" keyword.
 * It is supposed to be used in compilation units where the cross file array is
 * being accessed, but not defined using XFA_INIT.
 *
 * @param[in]   type    type of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_USE(type, name) \
    extern type name []; \
    extern type name ## _end []

/**
 * @brief Define variable in writable cross-file array
 *
 * Variables will end up sorted by prio.
 *
 * Add this to the type in a variable definition, e.g.:
 *
 *     XFA(driver_params_t, driver_params, 0) _onboard = { .pin=42 };
 *
 * @param[in]   type        type of the xfa elements
 * @param[in]   xfa_name    name of the xfa
 * @param[in]   prio        priority within the xfa
 */
#define XFA(type, xfa_name, prio) _XFA(type, xfa_name, 5_ ## prio)

/**
 * @brief Define variable in read-only cross-file array
 *
 * Variables will end up sorted by prio.
 *
 * Add this to the type in a variable definition, e.g.:
 *
 *     XFA(driver_params_t, driver_params, 0) _onboard = { .pin=42 };
 *
 * @param[in]   type        type of the xfa elements
 * @param[in]   xfa_name    name of the xfa
 * @param[in]   prio        priority within the xfa
 */
#define XFA_CONST(type, xfa_name, prio) _XFA_CONST(type, xfa_name, 5_ ## prio)

/**
 * @brief Add a pointer to cross-file array
 *
 * Pointers will end up sorted by prio.
 *
 * @note This implementation uses the __typeof__() C extension.
 *       It is available both in GCC and LLVM, and both don't complain with
 *       plain "-std=c11". If deemed necessary, use of __typeof__ can be removed
 *       and the type can be added as parameter, at the cost of much less
 *       convenience.
 *
 * @param[in]   xfa_name    name of the xfa
 * @param[in]   prio        priority within the xfa
 * @param[in]   name        symbol name
 * @param[in]   entry       pointer variable to add to xfa
 */
#define XFA_ADD_PTR(xfa_name, prio, name, entry) \
    _XFA_CONST(__typeof__(entry), xfa_name, 5_ ## prio) \
    xfa_name ## _ ## prio ## _ ## name = entry

/**
 * @brief Calculate number of entries in cross-file array
 */
#define XFA_LEN(type, \
                name) (((uintptr_t)name ## _end - (uintptr_t)name) / \
                       sizeof(type))

#ifdef __cplusplus
extern "C" {
#endif
/* making externc happy */
#ifdef __cplusplus
}
#endif

/** @} */
