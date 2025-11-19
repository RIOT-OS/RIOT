/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       This test was written to compare the runtime of the RIOT software
 *              implementation and the CryptoAuth hardware implementation of SHA-256.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "hashes/sha256.h"
#include "atca.h"
#include "atca_params.h"

#define SHA256_HASH_SIZE (32)

/**
 * Function to call RIOT software implementation of SHA-256
 */
static int test_riot_sha256(uint8_t *teststring, uint16_t len,
                            uint8_t *expected,
                            uint8_t *result)
{
    sha256_context_t ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, (void *)teststring, len);
    sha256_final(&ctx, result);
    return memcmp(expected, result, SHA256_HASH_SIZE);
}

/**
 * Function to call CryptoAuth hardware implementation of SHA-256
 */
static int test_atca_sha(uint8_t *teststring, uint16_t len, uint8_t *expected,
                         uint8_t *result)
{
    ATCA_STATUS status;
    ATCADevice dev = atca_devs_ptr[0];

    status = calib_sha_start(dev);
    if (status != ATCA_SUCCESS) {
        printf("ATCA SHA start failed: %02x\n", status);
    }

    status = calib_sha_end(dev, result, len, teststring);
    if (status != ATCA_SUCCESS) {
        printf("ATCA SHA end failed: %02x\n", status);
    }

    return memcmp(expected, result, SHA256_HASH_SIZE);
}

int main(void)
{
    uint8_t teststring[] = "chili cheese fries";
    uint8_t expected[] =
    { 0x36, 0x46, 0xEF, 0xD6, 0x27, 0x6C, 0x0D, 0xCB, 0x4B, 0x07, 0x73, 0x41,
      0x88, 0xF4, 0x17, 0xB4, 0x38, 0xAA, 0xCF, 0xC6, 0xAE, 0xEF, 0xFA, 0xBE,
      0xF3, 0xA8, 0x5D, 0x67, 0x42, 0x0D, 0xFE, 0xE5 };

    uint8_t result[SHA256_HASH_SIZE];                       /* +3 to fit 1 byte length and 2 bytes checksum */

    memset(result, 0, SHA256_HASH_SIZE);                    /* alles in result auf 0 setzen */

    uint16_t test_string_size = (sizeof(teststring) - 1);   /* -1 to ignore \0 */

    if (test_riot_sha256(teststring, test_string_size, expected, result) == 0) {
        printf("RIOT SHA256: Success\n");
    }
    else {
        printf("RIOT SHA256: Failure.\n");
    }
    atca_delay_us(10);
    memset(result, 0, SHA256_HASH_SIZE);

    if (test_atca_sha(teststring, test_string_size, expected, result) == 0) {
        printf("ATCA SHA256: Success\n");
    }
    else {
        printf("ATCA SHA256: Failure.\n");
    }

    return 0;
}
