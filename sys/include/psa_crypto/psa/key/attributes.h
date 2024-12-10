/*
 * Copyright (C) 2024 TU Dresden
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
 * @file        key/attributes.h
 * @brief       Key attributes definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_KEY_ATTRIBUTES_H
#define PSA_CRYPTO_PSA_KEY_ATTRIBUTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/algorithm.h"
#include "bits.h"
#include "id.h"
#include "lifetime.h"
#include "type.h"
#include "usage.h"

/**
 * @brief   Structure storing the key usage policies
 */
struct psa_key_policy_s {
    psa_key_usage_t usage;  /**< Key usage policy */
    psa_algorithm_t alg;    /**< Algorithm for key usage */
};

/**
 * @brief   Type for key usage policies.
 */
typedef struct psa_key_policy_s psa_key_policy_t;

/**
 * @brief   Structure storing key attributes
 */
struct psa_key_attributes_s {
    psa_key_type_t type;            /**< Type of key */
    psa_key_bits_t bits;            /**< Size of key in bits */
    psa_key_lifetime_t lifetime;    /**< Lifetime of key */
    psa_key_id_t id;                /**< Key identifier */
    psa_key_policy_t policy;        /**< Key usage policy */
};

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
 * @brief   This macro returns a suitable initializer for a key attribute object of
 *          type @ref psa_key_attributes_t.
 */
#define PSA_KEY_ATTRIBUTES_INIT { 0 }

/**
 * @brief Return an initial value for a key attribute object.
 *
 * @return psa_key_attributes_t
 */
static inline psa_key_attributes_t psa_key_attributes_init(void)
{
    const psa_key_attributes_t v = PSA_KEY_ATTRIBUTES_INIT;

    return v;
}

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_KEY_ATTRIBUTES_H */
/** @} */
