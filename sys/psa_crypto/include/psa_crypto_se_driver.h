/*
 * Copyright The Mbed TLS Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_se_driver    PSA Crypto Secure Element Wrapper
 * @{
 *
 * @file        psa_crypto_se_driver.h
 * @brief       PSA external cryptoprocessor driver module
 *
 * @details     This header declares types and function signatures for cryptography
 *              drivers that access key material via opaque references.
 *              This is meant for cryptoprocessors that have a separate key storage from the
 *              space in which the PSA Crypto implementation runs, typically secure
 *              elements (SEs).
 *
 *              This file is part of the PSA Crypto Driver HAL (hardware abstraction layer),
 *              containing functions for driver developers to implement to enable hardware
 *              to be called in a standardized way by a PSA Cryptography API
 *              implementation. The functions comprising the driver HAL, which driver
 *              authors implement, are not intended to be called by application developers.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "psa/crypto_values.h"
#include "psa/crypto_types.h"

/**
 * @brief   Maximum size of persistent driver data in bytes
 */
#define PSA_MAX_PERSISTENT_DATA_SIZE    (16)

/**
 * @defgroup    se_init Secure Element Driver Initialization
 */
/**@{*/

/**
 * @brief   Driver context structure
 *
 * @details Driver functions receive a pointer to this structure.
 *          Each registered driver has one instance of this structure.
 *
 *          Implementations must include the fields specified here and
 *          may include other fields.
 */
typedef struct {
    /**
     * A read-only pointer to the driver's persistent data.
     *
     * Drivers typically use this persistent data to keep track of
     * which slot numbers are available. This is only a guideline:
     * drivers may use the persistent data for any purpose, keeping
     * in mind the restrictions on when the persistent data is saved
     * to storage: the persistent data is only saved after calling
     * certain functions that receive a writable pointer to the
     * persistent data.
     *
     * The core allocates a memory buffer for the persistent data.
     * The pointer is guaranteed to be suitably aligned for any data type,
     * like a pointer returned by @c malloc (but the core can use any
     * method to allocate the buffer, not necessarily @c malloc).
     *
     * The size of this buffer is in the @c persistent_data_size field of
     * this structure.
     *
     * Before the driver is initialized for the first time, the content of
     * the persistent data is all-bits-zero. After a driver upgrade, if the
     * size of the persistent data has increased, the original data is padded
     * on the right with zeros; if the size has decreased, the original data
     * is truncated to the new size.
     *
     * This pointer is to read-only data. Only a few driver functions are
     * allowed to modify the persistent data. These functions receive a
     * writable pointer. These functions are:
     * - @ref psa_drv_se_t::p_init
     * - @ref psa_drv_se_key_management_t::p_allocate
     * - @ref psa_drv_se_key_management_t::p_destroy
     *
     * The PSA Cryptography core saves the persistent data from one
     * session to the next. It does this before returning from API functions
     * that call a driver method that is allowed to modify the persistent
     * data, specifically:
     * - @ref psa_crypto_init() causes a call to @ref psa_drv_se_t::p_init, and may call
     *   @ref psa_drv_se_key_management_t::p_destroy to complete an action that was
     *   interrupted by a power failure.
     * - Key creation functions cause a call to @ref psa_drv_se_key_management_t::p_allocate,
     *   and may cause a call to @ref psa_drv_se_key_management_t::p_destroy in case an error
     *   occurs.
     * - @ref psa_destroy_key() causes a call to @ref psa_drv_se_key_management_t::p_destroy.
     */
    const void * persistent_data;

    /**
     * The size of @c persistent_data in bytes.
     *
     * This is always equal to the value of the @c persistent_data_size field
     * of the @ref psa_drv_se_t structure when the driver is registered.
     */
    const size_t persistent_data_size;

    /**
     * Driver transient data.
     *
     * The core initializes this value to 0 and does not read or modify it
     * afterwards. The driver may store whatever it wants in this field.
     */
    uintptr_t transient_data;
} psa_drv_se_context_t;

/**
 * @brief   A driver initialization function.
 *
 * @param   drv_context     The driver context structure.
 * @param   persistent_data A pointer to the persistent data
 *                          that allows writing.
 * @param   location        The location value for which this driver
 *                          is registered. The driver will be invoked
 *                          for all keys whose lifetime is in this
 *                          location.
 *
 * @return  @ref PSA_SUCCESS    The driver is operational.
 *                              The core will update the persistent data in storage.
 *                              Any other return value prevents the driver from being used in
 *                              this session.
 *                              The core will NOT update the persistent data in storage.
 */
typedef psa_status_t (*psa_drv_se_init_t)(psa_drv_se_context_t *drv_context,
                                          void *persistent_data,
                                          psa_key_location_t location);

/**
 * @brief   Encoding of a key slot number on a secure element.
 */
typedef uint64_t psa_key_slot_number_t;
/**@}*/

/**
 * @defgroup    se_mac Secure Element Message Authentication Codes
 *
 * @details     Generation and authentication of Message Authentication Codes (MACs)
 *              using a secure element can be done either as a single function call
 *              (via the `psa_drv_se_mac_generate_t` or `psa_drv_se_mac_verify_t` functions),
 *              or in parts using the following sequence:
 *              - `psa_drv_se_mac_setup_t`
 *              - `psa_drv_se_mac_update_t`
 *              - `psa_drv_se_mac_update_t`
 *              - ...
 *              - `psa_drv_se_mac_finish_t` or `psa_drv_se_mac_finish_verify_t`
 *
 *              If a previously started secure element MAC operation needs to be terminated,
 *              it should be done so by the `psa_drv_se_mac_abort_t`. Failure to do so may
 *              result in allocated resources not being freed or in other undefined
 *              behavior.
 * @{
 */

/**
 * @brief   A function that starts a secure element MAC operation for a PSA
 *          Crypto Driver implementation
 *
 * @param   drv_context The driver context structure.
 * @param   op_context  A structure that will contain the
 *                      hardware-specific MAC context
 * @param   key_slot    The slot of the key to be used for the
 *                      operation
 * @param   algorithm   The algorithm to be used to underly the MAC
 *                      operation
 *
 * @return  @ref PSA_SUCCESS    Success.
 *
 */
typedef psa_status_t (*psa_drv_se_mac_setup_t)(psa_drv_se_context_t *drv_context,
                                               void *op_context,
                                               psa_key_slot_number_t key_slot,
                                               psa_algorithm_t algorithm);

