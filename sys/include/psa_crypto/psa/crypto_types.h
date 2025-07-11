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
 * @{
 *
 * @file        crypto_types.h
 * @brief       Type definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "psa/algorithm.h"
#include "psa/error.h"
#include "psa/key/attributes.h"

/**
 * @brief   For encrypt-decrypt functions, whether the operation is an encryption
 *          or a decryption.
 */
typedef enum {
    PSA_CRYPTO_DRIVER_DECRYPT,
    PSA_CRYPTO_DRIVER_ENCRYPT
} psa_encrypt_or_decrypt_t;

/**
 * @brief   The type of the state object for key derivation operations.
 *
 * @details Before calling any function on a key derivation operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_key_derivation_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static or
 *            global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_key_derivation_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_KEY_DERIVATION_OPERATION_INIT,
 *            for example:
 *            @code
 *            @ref psa_key_derivation_operation_t operation =
 *            @ref PSA_KEY_DERIVATION_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_key_derivation_operation_init() to
 *            the object, for example:
 *            @code
 *            @ref psa_key_derivation_operation_t operation;
 *            operation = @ref psa_key_derivation_operation_init();
 *            @endcode
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_key_derivation_operation_s psa_key_derivation_operation_t;

/**
 * @brief   Encoding of the step of a key derivation.
 */
typedef uint16_t psa_key_derivation_step_t;

/* These are all temporarily defined as some numeric type to prevent errors at compile time.*/
/**
 * @brief   The type of the state object for multi-part AEAD operations.
 *
 * @details Before calling any function on an AEAD operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_aead_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static
 *            or global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_aead_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_AEAD_OPERATION_INIT, for example:
 *            @code
 *            @ref psa_aead_operation_t operation = @ref PSA_AEAD_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_aead_operation_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_aead_operation_t operation;
 *            operation = @ref psa_aead_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_aead_operation_s psa_aead_operation_t;

/**
 * @brief   The type of the state object for multi-part MAC operations.
 *
 * @details Before calling any function on a MAC operation object, the application must initialize
 *          it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_mac_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static or
 *            global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_mac_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_MAC_OPERATION_INIT, for example:
 *            @code
 *            @ref psa_mac_operation_t operation = @ref PSA_MAC_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_mac_operation_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_mac_operation_t operation;
 *            operation = @ref psa_mac_operation_init();
 *            @endcode
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_mac_operation_s psa_mac_operation_t;

/**
 * @brief   The type of the state data structure for multipart hash operations.
 *
 * @details Before calling any function on a hash operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the structure to all-bits-zero, for example:
 *            @code
 *            @ref psa_hash_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the structure to logical zero values, for example:
 *            @code
 *            @ref psa_hash_operation_t operation = {0};
 *            @endcode
 *          - Initialize the structure to the initializer @ref PSA_HASH_OPERATION_INIT,
 *            for example:
 *            @code
 *            @ref psa_hash_operation_t operation = @ref PSA_HASH_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_hash_operation_init()
 *            to the structure, for example:
 *            @code
 *            @ref psa_hash_operation_t operation;
 *            operation = @ref psa_hash_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined struct. Applications should not
 *          make any assumptions about the content of this structure except
 *          as directed by the documentation of a specific implementation.
 */
typedef struct psa_hash_operation_s psa_hash_operation_t;

/**
 * @brief   The type of the state object for multi-part cipher operations.
 *
 * @details Before calling any function on a cipher operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_cipher_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static or
 *            global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_cipher_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_CIPHER_OPERATION_INIT, for example:
 *            @code
 *            @ref psa_cipher_operation_t operation = @ref PSA_CIPHER_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_cipher_operation_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_cipher_operation_t operation;
 *            operation = @ref psa_cipher_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_cipher_operation_s psa_cipher_operation_t;

#ifdef __cplusplus
}
#endif

/** @} */
