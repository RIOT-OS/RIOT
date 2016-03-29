/*
 * Copyright (C) 2014 Philipp Rosenkranz
 * Copyright (C) 2014 Nico von Geyso
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

#include <stddef.h>
#include <stdint.h>

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The entry point of this test suite.
 */
void tests_crypto(void);

/**
 * @brief   Generates tests for crypto/chacha.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_chacha_tests(void);

static inline int compare(uint8_t a[16], uint8_t b[16], uint8_t len)
{
    int result = 1;

    for (uint8_t i = 0; i < len; ++i) {
        result &= a[i] == b[i];
    }

    return result;
}

Test* tests_crypto_aes_tests(void);
Test* tests_crypto_3des_tests(void);
Test* tests_crypto_twofish_tests(void);
Test* tests_crypto_cipher_tests(void);
Test* tests_crypto_modes_ccm_tests(void);
Test* tests_crypto_modes_ecb_tests(void);
Test* tests_crypto_modes_cbc_tests(void);
Test* tests_crypto_modes_ctr_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_CRYPTO_H_ */
/** @} */
