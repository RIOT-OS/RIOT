/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_hashes_pbkdf2 PBKDF2
 * @ingroup     sys_hashes
 * @brief       PBKDF2 key derivation implementation.
 * @{
 *
 * @file
 * @brief       PBKDF2 key derivation implementation.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#ifndef HASHES_PBKDF2_H
#define HASHES_PBKDF2_H

#include "hashes/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PBKDF2_KEY_SIZE SHA256_DIGEST_LENGTH

/**
 * Create a key from a password and hash using PBKDF2.
 *
 * @param   iterations  Number of rounds. Must be >1.
 * @param[out]   output  Array of size PBKDF2_KEY_SIZE
 */
void pbkdf2_sha256(const uint8_t *password, size_t password_len,
                   const uint8_t *salt, size_t salt_len,
                   int iterations,
                   uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif /* HASHES_PBKDF2_H */
