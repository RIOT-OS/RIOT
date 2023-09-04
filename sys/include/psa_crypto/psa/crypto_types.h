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

/**
 * @brief   For encrypt-decrypt functions, whether the operation is an encryption
 *          or a decryption.
 */
typedef enum {
    PSA_CRYPTO_DRIVER_DECRYPT,
    PSA_CRYPTO_DRIVER_ENCRYPT
} psa_encrypt_or_decrypt_t;

/**
 * @brief   Encoding of a cryptographic algorithm.
 *
 * @details For algorithms that can be applied to multiple key types, this identifier does not
 *          encode the key type. For example, for symmetric ciphers based on a block cipher,
 *          @ref psa_algorithm_t encodes the block cipher mode and the padding mode while the
 *          block cipher itself is encoded via @ref psa_key_type_t.
 */
typedef uint32_t psa_algorithm_t;

/**
 * @brief   The type of PSA finite-field Diffie-Hellman group family identifiers.
 *
 * @details The group family identifier is required to create a finite-field Diffie-Hellman
 *          key using the @ref PSA_KEY_TYPE_DH_KEY_PAIR() or @ref PSA_KEY_TYPE_DH_PUBLIC_KEY()
 *          macros.
 *
 *          The specific Diffie-Hellman group within a family is identified by the @c key_bits
 *          attribute of the key.
 */
typedef uint8_t psa_dh_family_t;

/**
 * @brief   The type of PSA elliptic curve family identifiers.
 *
 * @details The curve identifier is required to create an ECC key using the
 *          @ref PSA_KEY_TYPE_ECC_KEY_PAIR() or @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY()
 *          macros.
 *
 *          The specific ECC curve within a family is identified by the @c key_bits
 *          attribute of the key.
 */
typedef uint8_t psa_ecc_family_t;

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

/**
 * @brief   Key identifier.
 *
 * @details A key identifier can be a permanent name for a persistent key, or a transient reference
 *          to volatile key.
 */
typedef uint32_t psa_key_id_t;

/**
 * @brief   Encoding of key lifetimes.
 *
 * @details The lifetime of a key indicates where it is stored and which application and system
 *          actions will create and destroy it.
 *
 *          Lifetime values have the following structure:
 *          - Bits[7:0]: Persistence level
 *            This value indicates what device management actions can cause it to be destroyed.
 *            In particular, it indicates whether the key is volatile or persistent.
 *            See @ref psa_key_persistence_t for more information.
 *            @ref PSA_KEY_LIFETIME_GET_PERSISTENCE(@p lifetime) returns the persistence level for
 *            a key lifetime value.
 *          - Bits[31:8]: Location indicator
 *            This value indicates where the key material is stored (or at least where it is
 *            accessible in cleartext) and where operations on the key are performed. See
 *            @ref psa_key_location_t for more information.
 *            @ref PSA_KEY_LIFETIME_GET_LOCATION(@p lifetime) returns the location indicator for a
 *            key lifetime value.
 *
 *          Volatile keys are automatically destroyed when the application instance terminates or
 *          on a power reset of the device. Persistent keys are preserved until the application
 *          explicitly destroys them or until an implementation-specific device management event
 *          occurs, for example, a factor reset.
 *
 *          Persistent keys have a key identifier of type @ref psa_key_id_t. This identifier
 *          remains valid throughout the lifetime of the key, even if the application instance
 *          that created the key terminates.
 *
 *          This specification defines two basic lifetime values:
 *          - Keys with the lifetime @ref PSA_KEY_LIFETIME_VOLATILE are volatile. All
 *            implementations should support this lifetime.
 *          - Keys with the lifetime @ref PSA_KEY_LIFETIME_PERSISTENT are persistent. All
 *            implementations that have access to persistent storage with appropriate security
 *            guarantees should support this lifetime.
 */
typedef uint32_t psa_key_lifetime_t;

