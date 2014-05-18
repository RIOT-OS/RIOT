/*
 * Copyright (C) 2014 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-crypto.h
 * @brief       Unittests for the ``crypto`` module
 *
 * @author      Nico von Geyso
 */
#ifndef __TESTS_CRYPTO_H_
#define __TESTS_CRYPTO_H_

#include "embUnit/embUnit.h"
#include "../unittests.h"
#include "kernel.h"

static inline int compare(uint8_t a[16], uint8_t b[16], uint8_t len)
{
    int result = 1;

    for (uint8_t i = 0; i < len; ++i)
        result &= a[i] == b[i];

    return result;
}


Test *tests_crypto_aes_tests(void);
Test *tests_crypto_null_tests(void);
Test *tests_crypto_aes_ccm_tests(void);

#endif /* __TESTS_CRYPTO_H_ */
/** @} */
