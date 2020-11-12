/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       For comparison: "Atomic" accesses using volatile
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * This header implements the `volatile_*()` family of functions of
 * @ref sys_volatile_utils with `volatile_` instead of `volatile_` as prefix.
 * These implementation rely on the `volatile` type qualifier for implementing
 * "atomic" accesses; which in many cases will not result in atomic operations.
 * So this is a known to be ***BROKEN*** implementation. Its sole purpose is
 * to verify that the tests does detect broken implementations. Do not use
 * these functions for anything else but testing ;-)
 */

#ifndef VOLATILE_UTILS_H
#define VOLATILE_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t volatile_load_u8(const uint8_t *var)
{
    return *((const volatile uint8_t *)var);
}
static inline uint16_t volatile_load_u16(const uint16_t *var)
{
    return *((const volatile uint16_t *)var);
}
static inline uint32_t volatile_load_u32(const uint32_t *var)
{
    return *((const volatile uint32_t *)var);
}
static inline uint64_t volatile_load_u64(const uint64_t *var)
{
    return *((const volatile uint64_t *)var);
}

static inline void volatile_store_u8(uint8_t *dest, uint8_t val)
{
    *((volatile uint8_t *)dest) = val;
}
static inline void volatile_store_u16(uint16_t *dest, uint16_t val)
{
    *((volatile uint16_t *)dest) = val;
}
static inline void volatile_store_u32(uint32_t *dest, uint32_t val)
{
    *((volatile uint32_t *)dest) = val;
}
static inline void volatile_store_u64(uint64_t *dest, uint64_t val)
{
    *((volatile uint64_t *)dest) = val;
}

static inline void volatile_fetch_add_u8(uint8_t *dest, uint8_t val)
{
    *((volatile uint8_t *)dest) += val;
}
static inline void volatile_fetch_sub_u8(uint8_t *dest, uint8_t val)
{
    *((volatile uint8_t *)dest) -= val;
}
static inline void volatile_fetch_or_u8(uint8_t *dest, uint8_t val)
{
    *((volatile uint8_t *)dest) |= val;
}
static inline void volatile_fetch_xor_u8(uint8_t *dest, uint8_t val)
{
    *((volatile uint8_t *)dest) ^= val;
}
static inline void volatile_fetch_and_u8(uint8_t *dest, uint8_t val)
{
    *((volatile uint8_t *)dest) &= val;
}

static inline void volatile_fetch_add_u16(uint16_t *dest, uint16_t val)
{
    *((volatile uint16_t *)dest) += val;
}
static inline void volatile_fetch_sub_u16(uint16_t *dest, uint16_t val)
{
    *((volatile uint16_t *)dest) -= val;
}
static inline void volatile_fetch_or_u16(uint16_t *dest, uint16_t val)
{
    *((volatile uint16_t *)dest) |= val;
}
static inline void volatile_fetch_xor_u16(uint16_t *dest, uint16_t val)
{
    *((volatile uint16_t *)dest) ^= val;
}
static inline void volatile_fetch_and_u16(uint16_t *dest, uint16_t val)
{
    *((volatile uint16_t *)dest) &= val;
}

static inline void volatile_fetch_add_u32(uint32_t *dest, uint32_t val)
{
    *((volatile uint32_t *)dest) += val;
}
static inline void volatile_fetch_sub_u32(uint32_t *dest, uint32_t val)
{
    *((volatile uint32_t *)dest) -= val;
}
static inline void volatile_fetch_or_u32(uint32_t *dest, uint32_t val)
{
    *((volatile uint32_t *)dest) |= val;
}
static inline void volatile_fetch_xor_u32(uint32_t *dest, uint32_t val)
{
    *((volatile uint32_t *)dest) ^= val;
}
static inline void volatile_fetch_and_u32(uint32_t *dest, uint32_t val)
{
    *((volatile uint32_t *)dest) &= val;
}

static inline void volatile_fetch_add_u64(uint64_t *dest, uint64_t val)
{
    *((volatile uint64_t *)dest) += val;
}
static inline void volatile_fetch_sub_u64(uint64_t *dest, uint64_t val)
{
    *((volatile uint64_t *)dest) -= val;
}
static inline void volatile_fetch_or_u64(uint64_t *dest, uint64_t val)
{
    *((volatile uint64_t *)dest) |= val;
}
static inline void volatile_fetch_xor_u64(uint64_t *dest, uint64_t val)
{
    *((volatile uint64_t *)dest) ^= val;
}
static inline void volatile_fetch_and_u64(uint64_t *dest, uint64_t val)
{
    *((volatile uint64_t *)dest) &= val;
}

static inline void volatile_set_bit_u8(uint8_t *mask, uint8_t bit)
{
    *((volatile uint8_t *)mask) |= 1 << bit;
}
static inline void volatile_set_bit_u16(uint16_t *mask, uint8_t bit)
{
    *((volatile uint16_t *)mask) |= 1 << bit;
}
static inline void volatile_set_bit_u32(uint32_t *mask, uint8_t bit)
{
    *((volatile uint32_t *)mask) |= 1UL << bit;
}
static inline void volatile_set_bit_u64(uint64_t *mask, uint8_t bit)
{
    *((volatile uint64_t *)mask) |= 1ULL << bit;
}

static inline void volatile_clear_bit_u8(uint8_t *mask, uint8_t bit)
{
    *((volatile uint8_t *)mask) &= ~(1 << bit);
}
static inline void volatile_clear_bit_u16(uint16_t *mask, uint8_t bit)
{
    *((volatile uint16_t *)mask) &= ~(1 << bit);
}
static inline void volatile_clear_bit_u32(uint32_t *mask, uint8_t bit)
{
    *((volatile uint32_t *)mask) &= ~(1UL << bit);
}
static inline void volatile_clear_bit_u64(uint64_t *mask, uint8_t bit)
{
    *((volatile uint64_t *)mask) &= ~(1ULL << bit);
}

#ifdef __cplusplus
}
#endif

#endif /* VOLATILE_UTILS_H */
/** @} */
