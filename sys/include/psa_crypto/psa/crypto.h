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
 * @file        crypto.h
 * @brief       Function declarations for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @see         https://armmbed.github.io/mbed-crypto/html/index.html
 *
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_H
#define PSA_CRYPTO_PSA_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include "kernel_defines.h"

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_CONFIG)
#include "psa/crypto_se_config.h"
#endif

#include "crypto_sizes.h"
#include "crypto_struct.h"
#include "crypto_values.h"
#include "crypto_types.h"

/**
 * @brief   The major version of this implementation of the PSA Crypto API
 */
#define PSA_CRYPTO_API_VERSION_MAJOR 1

/**
 * @brief   The minor version of this implementation of the PSA Crypto API
 */
#define PSA_CRYPTO_API_VERSION_MINOR 1

/**
 * @brief   Helper function to convert PSA status values humanly readable.
 *
 * @param   status PSA status value
 * @return  Pointer to string
 */
const char *psa_status_to_humanly_readable(psa_status_t status);

/**
 * @brief   Library initialization.
 *
 * @details Applications must call this function before calling any other function in this module.
 *          Applications are permitted to call this function more than once. Once a call succeeds,
 *          subsequent calls are guaranteed to succeed.
 *
 *          If the application calls other functions before calling @ref psa_crypto_init(),
 *          the behavior is undefined. In this situation:
 *          -   Implementations are encouraged to either perform the operation as if the library
 *              had been initialized or to return @ref PSA_ERROR_BAD_STATE or some other applicable
 *              error.
 *          -   Implementations must not return a success status if the lack of initialization might
 *              have security implications, for example due to improper seeding of the random number
 *              generator.
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 */
psa_status_t psa_crypto_init(void);

#if IS_USED(MODULE_PSA_AEAD) || defined(DOXYGEN)
/**
 * @brief   Process an authenticated encryption operation.
 *
 * @param   key                     Identifier of the key to use for the operation. It must allow
 *                                  the usage @ref PSA_KEY_USAGE_ENCRYPT.
 * @param   alg                     The AEAD algorithm to compute (@c PSA_ALG_XXX value such that
 *                                  @ref PSA_ALG_IS_AEAD(@p alg) is true).
 * @param   nonce                   Nonce or IV to use.
 * @param   nonce_length            Size of the nonce buffer in bytes. This must be appropriate
 *                                  for the selected algorithm. The default nonce size is
 *                                  @ref PSA_AEAD_NONCE_LENGTH(@p key_type, @p alg) where
 *                                  @c key_type is the type of key.
 * @param   additional_data         Additional data that will be authenticated but not encrypted.
 * @param   additional_data_length  Size of additional_data in bytes.
 * @param   plaintext               Data that will be authenticated and encrypted.
 * @param   plaintext_length        Size of plaintext in bytes.
 * @param   ciphertext              Output buffer for the authenticated and encrypted data. The
 *                                  additional data is not part of this output. For algorithms
 *                                  where the encrypted data and the authentication tag are defined
 *                                  as separate outputs, the authentication tag is appended to the
 *                                  encrypted data.
 * @param   ciphertext_size         Size of the ciphertext buffer in bytes. This must be
 *                                  appropriate for the selected algorithm and key:
 *                                  -   A sufficient output size is
 *                                      @ref PSA_AEAD_ENCRYPT_OUTPUT_SIZE(@p key_type, @p alg,
 *                                      @p plaintext_length) where key_type is the type of key.
 *                                  -   @ref PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(@p plaintext_length)
 *                                      evaluates to the maximum ciphertext size of any supported
 *                                      AEAD encryption.
 * @param   ciphertext_length       On success, the size of the output in the ciphertext buffer.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_NOT_PERMITTED            The key does not have the
 *                                                  @ref PSA_KEY_USAGE_ENCRYPT flag, or it does not
 *                                                  permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         @c key is not compatible with @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED            @c alg is not supported or is not an AEAD
 *                                                  algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         @c ciphertext_size is too small.
 *                                                  @ref PSA_AEAD_ENCRYPT_OUTPUT_SIZE() or
 *                                                  @ref PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE() can be
 *                                                  used to determine the required buffer size.
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_encrypt(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t *nonce,
                              size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *plaintext,
                              size_t plaintext_length,
                              uint8_t *ciphertext,
                              size_t ciphertext_size,
                              size_t *ciphertext_length);

/**
 * @brief   Process an authenticated decryption operation.
 *
 * @param   key                     Identifier of the key to use for the operation. It must allow
 *                                  the usage @ref PSA_KEY_USAGE_DECRYPT.
 * @param   alg                     The AEAD algorithm to compute (@c PSA_ALG_XXX value such that
 *                                  @ref PSA_ALG_IS_AEAD(@p alg) is true).
 * @param   nonce                   Nonce or IV to use.
 * @param   nonce_length            Size of the nonce buffer in bytes. This must be appropriate
 *                                  for the selected algorithm. The default nonce size is
 *                                  @ref PSA_AEAD_NONCE_LENGTH(@p key_type, @p alg) where
 *                                  @c key_type is the type of @c key.
 * @param   additional_data         Additional data that will be authenticated but not encrypted.
 * @param   additional_data_length  Size of @c additional_data in bytes.
 * @param   ciphertext              Data that has been authenticated and encrypted. For algorithms
 *                                  where the encrypted data and the authentication tag are defined
 *                                  as separate inputs, the buffer must contain the encrypted data
 *                                  followed by the authentication tag.
 * @param   ciphertext_length       Size of @c ciphertext in bytes.
 * @param   plaintext               Output buffer for the decrypted data.
 * @param   plaintext_size          Size of the plaintext buffer in bytes. This must be
 *                                  appropriate for the selected algorithm and key:
 *                                  -   A sufficient output size is
 *                                      @ref PSA_AEAD_DECRYPT_OUTPUT_SIZE(@p key_type, @p alg,
 *                                      @p ciphertext_length) where @c key_type is the type of key.
 *                                  -   @ref PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(@p ciphertext_length)
 *                                      evaluates to the maximum ciphertext size of any supported
 *                                      AEAD decryption.
 * @param   plaintext_length        On success, the size of the output in the plaintext buffer.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE        The ciphertext is not authentic.
 * @return  @ref PSA_ERROR_NOT_PERMITTED            The key does not have the @ref
 *                                                  PSA_KEY_USAGE_DECRYPT flag, or it does not
 *                                                  permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         @c key is not compatible with @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED            @c alg is not supported or is not an AEAD
 *                                                  algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         @c plaintext_size is too small.
 *                                                  @ref PSA_AEAD_DECRYPT_OUTPUT_SIZE() or
 *                                                  @ref PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE() can be
 *                                                  used to determine the required buffer size.
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_decrypt(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t *nonce,
                              size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *ciphertext,
                              size_t ciphertext_length,
                              uint8_t *plaintext,
                              size_t plaintext_size,
                              size_t *plaintext_length);

/**
 * @brief   Set the key for a multi-part authenticated encryption operation.
 *
 * @details The sequence of operations to encrypt a message with authentication is as follows:
 *          -#  Allocate an operation object which will be passed to all the functions listed here
 *          -#  Initialize the operation object with one of the methods described in the
 *              documentation for @ref psa_aead_operation_t, e.g. @ref PSA_AEAD_OPERATION_INIT.
 *          -#  Call @ref psa_aead_encrypt_setup() to specify the algorithm and key.
 *          -#  If needed, call @ref psa_aead_set_lengths() to specify the length of the inputs to
 *              the subsequent calls to @ref psa_aead_update_ad() and @ref psa_aead_update(). See
 *              the documentation of @ref psa_aead_set_lengths() for details.
 *          -#  Call either @ref psa_aead_generate_nonce() or @ref psa_aead_set_nonce() to generate
 *              or set the nonce. It is recommended to use @ref psa_aead_generate_nonce() unless
 *              the protocol being implemented requires a specific nonce value.
 *          -#  Call @ref psa_aead_update_ad() zero, one or more times, passing a fragment of the
 *              non-encrypted additional authenticated data each time.
 *          -#  Call @ref psa_aead_update() zero, one or more times, passing a fragment of the
 *              message to encrypt each time.
 *          -#  Call @ref psa_aead_finish().
 *
 *          If an error occurs at any step after a call to @ref psa_aead_encrypt_setup(),
 *          the operation will need to be reset by a call to @ref psa_aead_abort(). The application
 *          can call @ref psa_aead_abort() at any time after the operation has been initialized.
 *
 *          After a successful call to @ref psa_aead_encrypt_setup(), the application must
 *          eventually terminate the operation. The following events terminate an operation:
 *          -   A successful call to @ref psa_aead_finish().
 *          -   A call to @ref psa_aead_abort().
 *
 * @param   operation   The operation object to set up. It must have been initialized as per the
 *                      documentation for @ref psa_aead_operation_t and not yet in use.
 * @param   key         Identifier of the key to use for the operation. It must remain valid until
 *                      the operation terminates.
 *                      It must allow the usage @ref PSA_KEY_USAGE_ENCRYPT.
 * @param   alg         The AEAD algorithm to compute (@c PSA_ALG_XXX value such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true).
 *
 * @return  @ref PSA_SUCCESS                    Success.
 * @return  @ref PSA_ERROR_BAD_STATE            The operation state is not valid: it must be
 *                                              inactive.
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_NOT_PERMITTED        The key does not have the @ref PSA_KEY_USAGE_ENCRYPT
 *                                              flag, or it does not permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     @c key is not compatible with @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        @c alg is not supported or is not an AEAD algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE            The library has not been previously initialized by
 *                                              @ref psa_crypto_init(). It is
 *                                              implementation-dependent whether a failure to
 *                                              initialize results in this error code.
 */
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t *operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);

/**
 * @brief   Set the key for a multi-part authenticated decryption operation.
 *
 * @details The sequence of operations to decrypt a message with authentication is as follows:
 *          -#  Allocate an operation object which will be passed to all the functions listed here
 *          -#  Initialize the operation object with one of the methods described in the
 *              documentation for @ref psa_aead_operation_t, e.g. @ref PSA_AEAD_OPERATION_INIT.
 *          -#  Call @ref psa_aead_decrypt_setup() to specify the algorithm and key.
 *          -#  If needed, call @ref psa_aead_set_lengths() to specify the length of the inputs to
 *              the subsequent calls to @ref psa_aead_update_ad() and @ref psa_aead_update(). See
 *              the documentation of @ref psa_aead_set_lengths() for details.
 *          -#  Call @ref psa_aead_set_nonce() with the nonce for the decryption.
 *          -#  Call @ref psa_aead_update_ad() zero, one or more times, passing a fragment of the
 *              non-encrypted additional authenticated data each time.
 *          -#  Call @ref psa_aead_update() zero, one or more times, passing a fragment of the
 *              message to encrypt each time.
 *          -#  Call @ref psa_aead_verify().
 *
 *          If an error occurs at any step after a call to @ref psa_aead_decrypt_setup(),
 *          the operation will need to be reset by a call to @ref psa_aead_abort(). The application
 *          can call @ref psa_aead_abort() at any time after the operation has been initialized.
 *
 *          After a successful call to @ref psa_aead_decrypt_setup(), the application must
 *          eventually terminate the operation. The following events terminate an operation:
 *          -   A successful call to @ref psa_aead_verify().
 *          -   A call to @ref psa_aead_abort().
 *
 * @param   operation   The operation object to set up. It must have been initialized as per the
 *                      documentation for @ref psa_aead_operation_t and not yet in use.
 * @param   key         Identifier of the key to use for the operation. It must remain valid
 *                      until the operation terminates. It must allow the usage @ref
 *                      PSA_KEY_USAGE_DECRYPT.
 * @param   alg         The AEAD algorithm to compute (@c PSA_ALG_XXX value such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true).
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be
 *                                                  inactive.
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_NOT_PERMITTED            The key does not have the @ref
 *                                                  PSA_KEY_USAGE_DECRYPT flag, or it does not
 *                                                  permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         @c key is not compatible with @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED            @c alg is not supported or is not an AEAD
 *                                                  algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t *operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);

/**
 * @brief   Declare the lengths of the message and additional data for AEAD.
 *
 * @details The application must call this function before calling @ref psa_aead_set_nonce()
 *          or @ref psa_aead_generate_nonce(), if the algorithm for the operation requires it.
 *          If the algorithm does not require it, calling this function is optional, but if
 *          this function is called then the implementation must enforce the lengths.
 *          -   For @ref PSA_ALG_CCM, calling this function is required.
 *          -   For the other AEAD algorithms defined in this specification,
 *              calling this function is not required.
 *          -   For vendor-defined algorithm, refer to the vendor documentation.
 *
 *          If this function returns an error status, the operation enters an error state and
 *          must be aborted by calling @ref psa_aead_abort().
 *
 * @param   operation           Active AEAD operation.
 * @param   ad_length           Size of the non-encrypted additional authenticated data in bytes.
 * @param   plaintext_length    Size of the plaintext to encrypt in bytes.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be
 *                                                  active, and @ref psa_aead_set_nonce() and
 *                                                  @ref psa_aead_generate_nonce() must not have
 *                                                  been called yet.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         At least one of the lengths is not acceptable
 *                                                  for the chosen algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_set_lengths(psa_aead_operation_t *operation,
                                  size_t ad_length,
                                  size_t plaintext_length);

/**
 * @brief   Generate a random nonce for an authenticated encryption operation.
 *
 * @details This function generates a random nonce for the authenticated encryption operation
 *          with an appropriate size for the chosen algorithm, key type and key size.
 *
 *          The application must call @ref psa_aead_encrypt_setup() before calling this function.
 *          If applicable for the algorithm, the application must call @ref psa_aead_set_lengths()
 *          before calling this function.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_aead_abort().
 *
 * @param   operation       Active AEAD operation.
 * @param   nonce           Buffer where the generated nonce is to be written.
 * @param   nonce_size      Size of the nonce buffer in bytes. This must be at least
 *                          @ref PSA_AEAD_NONCE_LENGTH(@p key_type, @p alg) where @p key_type
 *                          and @p alg are type of key and the algorithm respectively that
 *                          were used to set up the AEAD operation.
 * @param   nonce_length    On success, the number of bytes of the generated nonce.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be
 *                                                  an active AEAD encryption operation, with no
 *                                                  nonce set.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: this is an
 *                                                  algorithm which requires @ref
 *                                                  psa_aead_set_lengths() to be called before
 *                                                  setting the nonce.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         The size of the nonce buffer is too small.
 *                                                  @ref PSA_AEAD_NONCE_LENGTH() or @ref
 *                                                  PSA_AEAD_NONCE_MAX_SIZE can be used to
 *                                                  determine the required buffer size.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_generate_nonce(psa_aead_operation_t *operation,
                                     uint8_t *nonce,
                                     size_t nonce_size,
                                     size_t *nonce_length);

/**
 * @brief   Set the nonce for an authenticated encryption or decryption operation.
 *
 * @details This function sets the nonce for the authenticated encryption or decryption operation.
 *          The application must call @ref psa_aead_encrypt_setup() or @ref
 *          psa_aead_decrypt_setup() before calling this function. If applicable for the algorithm,
 *          the application must call @ref psa_aead_set_lengths() before calling this function.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_aead_abort().
 *
 * @note    When encrypting, @ref psa_aead_generate_nonce() is recommended instead of using this
 *          function, unless implementing a protocol that requires a non-random IV.
 *
 * @param   operation       Active AEAD operation.
 * @param   nonce           Buffer containing the nonce to use.
 * @param   nonce_length    Size of the nonce in bytes. This must be a valid nonce size for the
 *                          chosen algorithm. The default nonce size is @ref PSA_AEAD_NONCE_LENGTH
 *                          (@p key_type, @p alg) where @c key_type and @c alg are type of key and
 *                          the algorithm respectively that were used to set up the AEAD operation.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be
 *                                                  an active AEAD encryption operation, with no
 *                                                  nonce set.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: this is an
 *                                                  algorithm which requires @ref
 *                                                  psa_aead_set_lengths() to be called before
 *                                                  setting the nonce.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The size of nonce is not acceptable for the
 *                                                  chosen algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_set_nonce(psa_aead_operation_t *operation,
                                const uint8_t *nonce,
                                size_t nonce_length);

/**
 * @brief   Pass additional data to an active AEAD operation.
 *
 * @details Additional data is authenticated, but not encrypted.
 *          This function can be called multiple times to pass successive fragments of the
 *          additional data. This function must not be called after passing data to encrypt
 *          or decrypt with @ref psa_aead_update().
 *
 *          The following must occur before calling this function:
 *          -#  Call either @ref psa_aead_encrypt_setup() or @ref psa_aead_decrypt_setup().
 *          -#  Set the nonce with @ref psa_aead_generate_nonce() or @ref psa_aead_set_nonce().
 *
 *          If this function returns an error status, the operation enters an error state and
 *          must be aborted by calling @ref psa_aead_abort().
 *
 * @warning When decrypting, do not trust the input until @ref psa_aead_verify() succeeds.
 *          See the detailed warning.
 *
 * @param   operation       Active AEAD operation.
 * @param   input           Buffer containing the fragment of additional data.
 * @param   input_length    Size of the input buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be
 *                                                  active, have a nonce set, have lengths set if
 *                                                  required by the algorithm, and @ref
 *                                                  psa_aead_update() must not have been called yet.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total input length overflows the additional
 *                                                  data length that was previously specified with
 *                                                  @ref psa_aead_set_lengths().
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_update_ad(psa_aead_operation_t *operation,
                                const uint8_t *input,
                                size_t input_length);

/**
 * @brief   Encrypt or decrypt a message fragment in an active AEAD operation.
 *
 * @details The following must occur before calling this function:
 *          -#  Call either @ref psa_aead_encrypt_setup() or @ref psa_aead_decrypt_setup().
 *              The choice of setup function determines whether this function encrypts or decrypts
 *              its input.
 *          -#  Set the nonce with @ref psa_aead_generate_nonce() or @ref psa_aead_set_nonce().
 *          -#  Call @ref psa_aead_update_ad() to pass all the additional data.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_aead_abort().
 *
 *          This function does not require the input to be aligned to any particular block
 *          boundary. If the implementation can only process a whole block at a time, it must
 *          consume all the input provided, but it might delay the end of the corresponding output
 *          until a subsequent call to @ref psa_aead_update(), @ref psa_aead_finish() or @ref
 *          psa_aead_verify() provides sufficient input.
 *          The amount of data that can be delayed in this way is bounded by @ref
 *          PSA_AEAD_UPDATE_OUTPUT_SIZE().
 *
 * @warning When decrypting, do not trust the input until @ref psa_aead_verify() succeeds.
 *          See the detailed warning.
 *
 * @param   operation       Active AEAD operation.
 * @param   input           Buffer containing the message fragment to encrypt or decrypt.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   output          Buffer where the output is to be written.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          -   A sufficient output size is @ref PSA_AEAD_UPDATE_OUTPUT_SIZE(
 *                              @p key_type, @p alg, @p input_length) where @c key_type is the
 *                              type of key and @c alg is the algorithm that were used to set up
 *                              the operation.
 *                          -   @ref PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(@p input_length) evaluates to
 *                              the maximum output size of any supported AEAD algorithm.
 * @param   output_length   On success, the number of bytes that make up the returned output.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be
 *                                                  active, have a nonce set, and have lengths set
 *                                                  if required by the algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         The size of the output buffer is too small.
 *                                                  @ref PSA_AEAD_UPDATE_OUTPUT_SIZE() or
 *                                                  @ref PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE() can be
 *                                                  used to determine the required buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total length of input to @ref
 *                                                  psa_aead_update_ad() so far is less than the
 *                                                  additional data length that was previously
 *                                                  specified with @ref psa_aead_set_lengths()
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total input length overflows the plaintext
 *                                                  length that was previously specified with
 *                                                  @ref psa_aead_set_lengths().
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_update(psa_aead_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length,
                             uint8_t *output,
                             size_t output_size,
                             size_t *output_length);

/**
 * @brief   Finish encrypting a message in an AEAD operation.
 *
 * @details The operation must have been set up with @ref psa_aead_encrypt_setup().
 *          This function finishes the authentication of the additional data formed
 *          by concatenating the inputs passed to preceding calls to @ref psa_aead_update_ad()
 *          with the plaintext formed by concatenating the inputs passed to preceding calls to
 *          @ref psa_aead_update(). This function has two output buffers:
 *          -   @p ciphertext contains trailing ciphertext that was buffered from preceding calls to
 *              @ref psa_aead_update().
 *          -   @p tag contains the authentication tag.
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_aead_abort().
 *
 * @param   operation           Active AEAD operation.
 * @param   ciphertext          Buffer where the last part of the ciphertext is to be written.
 * @param   ciphertext_size     Size of the ciphertext buffer in bytes. This must be appropriate
 *                              for the selected algorithm and key:
 *                              -   A sufficient output size is @ref PSA_AEAD_FINISH_OUTPUT_SIZE(
 *                                  @p key_type, @p alg) where @p key_type is the type of key and
 *                                  @p alg is the algorithm that were used to set up the operation
 *                              -   @ref PSA_AEAD_FINISH_OUTPUT_MAX_SIZE evaluates to the maximum
 *                                  output size of any supported AEAD algorithm.
 * @param   ciphertext_length   On success, the number of bytes of returned ciphertext.
 * @param   tag                 Buffer where the authentication tag is to be written.
 * @param   tag_size            Size of the tag buffer in bytes. This must be appropriate for the
 *                              selected algorithm and key:
 *                              -   The exact tag size is @ref PSA_AEAD_TAG_LENGTH(@p key_type,
 *                                  @p key_bits, @p alg) where @c key_type and @c key_bits are
 *                                  the type and bit-size of the key, and @c alg is the algorithm
 *                                  that were used in the call to @ref psa_aead_encrypt_setup().
 *                              -   @ref PSA_AEAD_TAG_MAX_SIZE evaluates to the maximum tag size
 *                                  of any supported AEAD algorithm.
 * @param   tag_length            On success, the number of bytes that make up the returned tag.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be an
 *                                                  active encryption operation with a nonce set.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         The size of the ciphertext or tag buffer is too
 *                                                  small. @ref PSA_AEAD_FINISH_OUTPUT_SIZE() or
 *                                                  @ref PSA_AEAD_FINISH_OUTPUT_MAX_SIZE can be
 *                                                  used to determine the required ciphertext
 *                                                  buffer size. @ref PSA_AEAD_TAG_LENGTH() or
 *                                                  @ref  PSA_AEAD_TAG_MAX_SIZE can be used to
 *                                                  determine the required tag buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total length of input to @ref
 *                                                  psa_aead_update_ad() so far is less than the
 *                                                  additional data length that was previously
 *                                                  specified with @ref psa_aead_set_lengths()
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total length of input to @ref
 *                                                  psa_aead_update() so far is less than the
 *                                                  plaintext length that was previously specified
 *                                                  with @ref psa_aead_set_lengths().
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_finish(psa_aead_operation_t *operation,
                             uint8_t *ciphertext,
                             size_t ciphertext_size,
                             size_t *ciphertext_length,
                             uint8_t *tag,
                             size_t tag_size,
                             size_t *tag_length);

/**
 * @brief   Finish authenticating and decrypting a message in an AEAD operation.
 *
 * @details The operation must have been set up with @ref psa_aead_decrypt_setup().
 *          This function finishes the authenticated decryption of the message components:
 *          -   The additional data consisting of the concatenation of the inputs passed to
 *              preceding calls to @ref psa_aead_update_ad().
 *          -   The ciphertext consisting of the concatenation of the inputs passed to
 *              preceding calls to @ref psa_aead_update().
 *          -   The tag passed to this function call.
 *
 *          If the authentication tag is correct, this function outputs any remaining plaintext
 *          and reports success. If the authentication tag is not correct, this function returns
 *          @ref PSA_ERROR_INVALID_SIGNATURE.
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_aead_abort().
 *
 * @note    Implementations must make the best effort to ensure that the comparison between
 *          the actual tag and the expected tag is performed in constant time.
 *
 * @param   operation           Active AEAD operation.
 * @param   plaintext           Buffer where the last part of the plaintext is to be written. This
 *                              is the remaining data from previous calls to @ref psa_aead_update()
 *                              that could not be processed until the end of the input.
 * @param   plaintext_size      Size of the plaintext buffer in bytes. This must be appropriate
 *                              for the selected algorithm and key:
 *                              -   A sufficient output size is @ref PSA_AEAD_FINISH_OUTPUT_SIZE(
 *                                  @p key_type, @p alg) where @c key_type is the type of key and
 *                                  @c alg is the algorithm that were used to set up the operation
 *                              -   @ref PSA_AEAD_FINISH_OUTPUT_MAX_SIZE evaluates to the maximum
 *                                  output size of any supported AEAD algorithm.
 * @param   plaintext_length    On success, the number of bytes of returned plaintext.
 * @param   tag                 Buffer containing the authentication tag.
 * @param   tag_length          Size of the tag buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE        The calculations were successful, but the
 *                                                  authentication tag is not correct.
 * @return  @ref PSA_ERROR_BAD_STATE                The operation state is not valid: it must be an
 *                                                  active encryption operation with a nonce set.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         The size of the plaintext buffer is too small.
 *                                                  @ref PSA_AEAD_VERIFY_OUTPUT_SIZE() or
 *                                                  @ref PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE can be
 *                                                  used to determine the required buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total length of input to @ref
 *                                                  psa_aead_update_ad() so far is less than the
 *                                                  additional data length that was previously
 *                                                  specified with @ref psa_aead_set_lengths()
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT         The total length of input to @ref
 *                                                  psa_aead_update() so far is less than the
 *                                                  plaintext length that was previously specified
 *                                                  with @ref psa_aead_set_lengths().
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by  @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_verify(psa_aead_operation_t *operation,
                             uint8_t *plaintext,
                             size_t plaintext_size,
                             size_t *plaintext_length,
                             const uint8_t *tag,
                             size_t tag_length);

/**
 * @brief   Abort an AEAD operation.
 *
 * @details Aborting an operation frees all associated resources except for the operation object
 *          itself. Once aborted, the operation object can be reused for another operation by
 *          calling @ref psa_aead_encrypt_setup() or @ref psa_aead_decrypt_setup() again.
 *
 *          This function can be called any time after the operation object has been initialized as
 *          described in @ref psa_aead_operation_t.
 *
 *          In particular, calling @ref psa_aead_abort() after the operation has been terminated
 *          by a call to @ref psa_aead_abort(), @ref psa_aead_finish() or @ref psa_aead_verify()
 *          is safe and has no effect.
 *
 * @param   operation   Initialized AEAD operation.
 *
 * @return  @ref PSA_SUCCESS                        Success.
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by  @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_aead_abort(psa_aead_operation_t *operation);
#endif /* MODULE_PSA_AEAD */

