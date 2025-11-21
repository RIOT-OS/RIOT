/*
 * SPDX-FileCopyrightText: 2015-2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         core_util
 * @{
 *
 * @file
 *
 * @brief Implementation of __sync atomic operations for GCC versions prior to v4.7
 *
 * @see https://gcc.gnu.org/wiki/Atomic/GCCMM/LIbrary
 * @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fsync-Builtins.html
 * @see https://gcc.gnu.org/onlinedocs/gcc-4.6.4/gcc/Atomic-Builtins.html
 *
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include "irq.h"

/*
 * uncrustify mis-formats the macros in this file, so disable it globally.
 * begin{code-style-ignore}
 */

/* use gcc/clang implementation if available */
#if defined(__GNUC__) \
    && (__GNUC__ > 4 || \
       (__GNUC__ == 4 && (__GNUC_MINOR__ > 7 || \
       (__GNUC_MINOR__ == 7 && __GNUC_PATCHLEVEL__ > 0)))) \
    || defined(__llvm__) || defined(__clang__)
#define HAVE_C11_SYNC
#endif

#if !defined(HAVE_C11_SYNC)

/* GCC documentation refers to the types as I1, I2, I4, I8, I16 */
typedef uint8_t  I1;
typedef uint16_t I2;
typedef uint32_t I4;
typedef uint64_t I8;
/* typedef __uint128_t I16; */ /* No 128 bit integer support yet */

/**
 * @brief This is a macro that defines a function named __sync_lock_test_and_set_<em>n</em>
 *
 * This is an exchange operation, not a compare and swap
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_SYNC_LOCK_TEST_AND_SET_N(n) \
    I##n __sync_lock_test_and_set_##n (I##n *ptr, I##n desired) \
    { \
        unsigned int mask = irq_disable(); \
        I##n old = *ptr;                   \
        *ptr = desired;                    \
        irq_restore(mask);                 \
        return old;                        \
    }

/**
 * @brief This is a macro that defines a function named __sync_lock_release_<em>n</em>
 *
 * This will write 0 to @p *ptr
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_SYNC_LOCK_RELEASE_N(n) \
    void __sync_lock_release_##n (I##n *ptr) \
    { \
        unsigned int mask = irq_disable(); \
        *ptr = 0;                          \
        irq_restore(mask);                 \
    }

/**
 * @brief This is a macro that defines a function named __sync_bool_compare_and_swap_<em>n</em>
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_SYNC_BOOL_COMPARE_AND_SWAP_N(n) \
    bool __sync_bool_compare_and_swap_##n (I##n *ptr, I##n expected, I##n desired) \
    { \
        unsigned int mask = irq_disable(); \
        I##n cur = *ptr;                   \
        if (cur != expected) {             \
            irq_restore(mask);             \
            return false;                  \
        }                                  \
                                           \
        *ptr = desired;                    \
        irq_restore(mask);                 \
        return true;                       \
    }

/**
 * @brief This is a macro that defines a function named __sync_val_compare_and_swap_<em>n</em>
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_SYNC_VAL_COMPARE_AND_SWAP_N(n) \
    I##n __sync_val_compare_and_swap_##n (I##n *ptr, I##n expected, I##n desired) \
    { \
        unsigned int mask = irq_disable(); \
        I##n cur = *ptr;                   \
        if (cur == expected) {             \
            *ptr = desired;                \
        }                                  \
                                           \
        irq_restore(mask);                 \
        return cur;                        \
    }

/**
 * @brief This is a macro that defines a function named __sync_fetch_and_<em>opname</em>_<em>n</em>
 *
 * @param[in]  opname    operator name that will be used in the function name
 * @param[in]  op        actual C language operator
 * @param[in]  n         width of the data, in bytes
 * @param[in]  prefixop  optional prefix unary operator (use ~ for inverting, NAND, NOR etc)
 */
#define TEMPLATE_SYNC_FETCH_AND_OP_N(opname, op, n, prefixop) \
    I##n __sync_fetch_and_##opname##_##n(I##n *ptr, I##n val) \
    { \
        unsigned int mask = irq_disable();             \
        I##n tmp = *ptr;                               \
        *ptr = prefixop(tmp op val);                   \
        irq_restore(mask);                             \
        return tmp;                                    \
    }

/**
 * @brief This is a macro that defines a function named __sync_<em>opname</em>_and_fetch_<em>n</em>
 *
 * @param[in]  opname    operator name that will be used in the function name
 * @param[in]  op        actual C language operator
 * @param[in]  n         width of the data, in bytes
 * @param[in]  prefixop  optional prefix unary operator (use ~ for inverting, NAND, NOR etc)
 */
