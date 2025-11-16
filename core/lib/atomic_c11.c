/*
 * Copyright (C) 2015-2016 Eistec AB
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
 * @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
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
 * uncrustify mis-formats the macros in this file, so disable it for them.
 * begin{code-style-ignore}
 */

/* GCC documentation refers to the types as I1, I2, I4, I8, I16 */
typedef uint8_t  I1;

/* the builtins are declared with "unsigned int", but "uint16_t" is typedef'ed
 * to "short unsigned int" on most platforms where "sizeof(int) == 2." */
#if __SIZEOF_INT__ == 2
typedef unsigned int I2;
#else
typedef uint16_t I2;
#endif

/* the builtins are declared with "unsigned int", but "uint32_t" is typedef'ed
 * to "long unsigned int" on most platforms where "sizeof(int) == 4. */
#if __SIZEOF_INT__ == 4
typedef unsigned int I4;
#else
typedef uint32_t I4;
#endif

typedef uint64_t I8;
/* typedef __uint128_t I16; */ /* No 128 bit integer support yet */

/**
 * @brief This is a macro that defines a function named __atomic_load_<em>n</em>
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_ATOMIC_LOAD_N(n) \
    I##n __atomic_load_##n (const volatile void *ptr, int memorder) \
    { \
        (void) memorder;                           \
        unsigned int mask = irq_disable();         \
        I##n old = *(const volatile I##n *)ptr;    \
        irq_restore(mask);                         \
        return old;                                \
    }

/**
 * @brief This is a macro that defines a function named __atomic_store_<em>n</em>
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_ATOMIC_STORE_N(n) \
    void __atomic_store_##n (volatile void *ptr, I##n val, int memorder) \
    { \
        (void) memorder;                   \
        unsigned int mask = irq_disable(); \
        *(volatile I##n *)ptr = val;       \
        irq_restore(mask);                 \
    }

/**
 * @brief This is a macro that defines a function named __atomic_exchange_<em>n</em>
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_ATOMIC_EXCHANGE_N(n) \
    I##n __atomic_exchange_##n (volatile void *ptr, I##n desired, int memorder) \
    { \
        (void) memorder;                   \
        unsigned int mask = irq_disable(); \
        I##n old = *(volatile I##n *)ptr;  \
        *(volatile I##n *)ptr = desired;   \
        irq_restore(mask);                 \
        return old;                        \
    }

/**
 * @brief This is a macro that defines a function named __atomic_compare_exchange_<em>n</em>
 *
 * @param[in]  n         width of the data, in bytes
 */