#if IS_USED(MODULE_PSA_ASYMMETRIC) || defined(DOXYGEN)
/**
 * @brief   Encrypt a short message with a public key.
 *
 * @details For @ref PSA_ALG_RSA_PKCS1V15_CRYPT, no salt is supported.
 *
 * @param   key             Identifier of the key to use for the operation. It must be a
 *                          public key or an asymmetric key pair. It must allow the usage
 *                          @ref PSA_KEY_USAGE_ENCRYPT.
 * @param   alg             An asymmetric encryption algorithm that is compatible with
 *                          the type of key.
 * @param   input           The message to encrypt.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   salt            A salt or label, if supported by the encryption algorithm. If the
 *                          algorithm does not support a salt, pass @c NULL. If the algorithm
 *                          supports an optional salt, pass @c NULL to indicate that there is
 *                          no salt.
 * @param   salt_length     Size of the salt buffer in bytes. If salt is @c NULL, pass 0.
 * @param   output          Buffer where the encrypted message is to be written.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          -   The required output size is @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE
 *                              (@p key_type, @p key_bits, @p alg) where @p key_type and
 *                              @p key_bits are the type and bit-size respectively of key
 *                          -   @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE evaluates to the maximum
 *                              output size of any supported asymmetric encryption.
 * @param   output_length   On success, the number of bytes that make up the returned output.
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_NOT_PERMITTED            The key does not have the @ref
 *                                                  PSA_KEY_USAGE_ENCRYPT flag, or it does not
 *                                                  permit the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         The size of the output buffer is too small.
 *                                                  @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE() or
 *                                                  @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE can
 *                                                  be used to determine the required buffer size.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by  @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_asymmetric_encrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt,
                                    size_t salt_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length);

/**
 * @brief   Decrypt a short message with a private key.
 *
 * @details For @ref PSA_ALG_RSA_PKCS1V15_CRYPT, no salt is supported.
 *
 * @param   key             Identifier of the key to use for the operation. It must be an asymmetric
 *                          key pair. It must allow the usage @ref PSA_KEY_USAGE_DECRYPT.
 * @param   alg             An asymmetric encryption algorithm that is compatible with
 *                          the type of key.
 * @param   input           The message to decrypt.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   salt            A salt or label, if supported by the encryption algorithm. If the
 *                          algorithm does not support a salt, pass @c NULL. If the algorithm
 *                          supports an optional salt, pass @c NULL to indicate that there is
 *                          no salt.
 * @param   salt_length     Size of the salt buffer in bytes. If salt is @c NULL, pass 0.
 * @param   output          Buffer where the decrypted message is to be written.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          -   The required output size is @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE
 *                              (@p key_type, @p key_bits, @p alg) where @p key_type and
 *                              @p key_bits are the type and bit-size respectively of key.
 *                          -   @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE evaluates to the maximum
 *                              output size of any supported asymmetric decryption.
 * @param   output_length   On success, the number of bytes that make up the returned output.
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_NOT_PERMITTED            The key does not have the @ref
 *                                                  PSA_KEY_USAGE_DECRYPT flag, or it does not
 *                                                  permit the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL         The size of the output buffer is too small.
 *                                                  @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE() or
 *                                                  @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE
 *                                                  can be used to determine the required buffer
 *                                                  size.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 * @return  @ref PSA_ERROR_INVALID_PADDING
 * @return  @ref PSA_ERROR_BAD_STATE                The library has not been previously initialized
 *                                                  by  @ref psa_crypto_init(). It is
 *                                                  implementation-dependent whether a failure to
 *                                                  initialize results in this error code.
 */
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt,
                                    size_t salt_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length);
#endif /* MODULE_PSA_ASYMMETRIC */


#if IS_USED(MODULE_PSA_CIPHER) || defined(DOXYGEN)
/**
 * @brief   Abort a cipher operation.
 *
 * @details Aborting an operation frees all associated resources except for the operation object
 *          itself. Once aborted, the operation object can be reused for another operation by
 *          calling @ref psa_cipher_encrypt_setup() or @ref psa_cipher_decrypt_setup() again.
 *
 *          This function can be called any time after the operation object has been initialized
 *          as described in @ref psa_cipher_operation_t.
 *
 *          In particular, calling @ref psa_cipher_abort() after the operation has been terminated
 *          by a call to @ref psa_cipher_abort() or @ref psa_cipher_finish() is safe and has no
 *          effect.
 *
 * @param   operation   Initialized cipher operation.
 *
 * @return  @ref PSA_SUCCESS                        Success. The operation object can now be
 *                                                  discarded or reused.
 * @return  @ref PSA_ERROR_BAD_STATE                The library requires initializing by a call
 *                                                  to @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation);

/**
 * @brief   Decrypt a message using a symmetric cipher.
 *
 * @details This function decrypts a message encrypted with a symmetric cipher. *
 *          The input to this function must contain the IV followed by the ciphertext, as output
 *          by @ref psa_cipher_encrypt(). The IV must be @ref PSA_CIPHER_IV_LENGTH(@p key_type,
 *          @p alg) bytes in length, where @c key_type is the type of key.
 *
 *          Use the multi-part operation interface with a @ref psa_cipher_operation_t object to
 *          decrypt data which is not in the expected input format.
 *
 * @param   key             Identifier of the key to use for the operation. It must remain
 *                          valid until the operation terminates. It must allow the usage
 *                          @ref PSA_KEY_USAGE_DECRYPT.
 * @param   alg             The cipher algorithm to compute: a value of type @ref psa_algorithm_t
 *                          such that @ref PSA_ALG_IS_CIPHER(@p alg) is true.
 * @param   input           Buffer containing the message to decrypt. This consists of the IV
 *                          followed by the ciphertext proper.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   output          Buffer where the plaintext is to be written.
 * @param   output_size     Size of the output buffer in bytes.
 *                          This must be appropriate for the selected algorithm and key:
 *                          -   A sufficient output size is @ref PSA_CIPHER_DECRYPT_OUTPUT_SIZE(
 *                              @p key_type, @p alg, @p input_length) where @c key_type is the
 *                              type of key.
 *                          -   @ref PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(@p input_length) evaluates
 *                              to the maximum output size of any supported cipher decryption.
 * @param   output_length   On success, the number of bytes that make up the output.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*output_length) bytes of
 *                                          @p output contain the plaintext.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref PSA_KEY_USAGE_DECRYPT
 *                                          flag, or it does not permit the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the output buffer is too small.
 *                                          @ref PSA_CIPHER_DECRYPT_OUTPUT_SIZE() or
 *                                          @ref PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE() can be
 *                                          used to determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_PADDING  The algorithm uses padding, and the input does not
 *                                          contain valid padding.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          -   @c alg is not a cipher algorithm.
 *                                          -   @c key is not compatible with alg.
 *                                          -   The input_length is not valid for the algorithm
 *                                              and key type.
 *                                              For example, the algorithm is a based on block
 *                                              cipher and requires a whole number of blocks,
 *                                              but the total input size is not a multiple
 *                                              of the block size.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          -   @c alg is not supported or is not a cipher
 *                                              algorithm.
 *                                          -   @c key is not supported for use with alg.
 *                                          -   @c input_length is too large for the implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_cipher_decrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length);

/**
 * @brief   Set the key for a multi-part symmetric decryption operation.
 *
 * @details The sequence of operations to decrypt a message with a symmetric cipher is as follows:
 *          -#  Allocate an operation object which will be passed to all the functions listed here.
 *          -#  Initialize the operation object with one of the methods described in the
 *              documentation for @ref psa_cipher_operation_t, e.g. @ref PSA_CIPHER_OPERATION_INIT.
 *          -#  Call @ref psa_cipher_decrypt_setup() to specify the algorithm and key.
 *          -#  Call @ref psa_cipher_set_iv() with the initialization vector (IV) for the
 *              decryption, if the algorithm requires one. This must match the IV used for the
 *              encryption.
 *          -#  Call @ref psa_cipher_update() zero, one or more times, passing a fragment of the
 *              message each time.
 *          -#  Call @ref psa_cipher_finish().
 *
 *          If an error occurs at any step after a call to @ref psa_cipher_decrypt_setup(),
 *          the operation will need to be reset by a call to @ref psa_cipher_abort().
 *          The application can call @ref psa_cipher_abort() at any time after the operation
 *          has been initialized.
 *
 *          After a successful call to @ref psa_cipher_decrypt_setup(), the application must
 *          eventually terminate the operation. The following events terminate an operation:
 *          - A successful call to @ref psa_cipher_finish().
 *          - A call to @ref psa_cipher_abort().
 *
 * @param   operation   The operation object to set up. It must have been initialized as per
 *                      the documentation for @ref psa_cipher_operation_t and not yet in use.
 * @param   key         Identifier of the key to use for the operation. It must remain valid
 *                      until the operation terminates. It must allow the usage @ref
 *                      PSA_KEY_USAGE_DECRYPT.
 * @param   alg         The cipher algorithm to compute: a value of type @ref psa_algorithm_t
 *                      such that @ref PSA_ALG_IS_CIPHER(@p alg) is true.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be
 *                                              inactive.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @p key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref PSA_KEY_USAGE_DECRYPT
 *                                          flag, or it does not permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not a cipher algorithm.
 *                                          - @c key is not compatible with alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a cipher algorithm.
 *                                          - @c key is not supported for use with alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);

/**
 * @brief   Encrypt a message using a symmetric cipher.
 *
 * @details This function encrypts a message with a random initialization vector (IV).
 *          The length of the IV is @ref PSA_CIPHER_IV_LENGTH(@p key_type, @p alg) where
 *          @p key_type is the type of key. The output of @ref psa_cipher_encrypt() is
 *          the IV followed by the ciphertext.
 *
 *          Use the multi-part operation interface with a @ref psa_cipher_operation_t object
 *          to provide other forms of IV or to manage the IV and ciphertext independently.
 *
 * @param   key             Identifier of the key to use for the operation. It must allow the usage
 *                          @ref PSA_KEY_USAGE_ENCRYPT.
 * @param   alg             The cipher algorithm to compute (@c PSA_ALG_XXX value such that
 *                          @ref PSA_ALG_IS_CIPHER(@p alg) is true).
 * @param   input           Buffer containing the message to encrypt.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   output          Buffer where the output is to be written. The output contains the IV
 *                          followed by the ciphertext proper.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          -   A sufficient output size is @ref PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(
 *                              @p key_type, @p alg, @p input_length) where @c key_type is the
 *                              type of key
 *                          -   @ref PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(@p input_length) evaluates
 *                              to the maximum output size of any supported cipher encryption.
 * @param   output_length   On success, the number of bytes that make up the output.
 *
 * @return  @ref PSA_SUCCESS                    Success.
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_NOT_PERMITTED        The key does not have the @ref
 *                                              PSA_KEY_USAGE_ENCRYPT flag, or it does not permit
 *                                              the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     @c key is not compatible with alg.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The @c input_length is not valid for the algorithm
 *                                              and key type. For example, the algorithm is a based
 *                                              on block cipher and requires a whole number of
 *                                              blocks, but the total input size is not a multiple
 *                                              of the block size.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        @c alg is not supported or is not a cipher
 *                                              algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL     @c output_size is too small.
 *                                              @ref PSA_CIPHER_ENCRYPT_OUTPUT_SIZE() or
 *                                              @ref PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE()
 *                                              can be used to determine the required buffer size.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE            The library has not been previously initialized
 *                                              by  @ref psa_crypto_init(). It is
 *                                              implementation-dependent whether a failure to
 *                                              initialize results in this error code.
 */
psa_status_t psa_cipher_encrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length);

