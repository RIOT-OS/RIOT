/*
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
 * @file        crypto_contexts.h
 * @brief       Context definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_CONTEXTS_H
#define PSA_CRYPTO_PSA_CRYPTO_CONTEXTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"

#include "psa/crypto_includes.h"


#if IS_USED(MODULE_PSA_HASH)
/**
 * @brief   Structure containing the hash contexts needed by the application.
 */
typedef union {
#if IS_USED(MODULE_PSA_HASH_MD5) || defined(DOXYGEN)
    psa_hashes_md5_ctx_t md5;   /**< MD5 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_1) || defined(DOXYGEN)
    psa_hashes_sha1_ctx_t sha1; /**< SHA-1 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_224) || defined(DOXYGEN)
    psa_hashes_sha224_ctx_t sha224; /**< SHA-224 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_256) || defined(DOXYGEN)
    psa_hashes_sha256_ctx_t sha256; /**< SHA-256 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_512) || defined(DOXYGEN)
    psa_hashes_sha512_ctx_t sha512; /**< SHA-512 context */
#endif
} psa_hash_context_t;
#endif

#if IS_USED(MODULE_PSA_CIPHER)
/**
 * @brief   Structure containing the cipher contexts needed by the application.
 */
typedef union {
#if IS_USED(MODULE_PSA_CIPHER_AES_128_ECB) ||\
    IS_USED(MODULE_PSA_CIPHER_AES_128_CBC) ||\
    defined(DOXYGEN)
    psa_cipher_aes_128_ctx_t aes_128;   /**< AES 128 context*/
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_192_CBC) || defined(DOXYGEN)
    psa_cipher_aes_192_ctx_t aes_192;   /**< AES 192 context*/
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_256_CBC) || defined(DOXYGEN)
    psa_cipher_aes_256_ctx_t aes_256;   /**< AES 256 context*/
#endif
} psa_cipher_context_t;
#endif

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
/**
 * @brief   Structure containing the secure element specific cipher contexts needed by the
 *          application.
 */
typedef struct {
    psa_encrypt_or_decrypt_t direction; /**< Direction of this cipher operation */
    /** Structure containing a driver specific cipher context */
    union driver_context {
        unsigned dummy; /**< Make the union non-empty even with no supported algorithms. */
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A) || defined(DOXYGEN)
        atca_aes_cbc_ctx_t atca_aes_cbc;    /**< ATCA AES CBC context*/
    #endif
    } drv_ctx;  /**< SE specific cipher operation context */
} psa_se_cipher_context_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_CRYPTO_CONTEXTS_H */
/** @} */
