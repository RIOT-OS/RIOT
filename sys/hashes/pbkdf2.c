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
 * @brief       PBKDF2 key derivation implementation- only sha256 is supported
 *              at the moment, and the key size is fixed.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "hashes/sha256.h"
#include "hashes/pbkdf2.h"
#include "crypto/helper.h"

static void inplace_xor_scalar(uint8_t *bytes, size_t len, uint8_t c)
{
    while (len--) {
        *bytes ^= c;
        bytes++;
    }
}

static void inplace_xor_digests(uint8_t *d1, const uint8_t *d2)
{
    int len = SHA256_DIGEST_LENGTH;

    while (len--) {
        *d1 ^= *d2;
        d1++;
        d2++;
    }
}

void pbkdf2_sha256(const uint8_t *password, size_t password_len,
                   const uint8_t *salt, size_t salt_len,
                   int iterations,
                   uint8_t *output)
{
    sha256_context_t inner;
    sha256_context_t outer;
    uint8_t tmp_digest[SHA256_DIGEST_LENGTH];
    int first_iter = 1;

    {
        uint8_t processed_pass[SHA256_INTERNAL_BLOCK_SIZE] = {0};

        if (password_len > sizeof(processed_pass)) {
            sha256_init(&inner);
            sha256_update(&inner, password, password_len);
            sha256_final(&inner, processed_pass);
        } else {
            memcpy(processed_pass, password, password_len);
        }

        sha256_init(&inner);
        sha256_init(&outer);

        /* Trick: doing inner.update(processed_pass XOR 0x36) followed by
         * inner.update(processed_pass XOR 0x5C) requires remembering
         * processed_pass. Instead undo the first XOR while doing the second.
         */
        inplace_xor_scalar(processed_pass, sizeof(processed_pass), 0x36);
        sha256_update(&inner, processed_pass, sizeof(processed_pass));

        inplace_xor_scalar(processed_pass, sizeof(processed_pass), 0x36 ^ 0x5C);
        sha256_update(&outer, processed_pass, sizeof(processed_pass));

        crypto_secure_wipe(&processed_pass, sizeof(processed_pass));
    }

    memset(output, 0, SHA256_DIGEST_LENGTH);

    while (iterations--) {
        sha256_context_t inner_copy = inner, outer_copy = outer;

        if (first_iter) {
            sha256_update(&inner_copy, salt, salt_len);
            sha256_update(&inner_copy, "\x00\x00\x00\x01", 4);
            first_iter = 0;
        } else {
            sha256_update(&inner_copy, tmp_digest, sizeof(tmp_digest));
        }

        sha256_final(&inner_copy, tmp_digest);

        sha256_update(&outer_copy, tmp_digest, sizeof(tmp_digest));
        sha256_final(&outer_copy, tmp_digest);

        inplace_xor_digests(output, tmp_digest);

        if (iterations == 0) {
            crypto_secure_wipe(&inner_copy, sizeof(inner_copy));
            crypto_secure_wipe(&outer_copy, sizeof(outer_copy));
        }
    }

    crypto_secure_wipe(&inner, sizeof(inner));
    crypto_secure_wipe(&outer, sizeof(outer));
    crypto_secure_wipe(&tmp_digest, sizeof(tmp_digest));
}