#define TEMPLATE_SYNC_OP_AND_FETCH_N(opname, op, n, prefixop) \
    I##n __sync_##opname##_and_fetch_##n(I##n *ptr, I##n val) \
    { \
        unsigned int mask = irq_disable();                  \
        I##n tmp = prefixop((*ptr) op val);                 \
        *ptr = tmp;                                         \
        irq_restore(mask);                                  \
        return tmp;                                         \
    }

/* Template instantiations below */
TEMPLATE_SYNC_LOCK_TEST_AND_SET_N(1)        /* __sync_lock_test_and_set_1 */
TEMPLATE_SYNC_LOCK_TEST_AND_SET_N(2)        /* __sync_lock_test_and_set_2 */
TEMPLATE_SYNC_LOCK_TEST_AND_SET_N(4)        /* __sync_lock_test_and_set_4 */
TEMPLATE_SYNC_LOCK_TEST_AND_SET_N(8)        /* __sync_lock_test_and_set_8 */

TEMPLATE_SYNC_LOCK_RELEASE_N(1)             /* __sync_lock_release_1 */
TEMPLATE_SYNC_LOCK_RELEASE_N(2)             /* __sync_lock_release_2 */
TEMPLATE_SYNC_LOCK_RELEASE_N(4)             /* __sync_lock_release_4 */
TEMPLATE_SYNC_LOCK_RELEASE_N(8)             /* __sync_lock_release_8 */

TEMPLATE_SYNC_BOOL_COMPARE_AND_SWAP_N(1)    /* __sync_bool_compare_and_swap_1 */
TEMPLATE_SYNC_BOOL_COMPARE_AND_SWAP_N(2)    /* __sync_bool_compare_and_swap_2 */
TEMPLATE_SYNC_BOOL_COMPARE_AND_SWAP_N(4)    /* __sync_bool_compare_and_swap_4 */
TEMPLATE_SYNC_BOOL_COMPARE_AND_SWAP_N(8)    /* __sync_bool_compare_and_swap_8 */

TEMPLATE_SYNC_VAL_COMPARE_AND_SWAP_N(1)     /* __sync_val_compare_and_swap_1 */
TEMPLATE_SYNC_VAL_COMPARE_AND_SWAP_N(2)     /* __sync_val_compare_and_swap_2 */
TEMPLATE_SYNC_VAL_COMPARE_AND_SWAP_N(4)     /* __sync_val_compare_and_swap_4 */
TEMPLATE_SYNC_VAL_COMPARE_AND_SWAP_N(8)     /* __sync_val_compare_and_swap_8 */

TEMPLATE_SYNC_FETCH_AND_OP_N( add, +, 1,  ) /* __sync_fetch_and_add_1 */
TEMPLATE_SYNC_FETCH_AND_OP_N( add, +, 2,  ) /* __sync_fetch_and_add_2 */
TEMPLATE_SYNC_FETCH_AND_OP_N( add, +, 4,  ) /* __sync_fetch_and_add_4 */
TEMPLATE_SYNC_FETCH_AND_OP_N( add, +, 8,  ) /* __sync_fetch_and_add_8 */

TEMPLATE_SYNC_FETCH_AND_OP_N( sub, -, 1,  ) /* __sync_fetch_and_sub_1 */
TEMPLATE_SYNC_FETCH_AND_OP_N( sub, -, 2,  ) /* __sync_fetch_and_sub_2 */
TEMPLATE_SYNC_FETCH_AND_OP_N( sub, -, 4,  ) /* __sync_fetch_and_sub_4 */
TEMPLATE_SYNC_FETCH_AND_OP_N( sub, -, 8,  ) /* __sync_fetch_and_sub_8 */

TEMPLATE_SYNC_FETCH_AND_OP_N( and, &, 1,  ) /* __sync_fetch_and_and_1 */
TEMPLATE_SYNC_FETCH_AND_OP_N( and, &, 2,  ) /* __sync_fetch_and_and_2 */
TEMPLATE_SYNC_FETCH_AND_OP_N( and, &, 4,  ) /* __sync_fetch_and_and_4 */
TEMPLATE_SYNC_FETCH_AND_OP_N( and, &, 8,  ) /* __sync_fetch_and_and_8 */

TEMPLATE_SYNC_FETCH_AND_OP_N(  or, |, 1,  ) /* __sync_fetch_and_or_1 */
TEMPLATE_SYNC_FETCH_AND_OP_N(  or, |, 2,  ) /* __sync_fetch_and_or_2 */
TEMPLATE_SYNC_FETCH_AND_OP_N(  or, |, 4,  ) /* __sync_fetch_and_or_4 */
TEMPLATE_SYNC_FETCH_AND_OP_N(  or, |, 8,  ) /* __sync_fetch_and_or_8 */

