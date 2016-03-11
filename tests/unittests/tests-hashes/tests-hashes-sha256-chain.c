/*
 * Copyright (C) 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha256-chain implementation
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"

#include "hashes/sha256.h"

#include "tests-hashes.h"

static void test_sha256_hash_chain(void)
{
    const char strSeed[] = "My cool secret seed, you'll never guess it ;) 12345";
    static unsigned char tail_hash_chain_element[SHA256_DIGEST_LENGTH];

    /* we produce a sha256-chain of 257 elements */
    size_t elements = 257;

    memset(tail_hash_chain_element, 0, SHA256_DIGEST_LENGTH);
    TEST_ASSERT(sha256_chain((unsigned char*)strSeed, strlen(strSeed),
                             elements, tail_hash_chain_element) != NULL);

    /* we check if the first element is part of the chain */
    unsigned char element_one[SHA256_DIGEST_LENGTH];
    sha256((unsigned char*)strSeed, strlen(strSeed), element_one);
    TEST_ASSERT(sha256_chain_verify_element(element_one, 0,
                                            tail_hash_chain_element, elements) == 0);

    /* now we check if the test fails if the index is wrong */
    TEST_ASSERT(sha256_chain_verify_element(element_one, 2,
                                            tail_hash_chain_element, elements) == 1);

    /* now we check if other elements are also part of the chain */
    unsigned char tmp_element[SHA256_DIGEST_LENGTH];
    memcpy((void*)tmp_element, (void*)element_one, SHA256_DIGEST_LENGTH);

    /* since we know the seed we build every element and test if its in the chain */
    for (size_t i = 1; i < elements; ++i) {
        sha256_context_t ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, tmp_element, SHA256_DIGEST_LENGTH);
        sha256_final(tmp_element, &ctx);

        TEST_ASSERT(sha256_chain_verify_element(tmp_element, i,
                                                tail_hash_chain_element, elements) == 0);
    }
}

static void test_sha256_hash_chain_with_waypoints(void)
{
    const char strSeed[] = "My cool secret seed, you'll never guess it ;P 123456!";
    static unsigned char tail_hash_chain_element[SHA256_DIGEST_LENGTH];

    /* we produce a sha256-chain of 257 elements */
    size_t elements = 257;

    /* the first element of the hash chain */
    unsigned char element_one[SHA256_DIGEST_LENGTH];
    sha256((unsigned char*)strSeed, strlen(strSeed), element_one);

    /* now we check storing some waypoints, lets say 10 */
    size_t waypoints_length = 10;
    sha256_chain_idx_elm_t waypoints[waypoints_length];
    memset(tail_hash_chain_element, 0, SHA256_DIGEST_LENGTH);

    sha256_chain_with_waypoints((unsigned char*)strSeed,
                                strlen(strSeed),
                                elements,
                                tail_hash_chain_element,
                                waypoints,
                                &waypoints_length);

    /* we test if the chain has been computed properly */
    TEST_ASSERT(sha256_chain_verify_element(element_one, 0,
                                            tail_hash_chain_element, elements) == 0);

    /* and we check if our waypoints are properly stored */
    for (size_t i = 0; i < (waypoints_length + 1); ++i) {
        TEST_ASSERT(sha256_chain_verify_element(waypoints[i].element,
                                                waypoints[i].index,
                                                tail_hash_chain_element, elements) == 0);
    }
}

static void test_sha256_hash_chain_store_whole(void)
{
    const char strSeed[] = "My cool secret seed, you'll never guess it ;P 123456!";
    static unsigned char tail_hash_chain_element[SHA256_DIGEST_LENGTH];

    /* now we check storing the whole chain
     * not a too large one though to remain inside the stack bounds
     */
    size_t elements = 17;

    /* the first element of the hash chain */
    unsigned char element_one[SHA256_DIGEST_LENGTH];
    sha256((unsigned char*)strSeed, strlen(strSeed), element_one);

    size_t whole_chain_length = elements;
    sha256_chain_idx_elm_t waypoints_whole_chain[whole_chain_length];

    memset(tail_hash_chain_element, 0, SHA256_DIGEST_LENGTH);

    sha256_chain_with_waypoints((unsigned char*)strSeed,
                                strlen(strSeed),
                                whole_chain_length,
                                tail_hash_chain_element,
                                waypoints_whole_chain,
                                &whole_chain_length);

    /* we test again if the chain has been computed properly */
    TEST_ASSERT(sha256_chain_verify_element(element_one, 0,
                                            tail_hash_chain_element, elements) == 0);

    /* and we check if our complete chain has been properly stored */
    TEST_ASSERT( (whole_chain_length + 1) == elements );

    for (size_t i = 0; i < (whole_chain_length + 1); ++i) {
        TEST_ASSERT(sha256_chain_verify_element(waypoints_whole_chain[i].element,
                                                waypoints_whole_chain[i].index,
                                                tail_hash_chain_element, elements) == 0);
    }
}

Test *tests_hashes_sha256_chain_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sha256_hash_chain),
        new_TestFixture(test_sha256_hash_chain_with_waypoints),
        new_TestFixture(test_sha256_hash_chain_store_whole),
    };

    EMB_UNIT_TESTCALLER(hashes_sha256_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha256_tests;
}