/**
 * @brief   A function that continues a previously started secure element MAC
 *          operation
 *
 * @param   op_context      A hardware-specific structure for the
 *                          previously-established MAC operation to be
 *                          updated
 * @param   p_input         A buffer containing the message to be appended
 *                          to the MAC operation
 * @param   input_length    The size in bytes of the input message buffer
 */
typedef psa_status_t (*psa_drv_se_mac_update_t)(void *op_context,
                                                const uint8_t *p_input,
                                                size_t input_length);

/**
 * @brief   A function that completes a previously started secure element MAC
 *          operation by returning the resulting MAC.
 *
 * @param   op_context      A hardware-specific structure for the
 *                          previously started MAC operation to be
 *                          finished
 * @param   p_mac           A buffer where the generated MAC will be
 *                          placed
 * @param   mac_size        The size in bytes of the buffer that has been
 *                          allocated for the @c output buffer
 * @param   p_mac_length    After completion, will contain the number of
 *                          bytes placed in the @c p_mac buffer
 *
 * @return  @ref PSA_SUCCESS    Success.
 */
typedef psa_status_t (*psa_drv_se_mac_finish_t)(void *op_context,
                                                uint8_t *p_mac,
                                                size_t mac_size,
                                                size_t *p_mac_length);

/**
 * @brief   A function that completes a previously started secure element MAC
 *          operation by comparing the resulting MAC against a provided value
 *
 * @param   op_context  A hardware-specific structure for the previously
 *                      started MAC operation to be fiinished
 * @param   p_mac       The MAC value against which the resulting MAC
 *                      will be compared against
 * @param   mac_length  The size in bytes of the value stored in @c p_mac
 *
 * @return  @ref PSA_SUCCESS                    The operation completed successfully and the MACs
 *                                              matched each other
 *          @ref PSA_ERROR_INVALID_SIGNATURE    The operation completed successfully, but the
 *                                              calculated MAC did not match the provided MAC
 */
typedef psa_status_t (*psa_drv_se_mac_finish_verify_t)(void *op_context,
                                                       const uint8_t *p_mac,
                                                       size_t mac_length);

/** @brief  A function that aborts a previous started secure element MAC
 *          operation
 *
 * @param   op_context  A hardware-specific structure for the previously
 *                      started MAC operation to be aborted
 */
typedef psa_status_t (*psa_drv_se_mac_abort_t)(void *op_context);

/**
 * @brief   A function that performs a secure element MAC operation in one
 *          command and returns the calculated MAC
 *
 * @param   drv_context     The driver context structure.
 * @param   p_input         A buffer containing the message to be MACed
 * @param   input_length    The size in bytes of  @c p_input
 * @param   key_slot        The slot of the key to be used
 * @param   alg             The algorithm to be used to underlie the MAC
 *                          operation
 * @param   p_mac           A buffer where the generated MAC will be
 *                          placed
 * @param   mac_size        The size in bytes of the @c p_mac buffer
 * @param   p_mac_length    After completion, will contain the number of
 *                          bytes placed in the @c output buffer
 *
 * @return  @ref PSA_SUCCESS    Success.
 */
typedef psa_status_t (*psa_drv_se_mac_generate_t)(psa_drv_se_context_t *drv_context,
                                                  const uint8_t *p_input,
                                                  size_t input_length,
                                                  psa_key_slot_number_t key_slot,
                                                  psa_algorithm_t alg,
                                                  uint8_t *p_mac,
                                                  size_t mac_size,
                                                  size_t *p_mac_length);

/**
 * @brief   A function that performs a secure element MAC operation in one
 *          command and compares the resulting MAC against a provided value
 *
 * @param   drv_context     The driver context structure.
 * @param   p_input         A buffer containing the message to be MACed
 * @param   input_length    The size in bytes of @c input
 * @param   key_slot        The slot of the key to be used
 * @param   alg             The algorithm to be used to underlie the MAC
 *                          operation
 * @param   p_mac           The MAC value against which the resulting MAC will
 *                          be compared against
 * @param   mac_length      The size in bytes of @c p_mac
 *
 * @return  @ref PSA_SUCCESS                    The operation completed successfully and the MACs
 *                                              matched each other
 *          @ref PSA_ERROR_INVALID_SIGNATURE    The operation completed successfully, but the
 *                                              calculated MAC did not match the provided MAC
 */
typedef psa_status_t (*psa_drv_se_mac_verify_t)(psa_drv_se_context_t *drv_context,
                                                const uint8_t *p_input,
                                                size_t input_length,
                                                psa_key_slot_number_t key_slot,
                                                psa_algorithm_t alg,
                                                const uint8_t *p_mac,
                                                size_t mac_length);

/**
 * @brief   A struct containing all of the function pointers needed to
 *          perform secure element MAC operations
 *
 * @details PSA Crypto API implementations should populate the table as appropriate
 *          upon startup.
 *
 *          If one of the functions is not implemented (such as
 *          @ref psa_drv_se_mac_generate_t), it should be set to NULL.
 *
 *          Driver implementers should ensure that they implement all of the functions
 *          that make sense for their hardware, and that they provide a full solution
 *          (for example, if they support @c p_setup, they should also support
 *          @c p_update and at least one of @c p_finish or @c p_finish_verify ).
 *
 */
typedef struct {
    /** The size in bytes of the hardware-specific secure element MAC context structure */
    size_t context_size;
    /** Function that performs a MAC setup operation */
    psa_drv_se_mac_setup_t p_setup;
    /** Function that performs a MAC update operation */
    psa_drv_se_mac_update_t p_update;
    /** Function that completes a MAC operation */
    psa_drv_se_mac_finish_t p_finish;
    /** Function that completes a MAC operation with a verify check */
    psa_drv_se_mac_finish_verify_t p_finish_verify;
    /** Function that aborts a previoustly started MAC operation */
    psa_drv_se_mac_abort_t p_abort;
    /** Function that performs a MAC operation in one call */
    psa_drv_se_mac_generate_t p_mac;
    /** Function that performs a MAC and verify operation in one call */
    psa_drv_se_mac_verify_t p_mac_verify;
} psa_drv_se_mac_t;
/**@}*/

