/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Test PBKDF2-sha256 implementation.
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "hashes/pbkdf2.h"
#include "hashes/sha256.h"
#include "kernel_defines.h"

static uint8_t key[SHA256_DIGEST_LENGTH];

struct testcase {
    const char *password;
    const char *salt;
    uint16_t iterations;
    const uint8_t digest[sizeof(key)];
};

struct testcase testcases[] = {
    {
        .password = "passwd",
        .salt = "salt",
        .iterations = 1,
        /* dig = hashlib.pbkdf2_hmac("sha256", "passwd".encode("utf-8"),
         *                           "salt".encode("utf-8"), 1)
         * "".join("0x{:02x}, ".format(b) for b in dig)
         */
        .digest = {
            0x55, 0xac, 0x04, 0x6e, 0x56, 0xe3, 0x08, 0x9f,
            0xec, 0x16, 0x91, 0xc2, 0x25, 0x44, 0xb6, 0x05,
            0xf9, 0x41, 0x85, 0x21, 0x6d, 0xde, 0x04, 0x65,
            0xe6, 0x8b, 0x9d, 0x57, 0xc2, 0x0d, 0xac, 0xbc,
        }
    },
    {
        .password = "RIOT",
        .salt = "rocks",
        .iterations = 16,
        /* dig = hashlib.pbkdf2_hmac("sha256", "RIOT".encode("utf-8"),
         *                           "rocks".encode("utf-8"), 16)
         * "".join("0x{:02x}, ".format(b) for b in dig)
         */
        .digest = {
            0x72, 0xa6, 0x06, 0xbb, 0x5c, 0xbe, 0x92, 0x4a,
            0xd2, 0x0a, 0xee, 0xc2, 0x4e, 0xa5, 0x17, 0xc4,
            0xd7, 0xb1, 0x1d, 0x04, 0x9d, 0x84, 0xbb, 0x29,
            0x6b, 0x36, 0xad, 0x90, 0x4d, 0x6f, 0x79, 0xdf,
        }
    },
    {
        .password = "This is a secure password", /* <-- no it is NOT! */
        .salt = "and this salt is even more secure",
        .iterations = 13,
        /* dig = hashlib.pbkdf2_hmac("sha256",
         *                           "This is a secure password".encode("utf-8"),
         *                           "and this salt is even more secure".encode("utf-8"),
         *                           13)
         * "".join("0x{:02x}, ".format(b) for b in dig)
         */
        .digest = {
            0x9a, 0x41, 0x83, 0x2b, 0x77, 0xc4, 0x61, 0x64,
            0x06, 0xd3, 0x2e, 0x97, 0x06, 0x5e, 0xc5, 0xc7,
            0xe1, 0xa0, 0x18, 0x75, 0x01, 0xfe, 0xb8, 0xc8,
            0x70, 0x92, 0x28, 0x0e, 0x1d, 0x1a, 0x00, 0xb6,
        }
    },
};

int main(void)
{
    bool failed = false;
    for (size_t i = 0; i < ARRAY_SIZE(testcases); i++) {
        struct testcase *tc = &testcases[i];
        size_t password_len = strlen(tc->password);
        size_t salt_len = strlen(tc->salt);
        memset(key, 0x00, sizeof(key));
        pbkdf2_sha256(tc->password, password_len, tc->salt, salt_len,
                      tc->iterations, key);

        if (memcmp(tc->digest, key, sizeof(key)) != 0) {
            failed = true;
            print_str("Test vector ");
            print_u32_dec((uint32_t)i);
            print_str(": FAILED\n");
        }

    }

    if (!failed) {
        print_str("TEST PASSED\n");
    }

    return 0;
}
