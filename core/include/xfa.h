/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @brief       Cross File Arrays
 * @{
 *
 * This macro, in combination with an entry in the linker scripts, allows the
 * definition of constant arrays to be spread over multiple C compilation
 * units. These arrays are called "cross-file arrays" or short xfa.
 *
 *
 *
 * @file
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef XFA_H
#define XFA_H

/*
 * Unfortunately, current gcc trips over accessing XFA's because of their
 * zero-size start/end array that are used of symbol markers, with an "array
 * index out of bounds" warning.  So until a solution for that is found, we
 * need to disable array bounds checks for files using XFAs.
 */
_Pragma("GCC diagnostic ignored \"-Warray-bounds\"")

/**
 * @brief helper macro for other XFA_* macros
 *
 * @internal
 */
#define _XFA(name, prio) __attribute__((used, section(".xfa." #name "." #prio)))

/**
 * @brief helper macro for other XFA_* macros
 *
 * @internal
 */
#define _XFA_CONST(name, prio) __attribute__((used, section(".roxfa." #name "." #prio)))

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
 * @param[in]   type    name of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_INIT_CONST(type, name) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    _XFA_CONST(name, 0_) const volatile type name [0] = {}; \
    _XFA_CONST(name, 9_) const volatile type name ## _end [0] = {}; \
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
 * @param[in]   type    name of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_INIT(type, name) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    _XFA(name, 0_) type name [0] = {}; \
    _XFA(name, 9_) type name ## _end [0] = {}; \
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
 * @param[in]   type    name of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_USE_CONST(type, name) \
    extern const type name []; \
    extern const type name ## _end []

/**
 * @brief Declare an external writable cross-file array
 *
 * This macro defines the symbols necessary to use XFA_START() and XFA_END().
 * Think of this as XFA_INIT() but with "extern" keyword.
 * It is supposed to be used in compilation units where the cross file array is
 * being accessed, but not defined using XFA_INIT.
 *
 * @param[in]   type    name of the cross-file array
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
 *     XFA(driver_params, 0) driver_params_t _onboard = { .pin=42 };
 *
 * @param[in]   name    name of the xfa
 * @param[in]   prio    priority within the xfa
 */
#define XFA(xfa_name, prio) _XFA(xfa_name, 5_ ##prio)

/**
 * @brief Define variable in read-only cross-file array
 *
 * Variables will end up sorted by prio.
 *
 * Add this to the type in a variable definition, e.g.:
 *
 *     XFA(driver_params, 0) driver_params_t _onboard = { .pin=42 };
 *
 * @param[in]   name    name of the xfa
 * @param[in]   prio    priority within the xfa
 */
#define XFA_CONST(xfa_name, prio) _XFA_CONST(xfa_name, 5_ ##prio)

/**
 * @brief Add a pointer to cross-file array
 *
 * Pointers will end up sorted by prio.
 *
 * @param[in]   xfa_name    name of the xfa
 * @param[in]   prio        priority within the xfa
 * @param[in]   name        symbol name
 * @param[in]   entry       pointer variable to add to xfa
 */
#define XFA_ADD_PTR(xfa_name, prio, name, entry) \
    _XFA_CONST(xfa_name, 5_ ##prio) \
    const typeof(entry) xfa_name ## _ ## prio ## _ ## name = entry

/**
 * @brief Calculate number of entries in cross-file array
 */
#define XFA_LEN(type, name) (((const char*)name ## _end - (const char*)name) / sizeof(type))

/** @} */
#endif /* XFA_H */
