/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``crypto`` module
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#include <stddef.h>
#include <stdint.h>

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int compare(const uint8_t *a, const uint8_t *b, uint8_t len)
{
    int result = 1;

    for (uint8_t i = 0; i < len; ++i) {
        result &= a[i] == b[i];
    }

    return result;
}

Test *tests_crypto_modes_ccm_tests_128(void);
Test *tests_crypto_modes_ccm_tests_192(void);
Test *tests_crypto_modes_ccm_tests_256(void);

#ifdef __cplusplus
}
#endif

/** @} */
