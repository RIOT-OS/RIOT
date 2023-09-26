/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_hashes   PSA Wrapper Functions: Hashes
 * @{
 *
 * @file        psa_hashes.h
 * @brief       Function declarations for low level wrapper functions for hash operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_HASHES_H
#define PSA_HASHES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/crypto.h"
#include "psa/crypto_contexts.h"

#if IS_USED(MODULE_PSA_HASH_MD5) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an MD5 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx
 * @return  @ref psa_status_t
 */
psa_status_t psa_hashes_md5_setup(psa_hashes_md5_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an MD5 hash update
 *          See @ref psa_hash_update()
 *
 * @param ctx
 * @param input
 * @param input_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_md5_update(psa_hashes_md5_ctx_t *ctx,
                                   const uint8_t *input,
                                   size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an MD5 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param ctx
 * @param hash
 * @param hash_size
 * @param hash_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_md5_finish(psa_hashes_md5_ctx_t *ctx,
                                   uint8_t *hash,
                                   size_t hash_size,
                                   size_t *hash_length);
#endif /* CONFIG_HASHES_MD5 */

#if IS_USED(MODULE_PSA_HASH_SHA_1) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA1 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param ctx
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha1_setup(psa_hashes_sha1_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA1 hash update
 *          See @ref psa_hash_update()
 *
 * @param ctx
 * @param input
 * @param input_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha1_update(psa_hashes_sha1_ctx_t *ctx,
                                    const uint8_t *input,
                                    size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA1 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param ctx
 * @param hash
 * @param hash_size
 * @param hash_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha1_finish(psa_hashes_sha1_ctx_t *ctx,
                                    uint8_t *hash,
                                    size_t hash_size,
                                    size_t *hash_length);
#endif /* CONFIG_HASHES_SHA1 */

#if IS_USED(MODULE_PSA_HASH_SHA_224) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA224 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param ctx
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha224_setup(psa_hashes_sha224_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA224 hash update
 *          See @ref psa_hash_update()
 *
 * @param ctx
 * @param input
 * @param input_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha224_update(psa_hashes_sha224_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA224 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param ctx
 * @param hash
 * @param hash_size
 * @param hash_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha224_finish(psa_hashes_sha224_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* CONFIG_HASHES_SHA224 */

#if IS_USED(MODULE_PSA_HASH_SHA_256) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA256 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param ctx
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha256_setup(psa_hashes_sha256_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA256 hash update
 *          See @ref psa_hash_update()
 *
 * @param ctx
 * @param input
 * @param input_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha256_update(psa_hashes_sha256_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA256 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param ctx
 * @param hash
 * @param hash_size
 * @param hash_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha256_finish(psa_hashes_sha256_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* CONFIG_HASHES_SHA256 */

#if IS_USED(MODULE_PSA_HASH_SHA_512) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA512 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param ctx
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha512_setup(psa_hashes_sha512_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512 hash update
 *          See @ref psa_hash_update()
 *
 * @param ctx
 * @param input
 * @param input_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha512_update(psa_hashes_sha512_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param ctx
 * @param hash
 * @param hash_size
 * @param hash_length
 * @return psa_status_t
 */
psa_status_t psa_hashes_sha512_finish(psa_hashes_sha512_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* CONFIG_HASHES_SHA512 */

#ifdef __cplusplus
}
#endif

#endif /* PSA_HASHES_H */
/**@}*/
