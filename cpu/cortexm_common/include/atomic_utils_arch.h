/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 *
 * @{
 *
 * @file
 * @brief       Implementation of fast atomic utility functions
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ATOMIC_UTILS_ARCH_H
#define ATOMIC_UTILS_ARCH_H
#ifndef DOXYGEN

#include "bit.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* clang provides no built-in atomic access to regular variables */
#ifndef __clang__

#define HAS_ATOMIC_LOAD_U8
static inline uint8_t atomic_load_u8(const volatile uint8_t *var)
{
    return __atomic_load_1(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_LOAD_U16
static inline uint16_t atomic_load_u16(const volatile uint16_t *var)
{
    return __atomic_load_2(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_LOAD_U32
static inline uint32_t atomic_load_u32(const volatile uint32_t *var)
{
    return __atomic_load_4(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U8
static inline void atomic_store_u8(volatile uint8_t *dest, uint8_t val)
{
    __atomic_store_1(dest, val, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U16
static inline void atomic_store_u16(volatile uint16_t *dest, uint16_t val)
{
    __atomic_store_2(dest, val, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U32
static inline void atomic_store_u32(volatile uint32_t *dest, uint32_t val)
{
    __atomic_store_4(dest, val, __ATOMIC_SEQ_CST);
}

#endif /* __clang__ */

#if CPU_HAS_SRAM_BITBAND
#define HAS_ATOMIC_BIT

typedef volatile uint32_t *atomic_bit_u8_t;
typedef volatile uint32_t *atomic_bit_u16_t;
typedef volatile uint32_t *atomic_bit_u32_t;
typedef volatile uint32_t *atomic_bit_u64_t;

static inline void __attribute__((always_inline)) _bit_barrier_pre(void)
{
    __asm__ volatile ("" : : : "memory");
}

static inline void __attribute__((always_inline)) _bit_barrier_post(void)
{
    __asm__ volatile ("" : : : "memory");
}

static inline bool _is_addr_valid_for_bitbanding(volatile void *_addr)
{
    /* SRAM bit-band region goes from 0x20000000 to 0x200fffff,
     * peripheral bit-band region goes from 0x40000000 to 0x400fffff */
    uintptr_t addr = (uintptr_t)_addr;
    if ((addr < 0x20000000UL) || (addr > 0x400fffffUL)) {
        return false;
    }

    if ((addr >= 0x200fffffUL) && (addr < 0x40000000UL)) {
        return false;
    }

    return true;
}

static inline atomic_bit_u8_t atomic_bit_u8(volatile uint8_t *dest, uint8_t bit)
{
    assert(_is_addr_valid_for_bitbanding(dest));
    return bitband_addr(dest, bit);
}

static inline atomic_bit_u16_t atomic_bit_u16(volatile uint16_t *dest, uint8_t bit)
{
    assert(_is_addr_valid_for_bitbanding(dest));
    return bitband_addr(dest, bit);
}

static inline atomic_bit_u32_t atomic_bit_u32(volatile uint32_t *dest, uint8_t bit)
{
    assert(_is_addr_valid_for_bitbanding(dest));
    return bitband_addr(dest, bit);
}

static inline atomic_bit_u64_t atomic_bit_u64(volatile uint64_t *dest, uint8_t bit)
{
    assert(_is_addr_valid_for_bitbanding(dest));
    return bitband_addr(dest, bit);
}

static inline void atomic_set_bit_u8(atomic_bit_u8_t bit)
{
    _bit_barrier_pre();
    *bit = 1;
    _bit_barrier_post();
}

static inline void atomic_set_bit_u16(atomic_bit_u16_t bit)
{
    _bit_barrier_pre();
    *bit = 1;
    _bit_barrier_post();
}

static inline void atomic_set_bit_u32(atomic_bit_u32_t bit)
{
    _bit_barrier_pre();
    *bit = 1;
    _bit_barrier_post();
}

static inline void atomic_set_bit_u64(atomic_bit_u64_t bit)
{
    _bit_barrier_pre();
    *bit = 1;
    _bit_barrier_post();
}

static inline void atomic_clear_bit_u8(atomic_bit_u8_t bit)
{
    _bit_barrier_pre();
    *bit = 0;
    _bit_barrier_post();
}
static inline void atomic_clear_bit_u16(atomic_bit_u16_t bit)
{
    _bit_barrier_pre();
    *bit = 0;
    _bit_barrier_post();
}

static inline void atomic_clear_bit_u32(atomic_bit_u32_t bit)
{
    _bit_barrier_pre();
    *bit = 0;
    _bit_barrier_post();
}

static inline void atomic_clear_bit_u64(atomic_bit_u64_t bit)
{
    _bit_barrier_pre();
    *bit = 0;
    _bit_barrier_post();
}

#endif /* CPU_HAS_BITBAND */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ATOMIC_UTILS_ARCH_H */
/** @} */
