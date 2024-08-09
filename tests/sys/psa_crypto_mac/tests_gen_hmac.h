/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_hmac  PSA Generic HMAC
 *
 *
 * @file        tests-gen-hmac.h
 *
 * @author      Daria Zatokovenko <daria.zatokovenko@mailbox.tu-dresden.de>
 *
 */

#ifndef TESTS_GEN_HMAC_H
#define TESTS_GEN_HMAC_H

#include <stdint.h>
#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generates HMAC SHA512 tests for generic HMAC
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_mac_hmac_sha512(void);

/**
 * @brief   Generates HMAC SHA384 tests for generic HMAC
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_mac_hmac_sha384(void);

/**
 * @brief   Generates HMAC SHA256 tests for generic HMAC
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_mac_hmac_sha256(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_GEN_HMAC_H */
/** @} */
