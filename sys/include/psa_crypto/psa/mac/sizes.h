/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       MAC size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/algorithm.h"
#include "psa/cipher/sizes.h"
#include "psa/hash/algorithm.h"
#include "psa/hash/sizes.h"
#include "psa/mac/algorithm.h"

/**
 * @brief   The size of the output of @ref psa_mac_compute() and @ref psa_mac_sign_finish(),
 *          in bytes.
 *
 * @details If the size of the MAC buffer is at least this large, it is guaranteed that
 *          @ref psa_mac_compute() and @ref psa_mac_sign_finish() will not fail due to an
 *          insufficient buffer size.
 *
 *          This is also the MAC length that @ref psa_mac_verify() and @ref psa_mac_verify_finish()
 *          expect.
 *
 *          See also @ref PSA_MAC_MAX_SIZE.
 *
 * @param   key_type    The type of the MAC key.
 * @param   key_bits    The size of the MAC key in bits.
 * @param   alg         A MAC algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_MAC(@p alg) is true.
 *
 * @return  The MAC length for the specified algorithm with the specified key parameters.
 *          0 if the MAC algorithm is not recognized or not supported.
 *          Unspecified if the key parameters are not consistent with the algorithm.
 */
#define PSA_MAC_LENGTH(key_type, key_bits, alg)  \
    ((PSA_ALG_IS_HMAC(alg)) ? PSA_HASH_LENGTH(PSA_ALG_HMAC_GET_HASH(alg)) :         \
     PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) ? PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) : \
     ((void)(key_type), (void)(key_bits), 0))

/**
 * @brief   A sufficient buffer size for storing the MAC output by @ref psa_mac_verify() and
 *          @ref psa_mac_verify_finish(), for any of the supported key types and MAC algorithms.
 *
 * @details If the size of the MAC buffer is at least this large, it is guaranteed that
 *          @ref psa_mac_verify() and @ref psa_mac_verify_finish() will not fail due to an
 *          insufficient buffer size.
 *
 *          See also @ref PSA_MAC_LENGTH().
 */
#if (IS_USED(MODULE_PSA_MAC_HMAC_SHA_512) || \
     IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_512))    /* 64 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_SHA_384) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_384))    /* 48 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_SHA_256) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA_512_256) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_256))    /* 32 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_SHA_224) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA_512_224) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA3_224))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_224))    /* 28 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_RIPEMD160) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA_1))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA_1))       /* 20 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_MD2) || \
       IS_USED(MODULE_PSA_MAC_HMAC_MD4) || \
       IS_USED(MODULE_PSA_MAC_HMAC_MD5))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_MD5))         /* 16 */
#else
#define PSA_MAC_MAX_SIZE 0
#endif

#ifdef __cplusplus
}
#endif

/** @} */
