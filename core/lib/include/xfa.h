/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2025 carl-tud
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup core_util_xfa
 * @{
 */

/**
 * @file
 * @brief       Cross-File Arrays (XFA)
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      carl-tud
 */
#ifndef XFA_H
#define XFA_H

#include <inttypes.h>
#include "compiler_hints.h"

/*
 * Unfortunately, current gcc trips over accessing XFA's because of their
 * zero-size start/end array that are used of symbol markers, with an "array
 * index out of bounds" warning.  So until a solution for that is found, we
 * need to disable array bounds checks for files using XFAs.
 */
#ifndef DOXYGEN
_Pragma("GCC diagnostic ignored \"-Warray-bounds\"")

#  if defined(__APPLE__) /* Darwin */

/**
 *   @brief Helper for @ref _IF_EMPTY
 *
 *   ```
 *   _IF_EMPTY_HELPER(           FULL, EMPTY)  ->  EMPTY
 *   _IF_EMPTY_HELPER(something, FULL, EMPTY)  ->  FULL
 *   ```
 */
#    define _IF_EMPTY_HELPER(_0, _1, ...) _1

/**
 *   @brief Ternary-operator-like macro.
 *
 *   @param if_empty Expression to be returned of argument is empty
 *   @param if_not_empty Expression to be returned of argument is **not** empty
 *   @param arg Argument to be check if empty
 *
 *   @returns @p if_empty or @p if_not_empty depending on @p arg
 */
#    define _IF_EMPTY(if_empty, if_not_empty, ...) \
        _IF_EMPTY_HELPER(__VA_OPT__(,) if_not_empty, if_empty)

/*   These are helpers for _XFA_ERROR */
#    define DO_PRAGMA_(x) _Pragma (#x)
#    define DO_PRAGMA__(x) DO_PRAGMA_(x)
#    define DO_PRAGMA___(x) DO_PRAGMA__(x)
#    define DO_PRAGMA____(x) DO_PRAGMA___(x)
#    define DO_PRAGMA(x) DO_PRAGMA____(x)

/**
 *   @brief Emits compiler error
 *   @param text Message to be emitted
 */
#    define _XFA_ERROR(text) DO_PRAGMA(message text)

/**
 *   @brief Defines symbol alias compatible with macOS linker
 *   @private
 *   @param old Old name
 *   @param new New name
 */
#    define _XFA_ALIAS(old, new) DO_PRAGMA(redefine_extname old new)

/**
 *   @brief Generates XFA symbol ID
 *   @private
 *
 *   @param xfa_name Name of XFA
 *   @param prio Priority label
 *   @param element_name Name of XFA element
 *
 *   @returns Identifier expression
 */
#    define _XFA_ID(xfa_name, prio, element_name) \
        __xfa__ ## $ ## xfa_name ## $ ## prio ## $ ## element_name

/**
 *   @brief Appends element to XFA (Mach-O)
 *   @private
 *
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 *   @param prio Priority label
 *   @param element_name Name of XFA element
 *
 *   If @p element_name is missing, this will fail, contrary to Linux.
 */
#    define _XFA_ELEMENT(type, xfa_name, prio, element_name)                            \
        _IF_EMPTY(                                                                        \
            /* if element name is missing, error: */                                      \
            _XFA_ERROR("Element of cross-file array '" #xfa_name "' "                   \
                       "must have an identifier (4th argument) on macOS."),    \
            /* if okay, define alias: */                                                  \
            _XFA_ALIAS(element_name, _XFA_ID(xfa_name, prio, element_name)),            \
            /* Check if this element name is missing: */                                  \
            element_name)                                                                 \
        __attribute__((used)) _Alignas(type) type element_name

/**
 *   @brief Appends read-only element to XFA (Mach-O)
 *   @private
 *
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 *   @param prio Priority label
 *   @param element_name Name of XFA element
 *
 *   If @p element_name is missing, this will fail, contrary to Linux.
 *
 *   @warning Currently maps to the read-write version due to an `invalid text-relocation` error.
 */
#    define _XFA_ELEMENT_CONST(type, xfa_name, prio, element_name)                      \
                  _XFA_ELEMENT(type, xfa_name, prio, element_name)

/**
 *   @brief Initializes XFA
 *   @private
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 */
#    define _XFA_INIT(type, xfa_name)                                                   \
        /* Declare start marker, will be defined by build system, hence extern. */        \
        _XFA_ALIAS(xfa_name, _XFA_ID(xfa_name, 0, __start__))                         \
        extern type xfa_name [] __attribute__((weak_import));                           \
                                                                                          \
        /* Define end marker */                                                           \
        _XFA_ALIAS(xfa_name ## _end, _XFA_ID(xfa_name, 9, __end__))                   \
        __attribute__((section("__DATA,__data"), used))                                   \
            type xfa_name ## _end [] = {0};

/**
 *   @brief Initializes read-only XFA
 *   @private
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 *
 *   @warning Currently maps to the read-write version due to an `invalid text-relocation` error.
 */
#    define _XFA_INIT_CONST(type, xfa_name)                                             \
                  _XFA_INIT(type, xfa_name)

/**
 *   @brief Imports XFA defined in another translation unit
 *   @private
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 */
#    define _XFA_USE(type, xfa_name)                                                    \
        /* Declare extern start marker */                                                 \
        _XFA_ALIAS(xfa_name, _XFA_ID(xfa_name, 0, __start__))                         \
        extern type xfa_name [] __attribute__((weak_import));                           \
                                                                                          \
        /* Declare extern end marker */                                                   \
        _XFA_ALIAS(xfa_name ## _end, _XFA_ID(xfa_name, 9, __end__))                   \
            extern type xfa_name ## _end [];

/**
 *   @brief Imports XFA defined in another translation unit
 *   @private
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 *
 *   @warning Currently maps to the read-write version due to an `invalid text-relocation` error.
 */
#    define _XFA_USE_CONST(type, xfa_name)                                              \
                  _XFA_USE(type, xfa_name)

#  else /* GNU/Linux */

#    define _XFA_ELEMENT(type, xfa_name, prio, element_name)                            \
        NO_SANITIZE_ARRAY                                                                 \
        __attribute__((used, section(".xfa." #xfa_name "." #prio))) _Alignas(type)      \
            type element_name

/**
 *   @brief Initializes XFA
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 *   @private
 *
 *   The pragmas prevent these errors:
 *
 *       error: ISO C forbids empty initializer braces
 *       error: ISO C forbids zero-size array ‘xfatest_end’
 */
#    define _XFA_INIT(type, xfa_name)                                                   \
        _Pragma("GCC diagnostic push")                                                    \
        _Pragma("GCC diagnostic ignored \"-Wpedantic\"")                                  \
        _XFA_ELEMENT(type, xfa_name, 0_, xfa_name) [0] = {};                          \
        _XFA_ELEMENT(type, xfa_name, 9_, xfa_name ## _end) [0] = {};                  \
        _Pragma("GCC diagnostic pop")                                                     \
        extern const unsigned __xfa_dummy

#    define _XFA_USE(type, xfa_name)                                                    \
        extern type xfa_name [];                                                        \
        extern type xfa_name ## _end []

#    define _XFA_ELEMENT_CONST(type, xfa_name, prio, element_name)                      \
        NO_SANITIZE_ARRAY                                                                 \
        __attribute__((used, section(".roxfa." #xfa_name "." #prio))) _Alignas(type)    \
            type element_name

/**
 *   @brief Initializes read-only XFA
 *   @param type C identifier of element type, e.g., `int`
 *   @param xfa_name Name of XFA
 *   @private
 *
 *   The pragmas prevent these errors:

 *       error: ISO C forbids empty initializer braces
 *       error: ISO C forbids zero-size array ‘xfatest_const_end’
 */
#    define _XFA_INIT_CONST(type, xfa_name)                                             \
        _Pragma("GCC diagnostic push")                                                    \
        _Pragma("GCC diagnostic ignored \"-Wpedantic\"")                                  \
        _XFA_ELEMENT_CONST(type, xfa_name, 0_, xfa_name) [0] = {};                    \
        _XFA_ELEMENT_CONST(type, xfa_name, 9_, xfa_name ## _end) [0] = {};            \
        _Pragma("GCC diagnostic pop")                                                     \
        extern const unsigned __xfa_dummy

#    define _XFA_USE_CONST(type, xfa_name)                                              \
        extern const type xfa_name [];                                                  \
        extern const type xfa_name ## _end []

#  endif /* OS-dependent */
#endif /* DOYGEN */

/* MARK: - Defining cross-file arrays */
/**
 * @name Defining cross-file arrays
 * @{
 */
/**
 * @brief Define a read-only cross-file array
 *
 * This macro defines the symbols necessary to use @ref XFA_LEN and @ref XFA_USE_CONST.
 * It needs to be part of one single compilation unit.
 *
 * @param[in] type     Type of the cross-file array
 * @param[in] xfa_name Name of the cross-file array
 */
#define XFA_INIT_CONST(type, xfa_name) _XFA_INIT_CONST(type, xfa_name)

/**
 * @brief Define a writable cross-file array
 *
 * This macro defines the symbols necessary to use @ref XFA_LEN and @ref XFA_USE.
 * It needs to be part of one single compilation unit.
 *
 *
 * @param[in] type     Type of the cross-file array
 * @param[in] xfa_name Name of the cross-file array
 */
#define XFA_INIT(type, xfa_name) _XFA_INIT(type, xfa_name)

/**
 * @brief Declare an external read-only cross-file array
 *
 * This macro defines the symbols necessary to use @ref XFA_LEN.
 * Think of this as @ref XFA_INIT but with `extern` keyword.
 * It is supposed to be used in compilation units where the cross file array is
 * being accessed, but not defined using @ref XFA_INIT.
 *
 * @param[in] type     Type of the cross-file array
 * @param[in] xfa_name Name of the cross-file array
 */
#define XFA_USE_CONST(type, xfa_name) _XFA_USE_CONST(type, xfa_name)

/**
 * @brief Declare an external writable cross-file array
 *
 * This macro defines the symbols necessary to use @ref XFA_LEN.
 * Think of this as @ref XFA_INIT but with `extern` keyword.
 * It is supposed to be used in compilation units where the cross file array is
 * being accessed, but not defined using @ref XFA_INIT.
 *
 * @param[in] type     Type of the cross-file array
 * @param[in] xfa_name Name of the cross-file array
 */
#define XFA_USE(type, xfa_name) _XFA_USE(type, xfa_name)
/** @} */

/* MARK: - Adding elements */
/**
 * @name Adding elements
 * @{
 */
/**
 * @brief Define variable in writable cross-file array
 *
 * Variables will end up sorted by prio.
 *
 * Add this to the type in a variable definition, e.g.:
 *
 *     XFA(driver_params_t, driver_params, 0, _onboard) = { .pin=42 };
 *
 * or (legacy version, does not work on macOS):
 *
 *     XFA(driver_params_t, driver_params, 0) _onboard = { .pin=42 };
 *
 * @param[in] type         Type of the xfa elements
 * @param[in] xfa_name     Name of the cross-file array
 * @param[in] prio         Priority within the xfa
 * @param[in] element_name Name of the element that is added to the cross-file array. Optional.
 */
#define XFA(type, xfa_name, prio, ...) \
    _XFA_ELEMENT(type, xfa_name, 5_ ## prio, __VA_ARGS__)

/**
 * @brief Define variable in read-only cross-file array
 *
 * Variables will end up sorted by prio.
 *
 * Add this to the type in a variable definition, e.g.:
 *
 *     XFA_CONST(driver_params_t, driver_params, 0, _onboard) = { .pin=42 };
 *
 * or (legacy version, does not work on macOS):
 *
 *     XFA_CONST(driver_params_t, driver_params, 0) _onboard = { .pin=42 };
 *
 * @param[in] type         Type of the XFA elements
 * @param[in] xfa_name     Name of the cross-file array
 * @param[in] prio         Priority within the XFA
 * @param[in] element_name Name of the element that is added to the cross-file array. Optional.
 */
#define XFA_CONST(type, xfa_name, prio, ...) \
    _XFA_ELEMENT_CONST(type, xfa_name, 5_ ## prio, __VA_ARGS__)

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
 * @param[in] xfa_name      Name of the XFA
 * @param[in] prio          Priority within the XFA
 * @param[in] element_name  Name of element
 * @param[in] entry         Pointer variable to add to XFA
 */
#define XFA_ADD_PTR(xfa_name, prio, element_name, entry) \
    _XFA_CONST(__typeof__(entry), xfa_name, 5_ ## prio, element_name, entry) = entry
/** @} */

/* MARK: - Accessing array properties */
/**
 * @name Accessing array properties
 * @{
 */
/**
 * @brief Calculate number of entries in cross-file array
 *
 * @param type C identifier of element type, e.g., `int`
 * @param xfa_name Name of cross-file array
 *
 * @returns Element count
 */
#define XFA_LEN(type, xfa_name) \
    (((uintptr_t)xfa_name ## _end - (uintptr_t)xfa_name) / sizeof(type))
/** @} */

#ifdef __cplusplus
extern "C" {
#endif
/* making externc happy */
#ifdef __cplusplus
}
#endif

#endif /* XFA_H */
/** @} */
