/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       PBKDF2 key derivation implementation- this is quite probably
 *              wrong, I'm no crypto expert.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#ifndef PBKDF2_H
#define PBKDF2_H

#include "hashes/sha256.h"

#define PBKDF2_KEY_SIZE SHA256_DIGEST_LENGTH

/**
 * Create a key from a password and hash using PBKDF2.
 *
 * @param[out]   output  Array of size PBKDF2_KEY_SIZE
 */
void pbkdf2_sha256(const uint8_t *password, size_t password_len,
                   const uint8_t *salt, size_t salt_len,
                   int iterations,
                   uint8_t *output);

#endif /* PBKDF2_H */
