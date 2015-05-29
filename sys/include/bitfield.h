/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_bitfield Bitfields
 * @ingroup     sys
 * @brief       Bitfields of arbitrary length
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

#ifndef BITFIELD_H
#define BITFIELD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
    field[idx / 8] |= (1u << (idx % 8));
}

/**
 * @brief   Clear the bit
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to clear
 */
static inline void bf_unset(uint8_t field[], size_t idx)
{
    field[idx / 8] &= ~(1u << (idx % 8));
}

/**
 * @brief   Toggle the bit
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to toggle
 */
static inline void bf_toggle(uint8_t field[], size_t idx)
{
    field[idx / 8] ^= (1u << (idx % 8));
}

/**
 * @brief  Check if the bet is set
 *
 * @param[in,out] field The bitfield
 * @param[in]     idx   The number of the bit to check
 */
static inline bool bf_isset(uint8_t field[], size_t idx)
{
    return (field[idx / 8] & (1u << (idx % 8)));
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BITFIELD_H */