/**
 * @defgroup    se_cipher Secure Element Symmetric Ciphers
 *
 * @details     Encryption and Decryption using secure element keys in block modes other
 *              than ECB must be done in multiple parts, using the following flow:
 *              - @ref psa_drv_se_cipher_setup_t
 *              - @ref psa_drv_se_cipher_set_iv_t (optional depending upon block mode)
 *              - @ref psa_drv_se_cipher_update_t
 *              - @ref psa_drv_se_cipher_update_t
 *              - ...
 *              - @ref psa_drv_se_cipher_finish_t
 *
 *              If a previously started secure element Cipher operation needs to be
 *              terminated, it should be done so by the @ref psa_drv_se_cipher_abort_t. Failure
 *              to do so may result in allocated resources not being freed or in other
 *              undefined behavior.
 *
 *              In situations where a PSA Cryptographic API implementation is using a block
 *              mode not-supported by the underlying hardware or driver, it can construct
 *              the block mode itself, while calling the @ref psa_drv_se_cipher_ecb_t function
 *              for the cipher operations.
 * @{
 */

/**
 * @brief   A function that provides the cipher setup function for a
 *          secure element driver
 *
 * @param   drv_context The driver context structure.
 * @param   op_context  A structure that will contain the
 *                      hardware-specific cipher context.
 * @param   key_slot    The slot of the key to be used for the
 *                      operation
 * @param   algorithm   The algorithm to be used in the cipher
 *                      operation
 * @param   direction   Indicates whether the operation is an encrypt
 *                      or decrypt
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_NOT_SUPPORTED
 */
typedef psa_status_t (*psa_drv_se_cipher_setup_t)(psa_drv_se_context_t *drv_context,
                                                  void *op_context,
                                                  psa_key_slot_number_t key_slot,
                                                  psa_algorithm_t algorithm,
                                                  psa_encrypt_or_decrypt_t direction);

/**
 * @brief   A function that sets the initialization vector (if necessary) for an secure element
 *          cipher operation
 *
 * @details Rationale: The @c psa_se_cipher_* operation in the PSA Cryptographic API has
 *          two IV functions: one to set the IV, and one to generate it internally. The
 *          generate function is not necessary for the drivers to implement as the PSA
 *          Crypto implementation can do the generation using its RNG features.
 *
 * @param   op_context  A structure that contains the previously set up
 *                      hardware-specific cipher context
 * @param   p_iv        A buffer containing the initialization vector
 * @param   iv_length   The size (in bytes) of the @c p_iv buffer
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_cipher_set_iv_t)(void *op_context,
                                                   const uint8_t *p_iv,
                                                   size_t iv_length);

/**
 * @brief   A function that continues a previously started secure element cipher
 *          operation
 *
 * @param   op_context      A hardware-specific structure for the
 *                          previously started cipher operation
 * @param   p_input         A buffer containing the data to be
 *                          encrypted/decrypted
 * @param   input_size      The size in bytes of the buffer pointed to
 *                          by @c p_input
 * @param   p_output        The caller-allocated buffer where the
 *                          output will be placed
 * @param   output_size     The allocated size in bytes of the
 *                          @c p_output buffer
 * @param   p_output_length After completion, will contain the number
 *                          of bytes placed in the @c p_output buffer
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_cipher_update_t)(void *op_context,
                                                   const uint8_t *p_input,
                                                   size_t input_size,
                                                   uint8_t *p_output,
                                                   size_t output_size,
                                                   size_t *p_output_length);

/**
 * @brief   A function that completes a previously started secure element cipher
 *          operation
 *
 * @param   op_context      A hardware-specific structure for the
 *                          previously started cipher operation
 * @param   p_output        The caller-allocated buffer where the output
 *                          will be placed
 * @param   output_size     The allocated size in bytes of the @c p_output
 *                          buffer
 * @param   p_output_length After completion, will contain the number of
 *                          bytes placed in the @c p_output buffer
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_cipher_finish_t)(void *op_context,
                                                   uint8_t *p_output,
                                                   size_t output_size,
                                                   size_t *p_output_length);

/**
 * @brief   A function that aborts a previously started secure element cipher
 *          operation
 *
 * @param   op_context  A hardware-specific structure for the
 *                      previously started cipher operation
 */
typedef psa_status_t (*psa_drv_se_cipher_abort_t)(void *op_context);

/**
 * @brief   A function that performs the ECB block mode for secure element
 *          cipher operations
 *
 * @note    This function should only be used with implementations that do not
 *          provide a needed higher-level operation.
 *
 * @param   drv_context The driver context structure.
 * @param   key_slot    The slot of the key to be used for the operation
 * @param   algorithm   The algorithm to be used in the cipher operation
 * @param   direction   Indicates whether the operation is an encrypt or
 *                      decrypt
 * @param   p_input     A buffer containing the data to be
 *                      encrypted/decrypted
 * @param   input_size  The size in bytes of the buffer pointed to by
 *                      @c input
 * @param   p_output    The caller-allocated buffer where the output
 *                      will be placed
 * @param   output_size The allocated size in bytes of the @c output
 *                      buffer
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_NOT_SUPPORTED
 */
typedef psa_status_t (*psa_drv_se_cipher_ecb_t)(psa_drv_se_context_t *drv_context,
                                                psa_key_slot_number_t key_slot,
                                                psa_algorithm_t algorithm,
                                                psa_encrypt_or_decrypt_t direction,
                                                const uint8_t *p_input,
                                                size_t input_size,
                                                uint8_t *p_output,
                                                size_t output_size);

/**
 * @brief   A struct containing all of the function pointers needed to implement
 *          cipher operations using secure elements.
 *
 * @details PSA Crypto API implementations should populate instances of the table as
 *          appropriate upon startup or at build time.
 *
 *          If one of the functions is not implemented (such as
 *          @c psa_drv_se_cipher_ecb_t ), it should be set to NULL.
 */
typedef struct {
    /** The size in bytes of the hardware-specific secure element cipher context structure */
    size_t context_size;
    /** Function that performs a cipher setup operation */
    psa_drv_se_cipher_setup_t p_setup;
    /** Function that sets a cipher IV (if necessary) */
    psa_drv_se_cipher_set_iv_t p_set_iv;
    /** Function that performs a cipher update operation */
    psa_drv_se_cipher_update_t p_update;
    /** Function that completes a cipher operation */
    psa_drv_se_cipher_finish_t p_finish;
    /** Function that aborts a cipher operation */
    psa_drv_se_cipher_abort_t p_abort;
    /**
     * Function that performs ECB mode for a cipher operation
     * @warning ECB mode should not be used directly by clients of the PSA
     *          Crypto Client API)
     */
    psa_drv_se_cipher_ecb_t p_ecb;
} psa_drv_se_cipher_t;

/**@}*/

