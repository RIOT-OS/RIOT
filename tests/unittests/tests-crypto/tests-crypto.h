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
 * @brief   Generates tests for crypto/c25519.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_c25519_tests(void);

/**
 * @brief   Generates tests for crypto/ed25519.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_ed25519_tests(void);

/**
 * @brief   Generates tests for crypto/edsign.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_edsign_tests(void);

/**
 * @brief   Generates tests for crypto/morph25519.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_morph25519_tests(void);

/**
 * @brief   Generates tests for crypto/sha256.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_sha256_tests(void);

/**
 * @brief   Generates tests for crypto/sha512.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_sha512_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_H_ */
/** @} */
