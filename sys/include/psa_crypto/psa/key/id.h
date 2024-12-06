/*
 * Copyright (C) 2024 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        key/id.h
 * @brief       Key ID definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_KEY_ID_H
#define PSA_CRYPTO_PSA_KEY_ID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Key identifier.
 *
 * @details A key identifier can be a permanent name for a persistent key, or a transient reference
 *          to volatile key.
 */
typedef uint32_t psa_key_id_t;

/**
 * @brief   The null key identifier.
 *
 * @details The null key identifier is always invalid, except when used without in a call to
 *          @ref psa_destroy_key() which will return @ref PSA_SUCCESS.
 */
#define PSA_KEY_ID_NULL ((psa_key_id_t)0)

/**
 * @brief The minimum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MIN ((psa_key_id_t)0x00000001)

/**
 * @brief The maximum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MAX ((psa_key_id_t)0x3fffffff)

/**
 * @brief The minimum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MIN ((psa_key_id_t)0x40000000)

/**
 * @brief The maximum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MAX ((psa_key_id_t)0x7fffffff)

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_KEY_ID_H */
/** @} */
