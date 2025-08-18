/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_mac  PSA Wrapper Functions: MAC
 * @{
 *
 * @file        psa_mac.h
 * @brief       Function declarations for low level wrapper functions for MAC operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 * @author      Armin Wolf <armin.wolf@mailbox.tu-dresden.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/crypto.h"

#if IS_USED(MODULE_PSA_MAC_HMAC_MD5) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC MD5 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_md5(const psa_key_attributes_t *attributes,
                                      const uint8_t *key_buffer,
                                      size_t key_buffer_size,
                                      const uint8_t *input,
                                      size_t input_length,
                                      uint8_t *mac,
                                      size_t mac_size,
                                      size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC MD5 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_md5(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size,
                                     const uint8_t *input,
                                     size_t input_length,
                                     const uint8_t *mac,
                                     size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC MD5 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_md5(psa_mac_operation_t *operation,
                                         const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC MD5 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_md5(psa_mac_operation_t *operation,
                                           const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC MD5 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_md5(psa_mac_operation_t *operation,
                                     const uint8_t *input,
                                     size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC MD5 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_md5(psa_mac_operation_t *operation,
                                          uint8_t *mac,
                                          size_t mac_size,
                                          size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC MD5 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_md5(psa_mac_operation_t *operation,
                                            const uint8_t *mac,
                                            size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC MD5 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_md5(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_MD5 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA1 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha1(const psa_key_attributes_t *attributes,
                                       const uint8_t *key_buffer,
                                       size_t key_buffer_size,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *mac,
                                       size_t mac_size,
                                       size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA1 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha1(const psa_key_attributes_t *attributes,
                                      const uint8_t *key_buffer,
                                      size_t key_buffer_size,
                                      const uint8_t *input,
                                      size_t input_length,
                                      const uint8_t *mac,
                                      size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA1 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha1(psa_mac_operation_t *operation,
                                          const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA1 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha1(psa_mac_operation_t *operation,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA1 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha1(psa_mac_operation_t *operation,
                                      const uint8_t *input,
                                      size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA1 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha1(psa_mac_operation_t *operation,
                                           uint8_t *mac,
                                           size_t mac_size,
                                           size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA1 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha1(psa_mac_operation_t *operation,
                                             const uint8_t *mac,
                                             size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA1 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha1(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA_1 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA224 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha224(const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *mac,
                                         size_t mac_size,
                                         size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA224 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha224(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *input,
                                        size_t input_length,
                                        const uint8_t *mac,
                                        size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA224 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha224(psa_mac_operation_t *operation,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA224 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha224(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA224 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha224(psa_mac_operation_t *operation,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA224 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha224(psa_mac_operation_t *operation,
                                             uint8_t *mac,
                                             size_t mac_size,
                                             size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA224 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha224(psa_mac_operation_t *operation,
                                               const uint8_t *mac,
                                               size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA224 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha224(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA_224 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA256 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha256(const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *mac,
                                         size_t mac_size,
                                         size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA256 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha256(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *input,
                                        size_t input_length,
                                        const uint8_t *mac,
                                        size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA256 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha256(psa_mac_operation_t *operation,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA256 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha256(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA256 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha256(psa_mac_operation_t *operation,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA256 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha256(psa_mac_operation_t *operation,
                                             uint8_t *mac,
                                             size_t mac_size,
                                             size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA256 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha256(psa_mac_operation_t *operation,
                                               const uint8_t *mac,
                                               size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA256 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha256(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA_256 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA384 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha384(const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *mac,
                                         size_t mac_size,
                                         size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA384 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha384(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *input,
                                        size_t input_length,
                                        const uint8_t *mac,
                                        size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA384 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha384(psa_mac_operation_t *operation,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA384 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha384(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA384 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha384(psa_mac_operation_t *operation,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA384 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha384(psa_mac_operation_t *operation,
                                             uint8_t *mac,
                                             size_t mac_size,
                                             size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA384 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha384(psa_mac_operation_t *operation,
                                               const uint8_t *mac,
                                               size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA384 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha384(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA_384 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA512 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha512(const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *mac,
                                         size_t mac_size,
                                         size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA512 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha512(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *input,
                                        size_t input_length,
                                        const uint8_t *mac,
                                        size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA512 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha512(psa_mac_operation_t *operation,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA512 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha512(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA512 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha512(psa_mac_operation_t *operation,
                                        const uint8_t *input,
                                        size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA512 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha512(psa_mac_operation_t *operation,
                                             uint8_t *mac,
                                             size_t mac_size,
                                             size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA512 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha512(psa_mac_operation_t *operation,
                                               const uint8_t *mac,
                                               size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA512 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha512(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA_512 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA3-256 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha3_256(const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size,
                                           const uint8_t *input,
                                           size_t input_length,
                                           uint8_t *mac,
                                           size_t mac_size,
                                           size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA3-256 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha3_256(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          const uint8_t *input,
                                          size_t input_length,
                                          const uint8_t *mac,
                                          size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA3-256 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha3_256(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA3-256 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha3_256(psa_mac_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                const uint8_t *key_buffer,
                                                size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA3-256 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha3_256(psa_mac_operation_t *operation,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA3-256 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha3_256(psa_mac_operation_t *operation,
                                               uint8_t *mac,
                                               size_t mac_size,
                                               size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA3-256 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha3_256(psa_mac_operation_t *operation,
                                                 const uint8_t *mac,
                                                 size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA3-256 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha3_256(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA3_256 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA3-384 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha3_384(const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size,
                                           const uint8_t *input,
                                           size_t input_length,
                                           uint8_t *mac,
                                           size_t mac_size,
                                           size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA3-384 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha3_384(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          const uint8_t *input,
                                          size_t input_length,
                                          const uint8_t *mac,
                                          size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA3-384 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha3_384(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA3-384 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha3_384(psa_mac_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                const uint8_t *key_buffer,
                                                size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA3-384 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha3_384(psa_mac_operation_t *operation,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA3-384 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha3_384(psa_mac_operation_t *operation,
                                               uint8_t *mac,
                                               size_t mac_size,
                                               size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA3-384 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha3_384(psa_mac_operation_t *operation,
                                                 const uint8_t *mac,
                                                 size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA3-384 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha3_384(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA3_384 */

#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA3-512 computation
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_mac_compute_hmac_sha3_512(const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size,
                                           const uint8_t *input,
                                           size_t input_length,
                                           uint8_t *mac,
                                           size_t mac_size,
                                           size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA3-512 verification
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_mac_verify_hmac_sha3_512(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          const uint8_t *input,
                                          size_t input_length,
                                          const uint8_t *mac,
                                          size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA3-512 computation
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_mac_sign_setup_hmac_sha3_512(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for setting up a HMAC SHA3-512 verification
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_mac_verify_setup_hmac_sha3_512(psa_mac_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                const uint8_t *key_buffer,
                                                size_t key_buffer_size);

/**
 * @brief   Low level wrapper function to call a driver for updating a HMAC SHA3-512 calculation
 *          See @ref psa_mac_update()
 */
psa_status_t psa_mac_update_hmac_sha3_512(psa_mac_operation_t *operation,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA3-512 computation
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_mac_sign_finish_hmac_sha3_512(psa_mac_operation_t *operation,
                                               uint8_t *mac,
                                               size_t mac_size,
                                               size_t *mac_length);

/**
 * @brief   Low level wrapper function to call a driver for finishing a HMAC SHA3-512 verification
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_mac_verify_finish_hmac_sha3_512(psa_mac_operation_t *operation,
                                                 const uint8_t *mac,
                                                 size_t mac_length);

/**
 * @brief   Low level wrapper function to call a driver for aborting a HMAC SHA3-512 operation
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_mac_abort_hmac_sha3_512(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC_HMAC_SHA3_512 */

#ifdef __cplusplus
}
#endif

/**@}*/
