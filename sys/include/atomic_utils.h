/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_atomic_utils    Utility functions for atomic access
 * @ingroup     sys
 *
 * This modules adds some utility functions to perform atomic accesses.
 *
 * # Usage
 *
 * The atomic utilitys allow atomic access to regular variables.
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *  uint32_t global_counter;
 *
 *  void irq_handler(void)
 *  {
 *      // No need to use atomic access in IRQ handlers, if other IRQ handlers
 *      // never touch global_counter: At the beginning and at the end of every
 *      // ISR a memory barrier is in place, so that at the end of the ISR the
 *      // memory will be in a state as if all memory accesses within the ISR
 *      // took place in sequential order.
 *      //
 *      // Extra detail only RIOT kernel hackers need to know: If all ISRs
 *      // accessing the same variable cannot interrupt each other, atomic
 *      // access is still not needed. (Currently only PendSV on ARM can be
 *      // interrupted by other IRQs with RIOTs default IRQ priorities. If
 *      // application developers modifies those, they can be assumed to know
 *      // what they are doing - or to happily face the consequences otherwise.)
 *      global_counter++;
 *  }
 *
 *  void called_by_thread_a(void) {
 *      if (atomic_load_u32(&global_counter) > THRESHOLD) {
 *          on_threshold_reached();
 *          atomic_store_u32(&global_counter, 0);
 *      }
 *  }
 *
 *  void called_by_thread_b(void) {
 *      atomic_add_u32(&global_counter, 42);
 *  }
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * # Motivation
 * There are some reasons why these functions might be chosen over the
 * [C11 Atomic Operations Library](https://en.cppreference.com/w/c/atomic) in
 * some advanced use cases:
 *
 * - The functions allow mixing of atomic and non-atomic accesses. E.g. while
 *   IRQs are disabled anyway, even plain accesses cannot be interrupted but
 *   are often more efficient.
 * - On platforms not supporting lock-free access, a library call is generated
 *   instead. The fallback implementation used here is more efficient in terms
 *   of both CPU instructions and ROM size.
 * - On platforms where some operations can be implemented lock free while
 *   others can't, at least LLVM will use the library call even for those
 *   accesses that can be implemented lock-free. This is because without
 *   assuming how the library call implements atomic access for the other
 *   functions, mixing library calls and lock free accesses could result in data
 *   corruption. But this implementation resorts to disabling IRQs when
 *   lock-free implementations are not possible, which mixes well with lock-free
 *   accesses. Thus, additional overhead for atomic accesses is only spent where
 *   needed.
 * - In some cases the fallback implementation performs better than the lock
 *   free implementation. E.g. if a specific platform has an atomic compare and
 *   swap instruction, this could be used to perform a read-modify-write in a
 *   loop until the value initially read was not changed in between. Just
 *   disabling IRQs to implement an atomic read-modify-write operation is likely
 *   more efficient. C11 atomics will however always use the lock free
 *   implementation (if such exists), assuming that this is more efficient.
 *   This assumption was made with desktop class hardware in mind, but is not
 *   generally true for bare metal targets. These function allow to optimize
 *   for the actual hardware RIOT is running on.
 * - This library provides "semi-atomic" read-modify-write operations, which are
 *   useful when at most one thread is ever writing to memory. In that case,
 *   only the write part of the read-modify-write operation needs to be
 *   performed in an atomic fashion in order for the reading threads to perceive
 *   atomic updates of the variable. This is significantly cheaper than atomic
 *   read-modify-write operations for many platforms
 *
 * # Guarantees
 *
 * - Every utility function here acts as a barrier for code reordering regarding
 * - For the `atomic_*()` family of functions: The whole operation will be done
 *   in an non-interruptible fashion
 * - For the `semi_atomic_*()` family of functions: The write part of the
 *   operation is done atomically. If at most one thread is ever performing
 *   changes to a variable using the `semi_atomic_()` functions, those changes
 *   will appear as if they were atomic to all other threads.
 *
 * # Porting to new CPUs
 *
 * At the bare minimum, create an empty `atomic_utils_arch.h` file. This will
 * result in the fallback implementations being used.
 *
 * To expose lock-free atomic operations, add an implementation to the
 * `atomic_utils_arch.h` file and disable the fallback implementation by
 * defining `HAS_<FN_NAME_ALL_CAPS>`, where `<FN_NAME_ALL_CAPS>` is the name
 * of the function provided in all upper case. E.g. most platforms will be able
 * to provide lock-free reads and writes up to their word size and can expose
 * this as follows for GCC:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *  // All the user header boilerplate
 *  #define HAS_ATOMIC_LOAD_U8
 *  static inline uint8_t atomic_load_u8(const uint8_t *var)
 *  {
 *      return __atomic_load_1(var, __ATOMIC_SEQ_CST);
 *  }
 *
 *  #define HAS_ATOMIC_STORE_U8
 *  static inline void atomic_store_u8(uint8_t *dest, uint8_t val)
 *  {
 *      __atomic_store_1(dest, val, __ATOMIC_SEQ_CST);
 *  }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: The `semi_atomic_*()` family of functions is always provided using
 * `atomic_*()` functions in the cheapest way possible.
 *
 * @{
 *
 * @file
 * @brief       API of the utility functions for atomic accesses
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ATOMIC_UTILS_H
#define ATOMIC_UTILS_H

#include <stdint.h>

#include "irq.h"
#include "macros/utils.h"
#include "sched.h"

#include "atomic_utils_arch.h" /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

/* Declarations and documentation: */

#if !defined(HAS_ATOMIC_BIT) || defined(DOXYGEN)
/**
 * @name    Types used to specify atomic bit access operations
 *
 * @warning These types are implementation dependent to allow exploiting
 *          hardware specific features like bit-banding. Use the provided helper
 *          functions to get or set the bit or destination they refer to.
 *
 * The motivation of a dedicated type is to allow ahead of time computation of
 * e.g. bit-banding addresses, so that actually access can be done in a single
 * CPU cycles even if the bit reference was initialized in a separate
 * compilation unit and no link time optimization is used.
 *
 * @{
 */
/**
 * @brief   Type specifying a bit in an `uint8_t`
 *
 * @warning This is an implementation specific type!
 */
typedef struct {
    volatile uint8_t *dest;     /**< Memory containing the bit to set/clear */
    uint8_t mask;               /**< Bitmask used for setting the bit */
} atomic_bit_u8_t;

/**
 * @brief   Type specifying a bit in an `uint16_t`
 *
 * @warning This is an implementation specific type!
 */
typedef struct {
    volatile uint16_t *dest;    /**< Memory containing the bit to set/clear */
    uint16_t mask;              /**< Bitmask used for setting the bit */
} atomic_bit_u16_t;

/**
 * @brief   Type specifying a bit in an `uint32_t`
 *
 * @warning This is an implementation specific type!
 */
typedef struct {
    volatile uint32_t *dest;    /**< Memory containing the bit to set/clear */
    uint32_t mask;              /**< Bitmask used for setting the bit */
} atomic_bit_u32_t;

/**
 * @brief   Type specifying a bit in an `uint64_t`
 *
 * @warning This is an implementation specific type!
 */
typedef struct {
    volatile uint64_t *dest;    /**< Memory containing the bit to set/clear */
    uint64_t mask;              /**< Bitmask used for setting the bit */
} atomic_bit_u64_t;
/** @} */
#endif /* HAS_ATOMIC_BIT */

/**
 * @name    Atomic Loads
 * @{
 */
/**
 * @brief   Load an `uint8_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint8_t atomic_load_u8(const volatile uint8_t *var);
/**
 * @brief   Load an `uint16_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint16_t atomic_load_u16(const volatile uint16_t *var);
/**
 * @brief   Load an `uint32_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint32_t atomic_load_u32(const volatile uint32_t *var);
/**
 * @brief   Load an `uint64_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint64_t atomic_load_u64(const volatile uint64_t *var);

/**
 * @brief   Load an `uintptr_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uintptr_t atomic_load_uintptr(const volatile uintptr_t *var) {
    if (sizeof(uintptr_t) == 2) {
        return atomic_load_u16((const volatile uint16_t *)var);
    }
    else if (sizeof(uintptr_t) == 4) {
        return atomic_load_u32((const volatile uint32_t *)(uintptr_t)var);
    }
    else {
        return atomic_load_u64((const volatile uint64_t *)(uintptr_t)var);
    }
}
/**
 * @brief   Load an `void *` atomically
 *
 * @param[in]       ptr_addr    Address to the pointer to load
 * @return  Value of the loaded pointer
 */
static inline void * atomic_load_ptr(void **ptr_addr) {
    return (void *)atomic_load_uintptr((const volatile uintptr_t *)ptr_addr);
}
/**
 * @brief   Load an `kernel_pid_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline kernel_pid_t atomic_load_kernel_pid(const volatile kernel_pid_t *var)
{
    return atomic_load_u16((const volatile uint16_t *)var);
}
/** @} */

/**
 * @name    Atomic Stores
 * @{
 */
/**
 * @brief  Store an `uint8_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u8(volatile uint8_t *dest, uint8_t val);
/**
 * @brief  Store an `uint16_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u16(volatile uint16_t *dest, uint16_t val);
/**
 * @brief  Store an `uint32_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u32(volatile uint32_t *dest, uint32_t val);
/**
 * @brief  Store an `uint64_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u64(volatile uint64_t *dest, uint64_t val);

/**
 * @brief   Store an `uintptr_t` atomically
 *
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_uintptr(volatile uintptr_t *dest, uintptr_t val)
{
    if (sizeof(uintptr_t) == 2) {
        atomic_store_u16((volatile uint16_t *)dest, (uint16_t)val);
    }
    else if (sizeof(uintptr_t) == 4) {
        atomic_store_u32((volatile uint32_t *)(uintptr_t)dest, (uint32_t)val);
    }
    else {
        atomic_store_u64((volatile uint64_t *)(uintptr_t)dest, (uint64_t)val);
    }
}
/**
 * @brief   Store an `void *` atomically
 *
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_ptr(void **dest, const void *val) {
    atomic_store_uintptr((volatile uintptr_t *)dest, (uintptr_t)val);
}
/**
 * @brief   Store an `kernel_pid_t` atomically
 *
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_kernel_pid(volatile kernel_pid_t *dest,
                                           kernel_pid_t val)
{
    atomic_store_u16((volatile uint16_t *)dest, (uint16_t)val);
}
/** @} */

/**
 * @name    Atomic In-Place Addition
 * @{
 */
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint8_t atomic_fetch_add_u8(volatile uint8_t *dest,
                                          uint8_t summand);
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint16_t atomic_fetch_add_u16(volatile uint16_t *dest,
                                            uint16_t summand);
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint32_t atomic_fetch_add_u32(volatile uint32_t *dest,
                                            uint32_t summand);
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint64_t atomic_fetch_add_u64(volatile uint64_t *dest,
                                            uint64_t summand);
/** @} */

/**
 * @name    Atomic In-Place Subtraction
 * @{
 */
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint8_t atomic_fetch_sub_u8(volatile uint8_t *dest,
                                          uint8_t subtrahend);
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint16_t atomic_fetch_sub_u16(volatile uint16_t *dest,
                                            uint16_t subtrahend);
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint32_t atomic_fetch_sub_u32(volatile uint32_t *dest,
                                            uint32_t subtrahend);
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint64_t atomic_fetch_sub_u64(volatile uint64_t *dest,
                                            uint64_t subtrahend);
/** @} */

/**
 * @name    Atomic In-Place Bitwise OR
 * @{
 */
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint8_t atomic_fetch_or_u8(volatile uint8_t *dest, uint8_t val);
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint16_t atomic_fetch_or_u16(volatile uint16_t *dest,
                                           uint16_t val);
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint32_t atomic_fetch_or_u32(volatile uint32_t *dest,
                                           uint32_t val);
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint64_t atomic_fetch_or_u64(volatile uint64_t *dest,
                                           uint64_t val);
/** @} */

/**
 * @name    Atomic In-Place Bitwise XOR
 * @{
 */
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint8_t atomic_fetch_xor_u8(volatile uint8_t *dest, uint8_t val);
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint16_t atomic_fetch_xor_u16(volatile uint16_t *dest,
                                            uint16_t val);
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint32_t atomic_fetch_xor_u32(volatile uint32_t *dest,
                                            uint32_t val);
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint64_t atomic_fetch_xor_u64(volatile uint64_t *dest,
                                            uint64_t val);
/** @} */

/**
 * @name    Atomic In-Place Bitwise AND
 * @{
 */
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint8_t atomic_fetch_and_u8(volatile uint8_t *dest, uint8_t val);
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint16_t atomic_fetch_and_u16(volatile uint16_t *dest,
                                            uint16_t val);
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint32_t atomic_fetch_and_u32(volatile uint32_t *dest,
                                            uint32_t val);
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint64_t atomic_fetch_and_u64(volatile uint64_t *dest,
                                            uint64_t val);
/** @} */

/**
 * @name    Helper Functions to Handle Atomic Bit References
 * @{
 */
/**
 * @brief   Create a reference to a bit in an `uint8_t`
 * @param[in]       dest        Memory containing the bit
 * @param[in]       bit         Bit number (`0` refers to the least significant)
 */
static inline atomic_bit_u8_t atomic_bit_u8(volatile uint8_t *dest,
                                            uint8_t bit);

/**
 * @brief   Create a reference to a bit in an `uint16_t`
 * @param[in]       dest        Memory containing the bit
 * @param[in]       bit         Bit number (`0` refers to the least significant)
 */
static inline atomic_bit_u16_t atomic_bit_u16(volatile uint16_t *dest,
                                              uint8_t bit);

/**
 * @brief   Create a reference to a bit in an `uint32_t`
 * @param[in]       dest        Memory containing the bit
 * @param[in]       bit         Bit number (`0` refers to the least significant)
 */
static inline atomic_bit_u32_t atomic_bit_u32(volatile uint32_t *dest,
                                              uint8_t bit);

/**
 * @brief   Create a reference to a bit in an `uint64_t`
 * @param[in]       dest        Memory containing the bit
 * @param[in]       bit         Bit number (`0` refers to the least significant)
 */
static inline atomic_bit_u64_t atomic_bit_u64(volatile uint64_t *dest,
                                              uint8_t bit);
/** @} */

/**
 * @name    Atomic Bit Setting
 * @{
 */
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_set_bit_u8(atomic_bit_u8_t bit);
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_set_bit_u16(atomic_bit_u16_t bit);
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_set_bit_u32(atomic_bit_u32_t bit);
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_set_bit_u64(atomic_bit_u64_t bit);
/** @} */

/**
 * @name    Atomic Bit Clearing
 * @{
 */
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_clear_bit_u8(atomic_bit_u8_t bit);
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_clear_bit_u16(atomic_bit_u16_t bit);
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_clear_bit_u32(atomic_bit_u32_t bit);
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   bit         bit to set
 */
static inline void atomic_clear_bit_u64(atomic_bit_u64_t bit);
/** @} */

/**
 * @name    Semi-Atomic In-Place Addition
 * @{
 */
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value semi-atomically
 *                          in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint8_t semi_atomic_fetch_add_u8(volatile uint8_t *dest,
                                               uint8_t summand);
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value semi-atomically
 *                          in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint16_t semi_atomic_fetch_add_u16(volatile uint16_t *dest,
                                                 uint16_t summand);
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value semi-atomically
 *                          in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint32_t semi_atomic_fetch_add_u32(volatile uint32_t *dest,
                                                 uint32_t summand);
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value semi-atomically
 *                          in-place
 * @param[in]       summand Value to add onto @p dest
 * @return  The value previously stored @p dest
 */
static inline uint64_t semi_atomic_fetch_add_u64(volatile uint64_t *dest,
                                                 uint64_t summand);
/** @} */

/**
 * @name    Semi-Atomic In-Place Subtraction
 * @{
 */
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              semi-atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint8_t semi_atomic_fetch_sub_u8(volatile uint8_t *dest,
                                               uint8_t subtrahend);
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              semi-atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint16_t semi_atomic_fetch_sub_u16(volatile uint16_t *dest,
                                                 uint16_t subtrahend);
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              semi-atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint32_t semi_atomic_fetch_sub_u32(volatile uint32_t *dest,
                                                 uint32_t subtrahend);
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value
 *                              semi-atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 * @return  The value previously stored @p dest
 */
static inline uint64_t semi_atomic_fetch_sub_u64(volatile uint64_t *dest,
                                                 uint64_t subtrahend);
/** @} */

/**
 * @name    Semi-atomic In-Place Bitwise OR
 * @{
 */
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint8_t semi_atomic_fetch_or_u8(volatile uint8_t *dest, uint8_t val);
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint16_t semi_atomic_fetch_or_u16(volatile uint16_t *dest,
                                                uint16_t val);
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint32_t semi_atomic_fetch_or_u32(volatile uint32_t *dest,
                                                uint32_t val);
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint64_t semi_atomic_fetch_or_u64(volatile uint64_t *dest,
                                                uint64_t val);
/** @} */

/**
 * @name    Semi-Atomic In-Place Bitwise XOR
 * @{
 */
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint8_t semi_atomic_fetch_xor_u8(volatile uint8_t *dest,
                                               uint8_t val);
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint16_t semi_atomic_fetch_xor_u16(volatile uint16_t *dest,
                                                 uint16_t val);
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint32_t semi_atomic_fetch_xor_u32(volatile uint32_t *dest,
                                                 uint32_t val);
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest ^ val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint64_t semi_atomic_fetch_xor_u64(volatile uint64_t *dest,
                                                 uint64_t val);
/** @} */

/**
 * @name    Semi-Atomic In-Place Bitwise AND
 * @{
 */
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint8_t semi_atomic_fetch_and_u8(volatile uint8_t *dest,
                                               uint8_t val);
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint16_t semi_atomic_fetch_and_u16(volatile uint16_t *dest,
                                                 uint16_t val);
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint32_t semi_atomic_fetch_and_u32(volatile uint32_t *dest,
                                                 uint32_t val);
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of
 *                              `*dest & val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 * @return  The value previously stored @p dest
 */
static inline uint64_t semi_atomic_fetch_and_u64(volatile uint64_t *dest,
                                                 uint64_t val);
/** @} */

/* Fallback implementations of atomic utility functions: */

/**
 * @brief   Generates a static inline function implementing
 *          `atomic_load_u<width>()`
 *
 * @param   name    Name of the variable type, e.g. "u8"
 * @param   type    Variable type, e.g. `uint8_t`
 */
#define ATOMIC_LOAD_IMPL(name, type)                                           \
    static inline type CONCAT(atomic_load_, name)(const volatile type *var)    \
    {                                                                          \
        unsigned state = irq_disable();                                        \
        type result = *var;                                                    \
        irq_restore(state);                                                    \
        return result;                                                         \
    }

#ifndef HAS_ATOMIC_LOAD_U8
ATOMIC_LOAD_IMPL(u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_LOAD_U16
ATOMIC_LOAD_IMPL(u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_LOAD_U32
ATOMIC_LOAD_IMPL(u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_LOAD_U64
ATOMIC_LOAD_IMPL(u64, uint64_t)
#endif

/**
 * @brief   Generates a static inline function implementing
 *          `atomic_store_u<width>()`
 *
 * @param   name    Name of the variable type, e.g. "u8"
 * @param   type    Variable type, e.g. `uint8_t`
 */
#define ATOMIC_STORE_IMPL(name, type)                                          \
    static inline void CONCAT(atomic_store_, name)                             \
            (volatile type *dest, type val)                                    \
    {                                                                          \
        unsigned state = irq_disable();                                        \
        *dest = val;                                                           \
        irq_restore(state);                                                    \
    }

#ifndef HAS_ATOMIC_STORE_U8
ATOMIC_STORE_IMPL(u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_STORE_U16
ATOMIC_STORE_IMPL(u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_STORE_U32
ATOMIC_STORE_IMPL(u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_STORE_U64
ATOMIC_STORE_IMPL(u64, uint64_t)
#endif

/**
 * @brief   Generates a static inline function implementing
 *          `atomic_fecth_<op>_u<width>()`
 *
 * @param   opname  Name of the operator in @p op, e.g. "and" for `+`
 * @param   op      Operator to implement atomically, e.g. `+`
 * @param   name    Name of the variable type, e.g. "u8"
 * @param   type    Variable type, e.g. `uint8_t`
 */
#define ATOMIC_FETCH_OP_IMPL(opname, op, name, type)                           \
    static inline type CONCAT4(atomic_fetch_, opname, _, name)                 \
            (volatile type *dest, type val)                                    \
    {                                                                          \
        unsigned state = irq_disable();                                        \
        const type result = *dest;                                             \
        *dest = result op val;                                                 \
        irq_restore(state);                                                    \
        return result;                                                         \
    }

#ifndef HAS_ATOMIC_FETCH_ADD_U8
ATOMIC_FETCH_OP_IMPL(add, +, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_ADD_U16
ATOMIC_FETCH_OP_IMPL(add, +, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_ADD_U32
ATOMIC_FETCH_OP_IMPL(add, +, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_ADD_U64
ATOMIC_FETCH_OP_IMPL(add, +, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_SUB_U8
ATOMIC_FETCH_OP_IMPL(sub, -, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_SUB_U16
ATOMIC_FETCH_OP_IMPL(sub, -, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_SUB_U32
ATOMIC_FETCH_OP_IMPL(sub, -, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_SUB_U64
ATOMIC_FETCH_OP_IMPL(sub, -, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_OR_U8
ATOMIC_FETCH_OP_IMPL(or, |, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_OR_U16
ATOMIC_FETCH_OP_IMPL(or, |, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_OR_U32
ATOMIC_FETCH_OP_IMPL(or, |, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_OR_U64
ATOMIC_FETCH_OP_IMPL(or, |, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_XOR_U8
ATOMIC_FETCH_OP_IMPL(xor, ^, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_XOR_U16
ATOMIC_FETCH_OP_IMPL(xor, ^, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_XOR_U32
ATOMIC_FETCH_OP_IMPL(xor, ^, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_XOR_U64
ATOMIC_FETCH_OP_IMPL(xor, ^, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_AND_U8
ATOMIC_FETCH_OP_IMPL(and, &, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_AND_U16
ATOMIC_FETCH_OP_IMPL(and, &, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_AND_U32
ATOMIC_FETCH_OP_IMPL(and, &, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_AND_U64
ATOMIC_FETCH_OP_IMPL(and, &, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_BIT
static inline atomic_bit_u8_t atomic_bit_u8(volatile uint8_t *dest,
                                            uint8_t bit)
{
    atomic_bit_u8_t result = { .dest = dest, .mask = 1U << bit };
    return result;
}
static inline atomic_bit_u16_t atomic_bit_u16(volatile uint16_t *dest,
                                              uint8_t bit)
{
    atomic_bit_u16_t result = { .dest = dest, .mask = 1U << bit };
    return result;
}
static inline atomic_bit_u32_t atomic_bit_u32(volatile uint32_t *dest,
                                              uint8_t bit)
{
    atomic_bit_u32_t result = { .dest = dest, .mask = 1UL << bit };
    return result;
}
static inline atomic_bit_u64_t atomic_bit_u64(volatile uint64_t *dest,
                                              uint8_t bit)
{
    atomic_bit_u64_t result = { .dest = dest, .mask = 1ULL << bit };
    return result;
}
static inline void atomic_set_bit_u8(atomic_bit_u8_t bit)
{
    atomic_fetch_or_u8(bit.dest, bit.mask);
}
static inline void atomic_set_bit_u16(atomic_bit_u16_t bit)
{
    atomic_fetch_or_u16(bit.dest, bit.mask);
}
static inline void atomic_set_bit_u32(atomic_bit_u32_t bit)
{
    atomic_fetch_or_u32(bit.dest, bit.mask);
}
static inline void atomic_set_bit_u64(atomic_bit_u64_t bit)
{
    atomic_fetch_or_u64(bit.dest, bit.mask);
}
static inline void atomic_clear_bit_u8(atomic_bit_u8_t bit)
{
    atomic_fetch_and_u8(bit.dest, ~bit.mask);
}
static inline void atomic_clear_bit_u16(atomic_bit_u16_t bit)
{
    atomic_fetch_and_u16(bit.dest, ~bit.mask);
}
static inline void atomic_clear_bit_u32(atomic_bit_u32_t bit)
{
    atomic_fetch_and_u32(bit.dest, ~bit.mask);
}
static inline void atomic_clear_bit_u64(atomic_bit_u64_t bit)
{
    atomic_fetch_and_u64(bit.dest, ~bit.mask);
}
#endif

/* Provide semi_atomic_*() functions on top.
 *
 * - If atomic_<FOO>() is provided: Use this for semi_atomic_<FOO>() as well
 * - Else:
 *      - If matching `atomic_store_u<BITS>()` is provided: Only make final
 *        store atomic, as we can avoid touching the IRQ state register that
 *        way
 *      - Else: We need to disable and re-enable IRQs anyway, we just use the
 *        fallback implementation of `atomic_<FOO>()` for `semi_atomic<FOO>()`
 *        as well
 */

/* FETCH_ADD */
#if defined(HAS_ATOMIC_FETCH_ADD_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline uint8_t semi_atomic_fetch_add_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    return atomic_fetch_add_u8(dest, val);
}
#else
static inline uint8_t semi_atomic_fetch_add_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    uint8_t result = atomic_load_u8(dest);
    atomic_store_u8(dest, result + val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_ADD_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_ADD_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline uint16_t semi_atomic_fetch_add_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    return atomic_fetch_add_u16(dest, val);
}
#else
static inline uint16_t semi_atomic_fetch_add_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    uint16_t result = atomic_load_u16(dest);
    atomic_store_u16(dest, result + val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_ADD_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_ADD_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline uint32_t semi_atomic_fetch_add_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    return atomic_fetch_add_u32(dest, val);
}
#else
static inline uint32_t semi_atomic_fetch_add_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    uint32_t result = atomic_load_u32(dest);
    atomic_store_u32(dest, result + val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_ADD_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_ADD_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline uint64_t semi_atomic_fetch_add_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    return atomic_fetch_add_u64(dest, val);
}
#else
static inline uint64_t semi_atomic_fetch_add_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    atomic_store_u64(dest, *dest + val);
}
#endif /* HAS_ATOMIC_FETCH_ADD_U32 || !HAS_ATOMIC_STORE_U32 */

/* FETCH_SUB */
#if defined(HAS_ATOMIC_FETCH_SUB_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline uint8_t semi_atomic_fetch_sub_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    return atomic_fetch_sub_u8(dest, val);
}
#else
static inline uint8_t semi_atomic_fetch_sub_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    uint8_t result = atomic_load_u8(dest);
    atomic_store_u8(dest, result - val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_SUB_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_SUB_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline uint16_t semi_atomic_fetch_sub_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    return atomic_fetch_sub_u16(dest, val);
}
#else
static inline uint16_t semi_atomic_fetch_sub_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    uint16_t result = atomic_load_u16(dest);
    atomic_store_u16(dest, result - val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_SUB_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_SUB_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline uint32_t semi_atomic_fetch_sub_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    return atomic_fetch_sub_u32(dest, val);
}
#else
static inline uint32_t semi_atomic_fetch_sub_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    uint32_t result = atomic_load_u32(dest);
    atomic_store_u32(dest, result - val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_SUB_U32 || !HAS_ATOMIC_STORE_U64 */

#if defined(HAS_ATOMIC_FETCH_SUB_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline uint64_t semi_atomic_fetch_sub_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    return atomic_fetch_sub_u64(dest, val);
}
#else
static inline uint64_t semi_atomic_fetch_sub_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    uint64_t result = atomic_load_u64(dest);
    atomic_store_u64(dest, result - val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_SUB_U64 || !HAS_ATOMIC_STORE_U64 */

/* FETCH_OR */
#if defined(HAS_ATOMIC_FETCH_OR_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline uint8_t semi_atomic_fetch_or_u8(volatile uint8_t *dest,
                                              uint8_t val)
{
    return atomic_fetch_or_u8(dest, val);
}
#else
static inline uint8_t semi_atomic_fetch_or_u8(volatile uint8_t *dest,
                                              uint8_t val)
{
    uint8_t result = atomic_load_u8(dest);
    atomic_store_u8(dest, result | val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_OR_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_OR_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline uint16_t semi_atomic_fetch_or_u16(volatile uint16_t *dest,
                                                uint16_t val)
{
    return atomic_fetch_or_u16(dest, val);
}
#else
static inline uint16_t semi_atomic_fetch_or_u16(volatile uint16_t *dest,
                                                uint16_t val)
{
    uint16_t result = atomic_load_u16(dest);
    atomic_store_u16(dest, result | val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_OR_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_OR_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline uint32_t semi_atomic_fetch_or_u32(volatile uint32_t *dest,
                                                uint32_t val)
{
    return atomic_fetch_or_u32(dest, val);
}
#else
static inline uint32_t semi_atomic_fetch_or_u32(volatile uint32_t *dest,
                                                uint32_t val)
{
    uint32_t result = atomic_load_u32(dest);
    atomic_store_u32(dest, result | val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_OR_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_OR_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline uint64_t semi_atomic_fetch_or_u64(volatile uint64_t *dest,
                                                uint64_t val)
{
    return atomic_fetch_or_u64(dest, val);
}
#else
static inline uint64_t semi_atomic_fetch_or_u64(volatile uint64_t *dest,
                                                uint64_t val)
{
    uint64_t result = atomic_load_u64(dest);
    atomic_store_u64(dest, result | val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_OR_U64 || !HAS_ATOMIC_STORE_U64 */

/* FETCH_XOR */
#if defined(HAS_ATOMIC_FETCH_XOR_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline uint8_t semi_atomic_fetch_xor_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    return atomic_fetch_xor_u8(dest, val);
}
#else
static inline uint8_t semi_atomic_fetch_xor_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    uint8_t result = atomic_load_u8(dest);
    atomic_store_u8(dest, result ^ val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_XOR_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_XOR_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline uint16_t semi_atomic_fetch_xor_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    return atomic_fetch_xor_u16(dest, val);
}
#else
static inline uint16_t semi_atomic_fetch_xor_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    uint16_t result = atomic_load_u16(dest);
    atomic_store_u16(dest, result ^ val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_XOR_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_XOR_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline uint32_t semi_atomic_fetch_xor_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    return atomic_fetch_xor_u32(dest, val);
}
#else
static inline uint32_t semi_atomic_fetch_xor_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    uint32_t result = atomic_load_u32(dest);
    atomic_store_u32(dest, result ^ val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_XOR_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_XOR_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline uint64_t semi_atomic_fetch_xor_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    return atomic_fetch_xor_u64(dest, val);
}
#else
static inline uint64_t semi_atomic_fetch_xor_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    uint64_t result = atomic_load_u64(dest);
    atomic_store_u64(dest, result ^ val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_XOR_U64 || !HAS_ATOMIC_STORE_U64 */

/* FETCH_AND */
#if defined(HAS_ATOMIC_FETCH_AND_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline uint8_t semi_atomic_fetch_and_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    return atomic_fetch_and_u8(dest, val);
}
#else
static inline uint8_t semi_atomic_fetch_and_u8(volatile uint8_t *dest,
                                               uint8_t val)
{
    uint8_t result = atomic_load_u8(dest);
    atomic_store_u8(dest, result & val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_AND_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_AND_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline uint16_t semi_atomic_fetch_and_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    return atomic_fetch_and_u16(dest, val);
}
#else
static inline uint16_t semi_atomic_fetch_and_u16(volatile uint16_t *dest,
                                                 uint16_t val)
{
    uint16_t result = atomic_load_u16(dest);
    atomic_store_u16(dest, result & val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_AND_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_AND_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline uint32_t semi_atomic_fetch_and_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    return atomic_fetch_and_u32(dest, val);
}
#else
static inline uint32_t semi_atomic_fetch_and_u32(volatile uint32_t *dest,
                                                 uint32_t val)
{
    uint32_t result = atomic_load_u32(dest);
    atomic_store_u32(dest, result & val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_AND_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_AND_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline uint64_t semi_atomic_fetch_and_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    return atomic_fetch_and_u64(dest, val);
}
#else
static inline uint64_t semi_atomic_fetch_and_u64(volatile uint64_t *dest,
                                                 uint64_t val)
{
    uint64_t result = atomic_load_u64(dest);
    atomic_store_u64(dest, result & val);
    return result;
}
#endif /* HAS_ATOMIC_FETCH_AND_U64 || !HAS_ATOMIC_STORE_U64 */

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_UTILS_H */
/** @} */
