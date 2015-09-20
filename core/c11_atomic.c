/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         core_util
 * @{
 *
 * @file
 *
 * @brief Implementation of C11 atomic operations if GCC does not provide an
 * implementation.
 *
 * GCC with -mcpu=cortex-m3 and cortex-m4 generate LDREX/STREX instructions
 * instead of library calls. There is however currently (2015-05-29) no
 * implementation for Cortex-M0, since it lacks the lock-free atomic operations
 * found in the M3 and M4 cores.
 *
 * @note Other CPUs (e.g. msp430, avr) might need this too, but current MSP430
 * GCC in Ubuntu/Debian is stuck at version 4.6 which does not provide C11
 * language support which makes it difficult to actually make use of this on
 * that platform.
 *
 * @note This implementation completely ignores the memory model parameter
 *
 * @see https://gcc.gnu.org/wiki/Atomic/GCCMM/LIbrary
 *
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include "irq.h"

/* GCC documentation refers to the types as I1, I2, I4, I8, I16 */
typedef uint8_t  I1;
typedef uint16_t I2;
typedef uint32_t I4;
typedef uint64_t I8;
/* typedef __uint128_t I16; */ /* No 128 bit integer support yet */

/**
 * @brief This is a macro that defines a function named __atomic_fetch_<em>opname</em>_<em>n</em>
 *
 * \param opname    operator name that will be used in the function name
 * \param op        actual C language operator
 * \param n         width of the data, in bytes
 * \param prefixop  optional prefix unary operator (use ~ for inverting, NAND, NOR etc)
 */
#define TEMPLATE_ATOMIC_FETCH_OP_N(opname, op, n, prefixop) \
    I##n __atomic_fetch_##opname##_##n (volatile void *ptr, I##n val, int memmodel) \
    { \
        unsigned int mask = disableIRQ();     \
        (void)memmodel;                       \
        I##n tmp = *((I##n*)ptr);             \
        *((I##n*)ptr) = prefixop(tmp op val); \
        restoreIRQ(mask);                     \
        return tmp;                           \
    }

/* Template instantiations below */
TEMPLATE_ATOMIC_FETCH_OP_N( add, +, 1,  ) /* __atomic_fetch_add_1 */
TEMPLATE_ATOMIC_FETCH_OP_N( add, +, 2,  ) /* __atomic_fetch_add_2 */
TEMPLATE_ATOMIC_FETCH_OP_N( add, +, 4,  ) /* __atomic_fetch_add_4 */
TEMPLATE_ATOMIC_FETCH_OP_N( add, +, 8,  ) /* __atomic_fetch_add_8 */

TEMPLATE_ATOMIC_FETCH_OP_N( sub, -, 1,  ) /* __atomic_fetch_sub_1 */
TEMPLATE_ATOMIC_FETCH_OP_N( sub, -, 2,  ) /* __atomic_fetch_sub_2 */
TEMPLATE_ATOMIC_FETCH_OP_N( sub, -, 4,  ) /* __atomic_fetch_sub_4 */
TEMPLATE_ATOMIC_FETCH_OP_N( sub, -, 8,  ) /* __atomic_fetch_sub_8 */

TEMPLATE_ATOMIC_FETCH_OP_N( and, &, 1,  ) /* __atomic_fetch_and_1 */
TEMPLATE_ATOMIC_FETCH_OP_N( and, &, 2,  ) /* __atomic_fetch_and_2 */
TEMPLATE_ATOMIC_FETCH_OP_N( and, &, 4,  ) /* __atomic_fetch_and_4 */
TEMPLATE_ATOMIC_FETCH_OP_N( and, &, 8,  ) /* __atomic_fetch_and_8 */

TEMPLATE_ATOMIC_FETCH_OP_N(  or, |, 1,  ) /* __atomic_fetch_or_1 */
TEMPLATE_ATOMIC_FETCH_OP_N(  or, |, 2,  ) /* __atomic_fetch_or_2 */
TEMPLATE_ATOMIC_FETCH_OP_N(  or, |, 4,  ) /* __atomic_fetch_or_4 */
TEMPLATE_ATOMIC_FETCH_OP_N(  or, |, 8,  ) /* __atomic_fetch_or_8 */

TEMPLATE_ATOMIC_FETCH_OP_N( xor, ^, 1,  ) /* __atomic_fetch_xor_1 */
TEMPLATE_ATOMIC_FETCH_OP_N( xor, ^, 2,  ) /* __atomic_fetch_xor_2 */
TEMPLATE_ATOMIC_FETCH_OP_N( xor, ^, 4,  ) /* __atomic_fetch_xor_4 */
TEMPLATE_ATOMIC_FETCH_OP_N( xor, ^, 8,  ) /* __atomic_fetch_xor_8 */

TEMPLATE_ATOMIC_FETCH_OP_N(nand, &, 1, ~) /* __atomic_fetch_nand_1 */
TEMPLATE_ATOMIC_FETCH_OP_N(nand, &, 2, ~) /* __atomic_fetch_nand_2 */
TEMPLATE_ATOMIC_FETCH_OP_N(nand, &, 4, ~) /* __atomic_fetch_nand_4 */
TEMPLATE_ATOMIC_FETCH_OP_N(nand, &, 8, ~) /* __atomic_fetch_nand_8 */

/** @} */