/**
 * @defgroup    se_asymmetric Secure Element Asymmetric Cryptography
 *
 * @details     Since the amount of data that can (or should) be encrypted or signed using
 *              asymmetric keys is limited by the key size, asymmetric key operations using
 *              keys in a secure element must be done in single function calls.
 * @{
 */

/**
 * @brief   A function that signs a hash or short message with a private key in
 *          a secure element
 *
 * @param   drv_context         The driver context structure.
 * @param   key_slot            Key slot of an asymmetric key pair
 * @param   alg                 A signature algorithm that is compatible
 *                              with the type of @c key
 * @param   p_hash              The hash to sign
 * @param   hash_length         Size of the @c p_hash buffer in bytes
 * @param   p_signature         Buffer where the signature is to be written
 * @param   signature_size      Size of the @c p_signature buffer in bytes
 * @param   p_signature_length  On success, the number of bytes
 *                              that make up the returned signature value
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_asymmetric_sign_t)(   psa_drv_se_context_t *drv_context,
                                                        psa_key_slot_number_t key_slot,
                                                        psa_algorithm_t alg,
                                                        const uint8_t *p_hash,
                                                        size_t hash_length,
                                                        uint8_t *p_signature,
                                                        size_t signature_size,
                                                        size_t *p_signature_length);

/**
 * @brief   A function that verifies the signature a hash or short message using
 *          an asymmetric public key in a secure element
 *
 * @param   drv_context         The driver context structure.
 * @param   key_slot            Key slot of a public key or an asymmetric key
 *                              pair
 * @param   alg                 A signature algorithm that is compatible with
 *                              the type of @c key
 * @param   p_hash              The hash whose signature is to be verified
 * @param   hash_length         Size of the @c p_hash buffer in bytes
 * @param   p_signature         Buffer containing the signature to verify
 * @param   signature_length    Size of the @c p_signature buffer in bytes
 *
 * @return  @ref PSA_SUCCESS    The signature is valid.
 */
typedef psa_status_t (*psa_drv_se_asymmetric_verify_t)(psa_drv_se_context_t *drv_context,
                                                       psa_key_slot_number_t key_slot,
                                                       psa_algorithm_t alg,
                                                       const uint8_t *p_hash,
                                                       size_t hash_length,
                                                       const uint8_t *p_signature,
                                                       size_t signature_length);

/**
 * @brief   A function that encrypts a short message with an asymmetric public
 *          key in a secure element
 *
 * @param   drv_context     The driver context structure.
 * @param   key_slot        Key slot of a public key or an asymmetric key pair
 * @param   alg             An asymmetric encryption algorithm that is compatible
 *                          with the type of @c key
 * @param   p_input         The message to encrypt
 * @param   input_length    Size of the @c p_input buffer in bytes
 * @param   p_salt          A salt or label, if supported by the
 *                          encryption algorithm
 *                          If the algorithm does not support a
 *                          salt, pass @c NULL.
 *                          If the algorithm supports an optional
 *                          salt and you do not want to pass a salt,
 *                          pass @c NULL.
 *                          For @ref PSA_ALG_RSA_PKCS1V15_CRYPT, no salt is
 *                          supported.
 * @param   salt_length     Size of the @c p_salt buffer in bytes
 *                          If @c p_salt is @c NULL, pass 0.
 * @param   p_output        Buffer where the encrypted message is to
 *                          be written
 * @param   output_size     Size of the @c p_output buffer in bytes
 * @param   p_output_length On success, the number of bytes that make up
 *                          the returned output
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_asymmetric_encrypt_t)(psa_drv_se_context_t *drv_context,
                                                        psa_key_slot_number_t key_slot,
                                                        psa_algorithm_t alg,
                                                        const uint8_t *p_input,
                                                        size_t input_length,
                                                        const uint8_t *p_salt,
                                                        size_t salt_length,
                                                        uint8_t *p_output,
                                                        size_t output_size,
                                                        size_t *p_output_length);

/**
 * @brief   A function that decrypts a short message with an asymmetric private
 *          key in a secure element.
 *
 * @param   drv_context     The driver context structure.
 * @param   key_slot        Key slot of an asymmetric key pair
 * @param   alg             An asymmetric encryption algorithm that is
 *                          compatible with the type of @c key
 * @param   p_input         The message to decrypt
 * @param   input_length    Size of the @c p_input buffer in bytes
 * @param   p_salt          A salt or label, if supported by the
 *                          encryption algorithm
 *                          If the algorithm does not support a
 *                          salt, pass @c NULL.
 *                          If the algorithm supports an optional
 *                          salt and you do not want to pass a salt,
 *                          pass @c NULL.
 *                          For #PSA_ALG_RSA_PKCS1V15_CRYPT, no salt is
 *                          supported.
 * @param   salt_length     Size of the @c p_salt buffer in bytes
 *                          If @c p_salt is @c NULL, pass 0.
 * @param   p_output        Buffer where the decrypted message is to
 *                          be written
 * @param   output_size     Size of the @c p_output buffer in bytes
 * @param   p_output_length On success, the number of bytes
 *                          that make up the returned output
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_asymmetric_decrypt_t)(psa_drv_se_context_t *drv_context,
                                                        psa_key_slot_number_t key_slot,
                                                        psa_algorithm_t alg,
                                                        const uint8_t *p_input,
                                                        size_t input_length,
                                                        const uint8_t *p_salt,
                                                        size_t salt_length,
                                                        uint8_t *p_output,
                                                        size_t output_size,
                                                        size_t *p_output_length);

/**
 * @brief   A struct containing all of the function pointers needed to implement
 *          asymmetric cryptographic operations using secure elements.
 *
 * @details PSA Crypto API implementations should populate instances of the table as
 *          appropriate upon startup or at build time.
 *
 *          If one of the functions is not implemented, it should be set to @c NULL.
 */
typedef struct {
    /** Function that performs an asymmetric sign operation */
    psa_drv_se_asymmetric_sign_t p_sign;
    /** Function that performs an asymmetric verify operation */
    psa_drv_se_asymmetric_verify_t p_verify;
    /** Function that performs an asymmetric encrypt operation */
    psa_drv_se_asymmetric_encrypt_t p_encrypt;
    /** Function that performs an asymmetric decrypt operation */
    psa_drv_se_asymmetric_decrypt_t p_decrypt;
} psa_drv_se_asymmetric_t;

/**@}*/

