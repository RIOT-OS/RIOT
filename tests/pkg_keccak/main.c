/*
 * Copyright (C) 2017 Adrian Herrmann <adrian.herrmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test cases for the Keccak hashing package
 *
 * @author      Adrian Herrmann <adrian.herrmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "keccak800.h"
#include "keccak1600.h"
#include "embUnit.h"

char datastring_empty[] = "";
bit_sequence* data_empty = (unsigned char*) datastring_empty;

char datastring_short[] = "0123456789";
bit_sequence* data_short = (unsigned char*) datastring_short;

char datastring_long[] =
    "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy "
    "eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam "
    "voluptua.";
bit_sequence* data_long = (unsigned char*) datastring_long;

char hashvalstring800_empty[] =
            "46f3a3935b3c6ba7c2699cbbfc96b13bb79b647ea34a47e151d20ee0edc5ee06";

char hashvalstring800_short[] =
            "a6305bbe48f1f8d2c58dfd9731974fe85321c09cba8a944b0635a9ba07443324";

char hashvalstring800_long[] =
            "0ab64f18ead9a3cc2d02a0daa2238167cd4de7f703280e93e6655bd1ce770fd8";

char hashvalstring1600_empty[] =
            "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a";

char hashvalstring1600_short[] =
            "8f8eaad16cbf8722a2165b660d47fcfd8496a41c611da758f3bb70f809f01ee3";

char hashvalstring1600_long[] =
            "90c113bf348ae55023c4ec49d280e7f8ddf2ef0337ed74f44718baabfe5bcad1";

static void test_keccak800_run(
    bit_sequence* data,
    char* datastring,
    char* hashvalstring)
{
    /* Initialize */
    keccak800_t hash800Instance;
    bit_sequence hashval[32];

    /* Testing hash initialization */
    keccak800hash_256_initialize(&hash800Instance);

    /* Testing hash update */
    hash_return return_update = keccak800hash_update(
        &hash800Instance,
        data,
        8*strlen(datastring)
    );
    TEST_ASSERT_EQUAL_INT(0, return_update);

    /* Testing hash finalization */
    hash_return return_final = keccak800hash_final(&hash800Instance, hashval);
    TEST_ASSERT_EQUAL_INT(0, return_final);

    /* Comparing digest with expected result */
    uint8_t eq = 1;
    char hashvalbuf[5];

    for (uint8_t i=0; i<32; i++) {
        sprintf(hashvalbuf, "%02x", hashval[i]);

        if (hashvalbuf[0] != hashvalstring[2*i] ||
            hashvalbuf[1] != hashvalstring[2*i+1]
        ) {
            eq = 0;
            break;
        }
    }

    TEST_ASSERT_EQUAL_INT(1, eq);
}

static void test_keccak1600_run(
    bit_sequence* data,
    char* datastring,
    char* hashvalstring)
{
    /* Initialize */
    keccak1600_t hash1600Instance;
    bit_sequence hashval[32];

    /* Testing hash initialization */
    sha3_256_initialize(&hash1600Instance);

    /* Testing hash update */
    hash_return return_update = keccak1600hash_update(
        &hash1600Instance,
        data,
        8*strlen(datastring)
    );
    TEST_ASSERT_EQUAL_INT(0, return_update);

    /* Testing hash finalization */
    hash_return return_final = keccak1600hash_final(&hash1600Instance, hashval);
    TEST_ASSERT_EQUAL_INT(0, return_final);

    /* Comparing digest with expected result */
    uint8_t eq = 1;
    char hashvalbuf[5];

    for (uint8_t i=0; i<32; i++) {
        sprintf(hashvalbuf, "%02x", hashval[i]);

        if (hashvalbuf[0] != hashvalstring[2*i] ||
            hashvalbuf[1] != hashvalstring[2*i+1]
        ) {
            eq = 0;
            break;
        }
    }

    TEST_ASSERT_EQUAL_INT(1, eq);
}

static void test_keccak800(void) {
    test_keccak800_run(data_empty, datastring_empty, hashvalstring800_empty);
    test_keccak800_run(data_short, datastring_short, hashvalstring800_short);
    test_keccak800_run(data_long, datastring_long, hashvalstring800_long);
}

static void test_keccak1600(void) {
    test_keccak1600_run(data_empty, datastring_empty, hashvalstring1600_empty);
    test_keccak1600_run(data_short, datastring_short, hashvalstring1600_short);
    test_keccak1600_run(data_long, datastring_long, hashvalstring1600_long);
}

Test *tests_keccak(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_keccak800),
        new_TestFixture(test_keccak1600)
    };

    EMB_UNIT_TESTCALLER(keccak_tests, NULL, NULL, fixtures);
    return (Test*)&keccak_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_keccak());
    TESTS_END();
    return 0;
}
