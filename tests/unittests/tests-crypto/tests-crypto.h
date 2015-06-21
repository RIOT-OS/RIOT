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
 * @file
 * @brief       Unittests for the ``crypto`` module
 *
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 */
#ifndef TESTS_CRYPTO_H_
#define TESTS_CRYPTO_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_crypto(void);

/**
 * @brief   Generates tests for crypto/sha256.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_sha256_tests(void);

/**
 * @brief   Generates tests for crypto/chacha.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_chacha_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_CRYPTO_H_ */
/** @} */
