/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @defgroup       <name> <description>
 * @{
 *
 * @file        tests_psa_cbor_enc_dec.h
 * @brief
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef TESTS_PSA_PERSISTENT_STORAGE_H
#define TESTS_PSA_PERSISTENT_STORAGE_H

#include <stdint.h>
#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AES_128_KEY_SIZE    (16)

#define TEST_ASSERT_PSA_SUCCESS(func_)          TEST_ASSERT_MESSAGE(func_ == PSA_SUCCESS, \
                                                                                #func_ " failed");
#define TEST_ASSERT_PSA_DOES_NOT_EXIST(func_)   TEST_ASSERT_MESSAGE(func_ == \
                                                    PSA_ERROR_DOES_NOT_EXIST, #func_ " failed");
#define TEST_ASSERT_PSA_ALREADY_EXISTS(func_)   TEST_ASSERT_MESSAGE(func_ == \
                                                    PSA_ERROR_ALREADY_EXISTS, #func_ " failed");

#define AES_128_KEY_SIZE    (16)

static const uint8_t KEY_128[AES_128_KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static const uint8_t OVERWRITE_KEY_128[AES_128_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
};

Test* tests_psa_persistent_single_key_storage(void);
Test* tests_psa_persistent_asym_keypair_storage(void);
Test* tests_psa_fail_overwrite_existing_key(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTS_PSA_PERSISTENT_STORAGE_H */
/** @} */