/**
 * @brief   Set the key for a multi-part symmetric encryption operation.
 *
 * @details The sequence of operations to encrypt a message with a symmetric cipher is as follows:
 *          -#  Allocate an operation object which will be passed to all the functions listed here.
 *          -#  Initialize the operation object with one of the methods described in the
 *              documentation for @ref psa_cipher_operation_t, e.g. @ref PSA_CIPHER_OPERATION_INIT.
 *          -#  Call @ref psa_cipher_encrypt_setup() to specify the algorithm and key.
 *          -#  Call either @ref psa_cipher_generate_iv() or @ref psa_cipher_set_iv() to generate
 *              or set the initialization vector (IV), if the algorithm requires one. It is
 *              recommended to use @ref psa_cipher_generate_iv() unless the protocol being
 *              implemented requires a specific IV value.
 *          -#  Call @ref psa_cipher_update() zero, one or more times, passing a fragment of
 *              the message each time.
 *          -#  Call @ref psa_cipher_finish().
 *
 *          If an error occurs at any step after a call to @ref psa_cipher_encrypt_setup(),
 *          the operation will need to be reset by a call to @ref psa_cipher_abort(). The
 *          application can call @ref psa_cipher_abort() at any time after the operation has
 *          been initialized.
 *
 *          After a successful call to @ref psa_cipher_encrypt_setup(), the application must
 *          eventually terminate the operation. The following events terminate an operation:
 *          - A successful call to @ref psa_cipher_finish().
 *          - A call to @ref psa_cipher_abort().
 *
 * @param   operation   The operation object to set up. It must have been initialized as per the
 *                      documentation for @ref psa_cipher_operation_t and not yet in use.
 * @param   key         Identifier of the key to use for the operation. It must remain valid
 *                      until the operation terminates. It must allow the usage @ref
 *                      PSA_KEY_USAGE_ENCRYPT.
 * @param   alg         The cipher algorithm to compute: a value of type @ref psa_algorithm_t
 *                      such that @ref PSA_ALG_IS_CIPHER(@p alg) is true.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be
 *                                              inactive.
 *                                          -   The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref
 *                                          PSA_KEY_USAGE_ENCRYPT flag, or it does not permit
 *                                          the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not a cipher algorithm.
 *                                          - @c key is not compatible with alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a cipher algorithm.
 *                                          - @c key is not supported for use with @c alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);

/**
 * @brief   Finish encrypting or decrypting a message in a cipher operation.
 *
 * @details The application must call @ref psa_cipher_encrypt_setup() or @ref
 *          psa_cipher_decrypt_setup() before calling this function. The choice of setup function
 *          determines whether this function encrypts or decrypts its input.
 *
 *          This function finishes the encryption or decryption of the message formed by
 *          concatenating the inputs passed to preceding calls to @ref psa_cipher_update().
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_cipher_abort().
 *
 * @param   operation       Active cipher operation.
 * @param   output          Buffer where the output is to be written.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          -   A sufficient output size is @ref PSA_CIPHER_FINISH_OUTPUT_SIZE(
 *                              @p key_type, @p alg) where @c key_type is the type of key and
 *                              @c alg is the algorithm that were used to set up the operation.
 *                          -   @ref PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE evaluates to the maximum
 *                              output size of any supported cipher algorithm.
 * @param   output_length   On success, the number of bytes that make up the returned output.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @c (*output_length) bytes of
 *                                          @c output contain the final output.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be
 *                                              active, with an IV set if required for the
 *                                              algorithm.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the output buffer is too small.
 *                                          @ref PSA_CIPHER_FINISH_OUTPUT_SIZE() or
 *                                          @ref PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE can be used
 *                                          to determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_PADDING  This is a decryption operation for an algorithm that
 *                                          includes padding, and the ciphertext does not contain
 *                                          valid padding.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The total input size passed to this operation is not
 *                                          valid for this particular algorithm. For example, the
 *                                          algorithm is a based on block cipher and requires a
 *                                          whole number of blocks, but the total input size
 *                                          is not a multiple of the block size.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length);

/**
 * @brief   Generate an initialization vector (IV) for a symmetric encryption operation.
 *
 * @details This function generates a random IV, nonce or initial counter value for the encryption
 *          operation as appropriate for the chosen algorithm, key type and key size.
 *
 *          The generated IV is always the default length for the key and algorithm:
 *          @ref PSA_CIPHER_IV_LENGTH(@p key_type, @p alg), where @p key_type is the type of key
 *          and @p alg is the algorithm that were used to set up the operation. To generate
 *          different lengths of IV, use @ref psa_generate_random() and @ref psa_cipher_set_iv().
 *
 *          If the cipher algorithm does not use an IV, calling this function returns a
 *          @ref PSA_ERROR_BAD_STATE error. For these algorithms, @ref PSA_CIPHER_IV_LENGTH(
 *          @p key_type, @p alg) will be zero.
 *
 *          The application must call @ref psa_cipher_encrypt_setup() before calling this function.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_cipher_abort().
 *
 * @param operation Active cipher operation.
 * @param iv        Buffer where the generated IV is to be written.
 * @param iv_size   Size of the iv buffer in bytes. This must be at least
 *                  @ref PSA_CIPHER_IV_LENGTH(@p key_type, @p alg) where @p key_type and
 *                  @p alg are type of key and the algorithm respectively that
 *                  were used to set up the cipher operation.
 * @param iv_length On success, the number of bytes of the generated IV.
 *
 * @return  @ref PSA_SUCCESS                    Success.
 * @return  @ref PSA_ERROR_BAD_STATE            Either:
 *                                              -   The cipher algorithm does not use an IV.
 *                                              -   The operation state is not valid: it must be
 *                                                  active, with no IV set.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL     The size of the iv buffer is too small.
 *                                              @ref PSA_CIPHER_IV_LENGTH() or
 *                                              @ref PSA_CIPHER_IV_MAX_SIZE can be used to
 *                                              determine the required buffer size.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE            The library has not been previously initialized
 *                                              by  @ref psa_crypto_init(). It is
 *                                              implementation-dependent whether a failure to
 *                                              initialize results in this error code.
 */
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t *operation,
                                    uint8_t *iv,
                                    size_t iv_size,
                                    size_t *iv_length);

/**
 * @brief   Set the initialization vector (IV) for a symmetric encryption or decryption operation.
 *
 * @details This function sets the IV, nonce or initial counter value for the encryption or
 *          decryption operation.
 *
 *          If the cipher algorithm does not use an IV, calling this function returns a
 *          @ref PSA_ERROR_BAD_STATE error.
 *          For these algorithms, @ref PSA_CIPHER_IV_LENGTH(key_type, alg) will be zero.
 *
 *          The application must call @ref psa_cipher_encrypt_setup() or @ref
 *          psa_cipher_decrypt_setup() before calling this function.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_cipher_abort().
 *
 * @note    When encrypting, @ref psa_cipher_generate_iv() is recommended instead of using this
 *          function, unless implementing a protocol that requires a non-random IV.
 *
 * @param   operation   Active cipher operation.
 * @param   iv          Buffer containing the IV to use.
 * @param   iv_length   Size of the IV in bytes.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The cipher algorithm does not use an IV.
 *                                          -   The operation state is not valid: it must be an
 *                                              active cipher encrypt operation, with no IV set.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - The chosen algorithm does not use an IV.
 *                                          - @c iv_length is not valid for the chosen algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    @c iv_length is not supported for use with the
 *                                          operation’s algorithm and key.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const uint8_t *iv,
                               size_t iv_length);

/**
 * @brief   Encrypt or decrypt a message fragment in an active cipher operation.
 *
 * @details The following must occur before calling this function:
 *          -#  Call either @ref psa_cipher_encrypt_setup() or @ref psa_cipher_decrypt_setup().
 *              The choice of setup function determines whether this function encrypts or decrypts
 *              its input.
 *          -#  If the algorithm requires an IV, call @ref psa_cipher_generate_iv() or @ref
 *              psa_cipher_set_iv(). @ref psa_cipher_generate_iv() is recommended when encrypting.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_cipher_abort().
 *
 * @param   operation       Active cipher operation.
 * @param   input           Buffer containing the message fragment to encrypt or decrypt.
 * @param   input_length    Size of the @c input buffer in bytes.
 * @param   output          Buffer where the output is to be written.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          -   A sufficient output size is @ref PSA_CIPHER_UPDATE_OUTPUT_SIZE(
 *                              @p key_type, @p alg, @p input_length) where @c key_type is the type
 *                              of key and @c alg is the algorithm that were used to set up the
 *                              operation.
 *                          -   @ref PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(@p input_length) evaluates
 *                              to the maximum output size of any supported cipher algorithm.
 * @param   output_length   On success, the number of bytes that make up the returned output.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*output_length) bytes of
 *                                          @p output contain the output data.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be
 *                                              active, with an IV set if required for the
 *                                              algorithm.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the output buffer is too small.
 *                                          @ref PSA_CIPHER_UPDATE_OUTPUT_SIZE() or
 *                                          @ref PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE() can be
 *                                          used to determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The total input size passed to this operation is
 *                                          too large for this particular algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The total input size passed to this operation is
 *                                          too large for the implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length);
#endif /* MODULE_PSA_CIPHER */

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT) || defined(DOXYGEN)
/**
 * @brief   Make a copy of a key.
 *
 * @details Copy key material from one location to another. *
 *          This function is primarily useful to copy a key from one location to another, as it
 *          populates a key using the material from another key which can have a different lifetime.
 *
 *          This function can be used to share a key with a different party, subject to
 *          implementation-defined restrictions on key sharing.
 *
 *          The policy on the source key must have the usage flag @ref PSA_KEY_USAGE_COPY set.
 *          This flag is sufficient to permit the copy if the key has the lifetime @ref
 *          PSA_KEY_LIFETIME_VOLATILE or @ref PSA_KEY_LIFETIME_PERSISTENT. Some secure elements
 *          do not provide a way to copy a key without making it extractable from the secure
 *          element. If a key is located in such a secure element, then the key must have
 *          both usage flags @ref PSA_KEY_USAGE_COPY and @ref PSA_KEY_USAGE_EXPORT in order to make
 *          a copy of the key outside the secure element.
 *
 *          The resulting key can only be used in a way that conforms to both the policy of the
 *          original key and the policy specified in the attributes parameter:
 *          -   The usage flags on the resulting key are the bitwise-and of the usage flags on the
 *              source policy and the usage flags in attributes.
 *          -   If both permit the same algorithm or wildcard-based algorithm, the resulting key
 *              has the same permitted algorithm.
 *          -   If either of the policies permits an algorithm and the other policy allows a
 *              wildcard-based permitted algorithm that includes this algorithm, the resulting key
 *              uses this permitted algorithm.
 *          -   If the policies do not permit any algorithm in common, this function fails with the
 *              status @ref PSA_ERROR_INVALID_ARGUMENT.
 *
 *          The effect of this function on implementation-defined attributes is
 *          implementation-defined.
 *
 * @param   source_key  The key to copy. It must allow the usage @ref PSA_KEY_USAGE_COPY. If a
 *                      private or secret key is being copied outside of a secure element it must
 *                      also allow @ref PSA_KEY_USAGE_EXPORT.
 * @param   attributes  The attributes for the new key. This function uses the attributes as
 *                      follows:
 *                      -   The key type and size can be 0. If either is nonzero, it must match the
 *                          corresponding attribute of the source key.
 *                      -   The key location (the lifetime and, for persistent keys, the key
 *                          identifier) is used directly.
 *                      -   The key policy (usage flags and permitted algorithm) are combined from
 *                          the source key and attributes so that both sets of restrictions apply,
 *                          as described in the documentation of this function.
 *                      @note   This is an input parameter: it is not updated with the final key
 *                              attributes. The final attributes of the new key can be queried by
 *                              calling @ref psa_get_key_attributes() with the key’s identifier.
 * @param   target_key  On success, an identifier for the newly created key. @ref PSA_KEY_ID_NULL
 *                      on failure.
 *
 * @return  @ref PSA_SUCCESS                Success. If the new key is persistent, the key material
 *                                          and the key’s metadata have been saved to persistent
 *                                          storage.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @p source_key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The following conditions can result in this error:
 *                                          -   @p source_key does not have the @ref
 *                                              PSA_KEY_USAGE_COPY usage flag.
 *                                          -   @p source_key does not have the @ref
 *                                              PSA_KEY_USAGE_EXPORT usage flag, and its storage
 *                                              location does not allow copying it to the target
 *                                              key’s storage location.
 *                                          -   @p The implementation does not permit creating
 *                                              a key with the specified attributes due to some
 *                                              implementation-specific policy.
 * @return  @ref PSA_ERROR_ALREADY_EXISTS   This is an attempt to create a persistent key,
 *                                          and there is already a persistent key with the given
 *                                          identifier.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          -   @p attributes specifies a key type or
 *                                              key size which does not match the attributes
 *                                              of @p source_key.
 *                                          -   The lifetime or identifier in @p attributes
 *                                              are invalid.
 *                                          -   The key policies from @p source_key and those
 *                                              specified in @p attributes are incompatible.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          -   The @p source_key storage location does not support
 *                                              copying to the target key’s storage location.
 *                                          -   The key attributes, as a whole, are not supported
 *                                              in the target key’s storage location.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_INSUFFICIENT_STORAGE
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t *attributes,
                          psa_key_id_t *target_key);

/**
 * @brief   Destroy a key.
 *
 * @details This function destroys a key from both volatile memory and, if applicable, non-volatile
 *          storage. Implementations must make a best effort to ensure that that the key material
 *          cannot be recovered.
 *
 *          This function also erases any metadata such as policies and frees resources associated
 *          with the key. Destroying the key makes the key identifier invalid, and the key
 *          identifier must not be used again by the application.
 *
 *          If a key is currently in use in a multi-part operation, then destroying the key will
 *          cause the multi-part operation to fail.
 *
 * @warning This implementation uses a virtual file system for storing and reading persistent keys
 *          to and from flash. Destroying a key only unlinks the file and does not erase the actual
 *          key data from flash. Anyone with hardware access can still recover the key material.
 *
 * @param   key Identifier of the key to erase. If this is @ref PSA_KEY_ID_NULL, do nothing and
 *              return @ref PSA_SUCCESS.
 *
 * @return  @ref PSA_SUCCESS                        Success. If @p key was a valid key identifier,
 *                                                  then the key material that it referred to has
 *                                                  been erased. Alternatively, @p key was
 *                                                  @ref PSA_KEY_ID_NULL.
 * @return  @ref PSA_ERROR_BAD_STATE                The library requires initializing by a call to
 *                                                  @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE           @p key is neither a valid key identifier,
 *                                                  nor @ref PSA_KEY_ID_NULL.
 * @return  @ref PSA_ERROR_NOT_PERMITTED            The key cannot be erased because it is
 *                                                  read-only, either due to a policy or due to
 *                                                  physical restrictions.
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE    There was an failure in communication with
 *                                                  the cryptoprocessor.
 *                                                  The key material might still be present in
 *                                                  the cryptoprocessor.
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED      An unexpected condition which is not a storage
 *                                                  corruption or a communication failure occurred.
 *                                                  The cryptoprocessor might have been compromised.
 * @return  @ref PSA_ERROR_STORAGE_FAILURE          The storage operation failed. Implementations
 *                                                  must make a best effort to erase key material
 *                                                  even in this situation, however, it might be
 *                                                  impossible to guarantee that the key material
 *                                                  is not recoverable in such cases.
 * @return  @ref PSA_ERROR_DATA_CORRUPT             The storage is corrupted. Implementations must
 *                                                  make a best effort to erase key material even
 *                                                  in this situation, however, it might be
 *                                                  impossible to guarantee that the key material
 *                                                  is not recoverable in such cases.
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_destroy_key(psa_key_id_t key);

/**
 * @brief   Export a key in binary format.
 *
 * @details The output of this function can be passed to @ref psa_import_key() to create an
 *          equivalent object.
 *
 *          If the implementation of @ref psa_import_key() supports other formats beyond the
 *          format specified here, the output from @ref psa_export_key() must use the
 *          representation specified here, not the original representation.
 *
 *          For standard key types, the output format is as follows:
 *          -   For symmetric keys, excluding HMAC keys, the format is the raw bytes of the key.
 *          -   For HMAC keys that are shorter than, or equal in size to, the underlying hash
 *              algorithm block size, the format is the raw bytes of the key.
 *          -   For HMAC keys that are longer than the underlying hash algorithm block size,
 *              the format is an implementation defined choice between the following formats:
 *              - The raw bytes of the key.
 *              - The raw bytes of the hash of the key, using the underlying hash algorithm.
 *              See also @ref PSA_KEY_TYPE_HMAC.
 *          -   For DES, the key data consists of 8 bytes. The parity bits must be correct.
 *          -   For Triple-DES, the format is the concatenation of the two or three DES keys.
 *          -   For RSA key pairs, with key type @ref PSA_KEY_TYPE_RSA_KEY_PAIR, the format is
 *              the non-encrypted DER encoding of the representation defined by in PKCS #1: RSA
 *              Cryptography Specifications Version 2.2 [RFC8017] as RSAPrivateKey, version 0.
 *              @code
 *              RSAPrivateKey ::= SEQUENCE {
 *                  version             INTEGER,  -- must be 0
 *                  modulus             INTEGER,  -- n
 *                  publicExponent      INTEGER,  -- e
 *                  privateExponent     INTEGER,  -- d
 *                  prime1              INTEGER,  -- p
 *                  prime2              INTEGER,  -- q
 *                  exponent1           INTEGER,  -- d mod (p-1)
 *                  exponent2           INTEGER,  -- d mod (q-1)
 *                  coefficient         INTEGER,  -- (inverse of q) mod p
 *              }
 *              @endcode
 *              @note   Although it is possible to define an RSA key pair or private key using a
 *                      subset of these elements, the output from @ref psa_export_key() for an RSA
 *                      key pair must include all of these elements.
 *          -   For elliptic curve key pairs, with key types for which PSA_KEY_TYPE_IS_ECC_KEY_PAIR
 *              () is true, the format is a representation of the private value.
 *              -   For Weierstrass curve families @c PSA_ECC_FAMILY_SECT_XX,
 *                  @c PSA_ECC_FAMILY_SECP_XX, @ref PSA_ECC_FAMILY_FRP and
 *                  @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1, the content of the @c privateKey field of
 *                  the @c ECPrivateKey format defined by Elliptic Curve Private Key Structure
 *                  [RFC5915]. This is a @c ceiling(m/8)-byte string in big-endian order where @c m
 *                  is the key size in bits.
 *              -   For curve family @ref PSA_ECC_FAMILY_MONTGOMERY, the scalar value of the
 *                  ‘private key’ in little-endian order as defined by Elliptic Curves for Security
 *                  [RFC7748] §6. The value must have the forced bits set to zero or one as
 *                  specified by @c decodeScalar25519() and @c decodeScalar448() in [RFC7748] §5.
 *                  This is a @c ceiling(m/8)-byte string where @c m is the key size in bits. This
 *                  is 32 bytes for Curve25519, and 56 bytes for Curve448.
 *              -   For the Twisted Edwards curve family @ref PSA_ECC_FAMILY_TWISTED_EDWARDS,
 *                  the private key is defined by Edwards-Curve Digital Signature Algorithm (EdDSA)
 *                  [RFC8032]. This is a 32-byte string for Edwards25519, and a 57-byte string
 *                  for Edwards448.
 *          -   For Diffie-Hellman key exchange key pairs, with key types for which @ref
 *              PSA_KEY_TYPE_IS_DH_KEY_PAIR() is true, the format is the representation of the
 *              private key x as a big-endian byte string. The length of the byte string is the
 *              private key size in bytes, and leading zeroes are not stripped.
 *          -   For public keys, with key types for which @ref PSA_KEY_TYPE_IS_PUBLIC_KEY() is
 *              true, the format is the same as for @ref psa_export_public_key().
 *
 *          The policy on the key must have the usage flag @ref PSA_KEY_USAGE_EXPORT set.
 *
 * @param   key         Identifier of the key to export. It must allow the usage @ref
 *                      PSA_KEY_USAGE_EXPORT, unless it is a public key.
 * @param   data        Buffer where the key data is to be written.
 * @param   data_size   Size of the data buffer in bytes. This must be appropriate for the key:
 *                      -   The required output size is @ref PSA_EXPORT_KEY_OUTPUT_SIZE(@p type,
 *                          @p bits) where @c type is the key type and @c bits is the key size in
 *                          bits.
 *                      -   @ref PSA_EXPORT_KEY_PAIR_MAX_SIZE evaluates to the maximum output size
 *                          of any supported key pair.
 *                      -   @ref PSA_EXPORT_PUBLIC_KEY_MAX_SIZE evaluates to the maximum output
 *                          size of any supported public key.
 *                      -   This API defines no maximum size for symmetric keys. Arbitrarily large
 *                          data items can be stored in the key store, for example certificates
 *                          that correspond to a stored private key or input material for key
 *                          derivation.
 * @param   data_length On success, the number of bytes that make up the key data.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*data_length) bytes of data
 *                                          contain the exported key
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @p key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref PSA_KEY_USAGE_EXPORT
 *                                          flag.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the data buffer is too small.
 *                                          @ref PSA_EXPORT_KEY_OUTPUT_SIZE() or @ref
 *                                          PSA_EXPORT_KEY_PAIR_MAX_SIZE can be used to determine
 *                                          a sufficient buffer size.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          -   The key’s storage location does not support export
 *                                              of the key
 *                                          -   The implementation does not support export of keys
 *                                              with this key type.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length);

/**
 * @brief   Export a public key or the public part of a key pair in binary format.
 *
 * @details The output of this function can be passed to @ref psa_import_key() to create an object
 *          that is equivalent to the public key.
 *
 *          If the implementation of @ref psa_import_key() supports other formats beyond the
 *          format specified here, the output from @ref psa_export_public_key() must use the
 *          representation specified here, not the original representation.
 *
 *          For standard key types, the output format is as follows:
 *          -   For RSA public keys, with key type @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY, the DER
 *              encoding of the representation defined by Algorithms and Identifiers for the
 *              Internet X.509 Public Key Infrastructure
 *              Certificate and Certificate Revocation List (CRL) Profile [RFC3279] §2.3.1 as
 *              @c RSAPublicKey.
 *              @code
 *              RSAPublicKey ::= SEQUENCE {
 *                  modulus            INTEGER,    -- n
 *                  publicExponent     INTEGER  }  -- e
 *              @endcode
 *          -   For elliptic curve key pairs, with key types for which
 *              @ref PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY() is true, the format depends on the key family:
 *              -   For Weierstrass curve families @c PSA_ECC_FAMILY_SECT_XX,
 *                  @c PSA_ECC_FAMILY_SECP_XX, @ref PSA_ECC_FAMILY_FRP and @ref
 *                  PSA_ECC_FAMILY_BRAINPOOL_P_R1, the uncompressed representation of an elliptic
 *                  curve point as an octet string defined in SEC 1: Elliptic Curve Cryptography
 *                  [SEC1] §2.3.3. If @c m is the bit size associated with the curve, i.e. the bit
 *                  size of @c q for a curve over @c F_q. The representation consists of:
 *                  -   The byte @c 0x04;
 *                  -   @c x_P as a @c ceiling(m/8)-byte string, big-endian;
 *                  -   @c y_P as a @c ceiling(m/8)-byte string, big-endian.
 *              -   For curve family @ref PSA_ECC_FAMILY_MONTGOMERY, the scalar value of the
 *                  ‘public key’ in little-endian order as defined by Elliptic Curves for Security
 *                  [RFC7748] §6. This is a @c ceiling(m/8)-byte string where @c m is the key size
 *                  in bits.
 *                  -   This is 32 bytes for Curve25519, computed as @c X25519(private_key, 9).
 *                  -   This is 56 bytes for Curve448, computed as @c X448(private_key, 5).
 *              -   For curve family @ref PSA_ECC_FAMILY_TWISTED_EDWARDS, the public key is defined
 *                  by Edwards-Curve Digital Signature Algorithm (EdDSA) [RFC8032].
 *                  This is a 32-byte string for Edwards25519, and a 57-byte string for Edwards448.
 *          -   For Diffie-Hellman key exchange public keys, with key types for which
 *              @ref PSA_KEY_TYPE_IS_DH_PUBLIC_KEY is true, the format is the representation of the
 *              public key `y = g^x mod p` as a big-endian byte string. The
 *              length of the byte string is the length of the base prime p in bytes.
 *          Exporting a public key object or the public part of a key pair is always permitted,
 *          regardless of the key’s usage flags.
 *
 * @param   key         Identifier of the key to export.
 * @param   data        Buffer where the key data is to be written.
 * @param   data_size   Size of the @c data buffer in bytes. This must be appropriate for the key:
 *                      -   The required output size is @ref PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(
 *                          @p type, @p bits) where @c type is the key type and @c bits is the key
 *                          size in bits.
 *                      -   @ref PSA_EXPORT_PUBLIC_KEY_MAX_SIZE evaluates to the maximum output
 *                          size of any supported public key or public part of a key pair.
 * @param   data_length On success, the number of bytes that make up the key data.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*data_length) bytes of @p data
 *                                          contain the exported public key.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the data buffer is too small.
 *                                          @ref PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE() or
 *                                          @ref PSA_EXPORT_PUBLIC_KEY_MAX_SIZE can be used to
 *                                          determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The key is neither a public key nor a key pair.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          -   The key’s storage location does not support export
 *                                              of the key.
 *                                          -   The implementation does not support export of keys
 *                                              with this key type.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_export_public_key(psa_key_id_t key,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length);

/**
 * @brief   Built-in key generation function.
 *
 * @details This function generates a random symmetric key. As of yet it does not implement
 *          asymmetric key generation.
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param key_buffer_length
 * @return psa_status_t
 */
