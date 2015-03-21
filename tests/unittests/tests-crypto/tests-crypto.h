/*
 * Copyright (C) 2014 Philipp Rosenkranz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-crypto.h
 * @brief       Unittests for the ``crypto`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 */
#ifndef __TESTS_CRYPTO_H_
#define __TESTS_CRYPTO_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_crypto(void);

/**
 * @brief   Generates tests for crypto/blake2b.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_blake2b_tests(void);

/**
 * @brief   Generates tests for crypto/blake2s.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_blake2s_tests(void);

/**
 * @brief   Generates tests for crypto/groestl256.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_groestl256_tests(void);

/**
 * @brief   Generates tests for crypto/groestl512.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_groestl512_tests(void);

/**
 * @brief   Generates tests for crypto/sha256.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_sha256_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_H_ */
/** @} */