/**
 * @defgroup    se_aead Secure Element Authenticated Encryption with Additional Data
 *
 * @details     Authenticated Encryption with Additional Data (AEAD) operations with secure
 *              elements must be done in one function call. While this creates a burden for
 *              implementers as there must be sufficient space in memory for the entire
 *              message, it prevents decrypted data from being made available before the
 *              authentication operation is complete and the data is known to be authentic.
 * @{
 */

/**
 * @brief   A function that performs a secure element authenticated encryption operation
 *
 * @param   drv_context             The driver context structure.
 * @param   key_slot                Slot containing the key to use.
 * @param   algorithm               The AEAD algorithm to compute
 *                                  (@c PSA_ALG_XXX value such that
 *                                  @ref PSA_ALG_IS_AEAD(@p alg) is true)
 * @param   p_nonce                 Nonce or IV to use
 * @param   nonce_length            Size of the @c p_nonce buffer in bytes
 * @param   p_additional_data       Additional data that will be
 *                                  authenticated but not encrypted
 * @param   additional_data_length  Size of @c p_additional_data in bytes
 * @param   p_plaintext             Data that will be authenticated and
 *                                  encrypted
 * @param   plaintext_length        Size of @c p_plaintext in bytes
 * @param   p_ciphertext            Output buffer for the authenticated and
 *                                  encrypted data. The additional data is
 *                                  not part of this output. For algorithms
 *                                  where the encrypted data and the
 *                                  authentication tag are defined as
 *                                  separate outputs, the authentication
 *                                  tag is appended to the encrypted data.
 * @param   ciphertext_size         Size of the @c p_ciphertext buffer in
 *                                  bytes
 * @param   p_ciphertext_length     On success, the size of the output in
 *                                  the @c p_ciphertext buffer
 *
 * @return  @ref PSA_SUCCESS        Success.
 */
typedef psa_status_t (*psa_drv_se_aead_encrypt_t)(psa_drv_se_context_t *drv_context,
                                                  psa_key_slot_number_t key_slot,
                                                  psa_algorithm_t algorithm,
                                                  const uint8_t *p_nonce,
                                                  size_t nonce_length,
                                                  const uint8_t *p_additional_data,
                                                  size_t additional_data_length,
                                                  const uint8_t *p_plaintext,
                                                  size_t plaintext_length,
                                                  uint8_t *p_ciphertext,
                                                  size_t ciphertext_size,
                                                  size_t *p_ciphertext_length);

/**
 * @brief   A function that performs a secure element authenticated decryption operation
 *
 * @param   drv_context             The driver context structure.
 * @param   key_slot                Slot containing the key to use
 * @param   algorithm               The AEAD algorithm to compute
 *                                  (@c PSA_ALG_XXX value such that
 *                                  @ref PSA_ALG_IS_AEAD(@p alg) is true)
 * @param   p_nonce                 Nonce or IV to use
 * @param   nonce_length            Size of the @c p_nonce buffer in bytes
 * @param   p_additional_data       Additional data that has been
 *                                  authenticated but not encrypted
 * @param   additional_data_length  Size of @c p_additional_data in bytes
 * @param   p_ciphertext            Data that has been authenticated and
 *                                  encrypted.
 *                                  For algorithms where the encrypted data
 *                                  and the authentication tag are defined
 *                                  as separate inputs, the buffer must
 *                                  contain the encrypted data followed by
 *                                  the authentication tag.
 * @param   ciphertext_length       Size of @c p_ciphertext in bytes
 * @param   p_plaintext             Output buffer for the decrypted data
 * @param   plaintext_size          Size of the @c p_plaintext buffer in
 *                                  bytes
 * @param   p_plaintext_length      On success, the size of the output in
 *                                  the @c p_plaintext buffer
 *
 * @return  @ref PSA_SUCCESS        Success.
 */
typedef psa_status_t (*psa_drv_se_aead_decrypt_t)(psa_drv_se_context_t *drv_context,
                                                  psa_key_slot_number_t key_slot,
                                                  psa_algorithm_t algorithm,
                                                  const uint8_t *p_nonce,
                                                  size_t nonce_length,
                                                  const uint8_t *p_additional_data,
                                                  size_t additional_data_length,
                                                  const uint8_t *p_ciphertext,
                                                  size_t ciphertext_length,
                                                  uint8_t *p_plaintext,
                                                  size_t plaintext_size,
                                                  size_t *p_plaintext_length);

/**
 * @brief   A struct containing all of the function pointers needed to implement
 *          secure element Authenticated Encryption with Additional Data operations
 *
 * @details PSA Crypto API implementations should populate instances of the table as
 *          appropriate upon startup.
 *
 *          If one of the functions is not implemented, it should be set to NULL.
 */
typedef struct {
    /** Function that performs the AEAD encrypt operation */
    psa_drv_se_aead_encrypt_t p_encrypt;
    /** Function that performs the AEAD decrypt operation */
    psa_drv_se_aead_decrypt_t p_decrypt;
} psa_drv_se_aead_t;
/**@}*/

/**
 * @defgroup    se_key_management Secure Element Key Management
 *
 * @details     Currently, key management is limited to importing keys in the clear,
 *              destroying keys, and exporting keys in the clear.
 *              Whether a key may be exported is determined by the key policies in place
 *              on the key slot.
 *
 * @{
 */

/**
 * An enumeration indicating how a key is created.
 */
typedef enum {
    PSA_KEY_CREATION_IMPORT,    /**< During @ref psa_import_key() */
    PSA_KEY_CREATION_GENERATE,  /**< During @ref psa_generate_key() */
    PSA_KEY_CREATION_DERIVE,    /**< During @ref psa_key_derivation_output_key() */
    PSA_KEY_CREATION_COPY,      /**< During @ref psa_copy_key() */
} psa_key_creation_method_t;