psa_status_t psa_builtin_generate_key(const psa_key_attributes_t *attributes, uint8_t *key_buffer,
                                      size_t key_buffer_size, size_t *key_buffer_length);

/**
 * @brief   Generate a key or key pair.
 *
 * @details The key is generated randomly. Its location, policy, type and size are taken from
 *          @c attributes.
 *
 *          Implementations must reject an attempt to generate a key of size 0.
 *
 *          The following type-specific considerations apply:
 *          -   For RSA keys (@ref PSA_KEY_TYPE_RSA_KEY_PAIR), the public exponent is 65537.
 *              The modulus is a product of two probabilistic primes between 2^{n-1} and 2^n where n
 *              is the bit size specified in the attributes.
 *
 * @param   attributes  The attributes for the new key. This function uses the attributes as
 *                      follows:
 *                      -   The key type is required. It cannot be an asymmetric public key.
 *                      -   The key size is required. It must be a valid size for the key type.
 *                      -   The key permitted-algorithm policy is required for keys that will be
 *                          used for a cryptographic operation, see Permitted algorithms.
 *                      -   The key usage flags define what operations are permitted with the key,
 *                          see Key usage flags.
 *                      -   The key lifetime and identifier are required for a persistent key.
 *                          @note   This is an input parameter: it is not updated with the final
 *                                  key attributes. The final attributes of the new key can be
 *                                  queried by calling @ref psa_get_key_attributes() with the key’s
 *                                  identifier.
 * @param   key         On success, an identifier for the newly created key. @ref PSA_KEY_ID_NULL
 *                      on failure.
 *
 * @return  @ref PSA_SUCCESS                    Success. If the key is persistent, the key material
 *                                              and the key’s metadata have been saved to persistent
 *                                              storage.
 * @return  @ref PSA_ERROR_ALREADY_EXISTS       This is an attempt to create a persistent key, and
 *                                              there is already a persistent key with the given
 *                                              identifier.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        The key type or key size is not supported, either by
 *                                              the implementation in general or in this particular
 *                                              persistent location.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The key attributes, as a whole, are invalid.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The key type is an asymmetric public key type.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The key size is not a valid size for the key type.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_INSUFFICIENT_STORAGE
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE            The library has not been previously initialized
 *                                              by  @ref psa_crypto_init(). It is
 *                                              implementation-dependent whether a failure to
 *                                              initialize results in this error code.
 */
psa_status_t psa_generate_key(const psa_key_attributes_t *attributes,
                              psa_key_id_t *key);
#endif /* MODULE_PSA_KEY_MANAGEMENT */

/**
 * @brief   Built-in function for random number generation.
 *
 * @details This function is being used to generate a random number if no other driver for random
 *          number generation is present. It uses the RIOT RNG module as a default backend.
 *
 * @param   output      Output buffer of the size of the random number to be generated.
 * @param   output_size Size of @c output in bytes
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT @c output is NULL
 */
psa_status_t psa_builtin_generate_random(   uint8_t *output,
                                            size_t output_size);

/**
 * @brief   Generate random bytes.
 *
 * @warning This function can fail! Callers MUST check the return status and MUST NOT use the
 *          content of the output buffer if the return status is not @ref PSA_SUCCESS.
 *
 * @note    To generate a key, use @ref psa_generate_key() instead.
 *
 * @param   output      Output buffer for the generated data.
 * @param   output_size Number of bytes to generate and output.
 *
 * @return  @ref PSA_SUCCESS                Success. @c output contains @c output_size bytes
 *                                          of generated random data.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT @c output is NULL
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_generate_random(uint8_t *output,
                                 size_t output_size);

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT) || defined(DOXYGEN)
/**
 * @brief   Declare the permitted algorithm policy for a key.
 *
 * @details The permitted algorithm policy of a key encodes which algorithm or algorithms
 *          are permitted to be used with this key.
 *          This function overwrites any permitted algorithm policy previously set in attributes.
 *
 * @param   attributes  The attribute object to write to.
 * @param   alg         The permitted algorithm to write.
 */
static inline void psa_set_key_algorithm(psa_key_attributes_t *attributes,
                                         psa_algorithm_t alg)
{
    attributes->policy.alg = alg;
}

/**
 * @brief   Retrieve the permitted algorithm policy from key attributes.
 *
 * @param   attributes  The key attribute object to query.
 *
 * @return  @ref psa_algorithm_t The algorithm stored in the attribute object.
 */
static inline psa_algorithm_t psa_get_key_algorithm(const psa_key_attributes_t *attributes)
{
    return attributes->policy.alg;
}

/**
 * @brief   Declare the size of a key.
 *
 * @details This function overwrites any key size previously set in attributes.
 *
 * @param   attributes  The attribute object to write to.
 * @param   bits        The key size in bits. If this is 0,
 *                      the key size in attributes becomes
 *                      unspecified. Keys of size 0 are not supported.
 */
static inline void psa_set_key_bits(psa_key_attributes_t *attributes,
                                    size_t bits)
{
    attributes->bits = bits;
}

/**
 * @brief   Retrieve the key size from key attributes.
 *
 * @param   attributes  The key attribute object to query.
 *
 * @return  size_t   The key size stored in the attribute object, in bits.
 */
static inline size_t psa_get_key_bits(const psa_key_attributes_t *attributes)
{
    return attributes->bits;
}

/**
 * @brief   Declare a key as persistent and set its key identifier.
 *
 * @details The application must choose a value for id between @ref PSA_KEY_ID_USER_MIN and
 *          @ref PSA_KEY_ID_USER_MAX. If the attribute object currently declares the key as
 *          volatile, which is the default lifetime of an attribute object, this function sets
 *          the lifetime attribute to @ref PSA_KEY_LIFETIME_PERSISTENT.
 *
 *          This function does not access storage, it merely stores the given value in the
 *          attribute object. The persistent key will be written to storage when the attribute
 *          object is passed to a key creation function such as @ref psa_import_key(),
 *          @ref psa_generate_key(), @ref psa_key_derivation_output_key() or @ref psa_copy_key().
 *
 * @param   attributes  The attribute object to write to.
 * @param   id          The persistent identifier for the key.
 */
static inline void psa_set_key_id(psa_key_attributes_t *attributes, psa_key_id_t id)
{
    attributes->id = id;
}

/**
 * @brief   Retrieve the key identifier from key attributes.
 *
 * @param   attributes  The key attribute object to query.
 *
 * @return  @ref psa_key_id_t   The persistent identifier stored in the attribute object.
 *                              This value is unspecified if the attribute object declares
 *                              the key as volatile.
 */
static inline psa_key_id_t psa_get_key_id(const psa_key_attributes_t *attributes)
{
    return attributes->id;
}

/**
 * @brief   Set the location of a persistent key.
 *
 * @details To make a key persistent, give it a persistent key identifier by using
 *          @ref psa_set_key_id(). By default, a key that has a persistent identifier is stored in
 *          the default storage area identifier by @ref PSA_KEY_LIFETIME_PERSISTENT. Call this
 *          function to choose a storage area, or to explicitly declare the key as volatile.
 *
 *          This function does not access storage, it merely stores the given value in the
 *          attribute object. The persistent key will be written to storage when the attribute
 *          object is passed to a key creation function such as @ref psa_import_key(),
 *          @ref psa_generate_key(), @ref psa_key_derivation_output_key() or @ref psa_copy_key().
 *
 * @param   attributes  The attribute object to write to.
 * @param   lifetime    The lifetime for the key. If this is @ref PSA_KEY_LIFETIME_VOLATILE,
 *                      the key will be volatile, and the key identifier attribute is reset
 *                      to @ref PSA_KEY_ID_NULL.
 */
static inline void psa_set_key_lifetime(psa_key_attributes_t *attributes,
                                        psa_key_lifetime_t lifetime)
{
    attributes->lifetime = lifetime;
}

/**
 * @brief   Retrieve the lifetime from key attributes.
 *
 * @param   attributes  The key attribute object to query.
 *
 * @return  @ref psa_key_lifetime_t The lifetime value stored in the attribute object.
 */
static inline psa_key_lifetime_t psa_get_key_lifetime(const psa_key_attributes_t *attributes)
{
    return attributes->lifetime;
}

/**
 * @brief   Declare the type of a key.
 *
 * @details This function overwrites any key type previously set in @c attributes.
 *
 * @param   attributes  The attribute object to write to.
 * @param   type        The key type to write. If this is @ref PSA_KEY_TYPE_NONE,
 *                      the key type in attributes becomes unspecified.
 */
static inline void psa_set_key_type(psa_key_attributes_t *attributes,
                                    psa_key_type_t type)
{
    attributes->type = type;
}

/**
 * @brief   Retrieve the key type from key attributes.
 *
 * @param   attributes  The key attribute object to query.
 *
 * @return  @ref psa_key_type_t The key type stored in the attribute object.
 */
static inline psa_key_type_t psa_get_key_type(const psa_key_attributes_t *attributes)
{
    return attributes->type;
}

/**
 * @brief   Declare usage flags for a key.
 *
 * @details Usage flags are part of a key’s policy. They encode what kind of operations are
 *          permitted on the key. For more details, see Key policies.
 *
 *          This function overwrites any usage flags previously set in attributes.
 *
 * @param   attributes  The attribute object to write to.
 * @param   usage_flags The usage flags to write.
 */
static inline void psa_set_key_usage_flags(psa_key_attributes_t *attributes,
                                           psa_key_usage_t usage_flags)
{
    attributes->policy.usage = usage_flags;
}

/**
 * @brief   Retrieve the usage flags from key attributes.
 *
 * @param   attributes  The key attribute object to query.
 *
 * @return  @ref psa_key_usage_t    The usage flags stored in the attribute object.
 */
static inline psa_key_usage_t psa_get_key_usage_flags(const psa_key_attributes_t *attributes)
{
    return attributes->policy.usage;
}

/**
 * @brief   Reset a key attribute object to a freshly initialized state.
 *
 * @details The attribute object must be initialized as described in the documentation of the type
 *          @ref psa_key_attributes_t before calling this function. Once the object has been
 *          initialized, this function can be called at any time.
 *
 *          This function frees any auxiliary resources that the object might contain.
 *
 * @param   attributes  The attribute object to reset.
 */
static inline void psa_reset_key_attributes(psa_key_attributes_t *attributes)
{
    *attributes = psa_key_attributes_init();
}

/**
 * @brief   Retrieve the attributes of a key.
 *
 * @details This function first resets the attribute object as with @ref psa_reset_key_attributes().
 *          It then copies the attributes of the given key into the given attribute object.
 *
 * @note    This function clears any previous content from the attribute object and therefore
 *          expects it to be in a valid state. In particular, if this function is called on a newly
 *          allocated attribute object, the attribute object must be initialized before calling
 *          this function.
 *
 * @param   key         Identifier of the key to query.
 * @param   attributes  On entry, @c *attributes must be in a valid state. On successful return,
 *                      it contains the attributes of the key. On failure, it is equivalent
 *                      to a freshly-initialized attribute object.
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INVALID_HANDLE
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 * @return  @ref PSA_ERROR_BAD_STATE        The library has not been previously initialized
 *                                          by  @ref psa_crypto_init(). It is
 *                                          implementation-dependent whether a failure to
 *                                          initialize results in this error code.
 */
psa_status_t psa_get_key_attributes(psa_key_id_t key,
                                    psa_key_attributes_t *attributes);
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_HASH) || defined(DOXYGEN)
/**
 * @brief   Abort a hash operation.
 *
 * @details Aborting an operation frees all associated resources except for the  @p operation
 *          structure itself. Once aborted, the operation object can be reused for another operation
 *          by calling @ref psa_hash_setup() again.
 *
 *          You may call this function any time after the operation object has
 *          been initialized by one of the methods described in @ref psa_hash_operation_t.
 *
 *          In particular, calling @ref psa_hash_abort() after the operation has been
 *          terminated by a call to @ref psa_hash_abort(), @ref psa_hash_finish() or
 *          @ref psa_hash_verify() is safe and has no effect.
 *
 * @param   operation   Initialized hash operation.
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_HARDWARE_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_BAD_STATE        The library has not been previously initialized
 *                                          by  @ref psa_crypto_init(). It is
 *                                          implementation-dependent whether a failure to
 *                                          initialize results in this error code.
 */
psa_status_t psa_hash_abort(psa_hash_operation_t *operation);

