/*
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    cpu_kinetis_bme Kinetis Bit Manipulation Engine (BME)
 * @ingroup     cpu_kinetis
 * @brief       Macros for using decorated memory accesses with the Bit
 *              Manipulation Engine available in Kinetis Cortex-M0+ devices
 *
 * @{
 * @file
 * @brief       Macro definitions for the Kinetis Bit Manipulation Engine (BME)
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Tell bit.h that we provide CPU specific bit manipulation functions
 */
#define BITBAND_FUNCTIONS_PROVIDED 1

#define BME_AND_MASK        (1 << 26) /**< AND decoration bitmask */
#define BME_OR_MASK         (1 << 27) /**< OR decoration bitmask */
#define BME_XOR_MASK        (3 << 26) /**< XOR decoration bitmask */
#define BME_LAC1_MASK(BIT)  ((1 << 27) | ((BIT) << 21)) /**< Load-and-clear 1 bit */
#define BME_LAS1_MASK(BIT)  ((3 << 26) | ((BIT) << 21)) /**< Load-and-set 1 bit */

/**
 * @brief Bit field extraction bitmask
 *
 * @param bit    LSB of the bitfield within the word/halfword/byte
 * @param width  Number of bits to extract
 */
#define BME_BF_MASK(bit, width) ((1 << 28) | ((bit) << 23) | (((width) - 1 ) << 19))

/**
 * @brief Bit field address macro
 *
 * @pre The target address must lie within a part of the peripheral address
 * space 0x40000000 - 0x40070000
 *
 * @param[in]  ptr   Pointer to target register
 * @param[in]  bit   Location of the LSB of the bitfield within the register
 * @param[in]  width Width of the bitfield, in bits
 *
 * @return   bitfield address as an uintptr_t
 */
static inline volatile void *bme_bf_addr(volatile void *ptr, uintptr_t bit, uintptr_t width)
{
    return (volatile void *)(((uintptr_t)ptr) | BME_BF_MASK(bit, width));
}

/**
 * @brief Access a bitfield (32 bit load/store)
 *
 * This macro can be used both for store `(*bme_bitfield32(xxx) = y)` and
 * load `(y = *bme_bitfield32(ptr, bit))`
 *
 * @pre The target address must lie within a part of the peripheral address
 * space 0x40000000 - 0x40070000
 *
 * @param[in]  ptr   Pointer to target register
 * @param[in]  bit   Location of the LSB of the bitfield within the register
 * @param[in]  width Width of the bitfield, in bits
 *
 * @return bitfield extracted as a (modifiable) lvalue
 */
static inline volatile uint32_t *bme_bitfield32(volatile uint32_t *ptr, uint8_t bit, uint8_t width)
{
    return (volatile uint32_t *)(bme_bf_addr(ptr, bit, width));
}

/**
 * @brief Access a bitfield (16 bit load/store)
 *
 * This macro can be used both for store `(*bme_bitfield16(xxx) = y)` and
 * load `(y = *bme_bitfield16(ptr, bit))`
 *
 * @pre The target address must lie within a part of the peripheral address
 * space 0x40000000 - 0x40070000
 *
 * @param[in]  ptr   Pointer to target register
 * @param[in]  bit   Location of the LSB of the bitfield within the register
 * @param[in]  width Width of the bitfield, in bits
 *
 * @return bitfield extracted as a (modifiable) lvalue
 */
static inline volatile uint16_t *bme_bitfield16(volatile uint16_t *ptr, uint8_t bit, uint8_t width)
{
    return (volatile uint16_t *)(bme_bf_addr(ptr, bit, width));
}

/**
 * @brief Access a bitfield (8 bit load/store)
 *
 * This macro can be used both for store `(*bme_bitfield8(xxx) = y)` and
 * load `(y = *bme_bitfield8(ptr, bit))`
 *
 * @pre The target address must lie within a part of the peripheral address
 * space 0x40000000 - 0x40070000
 *
 * @param[in]  ptr   Pointer to target register
 * @param[in]  bit   Location of the LSB of the bitfield within the register
 * @param[in]  width Width of the bitfield, in bits
 *
 * @return bitfield extracted as a (modifiable) lvalue
 */