/**
 * @brief   Encoding of key location indicators.
 *
 * @details If an implementation of this API can make calls to external cryptoprocessors such as
 *          secure elements, the location of a key indicates which secure element performs the
 *          operations on the key. If the key material is not stored persistently inside the secure
 *          element, it must be stored in a wrapped form such that only the secure element can
 *          access the key material in cleartext.
 *
 * @note    Key location indicators are 24-bit values. Key management interfaces operate on
 *          lifetimes (type @ref psa_key_lifetime_t), and encode the location as the upper 24 bits
 *          of a 32-bit value.
 *
 *          Values for location indicators defined by this specification are shown below:
 *          - @c 0:  Primary local storage.
 *                   All implementations should support this value. The primary local storage is
 *                   typically the same storage area that contains the key metadata.
 *          - @c 1:  Primary secure element.
 *                   @note  As of now, this value is not supported by this implementation.
 *                          Use the vendor-defined location values.
 *
 *                   Implementations should support this value if there is a secure element
 *                   attached to the operating environment. As a guideline, secure elements may
 *                   provide higher resistance against side channel and physical attacks than the
 *                   primary local storage, but may have restrictions on supported key types,
 *                   sizes, policies and operations and may have different performance
 *                   characteristics.
 *          - @c 2–0x7fffff:    Other locations defined by a PSA specification.
 *                              The PSA Cryptography API does not currently assign any meaning to
 *                              these locations, but future versions of this specification or other
 *                              PSA specifications may do so.
 *          - @c 0x800000–0xffffff: Vendor-defined locations. No PSA specification will assign a
 *                                  meaning to locations in this range.
 */
typedef uint32_t psa_key_location_t;

/**
 * @brief   Encoding of key persistence levels.
 *
 * @details What distinguishes different persistence levels is which device management events can
 *          cause keys to be destroyed. For example, power reset, transfer of device ownership,
 *          or a factory reset are device management events that can affect keys at different
 *          persistence levels. The specific management events which affect persistent keys at
 *          different levels is outside the scope of the PSA Cryptography specification.
 *
 * @note    Key persistence levels are 8-bit values. Key management interfaces operate on lifetimes
 *          (type @ref psa_key_lifetime_t), and encode the persistence value as the lower 8 bits of
 *          a 32-bit value.
 *
 *          Values for persistence levels defined by this specification are shown below:
 *          - @c 0 =    @ref PSA_KEY_PERSISTENCE_VOLATILE : Volatile key.
 *                      A volatile key is automatically destroyed by the implementation when the
 *                      application instance terminates. In particular, a volatile key is
 *                      automatically destroyed on a power reset of the device.
 *          - @c 1 =    @ref PSA_KEY_PERSISTENCE_DEFAULT : Persistent key with a default lifetime.
 *                      Implementations should support this value if they support persistent keys
 *                      at all. Applications should use this value if they have no specific needs
 *                      that are only met by implementation-specific features.
 *          - @c 2–127: Persistent key with a PSA-specified lifetime. The PSA Cryptography
 *                      specification does not define the meaning of these values, but other PSA
 *                      specifications may do so.
 *          - @c 128–254:   Persistent key with a vendor-specified lifetime. No PSA specification
 *                          will define the meaning of these values, so implementations may choose
 *                          the meaning freely. As a guideline, higher persistence levels should
 *                          cause a key to survive more management events than lower levels.
 *          - @c 255 =  @ref PSA_KEY_PERSISTENCE_READ_ONLY : Read-only or write-once key.
 *                      A key with this persistence level cannot be destroyed. Implementations that
 *                      support such keys may either allow their creation through the PSA
 *                      Cryptography API, preferably only to applications with the appropriate
 *                      privilege, or only expose keys created through implementation-specific
 *                      means such as a factory ROM engraving process.
 *                      @note   Keys that are read-only due to policy restrictions rather than
 *                              due to physical limitations should not have this persistence level.
 */
typedef uint8_t psa_key_persistence_t;

/**
 * @brief   Encoding of a key type.
 *
 * @details This is a structured bitfield that identifies the category and type of key. The range
 *          of key type values is divided as follows:
 *          - @ref PSA_KEY_TYPE_NONE == @c 0:
 *            Reserved as an invalid key type.
 *          - @c 0x0001–0x7fff:
 *            Specification-defined key types. Key types defined by this standard always have bit
 *            15 clear. Unallocated key type values in this range are reserved for future use.
 *          - @c 0x8000–0xffff:
 *            Implementation-defined key types. Implementations that define additional key types
 *            must use an encoding with bit 15 set.
 */
typedef uint16_t psa_key_type_t;

/**
 * @brief   Encoding of permitted usage on a key.
 */
typedef uint32_t psa_key_usage_t;

/**
 * @brief   Public interfaces use @c size_t, but internally we use a smaller type.
 */
typedef uint16_t psa_key_bits_t;

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
 * @brief   Function return status.
 *
 * @details This is either @ref PSA_SUCCESS, which is zero, indicating success; or a small
 *          negative value indicating that an error occurred. Errors are encoded as one of
 *          the @c PSA_ERROR_xxx values defined here.
 */
typedef int32_t psa_status_t;

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