/**
 * @brief   A function that allocates a slot for a key.
 *
 * @details To create a key in a specific slot in a secure element, the core first calls this
 *          function to determine a valid slot number, then calls a function to create the key
 *          material in that slot. In nominal conditions (that is, if no error occurs), the effect
 *          of a call to a key creation function in the PSA Cryptography API with a lifetime that
 *          places the key in a secure element is the following:
 *          -# The core calls @ref psa_drv_se_key_management_t::p_allocate
 *             (or in some implementations @ref
 *             psa_drv_se_key_management_t::p_validate_slot_number). The driver selects (or
 *             validates) a suitable slot number given the key attributes and the state of the
 *             secure element.
 *          -# The core calls a key creation function in the driver.
 *
 *          The key creation functions in the PSA Cryptography API are:
 *          - @ref psa_import_key(), which causes a call to @c p_allocate with
 *            @c method = @ref PSA_KEY_CREATION_IMPORT then a call to
 *            @ref psa_drv_se_key_management_t::p_import.
 *          - @ref psa_generate_key(), which causes a call to @c p_allocate with
 *            @c method = @ref PSA_KEY_CREATION_GENERATE then a call to
 *            @ref psa_drv_se_key_management_t::p_import.
 *          - @ref psa_key_derivation_output_key(), which causes a call to @c p_allocate with
 *            @p method = @ref PSA_KEY_CREATION_DERIVE then a call to
 *            @ref psa_drv_se_key_derivation_t::p_derive.
 *          - @ref psa_copy_key(), which causes a call to @c p_allocate with
 *            @p method = @ref PSA_KEY_CREATION_COPY then a call to
 *            @ref psa_drv_se_key_management_t::p_export.
 *
 *          In case of errors, other behaviors are possible.
 *          - If the PSA Cryptography subsystem dies after the first step, for example because the
 *            device has lost power abruptly, the second step may never happen, or may happen after
 *            a reset and re-initialization. Alternatively, after a reset and re-initialization,
 *            the core may call @ref psa_drv_se_key_management_t::p_destroy on the slot number that
 *            was allocated (or validated) instead of calling a key creation function.
 *          - If an error occurs, the core may call @ref psa_drv_se_key_management_t::p_destroy on
 *            the slot number that was allocated (or validated) instead of calling a key creation
 *            function.
 *
 *          Errors and system resets also have an impact on the driver's persistent
 *          data. If a reset happens before the overall key creation process is
 *          completed (before or after the second step above), it is unspecified
 *          whether the persistent data after the reset is identical to what it
 *          was before or after the call to @c p_allocate (or @c p_validate_slot_number).
 *
 * @param   drv_context     The driver context structure.
 * @param   persistent_data A pointer to the persistent data
 *                          that allows writing.
 * @param   attributes      Attributes of the key.
 * @param   method          The way in which the key is being created.
 * @param   key_slot        Slot where the key will be stored.
 *                          This must be a valid slot for a key of the
 *                          chosen type. It must be unoccupied.
 *
 * @return  @ref PSA_SUCCESS    Success. The core will record @c *key_slot as the key slot where
 *                              the key is stored and will update the persistent data in storage.
 *          @ref PSA_ERROR_NOT_SUPPORTED
 *          @ref PSA_ERROR_INSUFFICIENT_STORAGE
 */
typedef psa_status_t (*psa_drv_se_allocate_key_t)(  psa_drv_se_context_t *drv_context,
                                                    void *persistent_data,
                                                    const psa_key_attributes_t *attributes,
                                                    psa_key_creation_method_t method,
                                                    psa_key_slot_number_t *key_slot);

/**
 * @brief   A function that determines whether a slot number is valid for a key.
 *
 * @details To create a key in a specific slot in a secure element, the core
 *          first calls this function to validate the choice of slot number,
 *          then calls a function to create the key material in that slot.
 *          See the documentation of @ref psa_drv_se_allocate_key_t for more details.
 *
 *          As of the PSA Cryptography API specification version 1.0, there is no way
 *          for applications to trigger a call to this function. However some
 *          implementations offer the capability to create or declare a key in
 *          a specific slot via implementation-specific means, generally for the
 *          sake of initial device provisioning or onboarding. Such a mechanism may
 *          be added to a future version of the PSA Cryptography API specification.
 *
 *          This function may update the driver's persistent data through
 *          @c persistent_data. The core will save the updated persistent data at the
 *          end of the key creation process. See the description of
 *          @ref psa_drv_se_allocate_key_t for more information.
 *
 * @param   drv_context     The driver context structure.
 * @param   persistent_data A pointer to the persistent data
 *                          that allows writing.
 * @param   attributes      Attributes of the key.
 * @param   method          The way in which the key is being created.
 * @param   key_slot        Slot where the key is to be stored.
 *
 * @return  @ref PSA_SUCCESS                The given slot number is valid for a key with the given
 *                                          attributes.
 *          @ref PSA_ERROR_INVALID_ARGUMENT The given slot number is not valid for a key with the
 *                                          given attributes. This includes the case where the slot
 *                                          number is not valid at all.
 *          @ref PSA_ERROR_ALREADY_EXISTS   There is already a key with the specified slot number.
 *                                          Drivers may choose to return this error from the key
 *                                          creation function instead.
 */
typedef psa_status_t (*psa_drv_se_validate_slot_number_t)(  psa_drv_se_context_t *drv_context,
                                                            void *persistent_data,
                                                            const psa_key_attributes_t *attributes,
                                                            psa_key_creation_method_t method,
                                                            psa_key_slot_number_t key_slot);

/**
 * @brief   A function that imports a key into a secure element in binary format
 *
 * @details This function can support any output from @ref psa_export_key(). Refer to the
 *          documentation of @ref psa_export_key() for the format for each key type.
 *
 * @param   drv_context The driver context structure.
 * @param   key_slot    Slot where the key will be stored.
 *                      This must be a valid slot for a key of the
 *                      chosen type. It must be unoccupied.
 * @param   attributes  The key attributes, including the lifetime,
 *                      the key type and the usage policy.
 *                      Drivers should not access the key size stored
 *                      in the attributes: it may not match the
 *                      data passed in @c data.
 *                      Drivers can call @ref psa_get_key_lifetime(),
 *                      @ref psa_get_key_type(), @ref psa_get_key_usage_flags()
 *                      and @ref psa_get_key_algorithm() to access this
 *                      information.
 * @param   data        Buffer containing the key data.
 * @param   data_length Size of the @c data buffer in bytes.
 * @param   bits        On success, the key size in bits. The driver
 *                      must determine this value after parsing the
 *                      key according to the key type.
 *                      This value is not used if the function fails.
 *
 * @return   @ref PSA_SUCCESS   Success.
 */
typedef psa_status_t (*psa_drv_se_import_key_t)(psa_drv_se_context_t *drv_context,
                                                psa_key_slot_number_t key_slot,
                                                const psa_key_attributes_t *attributes,
                                                const uint8_t *data,
                                                size_t data_length,
                                                size_t *bits);

/**
 * @brief   A function that destroys a secure element key and restore the slot to
 *          its default state
 *
 * @details This function destroys the content of the key from a secure element.
 *          Implementations shall make a best effort to ensure that any previous content
 *          of the slot is unrecoverable.
 *
 *          This function returns the specified slot to its default state.
 *
 * @param   drv_context     The driver context structure.
 * @param   persistent_data A pointer to the persistent data
 *                          that allows writing.
 * @param   key_slot        The key slot to erase.
 *
 * @return  @ref PSA_SUCCESS    The slot's content, if any, has been erased.
 */
