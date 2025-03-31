/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CRYPTO_HELPER_H
#define CRYPTO_HELPER_H

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

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Increment a counter encoded in an 16 octet block. The counter is
 *        encoded from the least significant bit in the following form:
 *        block[15-L..15])
 *
 * @param block     encoded block
 * @param L         length of counter
 */
void crypto_block_inc_ctr(uint8_t block[16], int L);

/**
 * @brief   Compares two blocks of same size in deterministic time.
 *
 * @param a     block a
 * @param b     block b
 * @param len   size of both blocks
 *
 * @returns 0 iff the blocks are non-equal.
 */
int crypto_equals(const uint8_t *a, const uint8_t *b, size_t len);

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
 * @param[in]   buf     buffer to wipe
 * @param[in]   len     size of the buffer in bytes
 */
void crypto_secure_wipe(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CRYPTO_HELPER_H */