#define TEMPLATE_ATOMIC_COMPARE_EXCHANGE_N(n) \
    bool __atomic_compare_exchange_##n (volatile void *ptr, void *expected, I##n desired, \
        bool weak, int success_memorder, int failure_memorder) \
    { \
        (void) weak;                       \
        (void) success_memorder;           \
        (void) failure_memorder;           \
        unsigned int mask = irq_disable(); \
        I##n cur = *(volatile I##n *)ptr;  \
        if (cur != *(I##n *)expected) {    \
            *(I##n *)expected = cur;       \
            irq_restore(mask);             \
            return false;                  \
        }                                  \
                                           \
        *(volatile I##n *)ptr = desired;   \
        irq_restore(mask);                 \
        return true;                       \
    }

/**
 * @brief This is a macro that defines a function named __atomic_fetch_<em>opname</em>_<em>n</em>
 *
 * @param[in]  opname    operator name that will be used in the function name
 * @param[in]  op        actual C language operator
 * @param[in]  n         width of the data, in bytes
 * @param[in]  prefixop  optional prefix unary operator (use ~ for inverting, NAND, NOR etc)
 */
#define TEMPLATE_ATOMIC_FETCH_OP_N(opname, op, n, prefixop) \
    I##n __atomic_fetch_##opname##_##n(volatile void *ptr, I##n val, int memmodel) \
    { \
        unsigned int mask = irq_disable();             \
        (void)memmodel;                                \
        I##n tmp = *(volatile I##n *)ptr;              \
        *(volatile I##n *)ptr = prefixop(tmp op val);  \
        irq_restore(mask);                             \
        return tmp;                                    \
    }

/**
 * @brief This is a macro that defines a function named __atomic_<em>opname</em>_fetch_<em>n</em>
 *
 * @param[in]  opname    operator name that will be used in the function name
 * @param[in]  op        actual C language operator
 * @param[in]  n         width of the data, in bytes
 * @param[in]  prefixop  optional prefix unary operator (use ~ for inverting, NAND, NOR etc)
 */
#define TEMPLATE_ATOMIC_OP_FETCH_N(opname, op, n, prefixop) \
    I##n __atomic_##opname##_fetch_##n(volatile void *ptr, I##n val, int memmodel) \
    { \
        (void)memmodel;                                        \
        unsigned int mask = irq_disable();                     \
        I##n tmp = prefixop((*(volatile I##n *)ptr) op val);   \
        *(volatile I##n *)ptr = tmp;                           \
        irq_restore(mask);                                     \
        return tmp;                                            \
    }

/* Template instantiations below */
TEMPLATE_ATOMIC_LOAD_N(1)                 /* __atomic_load_1 */
TEMPLATE_ATOMIC_LOAD_N(2)                 /* __atomic_load_2 */
TEMPLATE_ATOMIC_LOAD_N(4)                 /* __atomic_load_4 */
TEMPLATE_ATOMIC_LOAD_N(8)                 /* __atomic_load_8 */

TEMPLATE_ATOMIC_STORE_N(1)                /* __atomic_store_1 */
TEMPLATE_ATOMIC_STORE_N(2)                /* __atomic_store_2 */
TEMPLATE_ATOMIC_STORE_N(4)                /* __atomic_store_4 */
TEMPLATE_ATOMIC_STORE_N(8)                /* __atomic_store_8 */

TEMPLATE_ATOMIC_EXCHANGE_N(1)             /* __atomic_exchange_1 */
TEMPLATE_ATOMIC_EXCHANGE_N(2)             /* __atomic_exchange_2 */
TEMPLATE_ATOMIC_EXCHANGE_N(4)             /* __atomic_exchange_4 */
TEMPLATE_ATOMIC_EXCHANGE_N(8)             /* __atomic_exchange_8 */

TEMPLATE_ATOMIC_COMPARE_EXCHANGE_N(1)     /* __atomic_compare_exchange_1 */
TEMPLATE_ATOMIC_COMPARE_EXCHANGE_N(2)     /* __atomic_compare_exchange_2 */
TEMPLATE_ATOMIC_COMPARE_EXCHANGE_N(4)     /* __atomic_compare_exchange_4 */
TEMPLATE_ATOMIC_COMPARE_EXCHANGE_N(8)     /* __atomic_compare_exchange_8 */

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

TEMPLATE_ATOMIC_OP_FETCH_N( add, +, 1,  ) /* __atomic_add_fetch_1 */
TEMPLATE_ATOMIC_OP_FETCH_N( add, +, 2,  ) /* __atomic_add_fetch_2 */
TEMPLATE_ATOMIC_OP_FETCH_N( add, +, 4,  ) /* __atomic_add_fetch_4 */
TEMPLATE_ATOMIC_OP_FETCH_N( add, +, 8,  ) /* __atomic_add_fetch_8 */

TEMPLATE_ATOMIC_OP_FETCH_N( sub, -, 1,  ) /* __atomic_sub_fetch_1 */
TEMPLATE_ATOMIC_OP_FETCH_N( sub, -, 2,  ) /* __atomic_sub_fetch_2 */
TEMPLATE_ATOMIC_OP_FETCH_N( sub, -, 4,  ) /* __atomic_sub_fetch_4 */
TEMPLATE_ATOMIC_OP_FETCH_N( sub, -, 8,  ) /* __atomic_sub_fetch_8 */

TEMPLATE_ATOMIC_OP_FETCH_N( and, &, 1,  ) /* __atomic_and_fetch_1 */
TEMPLATE_ATOMIC_OP_FETCH_N( and, &, 2,  ) /* __atomic_and_fetch_2 */
TEMPLATE_ATOMIC_OP_FETCH_N( and, &, 4,  ) /* __atomic_and_fetch_4 */
TEMPLATE_ATOMIC_OP_FETCH_N( and, &, 8,  ) /* __atomic_and_fetch_8 */

TEMPLATE_ATOMIC_OP_FETCH_N(  or, |, 1,  ) /* __atomic_or_fetch_1 */
TEMPLATE_ATOMIC_OP_FETCH_N(  or, |, 2,  ) /* __atomic_or_fetch_2 */
TEMPLATE_ATOMIC_OP_FETCH_N(  or, |, 4,  ) /* __atomic_or_fetch_4 */
TEMPLATE_ATOMIC_OP_FETCH_N(  or, |, 8,  ) /* __atomic_or_fetch_8 */

TEMPLATE_ATOMIC_OP_FETCH_N( xor, ^, 1,  ) /* __atomic_xor_fetch_1 */
TEMPLATE_ATOMIC_OP_FETCH_N( xor, ^, 2,  ) /* __atomic_xor_fetch_2 */
TEMPLATE_ATOMIC_OP_FETCH_N( xor, ^, 4,  ) /* __atomic_xor_fetch_4 */
TEMPLATE_ATOMIC_OP_FETCH_N( xor, ^, 8,  ) /* __atomic_xor_fetch_8 */

TEMPLATE_ATOMIC_OP_FETCH_N(nand, &, 1, ~) /* __atomic_nand_fetch_1 */
TEMPLATE_ATOMIC_OP_FETCH_N(nand, &, 2, ~) /* __atomic_nand_fetch_2 */
TEMPLATE_ATOMIC_OP_FETCH_N(nand, &, 4, ~) /* __atomic_nand_fetch_4 */
TEMPLATE_ATOMIC_OP_FETCH_N(nand, &, 8, ~) /* __atomic_nand_fetch_8 */

/* end{code-style-ignore} */

/* ***** Generic versions below ***** */

/* Clang objects if you redefine a builtin.  This little hack allows us to
 * define a function with the same name as an intrinsic. */
/* Hack origin: http://llvm.org/svn/llvm-project/compiler-rt/trunk/lib/builtins/atomic.c */
#pragma redefine_extname __atomic_load_c __atomic_load
#pragma redefine_extname __atomic_store_c __atomic_store
#pragma redefine_extname __atomic_exchange_c __atomic_exchange
#pragma redefine_extname __atomic_compare_exchange_c __atomic_compare_exchange
#pragma redefine_extname __atomic_test_and_set_c __atomic_test_and_set

/**
 * @brief Atomic generic load
 *
 * @param[in]  size       width of the data, in bytes
 * @param[in]  src        source address to load from
 * @param[in]  dest       destination address
 * @param[in]  memorder   memory ordering, ignored in this implementation
 */
void __atomic_load_c(size_t size, const void *src, void *dest, int memorder)
{
    (void)memorder;
    unsigned int mask = irq_disable();

    memcpy(dest, src, size);
    irq_restore(mask);
}

/**
 * @brief Atomic generic store
 *
 * @param[in]  size       width of the data, in bytes
 * @param[in]  dest       destination address to store to
 * @param[in]  src        source address
 * @param[in]  memorder   memory ordering, ignored in this implementation
 */
void __atomic_store_c(size_t size, void *dest, const void *src, int memorder)
{
    (void)memorder;
    unsigned int mask = irq_disable();

    memcpy(dest, src, size);
    irq_restore(mask);
}

/**
 * @brief Atomic generic exchange
 *
 * @param[in]  size       width of the data, in bytes
 * @param[in]  ptr        object to swap
 * @param[in]  val        value to swap to
 * @param[in]  ret        put the old value from @p ptr in @p ret
 * @param[in]  memorder   memory ordering, ignored in this implementation
 */
void __atomic_exchange_c(size_t size, void *ptr, void *val, void *ret,
                         int memorder)
{
    (void)memorder;
    unsigned int mask = irq_disable();

    memcpy(ret, ptr, size);
    memcpy(ptr, val, size);
    irq_restore(mask);
}

/**
 * @brief Atomic compare-and-swap operation
 *
 * This built-in function implements an atomic compare and exchange operation.
 * This compares the contents of *ptr with the contents of *expected. If equal,
 * the operation is a read-modify-write operation that writes desired into *ptr.
 * If they are not equal, the operation is a read and the current contents of *ptr
 * are written into *expected. weak is true for weak compare_exchange, which may
 * fail spuriously, and false for the strong variation, which never fails
 * spuriously. Many targets only offer the strong variation and ignore the
 * parameter. When in doubt, use the strong variation.
 *
 * If desired is written into *ptr then true is returned and memory is affected
 * according to the memory order specified by success_memorder. There are no
 * restrictions on what memory order can be used here.
 *
 * Otherwise, false is returned and memory is affected according to
 * failure_memorder. This memory order cannot be __ATOMIC_RELEASE nor
 * __ATOMIC_ACQ_REL. It also cannot be a stronger order than that specified by
 * success_memorder.
 *
 * @param[in]  len                  Length of @p ptr and @p expected in bytes
 * @param[in]  ptr                  The value to check
 * @param[in]  expected             the expected value of ptr
 * @param[in]  desired              the desired value of ptr
 * @param[in]  weak                 ignored in this implementation
 * @param[in]  success_memorder     ignored in this implementation
 * @param[in]  failure_memorder     ignored in this implementation
 *
 * @return true if *ptr had the expected value before the exchange and *ptr was updated
 * @return false otherwise
 */
bool __atomic_compare_exchange_c(size_t len, void *ptr, void *expected,
                                 void *desired, bool weak, int success_memorder,
                                 int failure_memorder)
{
    (void)weak;
    (void)success_memorder;
    (void)failure_memorder;
    unsigned int mask = irq_disable();
    bool ret;

    if (memcmp(ptr, expected, len) == 0) {
        memcpy(ptr, desired, len);
        ret = true;
    }
    else {
        memcpy(expected, ptr, len);
        ret = false;
    }
    irq_restore(mask);
    return ret;
}

/**
 * This built-in function performs an atomic test-and-set operation on the byte
 * at *ptr. The byte is set to some implementation defined nonzero “set” value
 * and the return value is true if and only if the previous contents were “set”.
 * It should be only used for operands of type bool or char. For other types
 * only part of the value may be set.
 *
 * All memory orders are valid.
 *
 * @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html#index-_005f_005fatomic_005ftest_005fand_005fset
 *
 * @param[in,out]   ptr         Value to test and then set
 * @param[in]       memorder    Ignored
 *
 * @retval  true    The value @p ptr was already set
 * @retval  false   The value @p ptr was previously unset
 *
 * @pre     @p ptr points to a value of `bool`
 */
bool __atomic_test_and_set_c(void *ptr, int memorder)
{
    (void)memorder;
    bool *target = ptr;
    unsigned mask = irq_disable();
    bool retval = *target;
    *target = true;
    irq_restore(mask);
    return retval;
}
#if !defined(__llvm__) && !defined(__clang__)
/* Memory barrier helper function, for platforms without barrier instructions */
void __sync_synchronize(void) __attribute__((__weak__));
void __sync_synchronize(void)
{
    /* ARMv4, ARMv5 do not have any hardware support for memory barriers,
     * This is a software only barrier and a no-op, and will likely break on SMP
     * systems, but we don't support any multi-CPU ARMv5 or ARMv4 boards in RIOT
     * so I don't care. /JN
     */
    __asm__ volatile ("" : : : "memory");
}
#endif
/** @} */
