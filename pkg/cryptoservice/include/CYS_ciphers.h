/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AES 128 context, as required by the PSA cipher API
 *
 * The secure world keeps no per operation state, so nothing is stored here.
 */
typedef struct {
    uint8_t unused[0];  /**< placeholder, the struct carries no state */
} psa_cipher_aes_128_ctx_t;

#ifdef __cplusplus
}
#endif