typedef psa_status_t (*psa_drv_se_destroy_key_t)(   psa_drv_se_context_t *drv_context,
                                                    void *persistent_data,
                                                    psa_key_slot_number_t key_slot);

/**
 * @brief   A function that exports a secure element key in binary format
 *
 * @details The output of this function can be passed to @ref psa_import_key() to
 *          create an equivalent object.
 *
 *          If a key is created with @ref psa_import_key() and then exported with
 *          this function, it is not guaranteed that the resulting data is
 *          identical: the implementation may choose a different representation
 *          of the same key if the format permits it.
 *
 *          This function should generate output in the same format that @ref psa_export_key()
 *          does. Refer to the documentation of @ref psa_export_key() for the format for each key
 *          type.
 *
 * @param   drv_context     The driver context structure.
 * @param   key_slot        Slot whose content is to be exported. This must be an occupied key slot.
 * @param   p_data          Buffer where the key data is to be written.
 * @param   data_size       Size of the @c p_data buffer in bytes.
 * @param   p_data_length   On success, the number of bytes that make up the key data.
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_DOES_NOT_EXIST
 *          @ref PSA_ERROR_NOT_PERMITTED
 *          @ref PSA_ERROR_NOT_SUPPORTED
 *          @ref PSA_ERROR_COMMUNICATION_FAILURE
 *          @ref PSA_ERROR_HARDWARE_FAILURE
 *          @ref PSA_ERROR_CORRUPTION_DETECTED
 */
typedef psa_status_t (*psa_drv_se_export_key_t)(psa_drv_se_context_t *drv_context,
                                                psa_key_slot_number_t key_slot,
                                                uint8_t *p_data,
                                                size_t data_size,
                                                size_t *p_data_length);

/**
 * @brief   A function that generates a symmetric or asymmetric key on a secure element
 *
 * @details If the key type @c type recorded in @c attributes is asymmetric (@ref
 *          PSA_KEY_TYPE_IS_ASYMMETRIC(@c type) = 1), the driver may export the public key at the
 *          time of generation, in the format documented for @ref psa_export_public_key() by
 *          writing it to the @c pubkey buffer.
 *          This is optional, intended for secure elements that output the public key at generation
 *          time and that cannot export the public key later. Drivers that do not need this feature
 *          should leave @c *pubkey_length set to 0 and should implement the
 *          @c p_export_public function. Some implementations do not
 *          support this feature, in which case @c pubkey is @c NULL and @c pubkey_size is 0.
 *
 * @param   drv_context     The driver context structure.
 * @param   key_slot        Slot where the key will be stored. This must be a valid slot for a key
 *                          of the chosen type. It must be unoccupied.
 * @param   attributes      The key attributes, including the lifetime, the key type and size, and
 *                          the usage policy. Drivers can call @ref psa_get_key_lifetime(),
 *                          @ref psa_get_key_type(), @ref psa_get_key_bits(),
 *                          @ref psa_get_key_usage_flags() and @ref psa_get_key_algorithm() to
 *                          access this information.
 * @param   pubkey          A buffer where the driver can write the public key, when generating an
 *                          asymmetric key pair. This is @c NULL when generating a symmetric key or
 *                          if the core does not support exporting the public key at generation
 *                          time.
 * @param   pubkey_size     The size of the @c pubkey buffer in bytes. This is 0 when generating a
 *                          symmetric key or if the core does not support exporting the public key
 *                          at generation time.
 * @param   pubkey_length   On entry, this is always 0. On success, the number of bytes written to
 *                          @c pubkey. If this is 0 or unchanged on return, the core will not read
 *                          the @c pubkey buffer, and will instead call the driver's
 *                          @c p_export_public function to export the
 *                          public key when needed.
 */
typedef psa_status_t (*psa_drv_se_generate_key_t)(  psa_drv_se_context_t *drv_context,
                                                    psa_key_slot_number_t key_slot,
                                                    const psa_key_attributes_t *attributes,
                                                    uint8_t *pubkey, size_t pubkey_size,
                                                    size_t *pubkey_length);

/**
 * @brief   A struct containing all of the function pointers needed to for secure element key
 *          management
 *
 * @details PSA Crypto API implementations should populate instances of the table as
 *          appropriate upon startup or at build time.
 *
 *          If one of the functions is not implemented, it should be set to @c NULL.
 */
typedef struct {
    /** Function that allocates a slot for a key. */
    psa_drv_se_allocate_key_t p_allocate;
    /** Function that checks the validity of a slot for a key. */
    psa_drv_se_validate_slot_number_t p_validate_slot_number;
    /** Function that performs a key import operation */
    psa_drv_se_import_key_t p_import;
    /** Function that performs a generation */
    psa_drv_se_generate_key_t p_generate;
    /** Function that performs a key destroy operation */
    psa_drv_se_destroy_key_t p_destroy;
    /** Function that performs a key export operation */
    psa_drv_se_export_key_t p_export;
    /** Function that performs a public key export operation */
    psa_drv_se_export_key_t p_export_public;
} psa_drv_se_key_management_t;

/**@}*/

