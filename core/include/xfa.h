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

/**
 * @brief helper macro for other XFA_* macros
 *
 * @internal
 */
#define _XFA(name, prio) __attribute__((used, section(".xfa." #name "." #prio)))

/**
 * @brief Define a cross-file array
 *
 * This macro defines the symbols necessary to use XFA_START() and XFA_END().
 * It needs to be part of one single compilation unit.
 *
 * @param[in]   type    name of the cross-file array
 * @param[in]   name    name of the cross-file array
 */
#define XFA_INIT(type, name) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    const _XFA(name, 0_) type name [0] = {}; \
    const _XFA(name, 9_) type name ## _end [0] = {}; \
    _Pragma("GCC diagnostic pop") \
    extern unsigned __xfa_dummy

/**
 * @brief Declare an external cross-file array
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
    extern const type name [0]; \
    extern const type name ## _end [0];

/**
 * @brief Define variable in cross-file array
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
    _XFA(xfa_name, 5_ ##prio) \
    const typeof(entry) xfa_name ## _ ## prio ## _ ## name = entry

/**
 * @brief Calculate number of entries in cross-file array
 *
 */
#define XFA_LEN(type, name) (((char*)name ## _end - (char*)name)/sizeof(type))

/** @} */
#endif /* XFA_H */
