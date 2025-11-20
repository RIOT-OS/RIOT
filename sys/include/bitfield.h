/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_bitfield Bitfields
 * @ingroup     sys
 * @brief       Bitfields of arbitrary length
 *
 * The bitfields in this module have their most significant bytes first and
 * their most significant bits within each byte of the bitfield also first.
 *
 * @file
 * @{
 *
 * @brief       bitfields operations on bitfields of arbitrary length
 *
 * @note        Code taken mostly from
 *              <a href="http://stackoverflow.com/questions/1590893/error-trying-to-define-a-1-024-bit-128-byte-bit-field">
 *              Stackoverflow, User Christoph</a>
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Declare a bitfield of a given size
 *
 * @note    SIZE should be a constant expression. This avoids variable length
 *          arrays.
 */
#define BITFIELD(NAME, SIZE)  uint8_t NAME[((SIZE) + 7) / 8]

/**
 * @brief   Set the bit to 1
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to set
 */
static inline void bf_set(uint8_t field[], size_t idx)
{
    field[idx / 8] |= (1u << (7 - (idx % 8)));
}

/**
 * @brief   Atomically set the bit to 1
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to set
 */
static inline void bf_set_atomic(uint8_t field[], size_t idx)
{
    unsigned state = irq_disable();
    bf_set(field, idx);
    irq_restore(state);
}

/**
 * @brief   Clear the bit
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to clear
 */
static inline void bf_unset(uint8_t field[], size_t idx)
{
    field[idx / 8] &= ~(1u << (7 - (idx % 8)));
}

/**
 * @brief   Atomically clear the bit
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to clear
 */
static inline void bf_unset_atomic(uint8_t field[], size_t idx)
{
    unsigned state = irq_disable();
    bf_unset(field, idx);
    irq_restore(state);
}

/**
 * @brief   Toggle the bit
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to toggle
 */
static inline void bf_toggle(uint8_t field[], size_t idx)
{
    field[idx / 8] ^= (1u << (7 - (idx % 8)));
}

/**
 * @brief   Atomically toggle the bit
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to toggle
 */
static inline void bf_toggle_atomic(uint8_t field[], size_t idx)
{
    unsigned state = irq_disable();
    bf_toggle(field, idx);
    irq_restore(state);
}

/**
 * @brief  Check if the bet is set
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to check
 */
static inline bool bf_isset(const uint8_t field[], size_t idx)
{
    return (field[idx / 8] & (1u << (7 - (idx % 8))));
}

/**
 * @brief  Perform a bitwise OR operation on two bitfields
 *         `out = a | b`
 *
 * @pre   The size of @p a, @p b and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The first bitfield
 * @param[in]     b     The second bitfield
 * @param[in]     len   The number of bits in the bitfields
 */
static inline void bf_or(uint8_t out[], const uint8_t a[], const uint8_t b[], size_t len)
{
    len = (len + 7) / 8;
    while (len--) {
        out[len] = a[len] | b[len];
    }
}

/**
 * @brief  Atomically perform a bitwise OR operation on two bitfields
 *         `out = a | b`
 *
 * @pre   The size of @p a, @p b and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The first bitfield
 * @param[in]     b     The second bitfield
 * @param[in]     len   The number of bits in the bitfields
 */
static inline void bf_or_atomic(uint8_t out[], const uint8_t a[],
                                const uint8_t b[], size_t len)
{
    unsigned state = irq_disable();
    bf_or(out, a, b, len);
    irq_restore(state);
}

/**
 * @brief  Perform a bitwise AND operation on two bitfields
 *         `out = a & b`
 *
 * @pre   The size of @p a, @p b and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The first bitfield
 * @param[in]     b     The second bitfield
 * @param[in]     len   The number of bits in the bitfields
 */
static inline void bf_and(uint8_t out[], const uint8_t a[], const uint8_t b[], size_t len)
{
    len = (len + 7) / 8;
    while (len--) {
        out[len] = a[len] & b[len];
    }
}

/**
 * @brief  Atomically perform a bitwise AND operation on two bitfields
 *         `out = a & b`
 *
 * @pre   The size of @p a, @p b and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The first bitfield
 * @param[in]     b     The second bitfield
 * @param[in]     len   The number of bits in the bitfields
 */
static inline void bf_and_atomic(uint8_t out[], const uint8_t a[],
                                 const uint8_t b[], size_t len)
{
    unsigned state = irq_disable();
    bf_and(out, a, b, len);
    irq_restore(state);
}

/**
 * @brief  Perform a bitwise XOR operation on two bitfields
 *         `out = a ^ b`
 *
 * @pre   The size of @p a, @p b and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The first bitfield
 * @param[in]     b     The second bitfield
 * @param[in]     len   The number of bits in the bitfields
 */
static inline void bf_xor(uint8_t out[], const uint8_t a[], const uint8_t b[], size_t len)
{
    len = (len + 7) / 8;
    while (len--) {
        out[len] = a[len] ^ b[len];
    }
}

/**
 * @brief  Atomically perform a bitwise XOR operation on two bitfields
 *         `out = a ^ b`
 *
 * @pre   The size of @p a, @p b and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The first bitfield
 * @param[in]     b     The second bitfield
 * @param[in]     len   The number of bits in the bitfields
 */
static inline void bf_xor_atomic(uint8_t out[], const uint8_t a[],
                                 const uint8_t b[], size_t len)
{
    unsigned state = irq_disable();
    bf_xor(out, a, b, len);
    irq_restore(state);
}

/**
 * @brief  Perform a bitwise NOT operation on a bitfield
 *         `out = ~a`
 *
 * @pre   The size of @p a and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The bitfield to invert
 * @param[in]     len   The number of bits in the bitfield
 */
static inline void bf_inv(uint8_t out[], const uint8_t a[], size_t len)
{
    len = (len + 7) / 8;
    while (len--) {
        out[len] = ~a[len];
    }
}

/**
 * @brief  Atomically perform a bitwise NOT operation on a bitfield
 *         `out = ~a`
 *
 * @pre   The size of @p a and @p out must be at least @p len bits
 *
 * @note  This operation will also affect unused bits of the bytes that make up
 *        the bitfield.
 *
 * @param[out]    out   The resulting bitfield
 * @param[in]     a     The bitfield to invert
 * @param[in]     len   The number of bits in the bitfield
 */
static inline void bf_inv_atomic(uint8_t out[], const uint8_t a[], size_t len)
{
    unsigned state = irq_disable();
    bf_inv(out, a, len);
    irq_restore(state);
}

/**
 * @brief  Atomically get the number of an unset bit and set it
 *
 * This function can be used to record e.g., empty entries in an array.
 *
 * @param[in,out] field The bitfield
 * @param[in]     len   The number of bits in the bitfield to consider
 *
 * @return      number of bit that was set
 * @return      -1 if no bit was unset
 */
int bf_get_unset(uint8_t field[], size_t len);

/**
 * @brief  Get the index of the first set bit in the field
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 *
 * @return      number of the first set bit
 * @return      -1 if no bit is set
 */
int bf_find_first_set(const uint8_t field[], size_t size);

/**
 * @brief  Get the index of the zero bit in the field
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 *
 * @return      number of the first unset bit
 * @return      -1 if all bits are set
 */
int bf_find_first_unset(const uint8_t field[], size_t size);

/**
 * @brief  Set all bits in the bitfield to 1
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 */
void bf_set_all(uint8_t field[], size_t size);

/**
 * @brief  Atomically set all bits in the bitfield to 1
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 */
static inline void bf_set_all_atomic(uint8_t field[], size_t size)
{
    unsigned state = irq_disable();
    bf_set_all(field, size);
    irq_restore(state);
}

/**
 * @brief  Clear all bits in the bitfield to 0
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 */
void bf_clear_all(uint8_t field[], size_t size);

/**
 * @brief  Atomically clear all bits in the bitfield to 0
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 */
static inline void bf_clear_all_atomic(uint8_t field[], size_t size)
{
    unsigned state = irq_disable();
    bf_clear_all(field, size);
    irq_restore(state);
}

/**
 * @brief  Count set bits in the bitfield
 *
 * @param[in]     field The bitfield
 * @param[in]     size  The number of bits in the bitfield
 *
 * @return number of '1' bits in the bitfield
 */
unsigned bf_popcnt(const uint8_t field[], size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