/**
 * @brief   Clone a hash operation.
 *
 * @details This function copies the state of an ongoing hash operation to a new operation object.
 *          In other words, this function is equivalent to calling @ref psa_hash_setup() on
 *          @p target_operation with the same algorithm that @p source_operation was set up for,
 *          then @ref psa_hash_update() on @p target_operation with the same input that that was
 *          passed to @p source_operation. After this function returns, the two objects are
 *          independent, i.e. subsequent calls involving one of the objects do not affect the other
 *          object.
 *
 * @param   source_operation    The active hash operation to clone.
 * @param   target_operation    The operation object to set up.
 *                              It must be initialized but not active.
 *
 * @return @ref PSA_SUCCESS
 *@return  @ref PSA_ERROR_BAD_STATE             The following conditions can result in this error:
 *                                              -   The @p source_operation state is not valid:
 *                                                  it must be active.
 *                                              -   The @p target_operation state is not valid:
 *                                                  it must be inactive.
 *                                              -   The library requires initializing by a call
 *                                                  to @ref psa_crypto_init().
 *@return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 *@return  @ref PSA_ERROR_HARDWARE_FAILURE
 *@return  @ref PSA_ERROR_CORRUPTION_DETECTED
 *@return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 */
psa_status_t psa_hash_clone(const psa_hash_operation_t *source_operation,
                            psa_hash_operation_t *target_operation);

/**
 * @brief   Calculate the hash (digest) of a message and compare it with a reference value.
 *
 * @param   alg The hash algorithm to compute: a value of type @ref psa_algorithm_t such
 *              that @ref PSA_ALG_IS_HASH(@p alg) is true.
 * @param   input           Buffer containing the message to hash.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   hash            Buffer containing the expected hash value.
 * @param   hash_length     Size of the hash buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                    Success. The expected hash is identical to the
 *                                              actual hash of the input.
 * @return  @ref PSA_ERROR_BAD_STATE            The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    The calculated hash of the message does not match
 *                                              the value in hash.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The following conditions can result in this error:
 *                                              -   @p alg is not a hash algorithm.
 *                                              -   @p input_length is too large for alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        The following conditions can result in this error:
 *                                              -   @p alg is not supported or is not a hash
 *                                                  algorithm.
 *                                              -   @p input_length is too large for the
 *                                                  implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              const uint8_t *hash,
                              size_t hash_length);

/**
 * @brief   Calculate the hash (digest) of a message.
 *
 * @note    To verify the hash of a message against an expected value,
 *          use @ref psa_hash_compare() instead.
 *
 * @param   alg             The hash algorithm to compute: a value of type @ref psa_algorithm_t
 *                          such that @ref PSA_ALG_IS_HASH(@p alg) is true.
 * @param   input           Buffer containing the message to hash.
 * @param   input_length    Size of the @p input buffer in bytes.
 * @param   hash            Buffer where the hash is to be written.
 * @param   hash_size       Size of the @p hash buffer in bytes.
 *                          This must be at least @ref PSA_HASH_LENGTH(@p alg).
 * @param   hash_length     On success, the number of bytes that make up the hash value.
 *                          This is always @ref PSA_HASH_LENGTH(@p alg).
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*hash_length) bytes of
 *                                          @p hash contain the hash value.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the hash buffer is too small.
 *                                          @ref PSA_HASH_LENGTH() can be used to determine
 *                                          a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @p alg is not a hash algorithm
 *                                          - @p input_length is too large for alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a hash algorithm.
 *                                          - @c input_length is too large for the implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              uint8_t *hash,
                              size_t hash_size,
                              size_t *hash_length);

/**
 * @brief   Finish the calculation of the hash of a message.
 *
 * @details The application must call @ref psa_hash_setup() before calling this function.
 *          This function calculates the hash of the message formed by concatenating
 *          the inputs passed to preceding calls to @ref psa_hash_update().
 *
 *          When this function returns successfully, the operation becomes inactive.
 *          If this function returns an error status, the operation enters an error
 *          state and must be aborted by calling @ref psa_hash_abort().
 *
 * @warning Applications should not call this function if they expect
 *          a specific value for the hash. Call @ref psa_hash_verify() instead.
 *          Beware that comparing integrity or authenticity data such as
 *          hash values with a function such as @c memcmp is risky
 *          because the time taken by the comparison may leak information
 *          about the hashed data which could allow an attacker to guess
 *          a valid hash and thereby bypass security controls.
 *
 * @param   operation   Active hash operation.
 * @param   hash        Buffer where the hash is to be written.
 * @param   hash_size   Size of the @p hash buffer in bytes. This must be at least
 *                      @ref PSA_HASH_LENGTH(@p alg) where @p alg is the algorithm that
 *                      the operation performs.
 * @param   hash_length On success, the number of bytes that make up the hash value. This is always
 *                      @ref PSA_HASH_LENGTH(@p alg) where @p alg is the hash algorithm that is
 *                      calculated.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*hash_length) bytes of @p hash
 *                                          contain the hash value.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be active.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the hash buffer is too small.
 *                                          @ref PSA_HASH_LENGTH() can be used to determine
 *                                          a sufficient buffer size.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length);

/**
 * @brief   Set up a multi-part hash operation using the hash suspend state from a previously
 *          suspended hash operation.
 *
 * @details See @ref psa_hash_suspend() for an example of how to use this function to suspend
 *          and resume a hash operation.
 *
 *          After a successful call to @ref psa_hash_resume(), the application must eventually
 *          terminate the operation. The following events terminate an operation:
 *          -   A successful call to @ref psa_hash_finish(), @ref psa_hash_verify() or
 *              @ref psa_hash_suspend().
 *          -   A call to @ref psa_hash_abort().
 *
 * @param   operation           The operation object to set up. It must have been initialized
 *                              as per the documentation for @ref psa_hash_operation_t and not
 *                              yet in use.
 * @param   hash_state          A buffer containing the suspended hash state which is to be
 *                              resumed. This must be in the format output by @ref
 *                              psa_hash_suspend().
 * @param   hash_state_length   Length of hash_state in bytes.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be
 *                                              inactive.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT @p hash_state does not correspond to a valid hash
 *                                          suspend state. See Hash suspend state format for
 *                                          the definition.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The provided hash suspend state is for an algorithm
 *                                          that is not supported.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_resume(psa_hash_operation_t *operation,
                             const uint8_t *hash_state,
                             size_t hash_state_length);

/**
 * @brief   Set up a multipart hash operation.
 *
 * @details The sequence of operations to calculate a hash (message digest)
 *          is as follows:
 *          -#  Allocate an operation object which will be passed to all the functions listed here.
 *          -#  Initialize the operation object with one of the methods described in the
 *              documentation for @ref psa_hash_operation_t, e.g. @ref PSA_HASH_OPERATION_INIT.
 *          -#  Call @ref psa_hash_setup() to specify the algorithm.
 *          -#  Call @ref psa_hash_update() zero, one or more times, passing a fragment of the
 *              message each time. The hash that is calculated is the hash of the concatenation
 *              of these messages in order.
 *          -#  To calculate the hash, call @ref psa_hash_finish(). To compare the hash with an
 *              expected value, call @ref psa_hash_verify().
 *
 *          If an error occurs at any step after a call to @ref psa_hash_setup(), the operation
 *          will need to be reset by a call to @ref psa_hash_abort(). The application may call
 *          @ref psa_hash_abort() at any time after the operation has been initialized.
 *
 *          After a successful call to @ref psa_hash_setup(), the application must eventually
 *          terminate the operation. The following events terminate an operation:
 *          - A successful call to @ref psa_hash_finish() or @ref psa_hash_verify().
 *          - A call to @ref psa_hash_abort().
 *
 * @param   operation   The operation object to set up. It must have been initialized as per the
 *                      documentation for @ref  psa_hash_operation_t and not yet in use.
 * @param   alg         The hash algorithm to compute: a value of type @ref psa_algorithm_t such
 *                      that @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be
 *                                              inactive.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT @p alg is not a hash algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    @p alg is not supported or is not a hash algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg);

/**
 * @brief   Halt the hash operation and extract the intermediate state of the hash computation.
 *
 * @details The application must call @ref psa_hash_setup() or @ref psa_hash_resume() before
 *          calling this function. This function extracts an intermediate state of the hash
 *          computation of the message formed by concatenating the inputs passed to preceding
 *          calls to @ref psa_hash_update().
 *
 *          This function can be used to halt a hash operation, and then resume the hash operation
 *          at a later time, or in another application, by transferring the extracted hash suspend
 *          state to a call to @ref psa_hash_resume().
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_hash_abort().
 *
 *          Hash suspend and resume is not defined for the SHA3 family of hash algorithms.
 *          Hash suspend state defines the format of the output from @ref psa_hash_suspend().
 *
 * @warning Applications must not use any of the hash suspend state as if it was a hash output.
 *          Instead, the suspend state must only be used to resume a hash operation, and
 *          @ref psa_hash_finish() or @ref psa_hash_verify() can then calculate or verify the
 *          final hash value.
 *
 *          ## Usage
 *          The sequence of operations to suspend and resume a hash operation is as follows:
 *          -#  Compute the first part of the hash.
 *              -#  Allocate an operation object and initialize it as described in the
 *                  documentation for @ref psa_hash_operation_t.
 *              -#  Call @ref psa_hash_setup() to specify the algorithm.
 *              -#  Call @ref psa_hash_update() zero, one or more times, passing a fragment
 *                  of the message each time.
 *              -#  Call @ref psa_hash_suspend() to extract the hash suspend state into a buffer.
 *          -#  Pass the hash state buffer to the application which will resume the operation.
 *          -#  Compute the rest of the hash.
 *              -#  Allocate an operation object and initialize it as described in the
 *                  documentation for @ref psa_hash_operation_t.
 *              -#  Call @ref psa_hash_resume() with the extracted hash state.
 *              -#  Call @ref psa_hash_update() zero, one or more times, passing a fragment
 *                  of the message each time.
 *              -#  To calculate the hash, call @ref psa_hash_finish(). To compare the hash
 *                  with an expected value, call @ref psa_hash_verify().
 *
 *          If an error occurs at any step after a call to @ref psa_hash_setup() or
 *          @ref psa_hash_resume(), the operation will need to be reset by a call to
 *          @ref psa_hash_abort(). The application can call @ref psa_hash_abort() at
 *          any time after the operation has been initialized.
 *
 * @param   operation           Active hash operation.
 * @param   hash_state          Buffer where the hash suspend state is to be written.
 * @param   hash_state_size     Size of the hash_state buffer in bytes. This must be
 *                              appropriate for the selected algorithm:
 *                              -   A sufficient output size is @ref
 *                                  PSA_HASH_SUSPEND_OUTPUT_SIZE(@p alg) where alg is
 *                                  the algorithm that was used to set up the operation.
 *                              -   @ref PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE evaluates to
 *                                  the maximum output size of any supported hash algorithm.
 * @param   hash_state_length   On success, the number of bytes that make up the hash suspend state.
 *
 * @return  @ref PSA_SUCCESS                Success. The first @p (*hash_state_length) bytes of
 *                                          @p hash_state contain the intermediate hash state.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be active.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the hash_state buffer is too small.
 *                                          @ref PSA_HASH_SUSPEND_OUTPUT_SIZE() or
 *                                          @ref PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE can be used
 *                                          to determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The hash algorithm being computed does not support
 *                                          suspend and resume.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_suspend(psa_hash_operation_t *operation,
                              uint8_t *hash_state,
                              size_t hash_state_size,
                              size_t *hash_state_length);

/**
 * @brief   Add a message fragment to a multi-part hash operation.
 *
 * @details The application must call @ref psa_hash_setup() before calling this function.
 *          If this function returns an error status, the operation enters an error
 *          state and must be aborted by calling @ref psa_hash_abort().
 *
 * @param   operation       Active hash operation.
 * @param   input           Buffer containing the message fragment to hash.
 * @param   input_length    Size of the @p input buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          -   The operation state is not valid: it must be active.
 *                                          -   The library requires initializing by a call to
 *                                              @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The total input for the operation is too large for the
 *                                          hash algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The total input for the operation is too large for the
 *                                          implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length);

/**
 * @brief   Finish the calculation of the hash of a message and compare it with an expected value.
 *
 * @details The application must call @ref psa_hash_setup() before calling this function. This
 *          function calculates the hash of the message formed by concatenating the inputs passed
 *          to preceding calls to @ref psa_hash_update(). It then compares the calculated hash with
 *          the expected hash passed as a parameter to this function.
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_hash_abort().
 *
 * @note    Implementations shall make the best effort to ensure that the comparison between the
 *          actual hash and the expected hash is performed in constant time.
 *
 * @param   operation   Active hash operation.
 * @param   hash        Buffer containing the expected hash value.
 * @param   hash_length Size of the @p hash buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                    Success. The expected hash is identical to the
 *                                              actual hash of the message.
 * @return  @ref PSA_ERROR_BAD_STATE            The following conditions can result in this error:
 *                                              -   The operation state is not valid: it must be
 *                                                  active.
 *                                              -   The library requires initializing by a call to
 *                                                  @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    The calculated hash of the message does not match
 *                                              the value in hash.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash,
                             size_t hash_length);
#endif /* MODULE_PSA_HASH */

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT) || defined(DOXYGEN)
/**
 * @brief   Built-in key import function.
 *
 * @details Copies a plain key into local memory. This function is used by the implementation,
 *          when an imported key is stored only in local memory.
 *
 * @param   attributes          The attributes for the new key.
 *                              This function uses the attributes as follows:
 *                              - The key type is required, and determines how the data buffer is
 *                                interpreted.
 *                              - The key size is always determined from the  data buffer. If the
 *                                key size in attributes is nonzero, it must be equal to the size
 *                                determined from data.
 *                              - The key permitted-algorithm policy is required for keys that will
 *                                be used for a cryptographic operation, see Permitted algorithms.
 *                              - The key usage flags define what operations are permitted with the
 *                                key, see Key usage flags.
 *                              - The key lifetime and identifier are required for a persistent key.
 *                              @note   This is an input parameter: it is not updated with the
 *                                      final key attributes. The final attributes of the new key
 *                                      can be queried by calling @ref psa_get_key_attributes()
 *                                      with the key’s identifier.
 * @param   data                Buffer containing the key data. The content of this buffer is
 *                              interpreted according to the type declared in attributes. All
 *                              implementations must support at least the format described in
 *                              the documentation of @ref psa_export_key() or @ref
 *                              psa_export_public_key() for the chosen type.
 *                              Implementations can support other formats, but be conservative in
 *                              interpreting the key data: it is recommended that implementations
 *                              reject content if it might be erroneous, for example, if it is the
 *                              wrong type or is truncated.
 * @param   data_length         Size of the data buffer in bytes.
 * @param   key_buffer          Pointer to buffer containing the plain text key material
 * @param   key_buffer_size     Size of @p key_buffer in bytes
 * @param   key_buffer_length   Size of the key buffer
 * @param   bits                Size of the key in bits
 *
 * @return  @ref PSA_SUCCESS                Success. If the key is persistent, the key material
 *                                          and the key’s metadata have been saved to persistent
 *                                          storage.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          -   The key size is nonzero, and is incompatible with
 *                                              the key data in @p data.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The key attributes, as a whole, are not supported,
 *                                          either by the implementation in general or in the
 *                                          specified storage location.
 */
psa_status_t psa_builtin_import_key(const psa_key_attributes_t *attributes,
                                    const uint8_t *data, size_t data_length,
                                    uint8_t *key_buffer, size_t key_buffer_size,
                                    size_t *key_buffer_length, size_t *bits);

