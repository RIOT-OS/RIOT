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
#endif /* MODULE_PSA_HASH_MD5 */

#if IS_USED(MODULE_PSA_HASH_SHA_1) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA1 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA1 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha1_setup(psa_hashes_sha1_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA1 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA1 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha1_update(psa_hashes_sha1_ctx_t *ctx,
                                    const uint8_t *input,
                                    size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA1 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA1 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha1_finish(psa_hashes_sha1_ctx_t *ctx,
                                    uint8_t *hash,
                                    size_t hash_size,
                                    size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_1 */

#if IS_USED(MODULE_PSA_HASH_SHA_224) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA224 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA224 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha224_setup(psa_hashes_sha224_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA224 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA224 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha224_update(psa_hashes_sha224_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA224 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA224 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha224_finish(psa_hashes_sha224_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_224 */

#if IS_USED(MODULE_PSA_HASH_SHA_256) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA256 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA256 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha256_setup(psa_hashes_sha256_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA256 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA256 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha256_update(psa_hashes_sha256_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA256 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA256 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha256_finish(psa_hashes_sha256_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_256 */

#if IS_USED(MODULE_PSA_HASH_SHA_384) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA384 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA384 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha384_setup(psa_hashes_sha384_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA384 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA384 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha384_update(psa_hashes_sha384_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA384 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA384 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha384_finish(psa_hashes_sha384_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_384 */

#if IS_USED(MODULE_PSA_HASH_SHA_512) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA512 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA512 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_setup(psa_hashes_sha512_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA512 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_update(psa_hashes_sha512_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA512 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_finish(psa_hashes_sha512_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_512 */

#if IS_USED(MODULE_PSA_HASH_SHA_512_224) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA512/224 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA512-224 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_224_setup(psa_hashes_sha512_224_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512/224 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA512-224 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_224_update(psa_hashes_sha512_224_ctx_t *ctx,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512/224 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA512-224 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_224_finish(psa_hashes_sha512_224_ctx_t *ctx,
                                          uint8_t *hash,
                                          size_t hash_size,
                                          size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_512_224 */

#if IS_USED(MODULE_PSA_HASH_SHA_512_256) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an SHA512/256 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA512-256 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_256_setup(psa_hashes_sha512_256_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512/256 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA512-256 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_256_update(psa_hashes_sha512_256_ctx_t *ctx,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for an SHA512/256 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA512-256 context to finalize
 * @param   hash        The computed hash will be stored here
 * @param   hash_size   Size of the buffer in @p hash
 * @param   hash_length Actual number of bytes written to @p hash
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha512_256_finish(psa_hashes_sha512_256_ctx_t *ctx,
                                          uint8_t *hash,
                                          size_t hash_size,
                                          size_t *hash_length);
#endif /* MODULE_PSA_HASH_SHA_512_256 */

#if IS_USED(MODULE_PSA_HASH_SHA3_256) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a general SHA3 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA3-256 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_256_update(psa_hashes_sha3_ctx_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for a SHA3-256 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA3-256 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_256_setup(psa_hashes_sha3_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for a SHA3-256 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA3-256 context to finalize
 * @param   hash        The computed hash will be stored here
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_256_finish(psa_hashes_sha3_ctx_t *ctx,
                                        uint8_t *hash);
#endif /* MODULE_PSA_HASH_SHA3_256 */

#if IS_USED(MODULE_PSA_HASH_SHA3_384) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a general SHA3 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA3-384 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_384_update(psa_hashes_sha3_ctx_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for a SHA3-384 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA3-384 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_384_setup(psa_hashes_sha3_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for a SHA3-384 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA3-384 context to finalize
 * @param   hash        The computed hash will be stored here
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_384_finish(psa_hashes_sha3_ctx_t *ctx,
                                        uint8_t *hash);
#endif /* MODULE_PSA_HASH_SHA3_384 */

#if IS_USED(MODULE_PSA_HASH_SHA3_512) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a general SHA3 hash update
 *          See @ref psa_hash_update()
 *
 * @param   ctx             PSA SHA3-512 context to update
 * @param   input           Data to add
 * @param   input_length    Length of @p input
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_512_update(psa_hashes_sha3_ctx_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for a SHA3-512 hash setup
 *          See @ref psa_hash_setup()
 *
 * @param   ctx         PSA SHA3-384 Context to set up
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_512_setup(psa_hashes_sha3_ctx_t *ctx);

/**
 * @brief   Low level wrapper function to call a driver for a SHA3-512 hash finish
 *          See @ref psa_hash_finish()
 *
 * @param   ctx         PSA SHA3-512 context to finalize
 * @param   hash        The computed hash will be stored here
 * @return  psa_status_t
 */
psa_status_t psa_hashes_sha3_512_finish(psa_hashes_sha3_ctx_t *ctx,
                                        uint8_t *hash);
#endif /* MODULE_PSA_HASH_SHA3_512 */

#ifdef __cplusplus
}
#endif

#endif /* PSA_HASHES_H */
/**@}*/
