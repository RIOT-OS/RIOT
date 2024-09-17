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
 * @{
 *
 * @file        psa_generic_hmac.h
 * @brief       Function declarations for generic PSA Crypto HMAC implementation.
 *
 * @author      Armin Wolf <armin.wolf@mailbox.tu-dresden.de>
 *
 */

#ifndef PSA_GENERIC_HMAC_H
#define PSA_GENERIC_HMAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/crypto.h"
#include "psa/crypto_contexts.h"

/**
 * @brief   Low level function to compute a generic hmac
 *          See @ref psa_mac_compute()
 *
 * @param   key_data
 * @param   key_length
 * @param   alg
 * @param   input
 * @param   input_length
 * @param   mac
 * @param   mac_size
 * @param   mac_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_compute(const uint8_t *key_data,
                                      size_t key_length,
                                      psa_algorithm_t alg,
                                      const uint8_t *input,
                                      size_t input_length,
                                      uint8_t *mac,
                                      size_t mac_size,
                                      size_t *mac_length);

/**
 * @brief   Low level function to verify a generic hmac
 *          See @ref psa_mac_verify()
 *
 * @param   key_data
 * @param   key_length
 * @param   alg
 * @param   input
 * @param   input_length
 * @param   mac
 * @param   mac_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_verify(const uint8_t *key_data,
                                     size_t key_length,
                                     psa_algorithm_t alg,
                                     const uint8_t *input,
                                     size_t input_length,
                                     const uint8_t *mac,
                                     size_t mac_length);

/**
 * @brief   Low level function to initialize the generic hmac
 *          See @ref psa_mac_sign_setup() and @ref psa_mac_verify_setup()
 *
 * @param   operation
 * @param   key_buffer
 * @param   key_buffer_size
 * @param   alg
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_setup(psa_mac_operation_t *operation,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    psa_algorithm_t alg);

/**
 * @brief   Low level function to feed data into the generic hmac
 *          See @ref psa_mac_update()
 *
 * @param   operation
 * @param   input
 * @param   input_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_update(psa_mac_operation_t *operation,
                                     const uint8_t *input,
                                     size_t input_length);

/**
 * @brief   Low level function to finish the generic hmac
 *          See @ref psa_mac_sign_finish()
 *
 * @param   operation
 * @param   mac
 * @param   mac_size
 * @param   mac_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_sign_finish(psa_mac_operation_t *operation,
                                          uint8_t *mac,
                                          size_t mac_size,
                                          size_t *mac_length);

/**
 * @brief   Low level function to verify the generic hmac
 *          See @ref psa_mac_verify_finish()
 * @param   operation
 * @param   mac
 * @param   mac_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_verify_finish(psa_mac_operation_t *operation,
                                            const uint8_t *mac,
                                            size_t mac_length);

/**
 * @brief   Low level function to abort the generic hmac
 *          See @ref psa_mac_abort()
 * @param   operation
 * @return  @ref psa_status_t
 */
psa_status_t psa_generic_hmac_abort(psa_mac_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif /* PSA_GENERIC_HMAC_H */
/**@}*/