/**
 * @brief   Import a key in binary format.
 *
 * @details This function supports any output from @ref psa_export_key(). Refer to the
 *          documentation of @ref psa_export_public_key() for the format of public keys
 *          and to the documentation of @ref psa_export_key() for the format for other key types.
 *
 *          The key data determines the key size. The attributes can optionally specify a key size;
 *          in this case it must match the size determined from the key data. A key size of 0 in
 *          attributes indicates that the key size is solely determined by the key data.
 *
 *          Implementations must reject an attempt to import a key of size 0.
 *
 *          This specification defines a single format for each key type. Implementations can
 *          optionally support other formats in addition to the standard format. It is recommended
 *          that implementations that support other formats ensure that the formats are clearly
 *          unambiguous, to minimize the risk that an invalid input is accidentally interpreted
 *          according to a different format.
 *
 * @note    The PSA Crypto API does not support asymmetric private key objects outside of a key
 *          pair. To import a private key, the attributes must specify the corresponding key pair
 *          type. Depending on the key type, either the import format contains the public key data
 *          or the implementation will reconstruct the public key from the private key as needed.
 *
 * @param   attributes  The attributes for the new key.
 *                      This function uses the attributes as follows:
 *                      - The key type is required, and determines how the data buffer is
 *                        interpreted.
 *                      - The key size is always determined from the  data buffer. If the key
 *                        size in attributes is nonzero, it must be equal to the size
 *                        determined from data.
 *                      - The key permitted-algorithm policy is required for keys that will be
 *                        used for a cryptographic operation, see Permitted algorithms.
 *                      - The key usage flags define what operations are permitted with the key,
 *                        see Key usage flags.
 *                      - The key lifetime and identifier are required for a persistent key.
 *                      @note   This is an input parameter: it is not updated with the final key
 *                              attributes. The final attributes of the new key can be queried
 *                              by calling @ref psa_get_key_attributes() with the key’s identifier.
 *
 * @param   data        Buffer containing the key data. The content of this buffer is interpreted
 *                      according to the type declared in attributes. All implementations must
 *                      support at least the format described in the documentation of
 *                      @ref psa_export_key() or @ref psa_export_public_key() for the chosen type.
 *                      Implementations can support other formats, but be conservative in
 *                      interpreting the key data: it is recommended that implementations reject
 *                      content if it might be erroneous, for example, if it is the wrong type or
 *                      is truncated.
 * @param   data_length Size of the data buffer in bytes.
 * @param   key         On success, an identifier for the newly created key.
 *                      @ref PSA_KEY_ID_NULL on failure.
 *
 * @return  @ref PSA_SUCCESS                Success. If the key is persistent, the key material
 *                                          and the key’s metadata have been saved to persistent
 *                                          storage.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to
 *                                          @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The implementation does not permit creating a
 *                                          key with the specified attributes due to some
 *                                          implementation-specific policy.
 * @return  @ref PSA_ERROR_ALREADY_EXISTS   This is an attempt to create a persistent key,
 *                                          and there is already a persistent key with the given
 *                                          identifier.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - The key type is invalid.
 *                                          - The key size is nonzero, and is incompatible with
 *                                            the key data in @p data.
 *                                          - The key lifetime is invalid.
 *                                          - The key identifier is not valid for the key
 *                                            lifetime.
 *                                          - The key usage flags include invalid values.
 *                                          - The key’s permitted-usage algorithm is invalid.
 *                                          - The key attributes, as a whole, are invalid.
 *                                          - The key data is not correctly formatted for the key
 *                                            type.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The key attributes, as a whole, are not supported,
 *                                          either by the implementation in general or in the
 *                                          specified storage location.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_INSUFFICIENT_STORAGE
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_import_key(const psa_key_attributes_t *attributes,
                            const uint8_t *data,
                            size_t data_length,
                            psa_key_id_t *key);
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_KEY_DERIVATION) || defined(DOXYGEN)
/**
 * @brief   Abort a key derivation operation.
 *
 * @details Aborting an operation frees all associated resources except for the operation object
 *          itself. Once aborted, the operation object can be reused for another operation by
 *          calling @ref psa_key_derivation_setup() again.
 *
 *          This function can be called at any time after the operation object has been initialized
 *          as described in @ref psa_key_derivation_operation_t.
 *
 *          In particular, it is valid to call @ref psa_key_derivation_abort() twice, or to call
 *          @ref psa_key_derivation_abort() on an operation that has not been set up.
 *
 * @param   operation   The operation to abort.
 *
 * @return  @ref PSA_SUCCESS            Success. The operation object can now be discarded or
 *                                      reused.
 * @return  @ref PSA_ERROR_BAD_STATE    The library requires initializing by a call to @ref
 *                                      psa_crypto_init().
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t *operation);

/**
 * @brief   Retrieve the current capacity of a key derivation operation.
 *
 * @details The capacity of a key derivation is the maximum number of bytes that it can return.
 *          Reading N bytes of output from a key derivation operation reduces its capacity by at
 *          least N. The capacity can be reduced by more than N in the following situations:
 *          - Calling @ref psa_key_derivation_output_key() can reduce the capacity by more than
 *            the key size, depending on the type of key being generated. See @ref
 *            psa_key_derivation_output_key() for details of the key derivation process.
 *          - When the @ref psa_key_derivation_operation_t object is operating as a deterministic
 *            random bit generator (DBRG), which reduces capacity in whole blocks, even when less
 *            than a block is read.
 *
 * @param   operation   The operation to query.
 * @param   capacity    On success, the capacity of the operation.
 *
 * @return  @ref PSA_SUCCESS            Success. The maximum number of bytes that this key
 *                                      derivation can return is (@c *capacity).
 * @return  @ref PSA_ERROR_BAD_STATE    The following conditions can result in this error:
 *                                      - The operation state is not valid: it must be active.
 *                                      - The library requires initializing by a call to @ref
 *                                        psa_crypto_init().
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t *operation,
                                             size_t *capacity);

/**
 * @brief   Provide an input for key derivation or key agreement.
 *
 * @details Which inputs are required and in what order depends on the algorithm. Refer to the
 *          documentation of each key derivation or key agreement algorithm for information.
 *
 *          This function passes direct inputs, which is usually correct for non-secret inputs.
 *          To pass a secret input, which is normally in a key object, call @ref
 *          psa_key_derivation_input_key() instead of this function. Refer to the documentation
 *          of individual step types (@c PSA_KEY_DERIVATION_INPUT_xxx values of type @ref
 *          psa_key_derivation_step_t) for more information.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_key_derivation_abort().
 *
 * @param   operation   The key derivation operation object to use. It must have been set up with
 *                      @ref psa_key_derivation_setup() and must not have produced any output yet.
 * @param   step        Which step the input data is for.
 * @param   data        Input data to use.
 * @param   data_length Size of the data buffer in bytes.
 * @return  @ref PSA_SUCCESS        Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid for this input step.
 *                                            This can happen if the application provides a step
 *                                            out of order or repeats a step that may not be
 *                                            repeated.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c step is not compatible with the operation’s
 *                                            algorithm.
 *                                          - @c step does not allow direct inputs.
 *                                          - @c data_length is too small or too large for step in
 *                                            this particular algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c step is not supported with the operation’s
 *                                            algorithm.
 *                                          - @c data_length is is not supported for step in this
 *                                            particular algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t *operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t *data,
                                            size_t data_length);

/**
 * @brief   Provide a numeric input for key derivation or key agreement.
 *
 * @details Which inputs are required and in what order depends on the algorithm. However, when an
 *          algorithm requires a particular order, numeric inputs usually come first as they tend
 *          to be configuration parameters. Refer to the documentation of each key derivation or
 *          key agreement algorithm for information.
 *
 *          This function is used for inputs which are fixed-size non-negative integers.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_key_derivation_abort().
 *
 * @param   operation   The key derivation operation object to use. It must have been set up with
 *                      @ref psa_key_derivation_setup() and must not have produced any output yet.
 * @param   step        Which step the input data is for.
 * @param   value       The value of the numeric input.
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid for this input step.
 *                                            This can happen if the application provides a step
 *                                            out of order or repeats a step that may not be
 *                                            repeated.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c step is not compatible with the operation’s
 *                                            algorithm.
 *                                          - @c step does not allow numerical inputs.
 *                                          - @c value is not valid for step in the operation’s
 *                                            algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c step is not supported with the operation’s
 *                                            algorithm.
 *                                          - @c value is not supported for step in the operation’s
 *                                            algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_input_integer(  psa_key_derivation_operation_t *operation,
                                                psa_key_derivation_step_t step,
                                                uint64_t value);

/**
 * @brief   Provide an input for key derivation in the form of a key.
 *
 * @details Which inputs are required and in what order depends on the algorithm. Refer to the
 *          documentation of each key derivation or key agreement algorithm for information.
 *
 *          This function obtains input from a key object, which is usually correct for secret
 *          inputs or for non-secret personalization strings kept in the key store. To pass a
 *          non-secret parameter which is not in the key store, call @ref
 *          psa_key_derivation_input_bytes() instead of this function. Refer to the documentation
 *          of individual step types (@c PSA_KEY_DERIVATION_INPUT_xxx values of type @ref
 *          psa_key_derivation_step_t) for more information.
 *
 *          @note   Once all inputs steps are completed, the following operations are permitted:
 *                  - @ref psa_key_derivation_output_bytes() — if each input was either a direct
 *                    input or a key with usage flag @ref PSA_KEY_USAGE_DERIVE.
 *                  - @ref psa_key_derivation_output_key() — if the input for step @ref
 *                    PSA_KEY_DERIVATION_INPUT_SECRET or @ref PSA_KEY_DERIVATION_INPUT_PASSWORD was
 *                    a key with usage flag @ref PSA_KEY_USAGE_DERIVE, and every other input was
 *                    either a direct input or a key with usage flag @ref PSA_KEY_USAGE_DERIVE.
 *                  - @ref psa_key_derivation_verify_bytes() — if each input was either a direct
 *                    input or a key with usage flag @ref PSA_KEY_USAGE_VERIFY_DERIVATION.
 *                  - @ref psa_key_derivation_verify_key() — under the same conditions as @ref
 *                    psa_key_derivation_verify_bytes().
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_key_derivation_abort().
 *
 * @param   operation   The key derivation operation object to use. It must have been set up with
 *                      @ref psa_key_derivation_setup() and must not have produced any output yet.
 * @param   step        Which step the input data is for.
 * @param   key         Identifier of the key. The key must have an appropriate type for step, it
 *                      must allow the usage @ref PSA_KEY_USAGE_DERIVE or @ref
 *                      PSA_KEY_USAGE_VERIFY_DERIVATION (see note), and it must permit the
 *                      algorithm used by the operation.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid for this input step.
 *                                            This can happen if the application provides a step
 *                                            out of order or repeats a step that may not be
 *                                            repeated.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key has neither the @ref PSA_KEY_USAGE_DERIVE nor
 *                                          the @ref PSA_KEY_USAGE_VERIFY_DERIVATION usage flag,
 *                                          or it does not permit the operation’s algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c step is not compatible with the operation’s
 *                                            algorithm.
 *                                          - @c step does not allow key inputs of the given type,
 *                                            or does not allow key inputs at all.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c step is not supported with the operation’s
 *                                            algorithm.
 *                                          - @c Key inputs of the given type are not supported for
 *                                            step in the operation’s algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t *operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key);

/**
 * @brief   Perform a key agreement and use the shared secret as input to a key derivation.
 *
 * @details A key agreement algorithm takes two inputs: a private key @c private_key, and a public
 *          key @c peer_key. The result of this function is passed as input to the key derivation
 *          operation. The output of this key derivation can be extracted by reading from the
 *          resulting operation to produce keys and other cryptographic material.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_key_derivation_abort().
 *
 * @param   operation       The key derivation operation object to use. It must have been set
 *                          up with @ref psa_key_derivation_setup() with a key agreement and
 *                          derivation algorithm alg: a value of type @ref psa_algorithm_t such
 *                          that @ref PSA_ALG_IS_KEY_AGREEMENT(@p alg) is true and @ref
 *                          PSA_ALG_IS_RAW_KEY_AGREEMENT(@p alg) is false.
 *                          The operation must be ready for an input of the type given by step.
 * @param   step            Which step the input data is for.
 * @param   private_key     Identifier of the private key to use. It must allow the usage @ref
 *                          PSA_KEY_USAGE_DERIVE.
 * @param   peer_key        Public key of the peer. The peer key must be in the same format that
 *                          @ref psa_import_key() accepts for the public key type corresponding to
 *                          the type of @c private_key. That is, this function performs the
 *                          equivalent of @ref psa_import_key(..., @p peer_key, @p
 *                          peer_key_length), with key attributes indicating the public key type
 *                          corresponding to the type of @c private_key. For example, for ECC keys,
 *                          this means that @c peer_key is interpreted as a point on the curve that
 *                          the private key is on. The standard formats for public keys are
 *                          documented in the documentation of @ref psa_export_public_key().
 * @param   peer_key_length Size of @c peer_key in bytes.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid for this key
 *                                            agreement step.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c private_key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    @c private_key does not have the @ref
 *                                          PSA_KEY_USAGE_DERIVE flag, or it does not permit the
 *                                          operation’s algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - The operation’s algorithm is not a key agreement
 *                                            algorithm.
 *                                          - @c step does not allow an input resulting from a key
 *                                            agreement.
 *                                          - @c private_key is not compatible with the operation’s
 *                                            algorithm.
 *                                          - @c peer_key is not a valid public key corresponding
 *                                            to @c private_key.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    @c private_key is not supported for use with the
 *                                          operation’s algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t *operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t *peer_key,
                                              size_t peer_key_length);

/**
 * @brief   Read some data from a key derivation operation.
 *
 * @details This function calculates output bytes from a key derivation algorithm and returns those
 *          bytes. If the key derivation’s output is viewed as a stream of bytes, this function
 *          consumes the requested number of bytes from the stream and returns them to the caller.
 *          The operation’s capacity decreases by the number of bytes read.
 *
 *          If this function returns an error status other than @ref PSA_ERROR_INSUFFICIENT_DATA,
 *          the operation enters an error state and must be aborted by calling @ref
 *          psa_key_derivation_abort().
 *
 * @param   operation       The key derivation operation object to read from.
 * @param   output          Buffer where the output will be written.
 * @param   output_length   Number of bytes to output.
 *
 * @return  @ref PSA_SUCCESS                    Success. The first output_length bytes of output
 *                                              contain the derived data.
 * @return  @ref PSA_ERROR_BAD_STATE            The following conditions can result in this error:
 *                                              - The operation state is not valid: it must be
 *                                                active, with all required input steps complete.
 *                                              - The library requires initializing by a call to
 *                                                @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_NOT_PERMITTED        One of the inputs was a key whose policy did not
 *                                              allow @ref PSA_KEY_USAGE_DERIVE.
 * @return  @ref PSA_ERROR_INSUFFICIENT_DATA    The operation’s capacity was less than
 *                                              @c output_length bytes. In this case, the following
 *                                              occurs:
 *                                              - No output is written to the output buffer.
 *                                              - The operation’s capacity is set to zero —
 *                                                subsequent calls to this function will not
 *                                                succeed, even with a smaller output buffer.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t *operation,
                                             uint8_t *output,
                                             size_t output_length);

/**
 * @brief   Derive a key from an ongoing key derivation operation.
 *
 * @details This function calculates output bytes from a key derivation algorithm and uses those
 *          bytes to generate a key deterministically. The key’s location, policy, type and size
 *          are taken from attributes.
 *
 *          If the key derivation’s output is viewed as a stream of bytes, this function consumes
 *          the required number of bytes from the stream. The operation’s capacity decreases by the
 *          number of bytes used to derive the key.
 *
 *          If this function returns an error status other than @ref PSA_ERROR_INSUFFICIENT_DATA,
 *          the operation enters an error state and must be aborted by calling @ref
 *          psa_key_derivation_abort().
 *
 *          How much output is produced and consumed from the operation, and how the key is
 *          derived, depends on the key type.
 *
 *          - For key types for which the key is an arbitrary sequence of bytes of a given size,
 *            this function is functionally equivalent to calling @ref
 *            psa_key_derivation_output_bytes() and passing the resulting output to @ref
 *            psa_import_key(). However, this function has a security benefit: if the
 *            implementation provides an isolation boundary then the key material is not exposed
 *            outside the isolation boundary. As a consequence, for these key types, this function
 *            always consumes exactly @c (bits/8) bytes from the operation.
 *            The following key types defined in this specification follow this scheme:
 *            - @ref PSA_KEY_TYPE_AES
 *            - @ref PSA_KEY_TYPE_ARC4
 *            - @ref PSA_KEY_TYPE_ARIA
 *            - @ref PSA_KEY_TYPE_CAMELLIA
 *            - @ref PSA_KEY_TYPE_CHACHA20
 *            - @ref PSA_KEY_TYPE_SM4
 *            - @ref PSA_KEY_TYPE_DERIVE
 *            - @ref PSA_KEY_TYPE_HMAC
 *            - @ref PSA_KEY_TYPE_PASSWORD_HASH
 *            - @ref PSA_KEY_TYPE_DES, 64 bits
 *              This function generates a key using the following process:
 *              -# Draw an 8-byte string.
 *              -# Set/clear the parity bits in each byte.
 *              -# If the result is a forbidden weak key, discard the result and return to step 1.
 *              -# Output the string.
 *            - @ref PSA_KEY_TYPE_DES, 192 bits
 *            - @ref PSA_KEY_TYPE_DES, 128 bits
 *              The two or three keys are generated by repeated application of the process used to
 *              generate a DES key.
 *              For example, for 3-key 3DES, if the first 8 bytes specify a weak key and the next 8
 *              bytes do not, discard the first 8 bytes, use the next 8 bytes as the first key, and
 *              continue reading output from the operation to derive the other two keys.
 *          - For Finite-field Diffie-Hellman keys @ref PSA_KEY_TYPE_DH_KEY_PAIR(@p dh_family)
 *            where @c dh_family designates any Diffie-Hellman family.
 *          - ECC keys on a Weierstrass elliptic curve (@ref PSA_KEY_TYPE_ECC_KEY_PAIR(
 *            @p ecc_family) where @c ecc_family designates a Weierstrass curve family.) require
 *            the generation of a private key which is an integer in the range [1, N - 1], where N
 *            is the boundary of the private key domain: N is the prime p for Diffie-Hellman, or
 *            the order of the curve’s base point for ECC.
 *            Let m be the bit size of N, such that 2^m > N >= 2^(m-1). This function generates the
 *            private key using the following process:
 *            -# Draw a byte string of length ceiling(m/8) bytes.
 *            -# If m is not a multiple of 8, set the most significant (8 * ceiling(m/8) - m) bits
 *               of the first byte in the string to zero.
 *            -# Convert the string to integer k by decoding it as a big-endian byte string.
 *            -# If k > N - 2, discard the result and return to step 1.
 *            -# Output k + 1 as the private key.
 *            This method allows compliance to NIST standards, specifically the methods titled
 *            Key-Pair Generation by Testing Candidates in the following publications:
 *            - NIST Special Publication 800-56A: Recommendation for Pair-Wise Key-Establishment
 *              Schemes Using Discrete Logarithm Cryptography SP800-56A §5.6.1.1.4 for
 *              Diffie-Hellman keys.
 *            - SP800-56A §5.6.1.2.2 or FIPS Publication 186-4: Digital Signature Standard (DSS)
 *              FIPS186-4 §B.4.2 for elliptic curve keys.
 *          - For ECC keys on a Montgomery elliptic curve (where @ref PSA_KEY_TYPE_ECC_KEY_PAIR
 *            (@ref PSA_ECC_FAMILY_MONTGOMERY)) this function always draws a byte string whose
 *            length is determined by the curve, and sets the mandatory bits accordingly. That is:
 *            - Curve25519 (@ref PSA_ECC_FAMILY_MONTGOMERY, 255 bits): draw a 32-byte string and
 *              process it as specified in Elliptic Curves for Security RFC7748 §5.
 *            - Curve448 (@ref PSA_ECC_FAMILY_MONTGOMERY, 448 bits): draw a 56-byte string and
 *              process it as specified in RFC7748 §5.
 *
 *          In all cases, the data that is read is discarded from the operation. The operation’s
 *          capacity is decreased by the number of bytes read.
 *
 *          For algorithms that take an input step @ref PSA_KEY_DERIVATION_INPUT_SECRET, the input
 *          to that step must be provided with @ref psa_key_derivation_input_key(). Future versions
 *          of this specification might include additional restrictions on the derived key based on
 *          the attributes and strength of the secret key.
 *
 * @param   attributes      The attributes for the new key. This function uses the attributes as
 *                          follows:
 *                          - The key type is required. It cannot be an asymmetric public key.
 *                          - The key size is required. It must be a valid size for the key type.
 *                          - The key permitted-algorithm policy is required for keys that will be
 *                            used for a cryptographic operation.
 *                          - If the key type to be created is @ref PSA_KEY_TYPE_PASSWORD_HASH,
 *                            then the permitted-algorithm policy must be the same as the current
 *                            operation’s algorithm.
 *                          - The key usage flags define what operations are permitted with the
 *                            key.
 *                          - The key lifetime and identifier are required for a persistent key.
 *                          @note   This is an input parameter: it is not updated with the final
 *                                  key attributes. The final attributes of the new key can be
 *                                  queried by calling @ref psa_get_key_attributes() with the key’s
 *                                  identifier.
 * @param   operation       The key derivation operation object to read from.
 * @param   key             On success, an identifier for the newly created key.
 *                          @ref PSA_KEY_ID_NULL on failure.
 * @return  @ref PSA_SUCCESS                Success. If the key is persistent, the key material
 *                                          and the key’s metadata have been saved to persistent
 *                                          storage.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be active,
 *                                            with all required input steps complete.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The following conditions can result in this error:
 *                                          - The @ref PSA_KEY_DERIVATION_INPUT_SECRET input step
 *                                            was neither provided through a key, nor the result of
 *                                            a key agreement.
 *                                          - One of the inputs was a key whose policy did not
 *                                            allow @ref PSA_KEY_USAGE_DERIVE.
 *                                          - The implementation does not permit creating a key
 *                                            with the specified attributes due to some
 *                                            implementation-specific policy.
 * @return  @ref PSA_ERROR_ALREADY_EXISTS   This is an attempt to create a persistent key, and
 *                                          there is already a persistent key with the given
 *                                          identifier.
 * @return  @ref PSA_ERROR_INSUFFICIENT_DATA    There was not enough data to create the desired
 *                                              key. In this case, the following occurs:
 *                                              - No key is generated.
 *                                              - The operation’s capacity is set to zero —
 *                                                subsequent calls to this function will not
 *                                                succeed, even if they require less data.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - The key type is invalid, or is an asymmetric public
 *                                            key type.
 *                                          - The key type is @ref PSA_KEY_TYPE_PASSWORD_HASH, and
 *                                            the permitted-algorithm policy is not the same as the
 *                                            current operation’s algorithm.
 *                                          - The key size is not valid for the key type.
 *                                          - The key lifetime is invalid.
 *                                          - The key identifier is not valid for the key lifetime.
 *                                          - The key usage flags include invalid values.
 *                                          - The key’s permitted-usage algorithm is invalid.
 *                                          - The key attributes, as a whole, are invalid.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The key attributes, as a whole, are not supported,
 *                                          either by the implementation in general or in the
 *                                          specified storage location.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_INSUFFICIENT_STORAGE
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t *attributes,
                                           psa_key_derivation_operation_t *operation,
                                           psa_key_id_t *key);

/**
 * @brief   Set the maximum capacity of a key derivation operation.
 *
 * @details The capacity of a key derivation operation is the maximum number of bytes that the key
 *          derivation operation can return from this point onwards.
 *
 * @param   operation   The key derivation operation object to modify.
 * @param   capacity    The new capacity of the operation. It must be less or equal to the
 *                      operation’s current capacity.
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be active.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT @c capacity is larger than the operation’s current
 *                                          capacity. In this case, the operation object remains
 *                                          valid and its capacity remains unchanged.
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t *operation,
                                             size_t capacity);

/**
 * @brief   Set up a key derivation operation.
 *
 * @details A key derivation algorithm takes some inputs and uses them to generate a byte stream in
 *          a deterministic way. This byte stream can be used to produce keys and other
 *          cryptographic material.
 *
 *          A key agreement and derivation algorithm uses a key agreement protocol to provide a
 *          shared secret which is used for the key derivation. See @ref
 *          psa_key_derivation_key_agreement().
 *
 *          To derive a key:
 *          -# Start with an initialized object of type @ref psa_key_derivation_operation_t.
 *          -# Call @ref psa_key_derivation_setup() to select the algorithm.
 *          -# Provide the inputs for the key derivation by calling @ref
 *             psa_key_derivation_input_bytes() or @ref psa_key_derivation_input_key() as
 *             appropriate. Which inputs are needed, in what order, whether keys are permitted,
 *             and what type of keys depends on the algorithm.
 *          -# Optionally set the operation’s maximum capacity with @ref
 *             psa_key_derivation_set_capacity(). This can be done before, in the middle of, or
 *             after providing inputs. For some algorithms, this step is mandatory because the
 *             output depends on the maximum capacity.
 *          -# To derive a key, call @ref psa_key_derivation_output_key(). To derive a byte string
 *             for a different purpose, call @ref psa_key_derivation_output_bytes(). Successive
 *             calls to these functions use successive output bytes calculated by the key
 *             derivation algorithm.
 *          -# Clean up the key derivation operation object with @ref psa_key_derivation_abort().
 *
 *          If this function returns an error, the key derivation operation object is not changed.
 *
 *          If an error occurs at any step after a call to @ref psa_key_derivation_setup(), the
 *          operation will need to be reset by a call to @ref psa_key_derivation_abort().
 *
 *          Implementations must reject an attempt to derive a key of size 0.
 *
 * @param   operation   The key derivation operation object to set up. It must have been
 *                      initialized but not set up yet.
 * @param   alg         The algorithm to compute. This must be one of the following:
 *                      - A key derivation algorithm: a value of type @ref psa_algorithm_t
 *                        such that @ref PSA_ALG_IS_KEY_DERIVATION(@p alg) is true.
 *                      - A key agreement and derivation algorithm: a value of type @ref
 *                        psa_algorithm_t such that @ref PSA_ALG_IS_KEY_AGREEMENT(@p alg) is
 *                        true and @ref PSA_ALG_IS_RAW_KEY_AGREEMENT(@p alg) is false.
 *
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be inactive.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT @c alg is neither a key derivation algorithm, nor a key
 *                                          agreement and derivation algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    @c alg is not supported or is not a key derivation
 *                                          algorithm, or a key agreement and derivation algorithm.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg);

/**
 * @brief   Compare output data from a key derivation operation to an expected value.
 *
 * @details This function calculates output bytes from a key derivation algorithm and compares
 *          those bytes to an expected value. If the key derivation’s output is viewed as a stream
 *          of bytes, this function destructively reads @c output_length bytes from the stream
 *          before comparing them with @c expected_output. The operation’s capacity decreases by
 *          the number of bytes read.
 *
 *          This is functionally equivalent to the following code:
 *          @code
 *          uint8_t tmp[output_length];
 *          psa_key_derivation_output_bytes(operation, tmp, output_length);
 *          if (memcmp(expected_output, tmp, output_length) != 0)
 *               return PSA_ERROR_INVALID_SIGNATURE;
 *          @endcode
 *
 *          However, calling @ref psa_key_derivation_verify_bytes() works even if the key’s policy
 *          does not allow output of the bytes.
 *
 *          If this function returns an error status other than @ref PSA_ERROR_INSUFFICIENT_DATA or
 *          @ref PSA_ERROR_INVALID_SIGNATURE, the operation enters an error state and must be
 *          aborted by calling @ref psa_key_derivation_abort().
 *
 *          @note   Implementations must make the best effort to ensure that the comparison between
 *                  the actual key derivation output and the expected output is performed in
 *                  constant time.
 *
 * @param   operation       The key derivation operation object to read from.
 * @param   expected_output Buffer containing the expected derivation output.
 * @param   output_length   Length ot the expected output. This is also the number of bytes that
 *                          will be read.
 *
 * @return  @ref PSA_SUCCESS                    Success. The output of the key derivation operation
 *                                              matches expected_output.
 * @return  @ref PSA_ERROR_BAD_STATE            The following conditions can result in this error:
 *                                              - The operation state is not valid: it must be
 *                                                active, with all required input steps complete.
 *                                              - The library requires initializing by a call to
 *                                                @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_NOT_PERMITTED        One of the inputs is a key whose policy does not
 *                                              permit @ref PSA_KEY_USAGE_VERIFY_DERIVATION.
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    The output of the key derivation operation does
 *                                              not match the value in expected_output.
 * @return  @ref PSA_ERROR_INSUFFICIENT_DATA    The operation’s capacity was less than
 *                                              @c output_length bytes. In this case, the
 *                                              operation’s capacity is set to zero — subsequent
 *                                              calls to this function will not succeed, even with
 *                                              a smaller expected output length.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_verify_bytes(psa_key_derivation_operation_t *operation,
                                             const uint8_t *expected_output,
                                             size_t output_length);

/**
 * @brief   Compare output data from a key derivation operation to an expected value stored in a
 *          key.
 *
 * @details This function calculates output bytes from a key derivation algorithm and compares
 *          those bytes to an expected value, provided as key of type @ref
 *          PSA_KEY_TYPE_PASSWORD_HASH. If the key derivation’s output is viewed as a stream of
 *          bytes, this function destructively reads the number of bytes corresponding to the
 *          length of the expected key from the stream before comparing them with the key value.
 *          The operation’s capacity decreases by the number of bytes read.
 *
 *          This is functionally equivalent to exporting the expected key and calling @ref
 *          psa_key_derivation_verify_bytes() on the result, except that it works when the key
 *          cannot be exported.
 *
 *          If this function returns an error status other than @ref PSA_ERROR_INSUFFICIENT_DATA or
 *          @ref PSA_ERROR_INVALID_SIGNATURE, the operation enters an error state and must be
 *          aborted by calling @ref psa_key_derivation_abort().
 *
 *          @note   Implementations must make the best effort to ensure that the comparison between
 *                  the actual key derivation output and the expected output is performed in
 *                  constant time.
 *
 * @param   operation   The key derivation operation object to read from.
 * @param   expected    A key of type @ref PSA_KEY_TYPE_PASSWORD_HASH containing the expected
 *                      output. The key must allow the usage @ref PSA_KEY_USAGE_VERIFY_DERIVATION,
 *                      and the permitted algorithm must match the operation’s algorithm.
 *                      The value of this key is typically computed by a previous call to @ref
 *                      psa_key_derivation_output_key().

 * @return  @ref PSA_SUCCESS                    Success. The output of the key derivation operation
 *                                              matches the expected key value.
 * @return  @ref PSA_ERROR_BAD_STATE            The following conditions can result in this error:
 *                                              - The operation state is not valid: it must be
 *                                                active, with all required input steps complete.
 *                                              - The library requires initializing by a call to
 *                                                @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE       @c expected is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED        The following conditions can result in this error:
 *                                              - The key does not have the @ref
 *                                                PSA_KEY_USAGE_VERIFY_DERIVATION flag, or it does
 *                                                not permit the requested algorithm.
 *                                              - One of the inputs is a key whose policy does not
 *                                                permit @ref PSA_KEY_USAGE_VERIFY_DERIVATION.
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    The output of the key derivation operation does not
 *                                              match the value of the expected key.
 * @return  @ref PSA_ERROR_INSUFFICIENT_DATA    The operation’s capacity was less than the length
 *                                              of the @c expected key. In this case, the
 *                                              operation’s capacity is set to zero — subsequent
 *                                              calls to this function will not succeed, even with
 *                                              a smaller expected key length.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The key type is not @ref PSA_KEY_TYPE_PASSWORD_HASH.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_key_derivation_verify_key(psa_key_derivation_operation_t *operation,
                                           psa_key_id_t expected);
#endif /* PSA_CRYPTO_KEY_DERIVATION */

