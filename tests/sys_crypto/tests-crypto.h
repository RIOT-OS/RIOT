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
#ifndef TESTS_CRYPTO_H
#define TESTS_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generates tests for helper functions
 *
 * @return  embUnit tests
 */
Test *tests_crypto_helper_tests(void);
/**
 * @brief   Generates tests for crypto/chacha.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_crypto_chacha_tests(void);

Test *tests_crypto_poly1305_tests(void);

Test *tests_crypto_chacha20poly1305_tests(void);

static inline int compare(const uint8_t *a, const uint8_t *b, uint8_t len)
{
    int result = 1;

    for (uint8_t i = 0; i < len; ++i) {
        result &= a[i] == b[i];
    }

    return result;
}

Test* tests_crypto_aes_tests(void);
Test* tests_crypto_cipher_tests(void);
Test* tests_crypto_modes_ccm_tests(void);
Test* tests_crypto_modes_ccm_tests_128(void);
Test* tests_crypto_modes_ccm_tests_192(void);
Test* tests_crypto_modes_ccm_tests_256(void);
Test* tests_crypto_modes_ocb_tests(void);
Test* tests_crypto_modes_ecb_tests(void);
Test* tests_crypto_modes_cbc_tests(void);
Test* tests_crypto_modes_ctr_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_CRYPTO_H */
/** @} */