/**
 * @defgroup    driver_derivation Secure Element Key Derivation and Agreement
 *
 * @details     Key derivation is the process of generating new key material using an
 *              existing key and additional parameters, iterating through a basic
 *              cryptographic function, such as a hash.
 *              Key agreement is a part of cryptographic protocols that allows two parties
 *              to agree on the same key value, but starting from different original key
 *              material.
 *              The flows are similar, and the PSA Crypto Driver Model uses the same functions
 *              for both of the flows.
 *
 *              There are two different final functions for the flows,
 *              @c psa_drv_se_key_derivation_derive and @c psa_drv_se_key_derivation_export.
 *              @c psa_drv_se_key_derivation_derive is used when the key material should be
 *              placed in a slot on the hardware and not exposed to the caller.
 *              @c psa_drv_se_key_derivation_export is used when the key material should be
 *              returned to the PSA Cryptographic API implementation.
 *
 *              Different key derivation algorithms require a different number of inputs.
 *              Instead of having an API that takes as input variable length arrays, which
 *              can be problemmatic to manage on embedded platforms, the inputs are passed
 *              to the driver via a function, @c psa_drv_se_key_derivation_collateral, that
 *              is called multiple times with different @c collateral_ids. Thus, for a key
 *              derivation algorithm that required 3 parameter inputs, the flow would look
 *              something like:
 *
 *              @code
 *              psa_drv_se_key_derivation_setup(kdf_algorithm, source_key,
 *                                              dest_key_size_bytes);
 *              psa_drv_se_key_derivation_collateral(kdf_algorithm_collateral_id_0,
*                                                    p_collateral_0,
*                                                    collateral_0_size);
 *              psa_drv_se_key_derivation_collateral(kdf_algorithm_collateral_id_1,
 *                                                   p_collateral_1,
 *                                                   collateral_1_size);
 *              psa_drv_se_key_derivation_collateral(kdf_algorithm_collateral_id_2,
 *                                                   p_collateral_2,
 *                                                   collateral_2_size);
 *              psa_drv_se_key_derivation_derive();
 *              @endcode
 *
 *              key agreement example:
 *              @code
 *              psa_drv_se_key_derivation_setup(alg, source_key. dest_key_size_bytes);
 *              psa_drv_se_key_derivation_collateral(DHE_PUBKEY, p_pubkey, pubkey_size);
 *              psa_drv_se_key_derivation_export(p_session_key,
 *                                               session_key_size,
 *                                               &session_key_length);
 *              @endcode
 * @{
 */

/**
 * @brief   A function that Sets up a secure element key derivation operation by specifying the
 *          algorithm and the source key sot
 *
 * @param   drv_context The driver context structure.
 * @param   op_context  A hardware-specific structure containing any
 *                      context information for the implementation
 * @param   kdf_alg     The algorithm to be used for the key derivation
 * @param   source_key  The key to be used as the source material for
 *                      the key derivation
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_key_derivation_setup_t)(psa_drv_se_context_t *drv_context,
                                                          void *op_context,
                                                          psa_algorithm_t kdf_alg,
                                                          psa_key_slot_number_t source_key);

/**
 * @brief   A function that provides collateral (parameters) needed for a secure
 *          element key derivation or key agreement operation
 *
 * @details Since many key derivation algorithms require multiple parameters, it is
 *          expected that this function may be called multiple times for the same
 *          operation, each with a different algorithm-specific `collateral_id`
 *
 * @param   op_context      A hardware-specific structure containing any
 *                          context information for the implementation
 * @param   collateral_id   An ID for the collateral being provided
 * @param   p_collateral    A buffer containing the collateral data
 * @param   collateral_size The size in bytes of the collateral
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_key_derivation_collateral_t)(void *op_context,
                                                               uint32_t collateral_id,
                                                               const uint8_t *p_collateral,
                                                               size_t collateral_size);

/**
 * @brief   A function that performs the final secure element key derivation
 *          step and place the generated key material in a slot
 *
 * @param   op_context  A hardware-specific structure containing any
 *                      context information for the implementation
 * @param   dest_key    The slot where the generated key material should be placed
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_key_derivation_derive_t)(void *op_context,
                                                           psa_key_slot_number_t dest_key);

/**
 * @brief   A function that performs the final step of a secure element key
 *          agreement and place the generated key material in a buffer
 *
 * @param   op_context      A hardware-specific structure containing any
 *                          context information for the implementation
 * @param   p_output        Buffer in which to place the generated key material
 * @param   output_size     The size in bytes of @c p_output
 * @param   p_output_length Upon success, contains the number of bytes of key
 *                          material placed in @c p_output
 *
 * @return  @ref PSA_SUCCESS
 */
typedef psa_status_t (*psa_drv_se_key_derivation_export_t)(void *op_context,
                                                           uint8_t *p_output,
                                                           size_t output_size,
                                                           size_t *p_output_length);

/**
 * @brief   A struct containing all of the function pointers needed to for secure
 *          element key derivation and agreement
 *
 * @details PSA Crypto API implementations should populate instances of the table as
 *          appropriate upon startup.
 *
 *          If one of the functions is not implemented, it should be set to @c NULL.
 */
typedef struct {
    /** The driver-specific size of the key derivation context */
    size_t context_size;
    /** Function that performs a key derivation setup */
    psa_drv_se_key_derivation_setup_t p_setup;
    /** Function that sets key derivation collateral */
    psa_drv_se_key_derivation_collateral_t p_collateral;
    /** Function that performs a final key derivation step */
    psa_drv_se_key_derivation_derive_t p_derive;
    /** Function that perforsm a final key derivation or agreement and exports the key */
    psa_drv_se_key_derivation_export_t p_export;
} psa_drv_se_key_derivation_t;

/**@}*/

/**
 * @defgroup se_registration Secure Element Driver Registration
 * @{
 */

/**
 * @brief   A structure containing pointers to all the entry points of a secure element driver.
 *
 * @details Future versions of this specification may add extra substructures at the end of this
 *          structure.
 */
typedef struct {
    /**
     * The version of the driver HAL that this driver implements.
     * This is a protection against loading driver binaries built against
     * a different version of this specification.
     * Use @ref PSA_DRV_SE_HAL_VERSION.
     */
    uint32_t hal_version;

    /**
     * The size of the driver's persistent data in bytes.
     *
     * This can be 0 if the driver does not need persistent data.
     *
     * See the documentation of @ref psa_drv_se_context_t::persistent_data
     * for more information about why and how a driver can use
     * persistent data.
     */
    size_t persistent_data_size;

    /**
     * The driver initialization function.
     *
     * This function is called once during the initialization of the
     * PSA Cryptography subsystem, before any other function of the
     * driver is called. If this function returns a failure status,
     * the driver will be unusable, at least until the next system reset.
     *
     * If this field is @c NULL, it is equivalent to a function that does
     * nothing and returns @ref PSA_SUCCESS.
     */
    psa_drv_se_init_t p_init;

    const psa_drv_se_key_management_t *key_management;  /**< Key management methods */
    const psa_drv_se_mac_t *mac;                        /**< MAC operation methods */
    const psa_drv_se_cipher_t *cipher;                  /**< Cipher operation methods */
    const psa_drv_se_aead_t *aead;                      /**< AEAD operation methods */
    const psa_drv_se_asymmetric_t *asymmetric;          /**< Asymmetric operation methods */
    const psa_drv_se_key_derivation_t *derivation;      /**< Key derivation methods */
} psa_drv_se_t;
/**@}*/

/**
 * The current version of the secure element driver HAL.
 */
#define PSA_DRV_SE_HAL_VERSION 0x00000005

#ifdef __cplusplus
}
#endif

/** @} */