#if IS_USED(MODULE_PSA_MAC) || defined(DOXYGEN)
/**
 * @brief   Abort a MAC operation.
 *
 * @details Aborting an operation frees all associated resources except for the operation object
 *          itself. Once aborted, the operation object can be reused for another operation by
 *          calling @ref psa_mac_sign_setup() or @ref psa_mac_verify_setup() again.
 *
 *          This function can be called any time after the operation object has been initialized by
 *          one of the methods described in @ref psa_mac_operation_t.
 *
 *          In particular, calling @ref psa_mac_abort() after the operation has been terminated
 *          by a call to @ref psa_mac_abort(), @ref psa_mac_sign_finish() or @ref
 *          psa_mac_verify_finish() is safe and has no effect.
 *
 * @param   operation   Initialized MAC operation.
 * @return  @ref PSA_SUCCESS            Success. The operation object can now be discarded or
 *                                      reused.
 * @return  @ref PSA_ERROR_BAD_STATE    The library requires initializing by a call to @ref
 *                                      psa_crypto_init().
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_mac_abort(psa_mac_operation_t *operation);

/**
 * @brief   Calculate the message authentication code (MAC) of a message.
 *
 * @note    To verify the MAC of a message against an expected value, use @ref psa_mac_verify()
 *          instead. Beware that comparing integrity or authenticity data such as MAC values with a
 *          function such as @c memcmp() is risky because the time taken by the comparison might
 *          leak information about the MAC value which could allow an attacker to guess a valid MAC
 *          and thereby bypass security controls.
 *
 * @param   key             Identifier of the key to use for the operation. It must allow the usage
 *                          @ref PSA_KEY_USAGE_SIGN_MESSAGE.
 * @param   alg             The MAC algorithm to compute (PSA_ALG_XXX value such that @ref
 *                          PSA_ALG_IS_MAC(@p alg) is true).
 * @param   input           Buffer containing the input message.
 * @param   input_length    Size of the input buffer in bytes.
 * @param   mac             Buffer where the MAC value is to be written.
 * @param   mac_size        Size of the mac buffer in bytes. This must be appropriate for the
 *                          selected algorithm and key:
 *                          - The exact MAC size is @ref PSA_MAC_LENGTH(@p key_type, @p key_bits,
 *                            @p alg) where @c key_type and @c key_bits are attributes of the key
 *                            used to compute the MAC.
 *                          - @ref PSA_MAC_MAX_SIZE evaluates to the maximum MAC size of any
 *                          supported MAC algorithm.
 * @param   mac_length      On success, the number of bytes that make up the MAC value.
 *
 * @return  @ref PSA_SUCCESS            Success. The first @c (*mac_length) bytes of @c mac contain
 *                                      the MAC value.
 * @return  @ref PSA_ERROR_BAD_STATE    The library requires initializing by a call to @ref
 *                                      psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref
 *                                          PSA_KEY_USAGE_SIGN_MESSAGE flag, or it does not permit
 *                                          the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the mac buffer is too small. @ref
 *                                          PSA_MAC_LENGTH() or @ref PSA_MAC_MAX_SIZE can be used
 *                                          to determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not a MAC algorithm.
 *                                          - @c key is not compatible with @c alg.
 *                                          - @c input_length is too large for @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a MAC algorithm.
 *                                          - @c key is not supported for use with @c alg.
 *                                          - @c input_length is too large for the implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_compute(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t *input,
                             size_t input_length,
                             uint8_t *mac,
                             size_t mac_size,
                             size_t *mac_length);

/**
 * @brief   Finish the calculation of the MAC of a message.
 *
 * @details The application must call @ref psa_mac_sign_setup() before calling this function. This
 *          function calculates the MAC of the message formed by concatenating the inputs passed to
 *          preceding calls to @ref psa_mac_update().
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_mac_abort().
 *
 * @warning It is not recommended to use this function when a specific value is expected for the
 *          MAC. Call @ref psa_mac_verify_finish() instead with the expected MAC value.
 *          Comparing integrity or authenticity data such as MAC values with a function such as
 *          @c memcmp() is risky because the time taken by the comparison might leak information
 *          about the hashed data which could allow an attacker to guess a valid MAC and thereby
 *          bypass security controls.
 *
 * @param   operation   Active MAC operation.
 * @param   mac         Buffer where the MAC value is to be written.
 * @param   mac_size    Size of the mac buffer in bytes. This must be appropriate for the selected
 *                      algorithm and key:
 *                      - The exact MAC size is @ref PSA_MAC_LENGTH(@p key_type, @p key_bits,
 *                        @p alg) where @c key_type and @c key_bits are attributes of the key,
 *                        and @c alg is the algorithm used to compute the MAC.
 *                      - @ref PSA_MAC_MAX_SIZE evaluates to the maximum MAC size of any supported
 *                        MAC algorithm.
 * @param   mac_length  On success, the number of bytes that make up the MAC value. This is always
 *                      @ref PSA_MAC_LENGTH(@p key_type, @p key_bits, @p alg) where @c key_type and
 *                      @c key_bits are attributes of the key, and @c alg is the algorithm used to
 *                      compute the MAC.
 * @return  @ref PSA_SUCCESS                Success. The first @c (*mac_length) bytes of mac
 *                                          contain the MAC value.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be an
 *                                            active mac sign operation.
 *                                          - The library requires initializing by a call to @ref
 *                                            psa_crypto_init().
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the mac buffer is too small. @ref
 *                                          PSA_MAC_LENGTH() or @ref PSA_MAC_MAX_SIZE can be used
 *                                          to determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac,
                                 size_t mac_size,
                                 size_t *mac_length);

/**
 * @brief   Set up a multi-part MAC calculation operation.
 *
 * @details This function sets up the calculation of the message authentication code (MAC)
 *          of a byte string. To verify the MAC of a message against an expected value,
 *          use @ref psa_mac_verify_setup() instead.
 *
 *          The sequence of operations to calculate a MAC is as follows:
 *          -# Allocate an operation object which will be passed to all the functions listed here.
 *          -# Initialize the operation object with one of the methods described in the
 *             documentation for @ref psa_mac_operation_t, e.g. @ref PSA_MAC_OPERATION_INIT.
 *          -# Call @ref psa_mac_sign_setup() to specify the algorithm and key.
 *          -# Call @ref psa_mac_update() zero, one or more times, passing a fragment of the
 *             message each time. The MAC that is calculated is the MAC of the concatenation of
 *             these messages in order.
 *          -# At the end of the message, call @ref psa_mac_sign_finish() to finish calculating the
 *             MAC value and retrieve it.
 *
 *          If an error occurs at any step after a call to @ref psa_mac_sign_setup(), the operation
 *          will need to be reset by a call to @ref psa_mac_abort(). The application can call @ref
 *          psa_mac_abort() at any time after the operation has been initialized.
 *
 *          After a successful call to @ref psa_mac_sign_setup(), the application must eventually
 *          terminate the operation through one of the following methods:
 *          - A successful call to @ref psa_mac_sign_finish().
 *          - A call to @ref psa_mac_abort().
 *
 * @param   operation   The operation object to set up. It must have been initialized as per the
 *                      documentation for @ref psa_mac_operation_t and not yet in use.
 * @param   key         Identifier of the key to use for the operation. It must remain valid until
 *                      the operation terminates. It must allow the usage @ref
 *                      PSA_KEY_USAGE_SIGN_MESSAGE.
 * @param   alg         The MAC algorithm to compute: a value of type @ref psa_algorithm_t such
 *                      that @ref PSA_ALG_IS_MAC(@p alg) is true.
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be inactive.
 *                                          - The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref
 *                                          PSA_KEY_USAGE_SIGN_MESSAGE flag, or it does not permit
 *                                          the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not a MAC algorithm.
 *                                          - @c key is not compatible with @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a MAC algorithm.
 *                                          - @c key is not supported for use with @c alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                psa_key_id_t key,
                                psa_algorithm_t alg);

/**
 * @brief   Add a message fragment to a multi-part MAC operation.
 *
 * @details The application must call @ref psa_mac_sign_setup() or @ref psa_mac_verify_setup()
 *          before calling this function.
 *
 *          If this function returns an error status, the operation enters an error state and must
 *          be aborted by calling @ref psa_mac_abort().
 *
 * @param   operation       Active MAC operation.
 * @param   input           Buffer containing the message fragment to add to the MAC calculation.
 * @param   input_length    Size of the @c input buffer in bytes.
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be active.
 *                                          - The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The total input for the operation is too large for the
 *                                          MAC algorithm.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The total input for the operation is too large for the
 *                                          implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input,
                            size_t input_length);

/**
 * @brief   Calculate the MAC of a message and compare it with a reference value.
 *
 * @param   key             Identifier of the key to use for the operation. It must allow the usage
 *                          @ref PSA_KEY_USAGE_VERIFY_MESSAGE.
 * @param   alg             The MAC algorithm to compute: a value of type @ref psa_algorithm_t such
 *                          that @ref PSA_ALG_IS_MAC(@p alg) is true.
 * @param   input           Buffer containing the input message.
 * @param   input_length    Size of the @c input buffer in bytes.
 * @param   mac             Buffer containing the expected MAC value.
 * @param   mac_length      Size of the @c mac buffer in bytes.
 * @return  @ref PSA_SUCCESS                    Success. The expected MAC is identical to the
 *                                              actual MAC of the input.
 * @return  @ref PSA_ERROR_BAD_STATE            The library requires initializing by a call to @ref
 *                                              psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE       @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED        The key does not have the @ref
 *                                              PSA_KEY_USAGE_VERIFY_MESSAGE flag, or it does not
 *                                              permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    The calculated MAC of the message does not match
 *                                              the value in @c mac.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The following conditions can result in this error:
 *                                              - @c alg is not a MAC algorithm.
 *                                              - @c key is not compatible with @c alg.
 *                                              - @c input_length is too large for @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        The following conditions can result in this error:
 *                                              - @c alg is not supported or is not a MAC algorithm.
 *                                              - @c key is not supported for use with @c alg.
 *                                              - @c input_length is too large for the
 *                                                implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_verify(psa_key_id_t key,
                            psa_algorithm_t alg,
                            const uint8_t *input,
                            size_t input_length,
                            const uint8_t *mac,
                            size_t mac_length);

/**
 * @brief   Finish the calculation of the MAC of a message and compare it with an expected value.
 *
 * @details The application must call @ref psa_mac_verify_setup() before calling this function.
 *          This function calculates the MAC of the message formed by concatenating the inputs
 *          passed to preceding calls to @ref psa_mac_update(). It then compares the calculated MAC
 *          with the expected MAC passed as a parameter to this function.
 *
 *          When this function returns successfully, the operation becomes inactive. If this
 *          function returns an error status, the operation enters an error state and must be
 *          aborted by calling @ref psa_mac_abort().
 *
 * @note    Implementations must make the best effort to ensure that the comparison between the
 *          actual MAC and the expected MAC is performed in constant time.
 *
 * @param   operation   Active MAC operation.
 * @param   mac         Buffer containing the expected MAC value.
 * @param   mac_length  Size of the @c mac buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                    Success. The expected MAC is identical to the
 *                                              actual MAC of the message.
 * @return  @ref PSA_ERROR_BAD_STATE            The following conditions can result in this error:
 *                                              - The operation state is not valid: it must be an
 *                                                active mac verify operation.
 *                                              - The library requires initializing by a call to
 *                                                @ref psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    The calculated MAC of the message does not match
 *                                              the value in mac.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac,
                                   size_t mac_length);

/**
 * @brief   Set up a multi-part MAC verification operation.
 *
 * @details This function sets up the verification of the message authentication code (MAC) of a
 *          byte string against an expected value.
 *
 *          The sequence of operations to verify a MAC is as follows:
 *          -# Allocate an operation object which will be passed to all the functions listed here.
 *          -# Initialize the operation object with one of the methods described in the
 *             documentation for @ref psa_mac_operation_t, e.g. @ref PSA_MAC_OPERATION_INIT.
 *          -# Call @ref psa_mac_verify_setup() to specify the algorithm and key.
 *          -# Call @ref psa_mac_update() zero, one or more times, passing a fragment of the
 *             message each time. The MAC that is calculated is the MAC of the concatenation of
 *             these messages in order.
 *          -# At the end of the message, call @ref psa_mac_verify_finish() to finish calculating
 *             the actual MAC of the message and verify it against the expected value.
 *
 *          If an error occurs at any step after a call to @ref psa_mac_verify_setup(), the
 *          operation will need to be reset by a call to @ref psa_mac_abort(). The application can
 *          call @ref psa_mac_abort() at any time after the operation has been initialized.
 *
 *          After a successful call to @ref psa_mac_verify_setup(), the application must eventually
 *          terminate the operation through one of the following methods:
 *          - A successful call to @ref psa_mac_verify_finish().
 *          - A call to @ref psa_mac_abort().

 * @param   operation   The operation object to set up. It must have been initialized as per the
 *                      documentation for @ref psa_mac_operation_t and not yet in use.
 * @param   key         Identifier of the key to use for the operation. It must remain valid until
 *                      the operation terminates. It must allow the usage @ref
 *                      PSA_KEY_USAGE_VERIFY_MESSAGE.
 * @param   alg         The MAC algorithm to compute: a value of type @ref psa_algorithm_t such
 *                      that @ref PSA_ALG_IS_MAC(@p alg) is true.
 * @return  @ref PSA_SUCCESS                Success.
 * @return  @ref PSA_ERROR_BAD_STATE        The following conditions can result in this error:
 *                                          - The operation state is not valid: it must be inactive.
 *                                          - The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref
 *                                          PSA_KEY_USAGE_VERIFY_MESSAGE flag, or it does not
 *                                          permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not a MAC algorithm.
 *                                          - @c key is not compatible with @c alg.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a MAC algorithm.
 *                                          - @c key is not supported for use with @c alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  psa_key_id_t key,
                                  psa_algorithm_t alg);
#endif /* MODULE_PSA_MAC */

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT) || defined(DOXYGEN)
/**
 * @brief   Remove non-essential copies of key material from memory.
 *
 * @details For keys that have been created with the @ref PSA_KEY_USAGE_CACHE usage flag, an
 *          implementation is permitted to make additional copies of the key material that are
 *          not in storage and not for the purpose of ongoing operations.
 *
 *          This function will remove these extra copies of the key material from memory.
 *
 *          This function is not required to remove key material from memory in any of the
 *          following situations:
 *          - The key is currently in use in a cryptographic operation.
 *          - The key is volatile.

 * @param   key Identifier of the key to purge.
 * @return  @ref PSA_SUCCESS                Success. The key material has been removed from memory,
 *                                          if the key material is not currently required.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_purge_key(psa_key_id_t key);
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_KEY_AGREEMENT) || defined(DOXYGEN)
/**
 * @brief   Perform a key agreement and return the raw shared secret.
 *
 * @warning The raw result of a key agreement algorithm such as finite-field Diffie-Hellman or
 *          elliptic curve Diffie-Hellman has biases, and is not suitable for use as key material.
 *          Instead it is recommended that the result is used as input to a key derivation
 *          algorithm. To chain a key agreement with a key derivation, use @ref
 *          psa_key_derivation_key_agreement() and other functions from the key derivation
 *          interface.
 *
 * @param   alg             The key agreement algorithm to compute: a value of type @ref
 *                          psa_algorithm_t such that @ref PSA_ALG_IS_RAW_KEY_AGREEMENT(@c alg)
 *                          is true.
 * @param   private_key     Identifier of the private key to use. It must allow the usage @ref
 *                          PSA_KEY_USAGE_DERIVE.
 * @param   peer_key        Public key of the peer. The peer key must be in the same format that
 *                          @ref psa_import_key() accepts for the public key type corresponding to
 *                          the type of private_key. That is, this function performs the equivalent
 *                          of @ref psa_import_key(..., @p peer_key, @p peer_key_length), with key
 *                          attributes indicating the public key type corresponding to the type of
 *                          @c private_key. For example, for ECC keys, this means that @c peer_key
 *                          is interpreted as a point on the curve that the private key is on. The
 *                          standard formats for public keys are documented in the documentation of
 *                          @ref psa_export_public_key().
 * @param   peer_key_length Size of peer_key in bytes.
 * @param   output          Buffer where the raw shared secret is to be written.
 * @param   output_size     Size of the output buffer in bytes. This must be appropriate for the
 *                          keys:
 *                          - The required output size is @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE
 *                            (@p type, @p bits) where type is the type of @c private_key and @c
 *                            bits is the bit-size of either @c private_key or the @c peer_key.
 *                          - @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE evaluates to the maximum
 *                            output size of any supported raw key agreement algorithm.
 * @param   output_length   On success, the number of bytes that make up the returned output.
 *
 * @return  @ref PSA_SUCCESS                Success. The first (*output_length) bytes of output
 *                                          contain the raw shared secret.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c private_key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    @c private_key does not have the @ref
 *                                          PSA_KEY_USAGE_DERIVE flag, or it does not permit
 *                                          the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the output buffer is too small.
 *                                          @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE() or @ref
 *                                          PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE can be used to
 *                                          determine a sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not a key agreement algorithm.
 *                                          - @c private_key is not compatible with @c alg.
 *                                          - @c peer_key is not a valid public key corresponding
 *                                            to @c private_key.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not a key agreement
 *                                            algorithm.
 *                                          - @c private_key is not supported for use with @c alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_id_t private_key,
                                   const uint8_t *peer_key,
                                   size_t peer_key_length,
                                   uint8_t *output,
                                   size_t output_size,
                                   size_t *output_length);
#endif /* MODULE_PSA_KEY_AGREEMENT */

