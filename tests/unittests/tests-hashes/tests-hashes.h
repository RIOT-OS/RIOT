/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``hashes`` module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_hashes(void);

/**
 * @brief   Generates tests for hashes/md5.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_cmac_tests(void);

/**
 * @brief   Generates tests for hashes/cmac.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_md5_tests(void);

/**
 * @brief   Generates tests for hashes/sha1.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha1_tests(void);

/**
 * @brief   Generates tests for hashes/sha224.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha224_tests(void);

/**
 * @brief   Generates tests for hashes/sha256.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha256_tests(void);

/**
 * @brief   Generates tests for hashes/sha384.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha384_tests(void);

/**
 * @brief   Generates tests for hashes/sha512.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha512_tests(void);

/**
 * @brief   Generates tests for hashes/sha512_224.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha512_224_tests(void);

/**
 * @brief   Generates tests for hashes/sha512_256.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha512_256_tests(void);

/**
 * @brief   Generates tests for hashes/sha256.h - hmac
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha256_hmac_tests(void);

/**
 * @brief   Generates tests for hashes/sha256.h - sha256-chain
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha256_chain_tests(void);

  /**
 * @brief   Generates tests for hashes/sha3.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_hashes_sha3_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
