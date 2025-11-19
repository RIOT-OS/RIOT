/*
 * SPDX-FileCopyrightText: 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "embUnit/embUnit.h"

#include "bitfield.h"
#include "coding/xor.h"
#include "tests-coding.h"

static void test_coding_xor_building_blocks(void)
{
    char string[] = "0123456789AB";
    uint8_t parity[CODING_XOR_PARITY_LEN(sizeof(string) - 1)];

    coding_xor_generate(string, sizeof(string) - 1, parity);

    TEST_ASSERT_EQUAL_INT(0x33, parity[0]);
    TEST_ASSERT_EQUAL_INT(0x32, parity[1]);
    TEST_ASSERT_EQUAL_INT(0x39, parity[2]);
    TEST_ASSERT_EQUAL_INT(0x3A, parity[3]);
    TEST_ASSERT_EQUAL_STRING("0369147A258B", string);

    uint8_t blocks = 0x80;
    bool success = coding_xor_recover(string, sizeof(string) - 1, parity,
                                      &blocks, sizeof(string) - 1, false);
    TEST_ASSERT(success);
    TEST_ASSERT_EQUAL_STRING("0123456789AB", string);
}

static void test_coding_xor_recovery(void)
{
    char string[] = "0123456789AB";
    char string_rx[sizeof(string)];
    uint8_t parity[CODING_XOR_PARITY_LEN(sizeof(string) - 1)];
    const uint8_t chunk_size = 4;
    const uint8_t num_chunks = sizeof(string) / chunk_size;
    const uint8_t num_parity_chunks = sizeof(parity) / chunk_size;

    BITFIELD(chunks, num_chunks + num_parity_chunks);
    memset(chunks, 0, sizeof(chunks));
    memset(string_rx, 0, sizeof(string_rx));

    coding_xor_generate(string, sizeof(string) - 1, parity);

    for (unsigned i = 0; i < num_chunks; ++i) {
        /* lose a single chunk / packet */
        if (i == 1) {
            continue;
        }

        memcpy(string_rx + i * chunk_size,
               string + i * chunk_size,
               chunk_size);
        bf_set(chunks, i);
    }
    /* we have all parity chunks */
    for (unsigned i = 0; i < num_parity_chunks; ++i) {
        bf_set(chunks, num_chunks + i);
    }

    bool success = coding_xor_recover(string_rx, sizeof(string_rx) - 1,
                                      parity, chunks, chunk_size, false);
    TEST_ASSERT(success);
    TEST_ASSERT_EQUAL_STRING("0123456789AB", string_rx);
}

static void test_coding_xor_recovery_failed(void)
{
    char string[] = "0123456789AB";
    char string_rx[sizeof(string)];
    uint8_t parity[CODING_XOR_PARITY_LEN(sizeof(string) - 1)];
    const uint8_t chunk_size = 4;
    const uint8_t num_chunks = sizeof(string) / chunk_size;
    const uint8_t num_parity_chunks = sizeof(parity) / chunk_size;

    BITFIELD(chunks, num_chunks + num_parity_chunks);
    memset(chunks, 0, sizeof(chunks));
    memset(string_rx, 0, sizeof(string_rx));

    coding_xor_generate(string, sizeof(string) - 1, parity);

    for (unsigned i = 0; i < num_chunks; ++i) {
        /* lose two chunks / packets */
        if (i == 1 || i == 2) {
            continue;
        }

        memcpy(string_rx + i * chunk_size,
               string + i * chunk_size,
               chunk_size);
        bf_set(chunks, i);
    }
    /* we have all parity chunks */
    for (unsigned i = 0; i < num_parity_chunks; ++i) {
        bf_set(chunks, num_chunks + i);
    }

    bool success = coding_xor_recover(string_rx, sizeof(string_rx) - 1,
                                      parity, chunks, chunk_size, false);
    TEST_ASSERT(!success);
}

static void test_coding_xor_recovery_parity(void)
{
    char string[] = "0123456789AB";
    uint8_t parity[CODING_XOR_PARITY_LEN(sizeof(string) - 1)];
    uint8_t parity_rx[CODING_XOR_PARITY_LEN(sizeof(string) - 1)];
    const uint8_t chunk_size = 4;
    const uint8_t num_chunks = sizeof(string) / chunk_size;
    const uint8_t num_parity_chunks = sizeof(parity) / chunk_size;

    BITFIELD(chunks, num_chunks + num_parity_chunks);
    memset(chunks, 0, sizeof(chunks));

    coding_xor_generate(string, sizeof(string) - 1, parity);

    /* we have all data chunks */
    for (unsigned i = 0; i < num_chunks; ++i) {
        bf_set(chunks, i);
    }

    bool success = coding_xor_recover(string, sizeof(string) - 1, parity_rx,
                                      chunks, chunk_size, true);
    TEST_ASSERT(success);
    TEST_ASSERT_EQUAL_STRING("0123456789AB", string);
    TEST_ASSERT(memcmp(parity, parity_rx, sizeof(parity)) == 0);
}

static void test_coding_xor_recovery_large(void)
{
    uint8_t txbuf[1024];
    const size_t data_len = 768;

    const size_t parity_len = CODING_XOR_PARITY_LEN(data_len);
    uint8_t *parity = &txbuf[data_len];

    const uint8_t chunk_size = 64;
    const size_t num_chunks = (data_len + parity_len) / chunk_size;
    BITFIELD(chunks, num_chunks);
    memset(chunks, 0, sizeof(chunks));

    TEST_ASSERT_EQUAL_INT(sizeof(txbuf), data_len + parity_len);

    /* fill TX buffer with known pattern */
    for (unsigned i = 0; i < data_len; ++i) {
        txbuf[i] = i & 0xFF;
    }
    coding_xor_generate(txbuf, data_len, parity);

    for (unsigned i = 0; i < num_chunks; ++i) {
        uint8_t *data = &txbuf[i * chunk_size];

        /* lose some chunks */
        if (i == 3 || i == 10 || i == 13) {
            memset(data, 0, chunk_size);
        } else {
            bf_set(chunks, i);
        }
    }

    bool success = coding_xor_recover(txbuf, data_len, parity, chunks,
                                      chunk_size, true);
    TEST_ASSERT(success);
    for (unsigned i = 0; i < data_len; ++i) {
        TEST_ASSERT_EQUAL_INT(i & 0xFF, txbuf[i]);
    }
}

Test *tests_coding_xor_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_coding_xor_building_blocks),
        new_TestFixture(test_coding_xor_recovery),
        new_TestFixture(test_coding_xor_recovery_failed),
        new_TestFixture(test_coding_xor_recovery_parity),
        new_TestFixture(test_coding_xor_recovery_large),
    };

    EMB_UNIT_TESTCALLER(coding_xor_tests, NULL, NULL, fixtures);

    return (Test *)&coding_xor_tests;
}
