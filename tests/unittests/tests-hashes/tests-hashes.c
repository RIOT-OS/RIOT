/*
 * Copyright (C) 2015 Freie Universität Berlin
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
    TESTS_RUN(tests_hashes_sha1_tests());
    TESTS_RUN(tests_hashes_sha256_tests());
    TESTS_RUN(tests_hashes_sha256_hmac_tests());
    TESTS_RUN(tests_hashes_sha256_chain_tests());
}
