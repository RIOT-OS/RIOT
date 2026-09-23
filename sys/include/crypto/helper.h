/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        helper.h
 * @brief       helper functions for sys_crypto_modes
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "compiler_hints.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Increment a counter encoded in an 16 octet block.
 *
 * The counter is encoded from the least significant bit in the following form:
 * `block[15-L..15]`.
 *
 * @param[in,out]   block   encoded block
 * @param[in]       ctr_len length of counter
 */
void crypto_block_inc_ctr(uint8_t block[16], size_t ctr_len);

/**
 * @brief   Compares two blocks of same size in deterministic time.
 *
 * @param[in]       a       block a
 * @param[in]       b       block b
 * @param[in]       len     size of both blocks
 *
 * @retval          true    blocks are equal
 * @retval          false   blocks differ
 *
 * The parameters @p a and @p b are marked as `volatile` to avoid that the
 * compiler optimizes this function, even if it detects that this function is
 * is functionally equivalent to `memcmp(a, b, len) == 0`. The memory of @p a
 * and @p b does not need to be (and usually *should not* be) `volatile`, as
 * only this specific operation should not be optimized to be somewhat time
 * constant.
 *
 * Example usage:
 *
 * ```c
 * static bool validate_input(const void *hash_expected, size_t hash_len,
 *                            const void *data, size_t data_len)
 * {
 *     if (hash_len > MAX_HASH_SIZE) {
 *         assert(0);
 *         return false;
 *     }
 *     const uint8_t hash_actual[MAX_HASH_SIZE];
 *     compute_some_crypto_hash(hash_actual, data, data_len);
 *     return crypto_equals(hash_expected, hash_actual, hash_len);
 * }
 * ```
 */
ACCESS(read_only, 1, 3)
ACCESS(read_only, 2, 3)
bool crypto_equals(volatile const uint8_t *a, volatile const uint8_t *b,
                   size_t len);

/**
 * @brief   Secure wipe function.
 *
 * This wipe function zeros the supplied buffer in a way that the compiler is
 * not allowed to optimize. This can be used to erase secrets from memory.
 *
 * Note that this function on its own could be insufficient against (data
 * remanence) attacks. It is outside the scope of this function to thoroughly
 * shred the memory area.
 *
 * @param[in]       buf     buffer to wipe
 * @param[in]       len     size of the buffer in bytes
 */
ACCESS(write_only, 1, 2)
void crypto_secure_wipe(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