#if IS_USED(MODULE_PSA_ASYMMETRIC) || defined(DOXYGEN)
/**
 * @brief   Sign an already-calculated hash with a private key.
 *
 * @details With most signature mechanisms that follow the hash-and-sign paradigm, the hash input
 *          to this function is the hash of the message to sign. The hash algorithm is encoded in
 *          the signature algorithm.
 *
 *          Some hash-and-sign mechanisms apply a padding or encoding to the hash. In such cases,
 *          the encoded hash must be passed to this function. The current version of this
 *          specification defines one such signature algorithm: @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW.
 *
 * @note    To perform a hash-and-sign signature algorithm, the hash must be calculated before
 *          passing it to this function. This can be done by calling @ref psa_hash_compute() or
 *          with a multi-part hash operation. The correct hash algorithm to use can be determined
 *          using @ref PSA_ALG_GET_HASH().
 *
 *          Alternatively, to hash and sign a message in a single call, use @ref psa_sign_message().
 *
 * @param   key                 Identifier of the key to use for the operation. It must be an
 *                              asymmetric key pair. The key must allow the usage @ref
 *                              PSA_KEY_USAGE_SIGN_HASH.
 * @param   alg                 An asymmetric signature algorithm that separates the hash and sign
 *                              operations: a value of type @ref psa_algorithm_t such that @ref
 *                              PSA_ALG_IS_SIGN_HASH(@p alg) is true.
 * @param   hash                The input to sign. This is usually the hash of a message. See the
 *                              detailed description of this function and the description of
 *                              individual signature algorithms for a detailed description of
 *                              acceptable inputs.
 * @param   hash_length         Size of the hash buffer in bytes.
 * @param   signature           Buffer where the signature is to be written.
 * @param   signature_size      Size of the signature buffer in bytes. This must be appropriate for
 *                              the selected algorithm and key:
 *                              - The required signature size is @ref PSA_SIGN_OUTPUT_SIZE(@p
 *                                key_type, @p key_bits, @p alg) where @c key_type and @c key_bits
 *                                are the type and bit-size respectively of @c key.
 *                              - @ref PSA_SIGNATURE_MAX_SIZE evaluates to the maximum signature
 *                                size of any supported signature algorithm.
 * @param   signature_length    On success, the number of bytes that make up the returned signature
 *                              value.
 * @return  @ref PSA_SUCCESS                Success. The first @c (*signature_length) bytes of
 *                                          @c signature contain the signature value.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref PSA_KEY_USAGE_SIGN_HASH
 *                                          flag, or it does not permit the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the signature buffer is too small.
 *                                          @ref PSA_SIGN_OUTPUT_SIZE() or @ref
 *                                          PSA_SIGNATURE_MAX_SIZE can be used to determine a
 *                                          sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not an asymmetric signature algorithm.
 *                                          - @c key is not an asymmetric key pair, that is
 *                                            compatible with @c alg.
 *                                          - @c hash_length is not valid for the algorithm and key
 *                                            type.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not an asymmetric
 *                                            signature algorithm.
 *                                          - @c key is not supported for use with alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_sign_hash(psa_key_id_t key,
                           psa_algorithm_t alg,
                           const uint8_t *hash,
                           size_t hash_length,
                           uint8_t *signature,
                           size_t signature_size,
                           size_t *signature_length);

/**
 * @brief   Sign a message with a private key. For hash-and-sign algorithms, this includes the
 *          hashing step.
 *
 * @note    To perform a multi-part hash-and-sign signature algorithm, first use a multi-part hash
 *          operation and then pass the resulting hash to @ref psa_sign_hash().
 *          @ref PSA_ALG_GET_HASH(@p alg) can be used to determine the hash algorithm to use.
 *
 * @param   key                 Identifier of the key to use for the operation. It must be an
 *                              asymmetric key pair. The key must allow the usage @ref
 *                              PSA_KEY_USAGE_SIGN_MESSAGE.
 * @param   alg                 An asymmetric signature algorithm: a value of type @ref
 *                              psa_algorithm_t such that @ref PSA_ALG_IS_SIGN_MESSAGE(@p alg) is
 *                              true.
 * @param   input               The input message to sign.
 * @param   input_length        Size of the input buffer in bytes.
 * @param   signature           Buffer where the signature is to be written.
 * @param   signature_size      Size of the signature buffer in bytes. This must be appropriate for
 *                              the selected algorithm and key:
 *                              - The required signature size is @ref PSA_SIGN_OUTPUT_SIZE
 *                                (@p key_type, @p key_bits, @p alg) where @c key_type and
 *                                @c key_bits are the type and bit-size respectively of @c key.
 *                              - @ref PSA_SIGNATURE_MAX_SIZE evaluates to the maximum signature
 *                                size of any supported signature algorithm.
 * @param   signature_length    On success, the number of bytes that make up the returned signature
 *                              value.
 * @return  @ref PSA_SUCCESS                Success. The first @c (*signature_length) bytes of
 *                                          @c signature contain the signature value.
 * @return  @ref PSA_ERROR_BAD_STATE        The library requires initializing by a call to @ref
 *                                          psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE   @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED    The key does not have the @ref
 *                                          PSA_KEY_USAGE_SIGN_MESSAGE flag, or it does not permit
 *                                          the requested algorithm.
 * @return  @ref PSA_ERROR_BUFFER_TOO_SMALL The size of the signature buffer is too small.
 *                                          @ref PSA_SIGN_OUTPUT_SIZE() or @ref
 *                                          PSA_SIGNATURE_MAX_SIZE can be used to determine a
 *                                          sufficient buffer size.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The following conditions can result in this error:
 *                                          - @c alg is not an asymmetric signature algorithm.
 *                                          - @c key is not an asymmetric key pair, that is
 *                                            compatible with @c alg.
 *                                          - @c input_length is too large for the algorithm and
 *                                            key type.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED    The following conditions can result in this error:
 *                                          - @c alg is not supported or is not an asymmetric
 *                                            signature algorithm.
 *                                          - @c key is not supported for use with @c alg.
 *                                          - @c input_length is too large for the implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_ENTROPY
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_sign_message(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              uint8_t *signature,
                              size_t signature_size,
                              size_t *signature_length);

/**
 * @brief   Verify the signature of a hash or short message using a public key.
 *
 * @details With most signature mechanisms that follow the hash-and-sign paradigm, the hash input
 *          to this function is the hash of the message to sign. The hash algorithm is encoded in
 *          the signature algorithm.
 *
 *          Some hash-and-sign mechanisms apply a padding or encoding to the hash. In such cases,
 *          the encoded hash must be passed to this function. The current version of this
 *          specification defines one such signature algorithm: @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW.
 *
 * @note    To perform a hash-and-sign verification algorithm, the hash must be calculated before
 *          passing it to this function. This can be done by calling @ref psa_hash_compute() or
 *          with a multi-part hash operation. Alternatively, to hash and verify a message signature
 *          in a single call, use @ref psa_verify_message().
 *
 * @note    When using secure elements as backends in this implementation, the key type can only be
 *          of type @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve) and must be stored on a secure element.
 *          To use the public key of a previously generated key pair, please export the public key
 *          first and then import it as a separate key with its own attributes and identifier.
 *
 * @param key               Identifier of the key to use for the operation. It must be a public key
 *                          or an asymmetric key pair. The key must allow the usage @ref
 *                          PSA_KEY_USAGE_VERIFY_HASH.
 * @param alg               An asymmetric signature algorithm that separates the hash and sign
 *                          operations (PSA_ALG_XXX value such that @ref PSA_ALG_IS_SIGN_HASH(@p
 *                          alg) is true), that is compatible with the type of key.
 * @param hash              The input whose signature is to be verified. This is usually the hash
 *                          of a message. See the detailed description of this function and the
 *                          description of individual signature algorithms for a detailed
 *                          description of acceptable inputs.
 * @param hash_length       Size of the hash buffer in bytes.
 * @param signature         Buffer containing the signature to verify.
 * @param signature_length  Size of the signature buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                    Success. The signature is valid.
 * @return  @ref PSA_ERROR_BAD_STATE            The library requires initializing by a call to @ref
 *                                              psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE       @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED        The key does not have the @ref
 *                                              PSA_KEY_USAGE_VERIFY_HASH flag, or it does not
 *                                              permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    @c signature is not the result of signing hash with
 *                                              algorithm @c alg using the private key
 *                                              corresponding to @c key.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The following conditions can result in this error:
 *                                              - @c alg is not an asymmetric signature algorithm.
 *                                              - @c key is not a public key or an asymmetric key
 *                                                pair, that is compatible with @c alg.
 *                                              - @c hash_length is not valid for the algorithm and
 *                                                key type.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        The following conditions can result in this error:
 *                                              - @c alg is not supported or is not an asymmetric
 *                                                signature algorithm.
 *                                              - @c key is not supported for use with @c alg.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_verify_hash(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t *hash,
                             size_t hash_length,
                             const uint8_t *signature,
                             size_t signature_length);

/**
 * @brief   Verify the signature of a message with a public key. For hash-and-sign algorithms,
 *          this includes the hashing step.
 *
 * @note    To perform a multi-part hash-and-sign signature verification algorithm, first use a
 *          multi-part hash operation to hash the message and then pass the resulting hash to @ref
 *          psa_verify_hash(). @ref PSA_ALG_GET_HASH(@p alg) can be used to determine the hash
 *          algorithm to use.
 *
 * @param   key                 Identifier of the key to use for the operation. It must be a public
 *                              key or an asymmetric key pair. The key must allow the usage @ref
 *                              PSA_KEY_USAGE_VERIFY_MESSAGE.
 * @param   alg                 An asymmetric signature algorithm: a value of type @ref
 *                              psa_algorithm_t such that @ref PSA_ALG_IS_SIGN_MESSAGE(@p alg)
 *                              is true.
 * @param   input               The message whose signature is to be verified.
 * @param   input_length        Size of the @c input buffer in bytes.
 * @param   signature           Buffer containing the signature to verify.
 * @param   signature_length    Size of the @c signature buffer in bytes.
 *
 * @return  @ref PSA_SUCCESS                    Success. The signature is valid.
 * @return  @ref PSA_ERROR_BAD_STATE            The library requires initializing by a call to @ref
 *                                              psa_crypto_init().
 * @return  @ref PSA_ERROR_INVALID_HANDLE       @c key is not a valid key identifier.
 * @return  @ref PSA_ERROR_NOT_PERMITTED        The key does not have the @ref
 *                                              PSA_KEY_USAGE_VERIFY_MESSAGE flag, or it does not
 *                                              permit the requested algorithm.
 * @return  @ref PSA_ERROR_INVALID_SIGNATURE    @c signature is not the result of signing the input
 *                                              message with algorithm @c alg using the private key
 *                                              corresponding to @c key.
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT     The following conditions can result in this error:
 *                                              - @c alg is not an asymmetric signature algorithm.
 *                                              - @c key is not a public key or an asymmetric key
 *                                                pair, that is compatible with @c alg.
 *                                              - @c input_length is too large for the algorithm
 *                                                and key type.
 * @return  @ref PSA_ERROR_NOT_SUPPORTED        The following conditions can result in this error:
 *                                              - @c alg is not supported or is not an asymmetric
 *                                                signature algorithm.
 *                                              - @c key is not supported for use with @c alg.
 *                                              - @c input_length is too large for the
 *                                                implementation.
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_COMMUNICATION_FAILURE
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_STORAGE_FAILURE
 * @return  @ref PSA_ERROR_DATA_CORRUPT
 * @return  @ref PSA_ERROR_DATA_INVALID
 */
psa_status_t psa_verify_message(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                const uint8_t *signature,
                                size_t signature_length);
#endif /* MODULE_PSA_ASYMMETRIC */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PSA_CRYPTO_PSA_CRYPTO_H */
