/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Central test setup file for the hashing module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "tests-hashes.h"

void tests_hashes(void)
{
    TESTS_RUN(tests_hashes_md5_tests());
    TESTS_RUN(tests_hashes_cmac_tests());
    TESTS_RUN(tests_hashes_sha1_tests());
    TESTS_RUN(tests_hashes_sha224_tests());
    TESTS_RUN(tests_hashes_sha256_tests());
    TESTS_RUN(tests_hashes_sha256_hmac_tests());
    TESTS_RUN(tests_hashes_sha256_chain_tests());
    TESTS_RUN(tests_hashes_sha384_tests());
    TESTS_RUN(tests_hashes_sha512_tests());
    TESTS_RUN(tests_hashes_sha512_224_tests());
    TESTS_RUN(tests_hashes_sha512_256_tests());
    TESTS_RUN(tests_hashes_sha3_tests());
}