TEMPLATE_SYNC_FETCH_AND_OP_N( xor, ^, 1,  ) /* __sync_fetch_and_xor_1 */
TEMPLATE_SYNC_FETCH_AND_OP_N( xor, ^, 2,  ) /* __sync_fetch_and_xor_2 */
TEMPLATE_SYNC_FETCH_AND_OP_N( xor, ^, 4,  ) /* __sync_fetch_and_xor_4 */
TEMPLATE_SYNC_FETCH_AND_OP_N( xor, ^, 8,  ) /* __sync_fetch_and_xor_8 */

TEMPLATE_SYNC_FETCH_AND_OP_N(nand, &, 1, ~) /* __sync_fetch_and_nand_1 */
TEMPLATE_SYNC_FETCH_AND_OP_N(nand, &, 2, ~) /* __sync_fetch_and_nand_2 */
TEMPLATE_SYNC_FETCH_AND_OP_N(nand, &, 4, ~) /* __sync_fetch_and_nand_4 */
TEMPLATE_SYNC_FETCH_AND_OP_N(nand, &, 8, ~) /* __sync_fetch_and_nand_8 */

TEMPLATE_SYNC_OP_AND_FETCH_N( add, +, 1,  ) /* __sync_add_and_fetch_1 */
TEMPLATE_SYNC_OP_AND_FETCH_N( add, +, 2,  ) /* __sync_add_and_fetch_2 */
TEMPLATE_SYNC_OP_AND_FETCH_N( add, +, 4,  ) /* __sync_add_and_fetch_4 */
TEMPLATE_SYNC_OP_AND_FETCH_N( add, +, 8,  ) /* __sync_add_and_fetch_8 */

TEMPLATE_SYNC_OP_AND_FETCH_N( sub, -, 1,  ) /* __sync_sub_and_fetch_1 */
TEMPLATE_SYNC_OP_AND_FETCH_N( sub, -, 2,  ) /* __sync_sub_and_fetch_2 */
TEMPLATE_SYNC_OP_AND_FETCH_N( sub, -, 4,  ) /* __sync_sub_and_fetch_4 */
TEMPLATE_SYNC_OP_AND_FETCH_N( sub, -, 8,  ) /* __sync_sub_and_fetch_8 */

TEMPLATE_SYNC_OP_AND_FETCH_N( and, &, 1,  ) /* __sync_and_and_fetch_1 */
TEMPLATE_SYNC_OP_AND_FETCH_N( and, &, 2,  ) /* __sync_and_and_fetch_2 */
TEMPLATE_SYNC_OP_AND_FETCH_N( and, &, 4,  ) /* __sync_and_and_fetch_4 */
TEMPLATE_SYNC_OP_AND_FETCH_N( and, &, 8,  ) /* __sync_and_and_fetch_8 */

TEMPLATE_SYNC_OP_AND_FETCH_N(  or, |, 1,  ) /* __sync_or_and_fetch_1 */
TEMPLATE_SYNC_OP_AND_FETCH_N(  or, |, 2,  ) /* __sync_or_and_fetch_2 */
TEMPLATE_SYNC_OP_AND_FETCH_N(  or, |, 4,  ) /* __sync_or_and_fetch_4 */
TEMPLATE_SYNC_OP_AND_FETCH_N(  or, |, 8,  ) /* __sync_or_and_fetch_8 */

TEMPLATE_SYNC_OP_AND_FETCH_N( xor, ^, 1,  ) /* __sync_xor_and_fetch_1 */
TEMPLATE_SYNC_OP_AND_FETCH_N( xor, ^, 2,  ) /* __sync_xor_and_fetch_2 */
TEMPLATE_SYNC_OP_AND_FETCH_N( xor, ^, 4,  ) /* __sync_xor_and_fetch_4 */
TEMPLATE_SYNC_OP_AND_FETCH_N( xor, ^, 8,  ) /* __sync_xor_and_fetch_8 */

TEMPLATE_SYNC_OP_AND_FETCH_N(nand, &, 1, ~) /* __sync_nand_and_fetch_1 */
TEMPLATE_SYNC_OP_AND_FETCH_N(nand, &, 2, ~) /* __sync_nand_and_fetch_2 */
TEMPLATE_SYNC_OP_AND_FETCH_N(nand, &, 4, ~) /* __sync_nand_and_fetch_4 */
TEMPLATE_SYNC_OP_AND_FETCH_N(nand, &, 8, ~) /* __sync_nand_and_fetch_8 */
#endif
/* end{code-style-ignore} */
/** @} */
