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
 * @file        crypto_types.h
 * @brief       Type definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_TYPES_H
#define PSA_CRYPTO_PSA_CRYPTO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "psa/algorithm.h"
#include "psa/error.h"
#include "psa/key/bits.h"
#include "psa/key/id.h"
#include "psa/key/lifetime.h"
#include "psa/key/usage.h"
#include "psa/key/type.h"

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
 * @brief   The type of an object containing key attributes.
 *
 * @details This is the object that represents the metadata of a key object. Metadata that can be
 *          stored in attributes includes:
 *          - The location of the key in storage, indicated by its key identifier and its lifetime.
 *          - The key’s policy, comprising usage flags and a specification of the permitted
 *            algorithm(s).
 *          - Information about the key itself: the key type and its size.
 *          - Implementations can define additional attributes.
 *
 *          The actual key material is not considered an attribute of a key. Key attributes do not
 *          contain information that is generally considered highly confidential.
 *
 * @note    Implementations are recommended to define the attribute object as a simple data
 *          structure, with fields corresponding to the individual key attributes. In such an
 *          implementation, each function @c psa_set_key_xxx() sets a field and the corresponding
 *          function @c psa_get_key_xxx() retrieves the value of the field.
 *          An implementation can report attribute values that are equivalent to the original one,
 *          but have a different encoding. For example, an implementation can use a more compact
 *          representation for types where many bit-patterns are invalid or not supported, and
 *          store all values that it does not support as a special marker value. In such an
 *          implementation, after setting an invalid value, the corresponding get function returns
 *          an invalid value which might not be the one that was originally stored.
 *
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 *
 *          An attribute object can contain references to auxiliary resources, for example pointers
 *          to allocated memory or indirect references to pre-calculated values. In order to free
 *          such resources, the application must call @ref psa_reset_key_attributes(). As an
 *          exception, calling @ref psa_reset_key_attributes() on an attribute object is optional
 *          if the object has only been modified by the following functions since it was
 *          initialized or last reset with @ref psa_reset_key_attributes():
 *          - @ref psa_set_key_id()
 *          - @ref psa_set_key_lifetime()
 *          - @ref psa_set_key_type()
 *          - @ref psa_set_key_bits()
 *          - @ref psa_set_key_usage_flags()
 *          - @ref psa_set_key_algorithm()
 *          Before calling any function on a key attribute object, the application must initialize
 *          it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_key_attributes_t attributes;
 *            memset(&attributes, 0, sizeof(attributes));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static or
 *            global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_key_attributes_t attributes;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_KEY_ATTRIBUTES_INIT, for example:
 *            @code
 *            @ref psa_key_attributes_t attributes = @ref PSA_KEY_ATTRIBUTES_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_key_attributes_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_key_attributes_t attributes;
 *            attributes = @ref psa_key_attributes_init();
 *            @endcode
 *
 *          A freshly initialized attribute object contains the following values:
 *          - lifetime:         @ref PSA_KEY_LIFETIME_VOLATILE.
 *          - key identifier:   @ref PSA_KEY_ID_NULL — which is not a valid key identifier.
 *          - type:             @ref PSA_KEY_TYPE_NONE — meaning that the type is unspecified.
 *          - key size:         @c 0 — meaning that the size is unspecified.
 *          - usage flags:      @c 0 — which allows no usage except exporting a public key.
 *          - algorithm:        @ref PSA_ALG_NONE — which does not allow cryptographic usage,
 *                              but allows exporting.
 *
 *          ## Usage
 *          A typical sequence to create a key is as follows:
 *          -# Create and initialize an attribute object.
 *          -# If the key is persistent, call @ref psa_set_key_id(). Also call @ref
 *             psa_set_key_lifetime() to place the key in a non-default location.
 *          -# Set the key policy with @ref psa_set_key_usage_flags() and @ref
 *             psa_set_key_algorithm().
 *          -# Set the key type with @ref psa_set_key_type(). Skip this step if copying an existing
 *             key with @ref psa_copy_key().
 *          -# When generating a random key with @ref psa_generate_key() or deriving a key with
 *             @ref psa_key_derivation_output_key(), set the desired key size with @ref
 *             psa_set_key_bits().
 *          -# Call a key creation function: @ref psa_import_key(), @ref psa_generate_key(),
 *             @ref psa_key_derivation_output_key() or @ref psa_copy_key(). This function reads the
 *             attribute object, creates a key with these attributes, and outputs an identifier for
 *             the newly created key.
 *          -# Optionally call @ref psa_reset_key_attributes(), now that the attribute object is no
 *             longer needed. Currently this call is not required as the attributes defined in this
 *             specification do not require additional resources beyond the object itself.
 *
 *          A typical sequence to query a key’s attributes is as follows:
 *          -# Call @ref psa_get_key_attributes().
 *          -# Call @c psa_get_key_xxx() functions to retrieve the required attribute(s).
 *          -# Call @ref psa_reset_key_attributes() to free any resources that can be used by the
 *             attribute object.
 *
 *          Once a key has been created, it is impossible to change its attributes.
 */
typedef struct psa_key_attributes_s psa_key_attributes_t;

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

#endif /* PSA_CRYPTO_PSA_CRYPTO_TYPES_H */
/** @} */