static inline volatile uint8_t *bme_bitfield8(volatile uint8_t *ptr, uint8_t bit, uint8_t width)
{
    return (volatile uint8_t *)(bme_bf_addr(ptr, bit, width));
}

/* For compatibility with the M3/M4 bitbanding macros: */

/**
 * @brief Set a single bit in the 32 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr |= (1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_set32(volatile uint32_t *ptr, uint8_t bit)
{
    *((volatile uint32_t *)(((uintptr_t)ptr) | BME_OR_MASK)) = (uint32_t)((1ul << bit));
}

/**
 * @brief Set a single bit in the 16 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr |= (1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_set16(volatile uint16_t *ptr, uint8_t bit)
{
    *((volatile uint16_t *)(((uintptr_t)ptr) | BME_OR_MASK)) = (uint16_t)((1ul << bit));
}

/**
 * @brief Set a single bit in the 8 bit byte pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr |= (1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target byte
 * @param[in]  bit bit number within the byte
 */
static inline void bit_set8(volatile uint8_t *ptr, uint8_t bit)
{
    *((volatile uint8_t *)(((uintptr_t)ptr) | BME_OR_MASK)) = (uint8_t)((1ul << bit));
}

/**
 * @brief Clear a single bit in the 32 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr &= ~(1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_clear32(volatile uint32_t *ptr, uint8_t bit)
{
    *((volatile uint32_t *)(((uintptr_t)ptr) | BME_AND_MASK)) = (uint32_t)(~(1ul << bit));
}

/**
 * @brief Clear a single bit in the 16 bit word pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr &= ~(1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target word
 * @param[in]  bit bit number within the word
 */
static inline void bit_clear16(volatile uint16_t *ptr, uint8_t bit)
{
    *((volatile uint16_t *)(((uintptr_t)ptr) | BME_AND_MASK)) = (uint16_t)(~(1ul << bit));
}

/**
 * @brief Clear a single bit in the 8 bit byte pointed to by @p ptr
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr &= ~(1 << bit);
 * @endcode
 *
 * There is a read-modify-write cycle occurring within the core, but this cycle
 * is atomic and can not be disrupted by IRQs
 *
 * @param[in]  ptr pointer to target byte
 * @param[in]  bit bit number within the byte
 */
static inline void bit_clear8(volatile uint8_t *ptr, uint8_t bit)
{
    *((volatile uint8_t *)(((uintptr_t)ptr) | BME_AND_MASK)) = (uint8_t)(~(1ul << bit));
}

/**
 * @brief Checks if a single bit in the 32 bit word pointed to by @p ptr is set.
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr & (1 << bit);
 * @endcode
 *
 * @param[in] ptr   Pointer to target word.
 * @param[in] bit   Bit number within the word.
 *
 * @return          True if the bit was set, false otherwise.
 */
static inline bool bit_check32(volatile uint32_t *ptr, uint8_t bit)
{
    return *((volatile uint32_t *)(((uintptr_t)ptr) | BME_AND_MASK)) & (uint32_t)(1ul << bit);
}

/**
 * @brief Checks if a single bit in the 16 bit word pointed to by @p ptr is set.
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr & (1 << bit);
 * @endcode
 *
 * @param[in] ptr   Pointer to target word.
 * @param[in] bit   Bit number within the word.
 *
 * @return          True if the bit was set, false otherwise.
 */
static inline bool bit_check16(volatile uint16_t *ptr, uint8_t bit)
{
    return *((volatile uint16_t *)(((uintptr_t)ptr) | BME_AND_MASK)) & (uint16_t)(1ul << bit);
}

/**
 * @brief Checks if a single bit in the 8 bit byte pointed to by @p ptr is set.
 *
 * The effect is the same as for the following snippet:
 *
 * @code{c}
 *   *ptr & (1 << bit);
 * @endcode
 *
 * @param[in] ptr   Pointer to target byte.
 * @param[in] bit   Bit number within the byte.
 *
 * @return          True if the bit was set, false otherwise.
 */
static inline bool bit_check8(volatile uint8_t *ptr, uint8_t bit)
{
    return *((volatile uint8_t *)(((uintptr_t)ptr) | BME_AND_MASK)) & (uint8_t)(1ul << bit);
}

#ifdef __cplusplus
}
#endif

/** @} */
