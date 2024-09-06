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
 * @file        crypto_values.h
 * @brief       Value definitions for PSA Crypto.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @note        Some of the macros in this file have already been copied here from
 *              the PSA API specification, but are not implemented, yet.
 *              They are marked by comments that either say "specification-defined"
 *              or "implementation-defined".
 *              These macros will be implemented successively in the future.
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_VALUES_H
#define PSA_CRYPTO_PSA_CRYPTO_VALUES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto_types.h"

/**
 * @brief   PSA algorithm category mask
 */
#define PSA_ALG_CATEGORY_MASK                   ((psa_algorithm_t)0x7f000000)

/**
 * @brief   Category for hash algorithms
 */
#define PSA_ALG_CATEGORY_HASH                   ((psa_algorithm_t)0x02000000)
/**
 * @brief   Category for MAC algorithms
 */
#define PSA_ALG_CATEGORY_MAC                    ((psa_algorithm_t)0x03000000)
/**
 * @brief   Category for cipher algorithms
 */
#define PSA_ALG_CATEGORY_CIPHER                 ((psa_algorithm_t)0x04000000)
/**
 * @brief   Category for AEAD algorithms
 */
#define PSA_ALG_CATEGORY_AEAD                   ((psa_algorithm_t)0x05000000)
/**
 * @brief   Category for signature algorithms
 */
#define PSA_ALG_CATEGORY_SIGN                   ((psa_algorithm_t)0x06000000)
/**
 * @brief   Category for asymmetric encryption algorithms
 */
#define PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION  ((psa_algorithm_t)0x07000000)
/**
 * @brief   Category for key derivation algorithms
 */
#define PSA_ALG_CATEGORY_KEY_DERIVATION         ((psa_algorithm_t)0x08000000)
/**
 * @brief   Category for key agreement algorithms
 */
#define PSA_ALG_CATEGORY_KEY_AGREEMENT          ((psa_algorithm_t)0x09000000)

/**
 * @brief   Macro to build an AEAD minimum-tag-length wildcard algorithm.
 *
 * @details A key with a minimum-tag-length AEAD wildcard algorithm as permitted algorithm policy
 *          can be used with all AEAD algorithms sharing the same base algorithm, and where the tag
 *          length of the specific algorithm is equal to or larger then the minimum tag length
 *          specified by the wildcard algorithm.
 *
 * @note    When setting the minimum required tag length to less than the smallest tag length
 *          allowed by the base algorithm, this effectively becomes an ‘any-tag-length-allowed’
 *          policy for that base algorithm.
 *
 *          The AEAD algorithm with a default length tag can be recovered using
 *          @ref PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG().
 *
 * @param   aead_alg        An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                          @ref PSA_ALG_IS_AEAD(@p aead_alg) is true.
 * @param   min_tag_length  Desired minimum length of the authentication tag in bytes. This must be
 *                          at least 1 and at most the largest allowed tag length of the algorithm.
 *
 * @return  The corresponding AEAD wildcard algorithm with the specified minimum tag length
 *          Unspecified if @c aead_alg is not a supported AEAD algorithm or if @c min_tag_length is
 *          less than 1 or too large for the specified AEAD algorithm.
 */
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
    /* specification-defined value */

/**
 * @brief   An AEAD algorithm with the default tag length.
 *
 * @details This macro can be used to construct the AEAD algorithm with default tag length from an
 *          AEAD algorithm with a shortened tag. See also @ref PSA_ALG_AEAD_WITH_SHORTENED_TAG().
 *
 *          @b Compatible @b key @b types:
 *          The resulting AEAD algorithm is compatible with the same key types as the
 *          AEAD algorithm used to construct it.
 */
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
    ((((aead_alg) & ~0x003f0000) == 0x05400100) ? PSA_ALG_CCM : \
     (((aead_alg) & ~0x003f0000) == 0x05400200) ? PSA_ALG_GCM : \
     (((aead_alg) & ~0x003f0000) == 0x05000500) ? PSA_ALG_CHACHA20_POLY1305 : \
     PSA_ALG_NONE)

/**
 * @brief   Macro to build a AEAD algorithm with a shortened tag.
 *
 * @details An AEAD algorithm with a shortened tag is similar to the corresponding AEAD algorithm,
 *          but has an authentication tag that consists of fewer bytes. Depending on the algorithm,
 *          the tag length might affect the calculation of the ciphertext.
 *
 *          The AEAD algorithm with a default length tag can be recovered using
 *          @ref PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG().
 *
 *          @b Compatible @b key @b types:
 *          The resulting AEAD algorithm is compatible with the same key types as the AEAD
 *          algorithm used to construct it.
 *
 * @param aead_alg      An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p aead_alg) is true.
 * @param tag_length    Desired length of the authentication tag in bytes.
 *
 * @return  The corresponding AEAD algorithm with the specified tag length.
 *          Unspecified if @c aead_alg is not a supported AEAD algorithm or if @c tag_length is not
 *          valid for the specified AEAD algorithm.
 */
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length) \
    ((psa_algorithm_t)(((aead_alg) & ~0x003f0000) | (((tag_length) & 0x3f) << 16)))

/**
 * @brief   When setting a hash-and-sign algorithm in a key policy, permit any hash algorithm.
 *
 * @details This value can be used to form the permitted algorithm attribute of a key policy for a
 *          signature algorithm that is parametrized by a hash. A key with this policy can then be
 *          used to perform operations using the same signature algorithm parametrized with any
 *          supported hash. A signature algorithm created using this macro is a wildcard algorithm,
 *          and @ref PSA_ALG_IS_WILDCARD() will return true.
 *
 *          This value must not be used to build other algorithms that are parametrized over a
 *          hash. For any valid use of this macro to build an algorithm alg,
 *          @ref PSA_ALG_IS_HASH_AND_SIGN(alg) is true. This value must not be used to build an
 *          algorithm specification to perform an operation. It is only valid for setting the
 *          permitted algorithm in a key policy.
 *
 *          @b Usage:
 *          For example, suppose that @c PSA_xxx_SIGNATURE is one of the following macros:
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN
 *          - @ref PSA_ALG_RSA_PSS
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *
 *          The following sequence of operations shows how @ref PSA_ALG_ANY_HASH can be used in a
 *          key policy:
 *          -#  Set the key usage flags using @ref PSA_ALG_ANY_HASH, for example:
 *              @code
 *              @ref psa_set_key_usage_flags(@p &attributes, @ref PSA_KEY_USAGE_SIGN_MESSAGE);
 *              // or VERIFY_MESSAGE
 *              @ref psa_set_key_algorithm(@p &attributes,
 *              PSA_xxx_SIGNATURE(@ref PSA_ALG_ANY_HASH));
 *              @endcode
 *          -#  Import or generate key material.
 *          -#  Call @ref psa_sign_message() or @ref psa_verify_message(), passing an algorithm
 *              built from PSA_xxx_SIGNATURE and a specific hash. Each call to sign or verify a
 *              message can use a different hash algorithm.
 *              @code
 *              @ref psa_sign_message(@p key, PSA_xxx_SIGNATURE(@ref PSA_ALG_SHA_256), ...);
 *              @ref psa_sign_message(@p key, PSA_xxx_SIGNATURE(@ref PSA_ALG_SHA_512), ...);
 *              @ref psa_sign_message(@p key, PSA_xxx_SIGNATURE(@ref PSA_ALG_SHA3_256), ...);
 *              @endcode
 */
#define PSA_ALG_ANY_HASH ((psa_algorithm_t)0x020000ff)

/**
 * @brief   Macro to build a MAC minimum-MAC-length wildcard algorithm.
 *
 * @details A key with a minimum-MAC-length MAC wildcard algorithm as permitted algorithm policy
 *          can be used with all MAC algorithms sharing the same base algorithm, and where the
 *          (potentially truncated) MAC length of the specific algorithm is equal to or larger then
 *          the wildcard algorithm’s minimum MAC length.
 *
 * @note    When setting the minimum required MAC length to less than the smallest MAC length
 *          allowed by the base algorithm, this effectively becomes an ‘any-MAC-length-allowed’
 *          policy for that base algorithm.
 *
 *          The untruncated MAC algorithm can be recovered using @ref PSA_ALG_FULL_LENGTH_MAC().
 *
 *          @b Compatible @b key @b types
 *          The resulting wildcard MAC algorithm is compatible with the same key types as the MAC
 *          algorithm used to construct it.
 *
 * @param   mac_alg         A MAC algorithm: a value of type psa_algorithm_t such that
 *                          @ref PSA_ALG_IS_MAC(alg) is true.
 *                          This can be a truncated or untruncated MAC algorithm.
 * @param   min_mac_length  Desired minimum length of the message authentication code in bytes.
 *                          This must be at most the untruncated length of the MAC and must be at
 *                          least 1.
 * @return  The corresponding MAC wildcard algorithm with the specified minimum MAC length.
 *          Unspecified if mac_alg is not a supported MAC algorithm or if min_mac_length is less
 *          than 1 or too large for the specified MAC algorithm.
 */
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length) \
    /* specification-defined value */

/**
 * @brief   Deterministic ECDSA signature scheme, with hashing.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This is the deterministic ECDSA signature scheme defined by Deterministic Usage of the
 *          Digital Signature Algorithm (DSA) and Elliptic Curve Digital Signature Algorithm
 *          (ECDSA) [RFC6979].
 *
 *          The representation of a signature is the same as with @ref PSA_ALG_ECDSA().
 *
 * @note    When based on the same hash algorithm, the verification operations for
 *          @ref PSA_ALG_ECDSA and @ref PSA_ALG_DETERMINISTIC_ECDSA are identical. A signature
 *          created using @ref PSA_ALG_ECDSA can be verified with the same key using either
 *          @ref PSA_ALG_ECDSA or @ref PSA_ALG_DETERMINISTIC_ECDSA. Similarly, a signature created
 *          using @ref PSA_ALG_DETERMINISTIC_ECDSA can be verified with the same key using
 *          either @ref PSA_ALG_ECDSA or @ref PSA_ALG_DETERMINISTIC_ECDSA.
 *
 *          In particular, it is impossible to determine whether a signature was produced with
 *          deterministic ECDSA or with randomized ECDSA: it is only possible to verify that a
 *          signature was made with ECDSA with the private key corresponding to the public key used
 *          for the verification.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(@p family)
 *          - @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(@p family) (signature verification only)
 *
 *          where family is a Weierstrass Elliptic curve family.
 *          That is, one of the following values:
 *          - PSA_ECC_FAMILY_SECT_XX
 *          - PSA_ECC_FAMILY_SECP_XX
 *          - @ref PSA_ECC_FAMILY_FRP
 *          - @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true. This includes
 *                      @ref PSA_ALG_ANY_HASH when specifying the algorithm in a key policy.
 *
 * @return  The corresponding deterministic ECDSA signature algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_DETERMINISTIC_ECDSA(hash_alg) \
    ((psa_algorithm_t)(0x06000700 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to construct the MAC algorithm with a full length MAC, from a truncated MAC
 *          algorithm.
 *
 * @details @b Compatible @b key @b types
 *          The resulting untruncated MAC algorithm is compatible with the same key types as the MAC
 *          algorithm used to construct it.
 *
 * @param   mac_alg A MAC algorithm: a value of type @ref psa_algorithm_t such that
 *                  @ref PSA_ALG_IS_MAC(@p mac_alg) is true.
 *                  This can be a truncated or untruncated MAC algorithm.
 *
 * @return  The corresponding MAC algorithm with a full length MAC.
 *          Unspecified if @c mac_alg is not a supported MAC algorithm. *
 */
#define PSA_ALG_FULL_LENGTH_MAC(mac_alg) \
    ((psa_algorithm_t)((mac_alg) & ~0x003f0000))

/**
 * @brief   Macro to build an HKDF algorithm.
 *
 * @details This is the HMAC-based Extract-and-Expand Key Derivation Function (HKDF) specified by
 *          HMAC-based Extract-and-Expand Key Derivation Function (HKDF) [RFC5869].
 *
 *          This key derivation algorithm uses the following inputs:
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SALT is the salt used in the “extract” step. It is
 *              optional; if omitted, the derivation uses an empty salt.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_INFO is the info string used in the “expand” step.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SECRET is the secret key used in the “extract” step.
 *
 *          If @ref PSA_KEY_DERIVATION_INPUT_SALT is provided, it must be before
 *          @ref PSA_KEY_DERIVATION_INPUT_SECRET. @ref PSA_KEY_DERIVATION_INPUT_INFO can be
 *          provided at any time after setup and before starting to generate output.
 *
 *          Each input may only be passed once.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the secret key)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the other inputs)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding HKDF algorithm. For example, @ref PSA_ALG_HKDF(@ref PSA_ALG_SHA_256)
 *          is HKDF using HMAC-SHA-256.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_HKDF(hash_alg) \
    ((psa_algorithm_t)(0x08000100 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Get the hash used by a composite algorithm.
 *
 *          The following composite algorithms require a hash algorithm:
 *          - @ref PSA_ALG_ECDSA()
 *          - @ref PSA_ALG_HKDF()
 *          - @ref PSA_ALG_HMAC()
 *          - @ref PSA_ALG_RSA_OAEP()
 *          - @ref PSA_ALG_IS_RSA_PKCS1V15_SIGN()
 *          - @ref PSA_ALG_RSA_PSS()
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT()
 *          - @ref PSA_ALG_TLS12_PRF()
 *          - @ref PSA_ALG_TLS12_PSK_TO_MS()
 *          - @ref PSA_ALG_PBKDF2_HMAC()
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  The underlying hash algorithm if alg is a composite algorithm that uses a hash algorithm
 *          @ref PSA_ALG_NONE if alg is not a composite algorithm that uses a hash.
 */
#define PSA_ALG_GET_HASH(alg) \
    (((alg) & PSA_ALG_HASH_MASK) == \
     0 ? ((psa_algorithm_t)0) : PSA_ALG_CATEGORY_HASH | ((alg) & PSA_ALG_HASH_MASK))

/**
 * @brief HMAC Base
 */
#define PSA_ALG_HMAC_BASE   (0x03800000)

/**
 * @brief   Macro to build an HMAC message-authentication-code algorithm from an underlying hash
 *          algorithm.
 *
 * @details For example, @ref PSA_ALG_HMAC(@ref PSA_ALG_SHA_256) is HMAC-SHA-256.
 *          The HMAC construction is defined in HMAC: Keyed-Hashing for Message Authentication
 *          [RFC2104].
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_HMAC
 *
 * @param   hash_alg    A hash algorithm (PSA_ALG_XXX value such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true).
 *
 * @return  The corresponding HMAC algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_HMAC(hash_alg) \
    ((psa_algorithm_t)(PSA_ALG_HMAC_BASE | ((hash_alg) & PSA_ALG_HASH_MASK)))

/**
 * @brief   Whether the specified algorithm is a symmetric cipher algorithm.
 *
 * @param   alg An algorithm identifier (value of type @ref psa_algorithm_t).
 *
 * @return  1 if alg is a symmetric cipher algorithm,
 *          0 if algorithm is not recognized or not supported
 */
#define PSA_ALG_IS_CIPHER(alg)                                          \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_CIPHER)

/**
 * @brief   Whether the specified algorithm is an authenticated encryption with
 *          associated data (AEAD) algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an AEAD algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_AEAD(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == 0x05000000)

/**
 * @brief   Whether the specified algorithm is an AEAD mode on a block cipher.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an AEAD algorithm which is an AEAD mode based on a block cipher
 *          0 otherwise.
 */
#define PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) \
    (((alg) & 0x7f400000) == 0x05400000)

/**
 * @brief   Whether the specified algorithm is an asymmetric encryption algorithm, also known as
 *          public-key encryption algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an asymmetric encryption algorithm,
 *          0 otherwise.
 */
#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == 0x07000000)

/**
 * @brief   Whether the specified algorithm is a MAC algorithm based on a block cipher.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a MAC algorithm based on a block cipher
 *          0 otherwise
 */
#define PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) \
    (((alg) & 0x7fc00000) == 0x03c00000)

/**
 * @brief   Whether the specified algorithm is deterministic ECDSA.
 *
 *          See also @ref PSA_ALG_IS_ECDSA() and @ref PSA_ALG_IS_RANDOMIZED_ECDSA().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a deterministic ECDSA algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_DETERMINISTIC_ECDSA(alg) \
    (((alg) & ~0x000000ff) == 0x06000700)

/**
 * @brief   Whether the specified algorithm is an elliptic curve Diffie-Hellman algorithm.
 *
 * @details This includes the raw elliptic curve Diffie-Hellman algorithm as well as elliptic curve
 *          Diffie-Hellman followed by any supporter key derivation algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an elliptic curve Diffie-Hellman algorithm
 *          0 otherwise.
 */
#define PSA_ALG_IS_ECDH(alg) \
    (((alg) & 0x7fff0000) == 0x09020000)

/**
 * @brief   Whether the specified algorithm is a finite field Diffie-Hellman algorithm.
 *
 * @details This includes the raw finite field Diffie-Hellman algorithm as well as finite-field
 *          Diffie-Hellman followed by any supported key derivation algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a finite field Diffie-Hellman algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_FFDH(alg) \
    (((alg) & 0x7fff0000) == 0x09010000)

/**
 * @brief   Whether the specified algorithm is ECDSA.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an ECDSA algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_ECDSA(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_SIGN)

/**
 * @brief   Whether the specified algorithm is a hash algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a hash algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HASH(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_HASH)

/**
 * @brief   Get the hash operation of a hmac algorithm
 *
 * @param   hmac_alg    HMAC algorithm
 */
#define PSA_ALG_HMAC_GET_HASH(hmac_alg)                             \
    (PSA_ALG_CATEGORY_HASH | ((hmac_alg) & PSA_ALG_HASH_MASK))

/**
 * @brief   Whether the specified algorithm is HashEdDSA.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a HashEdDSA algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HASH_EDDSA(alg) \
    (((alg) & ~0x000000ff) == 0x06000900)

/**
 * @brief   Whether the specified algorithm is a hash-and-sign algorithm that signs exactly the hash
 *          value.
 *
 * @details This macro identifies algorithms that can be used with @ref psa_sign_hash() that use
 *          the exact message hash value as an input the signature operation.
 *          For example, if @ref PSA_ALG_IS_HASH_AND_SIGN(@p alg) is true, the following call
 *          sequence is equivalent to
 *          @ref psa_sign_message(@p key, @p alg, @p msg, @p msg_len, ...):
 *
 *          @code
 *               @ref psa_hash_operation_t op = {0};
 *               uint8_t hash[@ref PSA_HASH_MAX_SIZE];
 *               size_t hash_len;
 *               @ref psa_hash_setup(&op, @ref PSA_ALG_GET_HASH(alg));
 *               @ref psa_hash_update(&op, msg, msg_len);
 *               @ref psa_hash_finish(&op, hash, sizeof(hash), &hash_len);
 *               @ref psa_sign_hash(key, alg, hash, hash_len, ...);
 *          @endcode
 *
 *          This excludes hash-and-sign algorithms that require a encoded or modified hash for the
 *          signature step in the algorithm, such as @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW. For such
 *          algorithms, @ref PSA_ALG_IS_SIGN_HASH() is true but @ref PSA_ALG_IS_HASH_AND_SIGN() is
 *          false.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a hash-and-sign algorithm that signs exactly the hash value
 *          0 otherwise
 *          A wildcard signature algorithm policy, using @ref PSA_ALG_ANY_HASH, returns the same
 *          value as the signature algorithm parameterised with a valid hash algorithm.
 */
#define PSA_ALG_IS_HASH_AND_SIGN(alg)   \
    (PSA_ALG_IS_SIGN_HASH(alg) &&       \
     ((alg) & PSA_ALG_HASH_MASK) != 0)

/**
 * @brief   Whether the specified algorithm is an HKDF algorithm.
 *
 * @details HKDF is a family of key derivation algorithms that are based on a hash function and the
 *          HMAC construction.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an HKDF algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HKDF(alg) \
    (((alg) & ~0x000000ff) == 0x08000100)

/**
 * @brief   Whether the specified algorithm is an HMAC algorithm.
 *
 * @details HMAC is a family of MAC algorithms that are based on a hash function.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an HMAC algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HMAC(alg) \
    (((alg) & 0x7fc0ff00) == 0x03800000)

/**
 * @brief   Whether the specified algorithm is a key agreement algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a key agreement algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_KEY_AGREEMENT(alg) \
    (((alg) & 0x7f000000) == 0x09000000)

/**
 * @brief   Whether the specified algorithm is a key derivation algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a key derivation algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_KEY_DERIVATION(alg) \
    (((alg) & 0x7f000000) == 0x08000000)

/**
 * @brief   Whether the specified algorithm is a key-stretching or password-hashing algorithm.
 *
 * @details A key-stretching or password-hashing algorithm is a key derivation algorithm that is
 *          suitable for use with a low-entropy secret such as a password. Equivalently, it’s a key
 *          derivation algorithm that uses a @ref PSA_KEY_DERIVATION_INPUT_PASSWORD input step.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a key-stretching or password-hashing algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_KEY_DERIVATION_STRETCHING(alg) \
    (((alg) & 0x7f800000) == 0x08800000)

/**
 * @brief   Whether the specified algorithm is a MAC algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a MAC algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_MAC(alg) \
    (((alg) & 0x7f000000) == 0x03000000)

/**
 * @brief   Whether the specified algorithm is a PBKDF2-HMAC algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a PBKDF2-HMAC algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_PBKDF2_HMAC(alg) \
    (((alg) & ~0x000000ff) == 0x08800100)

/**
 * @brief   Whether the specified algorithm is randomized ECDSA.
 *
 * @details See also @ref PSA_ALG_IS_ECDSA() and @ref PSA_ALG_IS_DETERMINISTIC_ECDSA().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is arandomized ECDSA algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_RANDOMIZED_ECDSA(alg) \
    (((alg) & ~0x000000ff) == 0x06000600)

/**
 * @brief   Whether the specified algorithm is a raw key agreement algorithm.
 *
 * @details A raw key agreement algorithm is one that does not specify a key derivation function.
 *          Usually, raw key agreement algorithms are constructed directly with a
 *          PSA_ALG_xxx macro while non-raw key agreement algorithms are constructed with
 *          @ref PSA_ALG_KEY_AGREEMENT().
 *
 *          The raw key agreement algorithm can be extracted from a full key agreement algorithm
 *          identifier using @ref PSA_ALG_KEY_AGREEMENT_GET_BASE().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a raw key agreement algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_RAW_KEY_AGREEMENT(alg) \
    (((alg) & 0x7f00ffff) == 0x09000000)

/**
 * @brief   Whether the specified algorithm is an RSA OAEP encryption algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a RSA OAEP algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_RSA_OAEP(alg) \
    (((alg) & ~0x000000ff) == 0x07000300)

/**
 * @brief   Whether the specified algorithm is an RSA PKCS#1 v1.5 signature algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a RSA PKCS#1 v1.5 signature algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) \
    (((alg) & ~0x000000ff) == 0x06000200)

/**
 * @brief   Whether the specified algorithm is a RSA PSS signature algorithm.
 *
 * @details This macro returns 1 for algorithms constructed using either @ref PSA_ALG_RSA_PSS() or
 *          @ref PSA_ALG_RSA_PSS_ANY_SALT().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a RSA PSS signature algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_RSA_PSS(alg) \
    (((alg) & ~0x000000ff) == 0x06000300)

/**
 * @brief   Whether the specified algorithm is an RSA PSS signature algorithm that permits any salt
 *          length.
 *
 * @details An RSA PSS signature algorithm that permits any salt length is constructed using
 *          @ref PSA_ALG_RSA_PSS_ANY_SALT().
 *
 *          See also @ref PSA_ALG_IS_RSA_PSS() and @ref PSA_ALG_IS_RSA_PSS_STANDARD_SALT().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a RSA PSS signature algorithm that permits any salt length
 *          0 otherwise
 */
#define PSA_ALG_IS_RSA_PSS_ANY_SALT(alg) \
    (((alg) & ~0x000000ff) == 0x06001300)

/**
 * @brief   Whether the specified algorithm is an RSA PSS signature algorithm that requires the
 *          standard salt length.
 *
 * @details An RSA PSS signature algorithm that requires the standard salt length is constructed
 *          using @ref PSA_ALG_RSA_PSS().
 *
 *          See also @ref PSA_ALG_IS_RSA_PSS() and @ref PSA_ALG_IS_RSA_PSS_ANY_SALT().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an RSA PSS signature algorithm that requires the standard salt length
 *          0 otherwise
 */
#define PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) \
    (((alg) & ~0x000000ff) == 0x06000300)

/**
 * @brief   Whether the specified algorithm is an asymmetric signature algorithm, also known as
 *          public-key signature algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an asymmetric signature algorithm algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_SIGN(alg) \
    (((alg) & 0x7f000000) == 0x06000000)

/**
 * @brief   Whether the specified algorithm is a signature algorithm that can be used with
 *          @ref psa_sign_hash() and @ref psa_verify_hash().
 *
 * @details This includes all algorithms such that @ref PSA_ALG_IS_HASH_AND_SIGN() is true,
 *          as well as signature algorithms for which the input to @ref psa_sign_hash() or
 *          @ref psa_verify_hash() is not directly a hash, such as
 *          @ref PSA_ALG_IS_RSA_PKCS1V15_SIGN.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a signature algorithm that can be used to sign a hash.
 *          0 if alg is a signature algorithm that can only be used to sign a message.
 *          0 if alg is not a signature algorithm
 */
#define PSA_ALG_IS_SIGN_HASH(alg) \
    (PSA_ALG_IS_RSA_PSS(alg) || PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||    \
     PSA_ALG_IS_ECDSA(alg) || PSA_ALG_IS_HASH_EDDSA(alg))

/**
 * @brief   Whether the specified algorithm is a signature algorithm that can be used with
 *          @ref psa_sign_message() and @ref psa_verify_message().
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a signature algorithm that can be used to sign a message.
 *          0 if alg is a signature algorithm that can only be used to sign an already-calculated
 *          hash.
 *          0 if alg is not a signature algorithm.
 */
#define PSA_ALG_IS_SIGN_MESSAGE(alg) \
     (PSA_ALG_IS_SIGN_HASH(alg) || (alg) == PSA_ALG_PURE_EDDSA)

/**
 * @brief   Whether the specified algorithm is a stream cipher.
 *
 * @details A stream cipher is a symmetric cipher that encrypts or decrypts messages by applying a
 *          bitwise-xor with a stream of bytes that is generated from a key.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a stream cipher
 *          0 otherwise
 */
#define PSA_ALG_IS_STREAM_CIPHER(alg) \
    (((alg) & 0x7f800000) == 0x04800000)

/**
 * @brief   Whether the specified algorithm is a TLS-1.2 PRF algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a TLS-1.2 PRF algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_TLS12_PRF(alg) \
    (((alg) & ~0x000000ff) == 0x08000200)

/**
 * @brief   Whether the specified algorithm is a TLS-1.2 PSK to MS algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a TLS-1.2 PSK to MS algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_TLS12_PSK_TO_MS(alg) \
    (((alg) & ~0x000000ff) == 0x08000300)

/**
 * @brief   Whether the specified algorithm encoding is a wildcard.
 *
 * @details Wildcard algorithm values can only be used to set the permitted algorithm field in a
 *          key policy, wildcard values cannot be used to perform an operation.
 *
 *          See @ref PSA_ALG_ANY_HASH for example of how a wildcard algorithm can be used in a key
 *          policy.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a wildcard algorithm encoding.
 *          0 if alg is a non-wildcard algorithm encoding that is suitable for an operation.
 */
#define PSA_ALG_IS_WILDCARD(alg) \
    (PSA_ALG_GET_HASH(alg) == PSA_ALG_HASH_ANY)

/**
 * @brief   Macro to build a combined algorithm that chains a key agreement with a key derivation.
 *
 * @details A combined key agreement algorithm is used with a multi-part key derivation operation,
 *          using a call to @ref psa_key_derivation_key_agreement().
 *
 *          The component parts of a key agreement algorithm can be extracted using
 *          @ref PSA_ALG_KEY_AGREEMENT_GET_BASE() and @ref PSA_ALG_KEY_AGREEMENT_GET_KDF().
 *
 *          @b Compatible @b key @b types
 *          The resulting combined key agreement algorithm is compatible with the same key types as
 *          the raw key agreement algorithm used to construct it.
 *
 * @param   ka_alg  A key agreement algorithm: a value of type @ref psa_algorithm_t such that
 *                  @ref PSA_ALG_IS_KEY_AGREEMENT(@p ka_alg) is true.
 * @param   kdf_alg A key derivation algorithm: a value of type @ref psa_algorithm_t such that
 *                  @ref PSA_ALG_IS_KEY_DERIVATION(@p kdf_alg) is true.
 *
 * @return  The corresponding key agreement and derivation algorithm.
 *          Unspecified if ka_alg is not a supported key agreement algorithm or kdf_alg is not a
 *          supported key derivation algorithm.
 */
#define PSA_ALG_KEY_AGREEMENT(ka_alg, kdf_alg) \
    ((ka_alg) | (kdf_alg))

/**
 * @brief   Get the raw key agreement algorithm from a full key agreement algorithm.
 *
 * @details See also @ref PSA_ALG_KEY_AGREEMENT() and @ref PSA_ALG_KEY_AGREEMENT_GET_KDF().
 *
 * @param   alg A key agreement algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_KEY_AGREEMENT(@p alg) is true.
 *
 * @return  The underlying raw key agreement algorithm if @c alg is a key agreement algorithm.
 *          Unspecified if alg is not a key agreement algorithm or if it is not supported by the
 *          implementation.
 */
#define PSA_ALG_KEY_AGREEMENT_GET_BASE(alg) \
    ((psa_algorithm_t)((alg) & 0xffff0000))

/**
 * @brief   Get the key derivation algorithm used in a full key agreement algorithm.
 *
 * @details See also @ref PSA_ALG_KEY_AGREEMENT() and @ref PSA_ALG_KEY_AGREEMENT_GET_BASE().
 *
 * @param   alg A key agreement algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_KEY_AGREEMENT(@p alg) is true.
 *
 * @return  The underlying key derivation algorithm if @c alg is a key agreement algorithm.
 *          Unspecified if alg is not a key agreement algorithm or if it is not supported by the
 *          implementation.
 */
#define PSA_ALG_KEY_AGREEMENT_GET_KDF(alg) \
    ((psa_algorithm_t)((alg) & 0xfe00ffff))

/**
 * @brief   An invalid algorithm identifier value.
 *
 * @details Zero is not the encoding of any algorithm.
 */
#define PSA_ALG_NONE ((psa_algorithm_t)0)

/**
 * @brief   Hash algorithm mask
 */
#define PSA_ALG_HASH_MASK   ((psa_algorithm_t)0x000000ff)

/**
 * @brief   The MD2 message-digest algorithm.
 *
 * @warning The MD2 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details MD2 is defined in *The MD2 Message-Digest Algorithm
 *          [RFC1319](https://tools.ietf.org/html/rfc1319.html)*.
 */
#define PSA_ALG_MD2 ((psa_algorithm_t)0x02000001)

/**
 * @brief   The MD4 message-digest algorithm.
 *
 * @warning The MD4 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details MD4 is defined in *The MD4 Message-Digest Algorithm
 *          [RFC1320](https://tools.ietf.org/html/rfc1320.html)*.
 */
#define PSA_ALG_MD4 ((psa_algorithm_t)0x02000002)

/**
 * @brief   The MD5 message-digest algorithm.
 *
 * @warning The MD5 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details MD5 is defined in *The MD5 Message-Digest Algorithm
 *          [RFC1321](https://tools.ietf.org/html/rfc1321.html)*.
 */
#define PSA_ALG_MD5 ((psa_algorithm_t)0x02000003)

/**
 * @brief   The RIPEMD-160 message-digest algorithm.
 *
 * @details RIPEMD-160 is defined in *RIPEMD-160: A Strengthened Version of RIPEMD*,
 *          and also in *ISO/IEC 10118-3:2018 IT Security techniques — Hash-functions — Part 3:
 *          Dedicated hash-functions [ISO10118](https://www.iso.org/standard/67116.html)*.
 */
#define PSA_ALG_RIPEMD160 ((psa_algorithm_t)0x02000004)

/**
 * @brief   The SHA-1 message-digest algorithm.
 *
 * @warning The SHA-1 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details SHA-1 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_1 ((psa_algorithm_t)0x02000005)

/**
 * @brief   The SHA-224 message-digest algorithm.
 *
 * @details SHA-224 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_224     ((psa_algorithm_t)0x02000008) /** SHA-224 */

/**
 * @brief   The SHA-256 message-digest algorithm.
 *
 * @details SHA-256 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_256     ((psa_algorithm_t)0x02000009) /** SHA-256 */

/**
 * @brief   The SHA-384 message-digest algorithm.
 *
 * @details SHA-384 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_384     ((psa_algorithm_t)0x0200000a) /** SHA-384 */

/**
 * @brief   The SHA-512 message-digest algorithm.
 *
 * @details SHA-512 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_512     ((psa_algorithm_t)0x0200000b) /** SHA-512 */

/**
 * @brief   The SHA-512/224 message-digest algorithm.
 *
 * @details SHA-512-224 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_512_224 ((psa_algorithm_t)0x0200000c) /** SHA-512/224 */

/**
 * @brief   The SHA-512/256 message-digest algorithm.
 *
 * @details SHA-512-256 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_512_256 ((psa_algorithm_t)0x0200000d) /** SHA-512/256 */

/**
 * @brief   The SHA3-224 message-digest algorithm.
 *
 * @details SHA3-224 is defined in FIPS Publication 202: *SHA-3 Standard: Permutation-Based
 *          Hash and Extendable-Output Functions [FIPS202](https://doi.org/10.6028/NIST.FIPS.202)*.
 */
#define PSA_ALG_SHA3_224    ((psa_algorithm_t)0x02000010) /** SHA-3-224 */

/**
 * @brief   The SHA3-256 message-digest algorithm.
 *
 * @details SHA3-256 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 */
#define PSA_ALG_SHA3_256    ((psa_algorithm_t)0x02000011) /** SHA-3-256 */

/**
 * @brief   The SHA3-384 message-digest algorithm.
 *
 * @details SHA3-384 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 */
#define PSA_ALG_SHA3_384    ((psa_algorithm_t)0x02000012) /** SHA-3-384 */

/**
 * @brief   The SHA3-512 message-digest algorithm.
 *
 * @details SHA3-512 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 */
#define PSA_ALG_SHA3_512    ((psa_algorithm_t)0x02000013) /** SHA-3-512 */

/**
 * @brief   The SM3 message-digest algorithm.
 *
 * @details SM3 is defined in *ISO/IEC 10118-3:2018 IT Security techniques — Hash-functions —
 *          Part 3: Dedicated hash-functions [ISO10118](https://www.iso.org/standard/67116.html)*,
 *          and also in *GM/T 0004-2012: SM3 cryptographic hash algorithm
 *          [CSTC0004](http://www.gmbz.org.cn/main/viewfile/20180108023812835219.html)
 *          (English version
 *          [CSTC0004/E](http://www.gmbz.org.cn/main/postDetail.html?id=20180724105928))*.
 */
#define PSA_ALG_SM3         ((psa_algorithm_t)0x02000014) /** SM3 */

/**
 * @brief   The first 512 bits (64 bytes) of the SHAKE256 output.
 *
 * @details This is the prehashing for Ed448ph (see @ref PSA_ALG_ED448PH).
 *
 *          SHAKE256 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 *
 * @note    For other scenarios where a hash function based on SHA3 or SHAKE is required, SHA3-512
 *          is recommended. SHA3-512 has the same output size, and a theoretically higher security
 *          strength.
 */
#define PSA_ALG_SHAKE256_512 ((psa_algorithm_t)0x02000015)

/**
 * @brief   The CBC-MAC message-authentication-code algorithm, constructed over a block cipher.
 *
 * @warning CBC-MAC is insecure in many cases. A more secure mode, such as @ref PSA_ALG_CMAC,
 *          is recommended.
 *
 * @details The CBC-MAC algorithm must be used with a key for a block cipher.
 *          For example, one of @ref PSA_KEY_TYPE_AES.
 *
 *          CBC-MAC is defined as MAC Algorithm 1 in ISO/IEC 9797-1:2011 Information technology —
 *          Security techniques — Message Authentication Codes (MACs) — Part 1: Mechanisms using a
 *          block cipher
 *          [ISO9797](https://www.iso.org/standard/50375.html).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CBC_MAC     ((psa_algorithm_t)0x03c00100)

/**
 * @brief   The CMAC message-authentication-code algorithm, constructed over a block cipher.
 *
 * @details The CMAC algorithm must be used with a key for a block cipher. For example, when used
 *          with a key with type @ref PSA_KEY_TYPE_AES, the resulting operation is AES-CMAC.
 *
 *          CMAC is defined in NIST Special Publication 800-38B: Recommendation for Block Cipher
 *          Modes of Operation: the CMAC Mode for Authentication
 *          [SP800-38B](https://doi.org/10.6028/NIST.SP.800-38B).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CMAC        ((psa_algorithm_t)0x03c00200)

/**
 * @brief   The Cipher Block Chaining (CBC) mode of a block cipher, with no padding.
 *
 * @details The underlying block cipher is determined by the key type. This symmetric cipher mode
 *          can only be used with messages whose lengths are a multiple of the block size of the
 *          chosen block cipher.
 *
 *          CBC mode requires an initialization vector (IV) that is the same size as the cipher
 *          block length.
 *
 * @note    The cipher block length can be determined using @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 *
 *          The CBC block cipher mode is defined in NIST Special Publication 800-38A:
 *          Recommendation for Block Cipher Modes of Operation: Methods and Techniques
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38B).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CBC_NO_PADDING  ((psa_algorithm_t)0x04404000)

/**
 * @brief   The Cipher Block Chaining (CBC) mode of a block cipher, with PKCS#7 padding.
 *
 * @details The underlying block cipher is determined by the key type.
 *
 *          CBC mode requires an initialization vector (IV) that is the same size as the cipher
 *          block length.
 *
 * @note    The cipher block length can be determined using @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 *
 *          The CBC block cipher mode is defined in NIST Special Publication 800-38A:
 *          Recommendation for Block Cipher Modes of Operation: Methods and Techniques
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38B).
 *          The padding operation is defined by PKCS #7: Cryptographic Message Syntax Version 1.5
 *          [RFC2315](https://tools.ietf.org/html/rfc2315.html) §10.3.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CBC_PKCS7       ((psa_algorithm_t)0x04404100)

/**
 * @brief   The Electronic Codebook (ECB) mode of a block cipher, with no padding.
 *
 * @warning ECB mode does not protect the confidentiality of the encrypted data except in extremely
 *          narrow circumstances. It is recommended that applications only use ECB if they need to
 *          construct an operating mode that the implementation does not provide. Implementations
 *          are encouraged to provide the modes that applications need in preference to supporting
 *          direct access to ECB.
 *
 * @details The underlying block cipher is determined by the key type.
 *
 *          This symmetric cipher mode can only be used with messages whose lengths are a multiple
 *          of the block size of the chosen block cipher.
 *
 *          ECB mode does not accept an initialization vector (IV). When using a multi-part cipher
 *          operation with this algorithm, @ref psa_cipher_generate_iv() and
 *          @ref psa_cipher_set_iv() must not be called.
 *
 * @note    The cipher block length can be determined using @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 *
 *          The ECB block cipher mode is defined in NIST Special Publication 800-38A:
 *          Recommendation for Block Cipher Modes of Operation: Methods and Techniques
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38A).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_ECB_NO_PADDING  ((psa_algorithm_t)0x04404400)

/**
 * @brief   The XEX with Ciphertext Stealing (XTS) cipher mode of a block cipher.
 *
 * @details XTS is a cipher mode which is built from a block cipher, designed for use in disk
 *          encryption. It requires at least one full cipher block length of input, but beyond this
 *          minimum the input does not need to be a whole number of blocks.
 *
 *          XTS mode uses two keys for the underlying block cipher. These are provided by using a
 *          key that is twice the normal key size for the cipher. For example, to use AES-256-XTS
 *          the application must create a key with type @ref PSA_KEY_TYPE_AES and bit size 512.
 *
 *          XTS mode requires an initialization vector (IV) that is the same size as the cipher
 *          block length. The IV for XTS is typically defined to be the sector number of the disk
 *          block being encrypted or decrypted.
 *
 *          The XTS block cipher mode is defined in 1619-2018 --- IEEE Standard for Cryptographic
 *          Protection of Data on Block-Oriented Storage Devices
 *          [IEEE-XTS](https://ieeexplore.ieee.org/servlet/opac?punumber=8637986).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_XTS             ((psa_algorithm_t)0x0440ff00)

/**
 * @brief   A stream cipher built using the Counter (CTR) mode of a block cipher.
 *
 * @details CTR is a stream cipher which is built from a block cipher. The underlying block cipher
 *          is determined by the key type. For example, to use AES-128-CTR, use this algorithm with
 *          a key of type @ref PSA_KEY_TYPE_AES and a size of 128 bits (16 bytes).
 *
 *          The CTR block cipher mode is defined in NIST Special Publication 800-38A:
 *          Recommendation for Block Cipher Modes of Operation: Methods and Techniques [SP800-38A].
 *
 *          CTR mode requires a counter block which is the same size as the cipher block length.
 *          The counter block is updated for each block (or a partial final block) that is
 *          encrypted or decrypted.
 *
 *          A counter block value must only be used once across all messages encrypted using the
 *          same key value. This is typically achieved by splitting the counter block into a nonce,
 *          which is unique among all message encrypted with the key, and a counter which is
 *          incremented for each block of a message.
 *
 *          For example, when using AES-CTR encryption, which uses a 16-byte block, the application
 *          can provide a 12-byte nonce when setting the IV. This leaves 4 bytes for the counter,
 *          allowing up to 2^32 blocks (64GB) of message data to be encrypted in each message.
 *
 *          The first counter block is constructed from the initialization vector (IV). The initial
 *          counter block is is constructed in the following ways:
 *          -   A call to @ref psa_cipher_encrypt() will generate a random counter block value.
 *              This is the first block of output.
 *          -   A call to @ref psa_cipher_decrypt() will use first block of the input buffer as the
 *              initial counter block value.
 *          -   A call to @ref psa_cipher_generate_iv() on a multi-part cipher operation will
 *              generate and return a random counter block value.
 *          -   A call to @ref psa_cipher_set_iv() on a multi-part cipher operation requires an IV
 *              that is between 1 and n bytes in length, where n is the cipher block length. The
 *              counter block is initialized using the IV, and padded with zero bytes up to the
 *              block length.
 *
 *          During the counter block update operation, the counter block is treated as a single
 *          big-endian encoded integer and the update operation increments this integer by 1.
 *
 *          This scheme meets the recommendations in Appendix B of
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38A).
 *
 * @note    The cipher block length can be determined using @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CTR             ((psa_algorithm_t)0x04c01000)

/**
 * @brief   A stream cipher built using the Cipher Feedback (CFB) mode of a block cipher.
 *
 * @details The underlying block cipher is determined by the key type. This is the variant of CFB
 *          where each iteration encrypts or decrypts a segment of the input that is the same
 *          length as the cipher block size. For example, using @ref PSA_ALG_CFB with a key of type
 *          @ref PSA_KEY_TYPE_AES will result in the AES-CFB-128 cipher.
 *
 *          CFB mode requires an initialization vector (IV) that is the same size as the cipher
 *          block length.
 *
 * @note    The cipher block length can be determined using @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 *
 *          The CFB block cipher mode is defined in NIST Special Publication 800-38A:
 *          Recommendation for Block Cipher Modes of Operation: Methods and Techniques
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38A), using a segment size s equal
 *          to the block size b. The definition in
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38A) is
 *          extended to allow an incomplete final block of input, in which case the algorithm
 *          discards the final bytes of the key stream when encrypting or decrypting the final
 *          partial block.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CFB             ((psa_algorithm_t)0x04c01100)

/**
 * @brief   A stream cipher built using the Output Feedback (OFB) mode of a block cipher.
 *
 * @details The underlying block cipher is determined by the key type.
 *
 *          OFB mode requires an initialization vector (IV) that is the same size as the cipher
 *          block length. OFB mode requires that the IV is a nonce, and must be unique for each use
 *          of the mode with the same key.
 *
 * @note    The cipher block length can be determined using @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 *
 *          The OFB block cipher mode is defined in NIST Special Publication 800-38A:
 *          Recommendation for Block Cipher Modes of Operation: Methods and Techniques
 *          [SP800-38A](https://doi.org/10.6028/NIST.SP.800-38A).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_OFB             ((psa_algorithm_t)0x04c01200)

/**
 * @brief   The stream cipher mode of a stream cipher algorithm.
 *
 * @details The underlying stream cipher is determined by the key type. The ARC4 and ChaCha20
 *          ciphers use this algorithm identifier.
 *
 *          @b ARC4
 *          To use ARC4, use a key type of @ref PSA_KEY_TYPE_ARC4 and algorithm id
 *          @ref PSA_ALG_STREAM_CIPHER.
 *
 * @warning The ARC4 cipher is weak and deprecated and is only recommended for use in legacy
 *          protocols.
 *
 *          The ARC4 cipher does not use an initialization vector (IV). When using a multi-part
 *          cipher operation with the @ref PSA_ALG_STREAM_CIPHER algorithm and an ARC4 key,
 *          @ref psa_cipher_generate_iv() and @ref psa_cipher_set_iv() must not be called.
 *
 *          @b ChaCha20
 *          To use ChaCha20, use a key type of @ref PSA_KEY_TYPE_CHACHA20 and algorithm id
 *          @ref PSA_ALG_STREAM_CIPHER.
 *
 *          Implementations must support the variant that is defined in ChaCha20 and Poly1305 for
 *          IETF Protocols [RFC7539](https://tools.ietf.org/html/rfc7539.html) §2.4, which has a
 *          96-bit nonce and a 32-bit counter.
 *          Implementations can optionally also support the original variant, as defined in ChaCha,
 *          a variant of Salsa20 [CHACHA20](http://cr.yp.to/chacha/chacha-20080128.pdf),
 *          which has a 64-bit nonce and a 64-bit counter. Except where noted, the
 *          [RFC7539](https://tools.ietf.org/html/rfc7539.html) variant must be used.
 *
 *          ChaCha20 defines a nonce and an initial counter to be provided to the encryption and
 *          decryption operations. When using a ChaCha20 key with the @ref PSA_ALG_STREAM_CIPHER
 *          algorithm, these values are provided using the initialization vector (IV) functions in
 *          the following ways:
 *          -   A call to @ref psa_cipher_encrypt() will generate a random 12-byte nonce, and set
 *              the counter value to zero. The random nonce is output as a 12-byte IV value in the
 *              output.
 *          -   A call to @ref psa_cipher_decrypt() will use first 12 bytes of the input buffer as
 *              the nonce and set the counter value to zero.
 *          -   A call to @ref psa_cipher_generate_iv() on a multi-part cipher operation will
 *              generate and return a random 12-byte nonce and set the counter value to zero.
 *          -   A call to @ref psa_cipher_set_iv() on a multi-part cipher operation can support
 *              the following IV sizes:
 *              -   12 bytes: the provided IV is used as the nonce, and the counter value is
 *                  set to zero.
 *               -  16 bytes: the first four bytes of the IV are used as the counter value (encoded
 *                  as little-endian), and the remaining 12 bytes is used as the nonce.
 *               -  8 bytes: the cipher operation uses the original
 *                  [CHACHA20](http://cr.yp.to/chacha/chacha-20080128.pdf) definition of
 *                  ChaCha20: the provided IV is used as the 64-bit nonce, and the 64-bit counter
 *                  value is set to zero.
 *               -  It is recommended that implementations do not support other sizes of IV.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ARC4
 *          - @ref PSA_KEY_TYPE_CHACHA20
 */
#define PSA_ALG_STREAM_CIPHER   ((psa_algorithm_t)0x04800100)

/**
 * @brief   The ChaCha20-Poly1305 AEAD algorithm.
 *
 * @details There are two defined variants of ChaCha20-Poly1305:
 *          -   An implementation that supports ChaCha20-Poly1305 must support the variant defined
 *              by ChaCha20 and Poly1305 for IETF Protocols
 *              [RFC7539](https://tools.ietf.org/html/rfc7539.html), which has a 96-bit nonce and
 *              32-bit counter.
 *          -   An implementation can optionally also support the original variant defined by
 *              ChaCha, a variant of Salsa20
 *              [CHACHA20](http://cr.yp.to/chacha/chacha-20080128.pdf), which has a 64-bit nonce
 *              and 64-bit counter.
 *
 *          The variant used for the AEAD encryption or decryption operation, depends on the nonce
 *          provided for an AEAD operation using @ref PSA_ALG_CHACHA20_POLY1305 :
 *          -   A nonce provided in a call to @ref psa_aead_encrypt(), @ref psa_aead_decrypt() or
 *              @ref psa_aead_set_nonce() must be 8 or 12 bytes. The size of nonce will select the
 *              appropriate variant of the algorithm.
 *          -   A nonce generated by a call to @ref psa_aead_generate_nonce() will be 12 bytes, and
 *              will use the [RFC7539](https://tools.ietf.org/html/rfc7539.html) variant.
 *
 *          Implementations must support 16-byte tags. It is recommended that truncated tag sizes
 *          are rejected.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_CHACHA20
 */
#define PSA_ALG_CHACHA20_POLY1305   ((psa_algorithm_t)0x05100500)

/**
 * @brief   The Counter with CBC-MAC (CCM) authenticated encryption algorithm.
 *
 * @details CCM is defined for block ciphers that have a 128-bit block size. The underlying block
 *          cipher is determined by the key type.
 *
 *          To use @ref PSA_ALG_CCM with a multi-part AEAD operation, the application must call
 *          @ref psa_aead_set_lengths() before providing the nonce, the additional data and
 *          plaintext to the operation.
 *
 *          CCM requires a nonce of between 7 and 13 bytes in length. The length of the nonce
 *          affects the `m` maximum length of the plaintext than can be encrypted or decrypted. If
 *          the nonce has length `N`, then the plaintext length `pLen` is encoded in `L = 15 - N`
 *          octets, this requires that `pLen < 28L`.
 *
 *          The value for L that is used with PSA_ALG_CCM depends on the function used to provide
 *          the nonce:
 *           -  A call to @ref psa_aead_encrypt(), @ref psa_aead_decrypt(), or @ref
 *              psa_aead_set_nonce() will set `L` to 15 - nonce_length. If the plaintext length
 *              cannot be encoded in `L` octets, then a @ref PSA_ERROR_INVALID_ARGUMENT error is
 *              returned.
 *           -  A call to @ref psa_aead_generate_nonce() on a multi-part cipher operation will
 *              select `L` as the smallest integer `>= 2` where `pLen < 28L`, with `pLen` being the
 *              plaintext_length provided to @ref psa_aead_set_lengths(). The call to @ref
 *              psa_aead_generate_nonce() will generate and return a random nonce of length
 *              `15 - L` bytes.
 *
 *          CCM supports authentication tag sizes of 4, 6, 8, 10, 12, 14, and 16 bytes. The default
 *          tag length is 16. Shortened tag lengths can be requested using
 *          @ref PSA_ALG_AEAD_WITH_SHORTENED_TAG(@ref PSA_ALG_CCM, @p tag_length),
 *          where @c tag_length is a valid CCM tag length.
 *
 *          The CCM block cipher mode is defined in Counter with CBC-MAC (CCM)
 *          [RFC3610](https://tools.ietf.org/html/rfc3610).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CCM                 ((psa_algorithm_t)0x05500100)

/**
 * @brief   The Galois/Counter Mode (GCM) authenticated encryption algorithm.
 *
 * @details GCM is defined for block ciphers that have a 128-bit block size. The underlying block
 *          cipher is determined by the key type.
 *
 *          GCM requires a nonce of at least 1 byte in length. The maximum supported nonce size is
 *          implementation defined. Calling @ref psa_aead_generate_nonce() will generate a random
 *          12-byte nonce.
 *
 *          GCM supports authentication tag sizes of 4, 8, 12, 13, 14, 15, and 16 bytes. The
 *          default tag length is 16. Shortened tag lengths can be requested using
 *          @ref PSA_ALG_AEAD_WITH_SHORTENED_TAG(@ref PSA_ALG_GCM, @p tag_length), where
 *          @c tag_length is a valid GCM tag length.
 *
 *          The GCM block cipher mode is defined in NIST Special Publication 800-38D:
 *          Recommendation for Block Cipher Modes of Operation: Galois/Counter Mode (GCM) and GMAC
 *          [SP800-38D](https://doi.org/10.6028/NIST.SP.800-38D).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_GCM ((psa_algorithm_t)0x05500200)

/**
 * @brief   The PBKDF2-AES-CMAC-PRF-128 password-hashing or key-stretching algorithm.
 *
 * @details PBKDF2 is specified by PKCS #5: Password-Based Cryptography Specification Version 2.1
 *          [RFC8018](https://tools.ietf.org/html/rfc8018.html) §5.2. This algorithm specifies the
 *          PBKDF2 algorithm using the AES-CMAC-PRF-128 pseudo-random function specified by
 *          [RFC4615](https://tools.ietf.org/html/rfc4615.html).
 *
 *          This key derivation algorithm uses the same inputs as @ref PSA_ALG_PBKDF2_HMAC() with
 *          the same constraints.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for password input)
 *          - @ref PSA_KEY_TYPE_PASSWORD (for password input)
 *          - @ref PSA_KEY_TYPE_PEPPER (for salt input)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for salt input)
 *          - @ref PSA_KEY_TYPE_PASSWORD_HASH (for key verification)
 */
#define PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ((psa_algorithm_t)0x08800200)

/**
 * @brief   Macro to build a PBKDF2-HMAC password-hashing or key-stretching algorithm.
 *
 * @details PBKDF2 is specified by PKCS #5: Password-Based Cryptography Specification Version 2.1
 *          [RFC8018](https://tools.ietf.org/html/rfc8018.html) §5.2. This macro constructs a
 *          PBKDF2 algorithm that uses a pseudo-random function based on HMAC with the specified
 *          hash.
 *
 *          This key derivation algorithm uses the following inputs, which must be provided in the
 *          following order:
 *          -   @ref PSA_KEY_DERIVATION_INPUT_COST is the iteration count. This input step must be
 *              used exactly once.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SALT is the salt. This input step must be used one or
 *              more times; if used several times, the inputs will be concatenated. This can be
 *              used to build the final salt from multiple sources, both public and secret (also
 *              known as pepper).
 *          -   @ref PSA_KEY_DERIVATION_INPUT_PASSWORD is the password to be hashed. This input
 *              step must be used exactly once.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for password input)
 *          - @ref PSA_KEY_TYPE_PASSWORD (for password input)
 *          - @ref PSA_KEY_TYPE_PEPPER (for salt input)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for salt input)
 *          - @ref PSA_KEY_TYPE_PASSWORD_HASH (for key verification)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding `PBKDF2-HMAC-XXX` algorithm. For example,
 *          @ref PSA_ALG_PBKDF2_HMAC(@ref PSA_ALG_SHA_256) is the algorithm identifier for
 *          `PBKDF2-HMAC-SHA-256`.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_PBKDF2_HMAC(hash_alg) \
    ((psa_algorithm_t)(0x08800100 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   The raw RSA PKCS#1 v1.5 signature algorithm, without hashing.
 *
 * @details This algorithm can be only used with the @ref psa_sign_hash() and @ref psa_verify_hash()
 *          functions.
 *          This signature scheme is defined by PKCS #1: RSA Cryptography Specifications Version 2.2
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §8.2 under the name
 *          RSASSA-PKCS1-v1_5.
 *
 *          The hash parameter to @ref psa_sign_hash() or @ref psa_verify_hash() is used as `T`
 *          from step 3 onwards in the message encoding algorithm EMSA-PKCS1-V1_5-ENCODE() in
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §9.2.0 `T` is the DER encoding of
 *          the DigestInfo structure normally produced by step 2 in the message encoding algorithm.
 *
 *          The wildcard key policy @ref PSA_ALG_RSA_PKCS1V15_SIGN(@ref PSA_ALG_ANY_HASH) also
 *          permits a key to be used with the @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW signature
 *          algorithm.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (signature verification only)
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW   ((psa_algorithm_t)0x06000200)

/**
 * @brief   Base of ECDSA algorithms
 */
#define PSA_ALG_ECDSA_BASE      ((psa_algorithm_t)0x06000600)

/**
 * @brief   Edwards-curve digital signature algorithm without prehashing (PureEdDSA),
 *          using standard parameters.
 *
 * @details This algorithm can be only used with the @ref psa_sign_message() and
 *          @ref psa_verify_message() functions.
 *
 *          This is the PureEdDSA digital signature algorithm defined by Edwards-Curve Digital
 *          Signature Algorithm (EdDSA) [RFC8032](https://tools.ietf.org/html/rfc8032.html),
 *          using standard parameters.
 *
 *          PureEdDSA requires an elliptic curve key on a twisted Edwards curve. The following
 *          curves are supported:
 *          -   Edwards25519: the Ed25519 algorithm is computed. The output signature is a 64-byte
 *              string: the concatenation of R and S as defined by
 *              [RFC8032](https://tools.ietf.org/html/rfc8032.html) §5.1.6.
 *          -   Edwards448: the Ed448 algorithm is computed with an empty string as the context.
 *              The output signature is a 114-byte string: the concatenation of R and S as defined
 *              by [RFC8032](https://tools.ietf.org/html/rfc8032.html) §5.2.6.
 *
 * @note    Contexts are not supported in the current version of this specification because there
 *          is no suitable signature interface that can take the context as a parameter. A future
 *          version of this specification may add suitable functions and extend this algorithm to
 *          support contexts.
 *
 * @note    To sign or verify the pre-computed hash of a message using EdDSA, the HashEdDSA
 *          algorithms (@ref PSA_ALG_ED25519PH and @ref PSA_ALG_ED448PH) can be used with
 *          @ref psa_sign_hash() and @ref psa_verify_hash().
 *          The signature produced by HashEdDSA is distinct from that produced by PureEdDSA.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS)
 *          - @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS) (signature
 *            verification only)
 */
#define PSA_ALG_PURE_EDDSA ((psa_algorithm_t)0x06000800)

/**
 * @brief   The randomized ECDSA signature scheme, with hashing.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This algorithm is randomized: each invocation returns a different, equally valid
 *          signature.
 *
 * @note    When based on the same hash algorithm, the verification operations for
 *          @ref PSA_ALG_ECDSA and @ref PSA_ALG_DETERMINISTIC_ECDSA are identical.
 *          A signature created using @ref PSA_ALG_ECDSA can be verified with the same key using
 *          either @ref PSA_ALG_ECDSA or @ref PSA_ALG_DETERMINISTIC_ECDSA.
 *          Similarly, a signature created using @ref PSA_ALG_DETERMINISTIC_ECDSA can be verified
 *          with the same key using either @ref PSA_ALG_ECDSA or @ref PSA_ALG_DETERMINISTIC_ECDSA.
 *
 *          In particular, it is impossible to determine whether a signature was produced with
 *          deterministic ECDSA or with randomized ECDSA: it is only possible to verify that a
 *          signature was made with ECDSA with the private key corresponding to the public key used
 *          for the verification.
 *
 *          This signature scheme is defined by SEC 1: Elliptic Curve Cryptography
 *          [SEC1](https://www.secg.org/sec1-v2.pdf), and also  by Public Key Cryptography For The
 *          Financial Services Industry: The Elliptic Curve  Digital Signature Algorithm (ECDSA)
 *          [X9-62])(https://standards.globalspec.com/std/1955141/ANSI%20X9.62), with a random
 *          per-message secret number `k`.
 *
 *          The representation of the signature as a byte string consists of the concatenation of
 *          the signature values `r` and `s`. Each of `r` and `s` is encoded as an `N`-octet
 *          string, where `N` is the length of the base point of the curve in octets. Each value is
 *          represented in big-endian order, with the most significant octet first.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(family)
 *          - @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(family) (signature verification only)
 *
 *          where family is a Weierstrass Elliptic curve family. That is, one of the following
 *          values:
 *          - @c PSA_ECC_FAMILY_SECT_XX
 *          - @c PSA_ECC_FAMILY_SECP_XX
 *          - @ref PSA_ECC_FAMILY_FRP
 *          - @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true. This includes
 *                      @ref PSA_ALG_ANY_HASH when specifying the algorithm in a key policy.
 *
 * @return  The corresponding randomized ECDSA signature algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_ECDSA(hash_alg)                                 \
    (PSA_ALG_ECDSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @brief   The randomized ECDSA signature scheme, without hashing.
 *
 * @details This algorithm can be only used with the @ref psa_sign_hash() and @ref psa_verify_hash()
 *          functions.
 *
 *          This algorithm is randomized: each invocation returns a different, equally valid
 *          signature.
 *
 *          This is the same signature scheme as @ref PSA_ALG_ECDSA(), but without specifying a hash
 *          algorithm, and skipping the message hashing operation.
 *
 *          This algorithm is only recommended to sign or verify a sequence of bytes that are an
 *          already-calculated hash. Note that the input is padded with zeros on the left or
 *          truncated on the right as required to fit the curve size.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(@p family)
 *          - @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(@p family) (signature verification only)
 *
 *          where @c family is a Weierstrass Elliptic curve family. That is, one of the following
 *          values:
 *          - @c PSA_ECC_FAMILY_SECT_XX
 *          - @c PSA_ECC_FAMILY_SECP_XX
 *          - @ref PSA_ECC_FAMILY_FRP
 *          - @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1
 */
#define PSA_ALG_ECDSA_ANY       PSA_ALG_ECDSA_BASE

/**
 * @brief   Edwards-curve digital signature algorithm with prehashing (HashEdDSA), using the
 *          Edwards25519 curve.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This computes the Ed25519ph algorithm as specified in Edwards-Curve Digital Signature
 *          Algorithm (EdDSA) [RFC8032](https://tools.ietf.org/html/rfc8032.html) §5.1, and
 *          requires an Edwards25519 curve key. An empty string is used as the context. The prehash
 *          function is SHA-512.
 *
 *          @b Usage
 *          This is a hash-and-sign algorithm. To calculate a signature, use one of the following
 *          approaches:
 *          -   Call @ref psa_sign_message() with the message.
 *          -   Calculate the SHA-512 hash of the message with @ref psa_hash_compute(), or with a
 *              multi-part hash operation, using the hash algorithm @ref PSA_ALG_SHA_512. Then sign
 *              the calculated hash with @ref psa_sign_hash().
 *
 *          Verifying a signature is similar, using @ref psa_verify_message() or
 *          @ref psa_verify_hash() instead of the signature function.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(@ref PSA_ECC_FAMILY_TWISTED_EDWARDS)
 *          - @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(@ref PSA_ECC_FAMILY_TWISTED_EDWARDS) (signature
 *            verification only)
 */
#define PSA_ALG_ED25519PH ((psa_algorithm_t)0x0600090B)

/**
 * @brief   Edwards-curve digital signature algorithm with prehashing (HashEdDSA), using the
 *          Edwards448 curve.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This computes the Ed448ph algorithm as specified in Edwards-Curve Digital Signature
 *          Algorithm (EdDSA) [RFC8032](https://tools.ietf.org/html/rfc8032.html) §5.2, and
 *          requires an Edwards448 curve key. An empty string is used as the context. The prehash
 *          function is the first 64 bytes of the output from SHAKE256.
 *
 *          @b Usage
 *          This is a hash-and-sign algorithm. To calculate a signature, use one of the following
 *          approaches:
 *          -   Call @ref psa_sign_message() with the message.
 *          -   Calculate the first 64 bytes of the SHAKE256 output of the message with
 *              @ref psa_hash_compute(), or with a multi-part hash operation, using the hash
 *              algorithm @ref PSA_ALG_SHAKE256_512. Then sign the calculated hash with
 *              @ref psa_sign_hash().
 *
 *          Verifying a signature is similar, using @ref psa_verify_message() or
 *          @ref psa_verify_hash() instead of the signature function.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(@ref PSA_ECC_FAMILY_TWISTED_EDWARDS)
 *          - @ref PSA_KEY_TYPE_ECC_PUBLIC_KEY(@ref PSA_ECC_FAMILY_TWISTED_EDWARDS) (signature
 *            verification only)
 */
#define PSA_ALG_ED448PH ((psa_algorithm_t)0x06000915)

/**
 * @brief   The RSA PKCS#1 v1.5 asymmetric encryption algorithm.
 *
 * @details This encryption scheme is defined by PKCS #1: RSA Cryptography Specifications
 *          Version 2.2 [RFC8017](https://tools.ietf.org/html/rfc8017.html) §7.2 under the
 *          name RSAES-PKCS-v1_5.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (asymmetric encryption only)
 */
#define PSA_ALG_RSA_PKCS1V15_CRYPT ((psa_algorithm_t)0x07000200)

/**
 * @brief   The finite-field Diffie-Hellman (DH) key agreement algorithm.
 *
 * @details This algorithm can be used directly in a call to @ref psa_raw_key_agreement(),
 *          or combined with a key derivation operation using @ref PSA_ALG_KEY_AGREEMENT() for use
 *          with @ref psa_key_derivation_key_agreement().
 *
 *          When used as a key’s permitted algorithm policy, the following uses are permitted:
 *          -   In a call to @ref psa_raw_key_agreement(), with algorithm @ref PSA_ALG_FFDH.
 *          -   In a call to @ref psa_key_derivation_key_agreement(), with any combined key
 *              agreement and key derivation algorithm constructed with @ref PSA_ALG_FFDH.
 *
 *          When used as part of a multi-part key derivation operation, this implements a
 *          Diffie-Hellman key agreement scheme using a single Diffie-Hellman key-pair for each
 *          participant. This includes the `dhEphem`, `dhOneFlow`, and `dhStatic` schemes.
 *          The input step @ref PSA_KEY_DERIVATION_INPUT_SECRET is used when providing the secret
 *          and peer keys to the operation.
 *
 *          The shared secret produced by this key agreement algorithm is `g^{ab}` in big-endian
 *          format. It is `ceiling(m / 8)` bytes long where `m` is the size of the prime `p` in
 *          bits.
 *
 *          This key agreement scheme is defined by NIST Special Publication 800-56A:
 *          Recommendation for Pair-Wise Key-Establishment Schemes Using Discrete Logarithm
 *          Cryptography [SP800-56A](https://doi.org/10.6028/NIST.SP.800-56Ar3) §5.7.1.1
 *          under the name FFC DH.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DH_KEY_PAIR()
 */
#define PSA_ALG_FFDH ((psa_algorithm_t)0x09010000)

/**
 * @brief   The elliptic curve Diffie-Hellman (ECDH) key agreement algorithm.
 *
 * @details This algorithm can be used directly in a call to @ref psa_raw_key_agreement(), or
 *          combined with a key derivation operation using @ref PSA_ALG_KEY_AGREEMENT() for use
 *          with @ref psa_key_derivation_key_agreement().
 *
 *          When used as a key’s permitted algorithm policy, the following uses are permitted:
 *          -   In a call to @ref psa_raw_key_agreement(), with algorithm @ref PSA_ALG_ECDH.
 *          -   In a call to @ref psa_key_derivation_key_agreement(), with any combined key
 *              agreement and key derivation algorithm constructed with @ref PSA_ALG_ECDH.
 *
 *          When used as part of a multi-part key derivation operation, this implements a
 *          Diffie-Hellman key agreement scheme using a single elliptic curve key-pair for each
 *          participant. This includes the Ephemeral unified model, the Static unified model, and
 *          the One-pass Diffie-Hellman schemes. The input step
 *          @ref PSA_KEY_DERIVATION_INPUT_SECRET is used when providing the secret and peer keys
 *          to the operation.
 *
 *          The shared secret produced by key agreement is the x-coordinate of the shared secret
 *          point. It is always `ceiling(m / 8)` bytes long where `m` is the bit size associated
 *          with the curve, i.e. the bit size of the order of the curve’s coordinate field. When
 *          `m` is not a multiple of 8, the byte containing the most significant bit of the shared
 *          secret is padded with zero bits. The byte order is either little-endian or big-endian
 *          depending on the curve type.
 *
 *          -   For Montgomery curves (curve family @ref PSA_ECC_FAMILY_MONTGOMERY), the shared
 *              secret is the x-coordinate of `Z = d_A Q_B = d_B Q_A` in little-endian byte order.
 *              -   For Curve25519, this is the X25519 function defined in Curve25519: new
 *                  Diffie-Hellman speed records
 *                  [Curve25519](https://www.iacr.org/archive/pkc2006/39580209/39580209.pdf).
 *                  The bit size `m` is 255.
 *              -   For Curve448, this is the X448 function defined in Ed448-Goldilocks, a new
 *                  elliptic curve [Curve448](https://eprint.iacr.org/2015/625.pdf).
 *                  The bit size `m` is 448.
 *          -   For Weierstrass curves (curve families @c PSA_ECC_FAMILY_SECP_XX,
 *              @c PSA_ECC_FAMILY_SECT_XX, @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1 and
 *              @ref PSA_ECC_FAMILY_FRP) the shared secret is the x-coordinate of
 *              `Z = h d_A Q_B = h d_B Q_A` in big-endian byte order. This is the Elliptic Curve
 *              Cryptography Cofactor Diffie-Hellman primitive defined by SEC 1: Elliptic Curve
 *              Cryptography [SEC1](https://www.secg.org/sec1-v2.pdf) §3.3.2 as, and also as ECC
 *              CDH by NIST Special Publication 800-56A: Recommendation for Pair-Wise
 *              Key-Establishment Schemes Using Discrete Logarithm Cryptography
 *              [SP800-56A](https://doi.org/10.6028/NIST.SP.800-56Ar3) §5.7.1.2.
 *              -   Over prime fields (curve families @c PSA_ECC_FAMILY_SECP_XX,
 *                  @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1 and @ref PSA_ECC_FAMILY_FRP),
 *                  the bit size is `m = ceiling(log_2(p))` for the field `F_p`.
 *              -   Over binary fields (curve families @c PSA_ECC_FAMILY_SECT_XX), the bit size
 *                  is `m` for the field `F_{2^m}`.
 *
 * @note    The cofactor Diffie-Hellman primitive is equivalent to the standard elliptic curve
 *          Diffie-Hellman calculation `Z = d_A Q_B = d_B Q_A`
 *          ([SEC1](https://www.secg.org/sec1-v2.pdf) §3.3.1) for curves where the cofactor `h`
 *          is 1. This is true for all curves in the @c PSA_ECC_FAMILY_SECP_XX,
 *          @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1, and @ref PSA_ECC_FAMILY_FRP families.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_ECC_KEY_PAIR(@p family)
 *
 *          where @p family is a Weierstrass or Montgomery Elliptic curve family. That is, one of
 *          the following values:
 *          - @c PSA_ECC_FAMILY_SECT_XX
 *          - @c PSA_ECC_FAMILY_SECP_XX
 *          - @ref PSA_ECC_FAMILY_FRP
 *          - @ref PSA_ECC_FAMILY_BRAINPOOL_P_R1
 *          - @ref PSA_ECC_FAMILY_MONTGOMERY
 */
#define PSA_ALG_ECDH ((psa_algorithm_t)0x09020000)

/**
 * @brief   The RSA OAEP asymmetric encryption algorithm.
 *
 * @details This encryption scheme is defined by
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §7.1 under the name RSAES-OAEP,
 *          with the following options:
 *          -   The mask generation function MGF1 defined in
 *              [RFC8017](https://tools.ietf.org/html/rfc8017.html) Appendix B.2.1.
 *          -   The specified hash algorithm is used to hash the label, and for the mask generation
 *              function.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (asymmetric encryption only)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true. The hash algorithm is used for
 *                      MGF1.
 *
 * @return  The corresponding RSA OAEP encryption algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_RSA_OAEP(hash_alg) \
    ((psa_algorithm_t)(0x07000300 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   The RSA PKCS#1 v1.5 message signature scheme, with hashing.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This signature scheme is defined by PKCS #1: RSA Cryptography Specifications Version 2.2
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §8.2 under the name
 *          RSASSA-PKCS1-v1_5.
 *
 *          When used with @ref psa_sign_hash() or @ref psa_verify_hash(), the provided hash
 *          parameter is used as `H` from step 2 onwards in the message encoding algorithm
 *          EMSA-PKCS1-V1_5-ENCODE() in [RFC8017](https://tools.ietf.org/html/rfc8017.html) §9.2.
 *          `H` is usually the message digest, using the @c hash_alg hash algorithm.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (signature verification only)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true. This includes
 *                      @ref PSA_ALG_ANY_HASH when specifying the algorithm in a key policy.
 *
 * @return  The corresponding RSA PKCS#1 v1.5 signature algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN(hash_alg) \
    ((psa_algorithm_t)(0x06000200 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   The RSA PSS message signature scheme, with hashing.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This algorithm is randomized: each invocation returns a different, equally valid
 *          signature.
 *
 *          This is the signature scheme defined by
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §8.1 under the name RSASSA-PSS,
 *          with the following options:
 *          -   The mask generation function is MGF1 defined by
 *              [RFC8017](https://tools.ietf.org/html/rfc8017.html) Appendix B.
 *          -   When creating a signature, the salt length is equal to the length of the hash, or
 *              the largest possible salt length for the algorithm and key size if that is smaller
 *              than the hash length.
 *          -   When verifying a signature, the salt length must be equal to the length of the
 *              hash, or the largest possible salt length for the algorithm and key size if that is
 *              smaller than the hash length.
 *          -   The specified hash algorithm is used to hash the input message, to create the
 *              salted hash, and for the mask generation.
 *
 * @note    The @ref PSA_ALG_RSA_PSS_ANY_SALT() algorithm is equivalent to @ref PSA_ALG_RSA_PSS()
 *          when creating a signature, but permits any salt length when verifying a signature.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (signature verification only)
 */
#define PSA_ALG_RSA_PSS(hash_alg) \
    ((psa_algorithm_t)(0x06000300 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   The RSA PSS message signature scheme, with hashing. This variant permits any salt length
 *          for signature verification.
 *
 * @details This algorithm can be used with both the message and hash signature functions.
 *
 *          This algorithm is randomized: each invocation returns a different, equally valid
 *          signature.
 *
 *          This is the signature scheme defined by
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §8.1 under the name RSASSA-PSS,
 *          with the following options:
 *          -   The mask generation function is MGF1 defined by
 *              [RFC8017](https://tools.ietf.org/html/rfc8017.html) Appendix B.
 *          -   When creating a signature, the salt length is equal to the length of the hash, or
 *              the largest possible salt length for the algorithm and key size if that is smaller
 *              than the hash length.
 *          -   When verifying a signature, any salt length permitted by the RSASSA-PSS signature
 *              algorithm is accepted.
 *          -   The specified hash algorithm is used to hash the input message, to create the
 *              salted hash, and for the mask generation.
 *
 * @note    The @ref PSA_ALG_RSA_PSS() algorithm is equivalent to @ref PSA_ALG_RSA_PSS_ANY_SALT()
 *          when creating a signature, but is strict about the permitted salt length when verifying
 *          a signature.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (signature verification only)
 */
#define PSA_ALG_RSA_PSS_ANY_SALT(hash_alg) \
    ((psa_algorithm_t)(0x06001300 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build a TLS-1.2 PRF algorithm.
 *
 * @details TLS 1.2 uses a custom pseudorandom function (PRF) for key schedule, specified in
 *          The Transport Layer Security (TLS) Protocol Version 1.2
 *          [RFC5246](https://tools.ietf.org/html/rfc5246.html) §5. It is based on
 *          HMAC and can be used with either SHA-256 or SHA-384.
 *
 *          This key derivation algorithm uses the following inputs, which must be passed in the
 *          order given here:
 *          - @ref PSA_KEY_DERIVATION_INPUT_SEED is the seed.
 *          - @ref PSA_KEY_DERIVATION_INPUT_SECRET is the secret key.
 *          - @ref PSA_KEY_DERIVATION_INPUT_LABEL is the label.
 *
 *          Each input may only be passed once.
 *
 *          For the application to TLS-1.2 key expansion:
 *          - The seed is the concatenation of @c ServerHello.Random + @c ClientHello.Random.
 *          - The label is `key expansion`.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the secret key)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the other inputs)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding TLS-1.2 PRF algorithm. For example,
 *          @ref PSA_ALG_TLS12_PRF(@p PSA_ALG_SHA_256) represents the TLS 1.2 PRF using
 *          HMAC-SHA-256.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_TLS12_PRF(hash_alg) \
    ((psa_algorithm_t)(0x08000200 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build a TLS-1.2 PSK-to-MasterSecret algorithm.
 *
 * @details In a pure-PSK handshake in TLS 1.2, the master secret (MS) is derived from the
 *          pre-shared key (PSK) through the application of padding (Pre-Shared Key Ciphersuites
 *          for Transport Layer Security (TLS) [RFC4279](https://tools.ietf.org/html/rfc4279.html)
 *          §2) and the TLS-1.2 PRF (The Transport Layer Security (TLS) Protocol Version 1.2
 *          [RFC5246](https://tools.ietf.org/html/rfc5246.html) §5). The latter is based on HMAC
 *          and can be used with either SHA-256 or SHA-384.
 *
 *          This key derivation algorithm uses the following inputs, which must be passed in the
 *          order given here:
 *          - @ref PSA_KEY_DERIVATION_INPUT_SEED is the seed.
 *          - @ref PSA_KEY_DERIVATION_INPUT_SECRET is the PSK. The PSK must not be larger than
 *            @ref PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE.
 *          - @ref PSA_KEY_DERIVATION_INPUT_LABEL is the label.
 *
 *          Each input may only be passed once.
 *
 *          For the application to TLS-1.2:
 *          -   The seed, which is forwarded to the TLS-1.2 PRF, is the concatenation of the
 *              @c ClientHello.Random + @c ServerHello.Random.
 *          -   The label is "master secret" or "extended master secret".
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the PSK)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the other inputs)
 */
#define PSA_ALG_TLS12_PSK_TO_MS(hash_alg) \
    ((psa_algorithm_t)(0x08000300 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build a truncated MAC algorithm.
 *
 * @details A truncated MAC algorithm is identical to the corresponding MAC algorithm except that
 *          the MAC value for the truncated algorithm consists of only the first mac_length bytes
 *          of the MAC value for the untruncated algorithm.
 *
 * @note    This macro might allow constructing algorithm identifiers that are not valid, either
 *          because the specified length is larger than the untruncated MAC or because the
 *          specified length is smaller than permitted by the implementation.
 *
 * @note    It is implementation-defined whether a truncated MAC that is truncated to the same
 *          length as the MAC of the untruncated algorithm is considered identical to the
 *          untruncated algorithm for policy comparison purposes.
 *
 *          The untruncated MAC algorithm can be recovered using @ref PSA_ALG_FULL_LENGTH_MAC().
 *
 *          @b Compatible @b key @b types
 *          The resulting truncated MAC algorithm is compatible with the same key types as the MAC
 *          algorithm used to construct it.
 *
 * @param   mac_alg     A MAC algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_MAC(@p mac_alg) is true. This can be a truncated or
 *                      untruncated MAC algorithm.
 * @param   mac_length  Desired length of the truncated MAC in bytes. This must be at most the
 *                      untruncated length of the MAC and must be at least an
 *                      implementation-specified minimum. The implementation-specified minimum must
 *                      not be zero.
 *
 * @return  The corresponding MAC algorithm with the specified length.
 *          Unspecified if @c mac_alg is not a supported MAC algorithm or if @c mac_length is too
 *          small or too large for the specified MAC algorithm.
 */
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length) \
    ((psa_algorithm_t)(((mac_alg) & ~0x003f0000) | (((mac_length) & 0x3f) << 16)))

/**
 * @brief   Finite-field Diffie-Hellman groups defined for TLS in RFC 7919.
 *
 * @details This family includes groups with the following key sizes (in bits): 2048, 3072, 4096,
 *          6144, 8192. An implementation can support all of these sizes or only a subset.
 *
 *          Keys is this group can only be used with the @ref PSA_ALG_FFDH key agreement algorithm.
 *
 *          These groups are defined by Negotiated Finite Field Diffie-Hellman Ephemeral Parameters
 *          for Transport Layer Security (TLS) [RFC7919](https://tools.ietf.org/html/rfc7919.html)
 *          Appendix A.
 */
#define PSA_DH_FAMILY_RFC7919 ((psa_dh_family_t)0x03)

/**
 * @brief   Brainpool `P` random curves.
 *
 * @details This family comprises the following curves:
 *          - `brainpoolP160r1` : @c key_bits = 160 (Deprecated)
 *          - `brainpoolP192r1` : @c key_bits = 192
 *          - `brainpoolP224r1` : @c key_bits = 224
 *          - `brainpoolP256r1` : @c key_bits = 256
 *          - `brainpoolP320r1` : @c key_bits = 320
 *          - `brainpoolP384r1` : @c key_bits = 384
 *          - `brainpoolP512r1` : @c key_bits = 512
 *
 *          They are defined in Elliptic Curve Cryptography (ECC) Brainpool Standard Curves and
 *          Curve Generation [RFC5639](https://tools.ietf.org/html/rfc5639.html).
 *
 * @warning The 160-bit curve `brainpoolP160r1` is weak and deprecated and is only recommended for
 *          use in legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1 ((psa_ecc_family_t)0x30)

/**
 * @brief   Curve used primarily in France and elsewhere in Europe.
 *
 * @details This family comprises one 256-bit curve:
 *          - `FRP256v1` : @c key_bits = 256
 *
 *          This is defined by Publication d'un paramétrage de courbe elliptique visant des
 *          applications de passeport électronique et de l'administration électronique française
 *          [FRP](https://www.ssi.gouv.fr/agence/rayonnement-scientifique/
 *          publications-scientifiques/articles-ouvrages-actes).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_FRP ((psa_ecc_family_t)0x33)

/**
 * @brief   Montgomery curves.
 *
 * @details This family comprises the following Montgomery curves:
 *          - `Curve25519` : @c key_bits = 255
 *          - `Curve448` : @c key_bits = 448
 *
 *          `Curve25519` is defined in Curve25519: new Diffie-Hellman speed records
 *          [Curve25519](https://www.iacr.org/archive/pkc2006/39580209/39580209.pdf).
 *          `Curve448` is defined in Ed448-Goldilocks, a new elliptic curve
 *          [Curve448](https://eprint.iacr.org/2015/625.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_MONTGOMERY ((psa_ecc_family_t)0x41)

/**
 * @brief   SEC Koblitz curves over prime fields.
 *
 * @details This family comprises the following curves:
 *          - `secp192k1` : @c key_bits = 192
 *          - `secp224k1` : @c key_bits = 225
 *          - `secp256k1` : @c key_bits = 256
 *
 *          They are defined in SEC 2: Recommended Elliptic Curve Domain Parameters
 *          [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECP_K1 ((psa_ecc_family_t)0x17)

/**
 * @brief   SEC random curves over prime fields.
 *
 * @details This family comprises the following curves:
 *          - `secp192r1` : @c key_bits = 192
 *          - `secp224r1` : @c key_bits = 224
 *          - `secp256r1` : @c key_bits = 256
 *          - `secp384r1` : @c key_bits = 384
 *          - `secp521r1` : @c key_bits = 521
 *
 *          They are defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECP_R1 ((psa_ecc_family_t)0x12)

/**
 * @warning This family of curves is weak and deprecated.
 *
 * @details This family comprises the following curves:
 *          - `secp160r2` : @c key_bits = 160 (Deprecated)
 *
 *          It is defined in the superseded SEC 2: Recommended Elliptic Curve Domain Parameters,
 *          Version 1.0 [SEC2v1](https://www.secg.org/SEC2-Ver-1.0.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECP_R2 ((psa_ecc_family_t)0x1b)

/**
 * @brief   SEC Koblitz curves over binary fields.
 *
 * @details This family comprises the following curves:
 *          - `sect163k1` : @c key_bits = 163 (Deprecated)
 *          - `sect233k1` : @c key_bits = 233
 *          - `sect239k1` : @c key_bits = 239
 *          - `sect283k1` : @c key_bits = 283
 *          - `sect409k1` : @c key_bits = 409
 *          - `sect571k1` : @c key_bits = 571
 *
 *          They are defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 * @warning The 163-bit curve `sect163k1` is weak and deprecated and is only recommended for use in
 *          legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECT_K1 ((psa_ecc_family_t)0x27)

/**
 * @brief   SEC random curves over binary fields.
 *
 * @details This family comprises the following curves:
 *          - `sect163r1` : @c key_bits = 163 (Deprecated)
 *          - `sect233r1` : @c key_bits = 233
 *          - `sect283r1` : @c key_bits = 283
 *          - `sect409r1` : @c key_bits = 409
 *          - `sect571r1` : @c key_bits = 571
 *
 *          They are defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 * @warning The 163-bit curve `sect163r1` is weak and deprecated and is only recommended for use in
 *          legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECT_R1 ((psa_ecc_family_t)0x22)

/**
 * @brief   SEC additional random curves over binary fields.
 *
 * @details This family comprises the following curves:
 *          - `sect163r2` : @c key_bits = 163 (Deprecated)
 *
 *          It is defined in [SEC2](https://www.secg.org/sec2-v2.pdf).
 *
 * @warning The 163-bit curve `sect163r2` is weak and deprecated and is only recommended for use in
 *          legacy protocols.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_DETERMINISTIC_ECDSA
 *          - @ref PSA_ALG_ECDSA
 *          - @ref PSA_ALG_ECDSA_ANY
 *          - @ref PSA_ALG_ECDH (key pair only)
 */
#define PSA_ECC_FAMILY_SECT_R2 ((psa_ecc_family_t)0x2b)

/**
 * @brief   Twisted Edwards curves.
 *
 * @details This family comprises the following twisted Edwards curves:
 *          - `Edwards25519` : @c key_bits = 255. This curve is birationally equivalent to
 *            `Curve25519`.
 *          - `Edwards448` : @c key_bits = 448. This curve is birationally equivalent to `Curve448`.
 *
 *          Edwards25519 is defined in Twisted Edwards curves
 *          [Ed25519](https://eprint.iacr.org/2008/013.pdf). Edwards448 is defined in
 *          Ed448-Goldilocks, a new elliptic curve [Curve448](https://eprint.iacr.org/2015/625.pdf).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PURE_EDDSA
 *          - @ref PSA_ALG_ED25519PH (Edwards25519 only)
 *          - @ref PSA_ALG_ED448PH (Edwards448 only)
 */
#define PSA_ECC_FAMILY_TWISTED_EDWARDS ((psa_ecc_family_t)0x42)

/**
 * @brief   A context for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_CONTEXT /* implementation-defined value */

/**
 * @brief   A cost parameter for password hashing or key stretching.
 *
 * @details This must be a direct input, passed to @ref psa_key_derivation_input_integer().
 */
#define PSA_KEY_DERIVATION_INPUT_COST /* implementation-defined value */

/**
 * @brief   An information string for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_INFO /* implementation-defined value */

/**
 * @brief   A label for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_LABEL /* implementation-defined value */

/**
 * @brief   A low-entropy secret input for password hashing or key stretching.
 *
 * @details This is usually a key of type @ref PSA_KEY_TYPE_PASSWORD passed to
 *          @ref psa_key_derivation_input_key() or a direct input passed to
 *          @ref psa_key_derivation_input_bytes() that is a password or passphrase. It can also be
 *          high-entropy secret, for example, a key of type @ref PSA_KEY_TYPE_DERIVE, or the shared
 *          secret resulting from a key agreement.
 *
 *          If the secret is a direct input, the derivation operation cannot be used to derive
 *          keys: the operation will not allow a call to @ref psa_key_derivation_output_key().
 */
#define PSA_KEY_DERIVATION_INPUT_PASSWORD /* implementation-defined value */

/**
 * @brief   A salt for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA or @ref PSA_KEY_TYPE_PEPPER.
 */
#define PSA_KEY_DERIVATION_INPUT_SALT /* implementation-defined value */

/**
 * @brief   A high-entropy secret input for key derivation.
 *
 * @details This is typically a key of type @ref PSA_KEY_TYPE_DERIVE passed to
 *          @ref psa_key_derivation_input_key(), or the shared secret resulting from a key
 *          agreement obtained via @ref psa_key_derivation_key_agreement().
 *
 *          The secret can also be a direct input passed to @ref psa_key_derivation_input_bytes().
 *          In this case, the derivation operation cannot be used to derive keys: the operation
 *          will not allow a call to @ref psa_key_derivation_output_key().
 */
#define PSA_KEY_DERIVATION_INPUT_SECRET /* implementation-defined value */

/**
 * @brief   A seed for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_SEED /* implementation-defined value */

/**
 * @brief   Use the maximum possible capacity for a key derivation operation.
 *
 * @details Use this value as the capacity argument when setting up a key derivation to specify
 *          that the operation will use the maximum possible capacity. The value of the maximum
 *          possible capacity depends on the key derivation algorithm.
 */
#define PSA_KEY_DERIVATION_UNLIMITED_CAPACITY \
/* implementation-defined value */

/**
 * @brief   The null key identifier.
 *
 * @details The null key identifier is always invalid, except when used without in a call to
 *          @ref psa_destroy_key() which will return @ref PSA_SUCCESS.
 */
#define PSA_KEY_ID_NULL ((psa_key_id_t)0)

/**
 * @brief The maximum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MAX ((psa_key_id_t)0x3fffffff)

/**
 * @brief The minimum value for a key identifier chosen by the application.
 */
#define PSA_KEY_ID_USER_MIN ((psa_key_id_t)0x00000001)

/**
 * @brief The maximum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MAX ((psa_key_id_t)0x7fffffff)

/**
 * @brief The minimum value for a key identifier chosen by the implementation.
 */
#define PSA_KEY_ID_VENDOR_MIN ((psa_key_id_t)0x40000000)

/**
 * @brief   Construct a lifetime from a persistence level and a location.
 *
 * @param   persistence The persistence level: a value of type @ref psa_key_persistence_t.
 * @param   location    The location indicator: a value of type @ref psa_key_location_t.
 *
 * @return  The constructed lifetime value.
 */
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
    ((location) << 8 | (persistence))

/**
 * @brief   Extract the location indicator from a key lifetime.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime) \
    ((psa_key_location_t)((lifetime) >> 8))

/**
 * @brief   Extract the persistence level from a key lifetime.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) \
    ((psa_key_persistence_t)((lifetime) & 0x000000ff))

/**
 * @brief   Whether a key lifetime indicates that the key is volatile.
 *
 *          A volatile key is automatically destroyed by the implementation when the application
 *          instance terminates. In particular, a volatile key is automatically destroyed on a
 *          power reset of the device.
 *
 *          A key that is not volatile is persistent. Persistent keys are preserved until the
 *          application explicitly destroys them or until an implementation-specific device
 *          management event occurs, for example, a factory reset.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 *
 * @return  1 if the key is volatile, otherwise 0.
 */
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime) \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)

/**
 * @brief   The default lifetime for persistent keys.
 *
 * @details A persistent key remains in storage until it is explicitly destroyed or until the
 *          corresponding storage area is wiped. This specification does not define any mechanism
 *          to wipe a storage area. Implementations are permitted to provide their own mechanism,
 *          for example, to perform a factory reset, to prepare for device refurbishment, or to
 *          uninstall an application.
 *
 *          This lifetime value is the default storage area for the calling application.
 *          Implementations can offer other storage areas designated by other lifetime values as
 *          implementation-specific extensions.
 */
#define PSA_KEY_LIFETIME_PERSISTENT ((psa_key_lifetime_t)0x00000001)

/**
 * @brief   The default lifetime for volatile keys.
 *
 * @details A volatile key only exists as long as its identifier is not destroyed. The key material
 *          is guaranteed to be erased on a power reset.
 *
 *          A key with this lifetime is typically stored in the RAM area of the PSA Crypto
 *          subsystem. However this is an implementation choice. If an implementation stores data
 *          about the key in a non-volatile memory, it must release all the resources associated
 *          with the key and erase the key material if the calling application terminates.
 */
#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t)0x00000000)

/**
 * @brief   The local storage area for persistent keys.
 *
 * @details This storage area is available on all systems that can store persistent keys without
 *          delegating the storage to a third-party cryptoprocessor.
 *
 *          See @ref psa_key_location_t for more information.
 */
#define PSA_KEY_LOCATION_LOCAL_STORAGE          ((psa_key_location_t)0x000000)

/**
 * @brief   The default secure element storage area for persistent keys.
 *
 * @details This storage location is available on systems that have one or more secure elements
 *          that are able to store keys.
 *
 *          Vendor-defined locations must be provided by the system for storing keys in additional
 *          secure elements.
 *
 *          See @ref psa_key_location_t for more information.
 */
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT ((psa_key_location_t)0x000001)

/**
 * @brief   Mark vendor defined key locations
 */
#define PSA_KEY_LOCATION_VENDOR_FLAG            ((psa_key_location_t)0x800000)

/**
 * @brief   Minimum location value for secure elements
 */
#define PSA_KEY_LOCATION_SE_MIN (PSA_KEY_LOCATION_VENDOR_FLAG)

/**
 * @brief   Maximum location value for secure elements
 */
#define PSA_KEY_LOCATION_SE_MAX ((psa_key_location_t)0x8000ff)

/**
 * @brief   The default persistence level for persistent keys.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_DEFAULT ((psa_key_persistence_t)0x01)

/**
 * @brief   A persistence level indicating that a key is never destroyed.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_READ_ONLY ((psa_key_persistence_t)0xff)

/**
 * @brief   The persistence level of volatile keys.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_VOLATILE ((psa_key_persistence_t)0x00)

/**
 * @brief   Vendor-defined key type flag.
 *
 * @details Key types defined by this standard will never have the
 *          @ref PSA_KEY_TYPE_VENDOR_FLAG bit set. Vendors who define additional key types
 *          must use an encoding with the @ref PSA_KEY_TYPE_VENDOR_FLAG bit set and should
 *          respect the bitwise structure used by standard encodings whenever practical.
 */
#define PSA_KEY_TYPE_VENDOR_FLAG                    ((psa_key_type_t)0x8000)

/**
 * @brief   Mask for key type categories
 */
#define PSA_KEY_TYPE_CATEGORY_MASK                  ((psa_key_type_t)0x7000)

/**
 * @brief   Raw key data type
 */
#define PSA_KEY_TYPE_CATEGORY_RAW                   ((psa_key_type_t)0x1000)

/**
 * @brief   Symmetric key type
 */
#define PSA_KEY_TYPE_CATEGORY_SYMMETRIC             ((psa_key_type_t)0x2000)

/**
 * @brief   Asymmetric public key type
 */
#define PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY            ((psa_key_type_t)0x4000)

/**
 * @brief   Asymmetric key pair type
 */
#define PSA_KEY_TYPE_CATEGORY_KEY_PAIR              ((psa_key_type_t)0x7000)

/**
 * @brief   Asymmetric key pair flag
 */
#define PSA_KEY_TYPE_CATEGORY_FLAG_PAIR             ((psa_key_type_t)0x3000)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the AES block cipher.
 *
 * @details The size of the key is related to the AES algorithm variant. For algorithms except the
 *          XTS block cipher mode, the following key sizes are used:
 *          - AES-128 uses a 16-byte key : @c key_bits = 128
 *          - AES-192 uses a 24-byte key : @c key_bits = 192
 *          - AES-256 uses a 32-byte key : @c key_bits = 256
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the following key sizes are used:
 *          - AES-128-XTS uses two 16-byte keys : @c key_bits = 256
 *          - AES-192-XTS uses two 24-byte keys : @c key_bits = 384
 *          - AES-256-XTS uses two 32-byte keys : @c key_bits = 512
 *
 *          The AES block cipher is defined in FIPS Publication 197: Advanced Encryption Standard
 *          (AES) [FIPS197](https://doi.org/10.6028/NIST.FIPS.197).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_AES ((psa_key_type_t)0x2400)

/**
 * @brief   Key for the ARC4 stream cipher.
 *
 * @warning The ARC4 cipher is weak and deprecated and is only recommended for use in legacy
 *          protocols.
 *
 * @details The ARC4 cipher supports key sizes between 40 and 2048 bits, that are multiples of 8.
 *          (5 to 256 bytes)
 *
 *          Use algorithm @ref PSA_ALG_STREAM_CIPHER to use this key with the ARC4 cipher.
 */
#define PSA_KEY_TYPE_ARC4 ((psa_key_type_t)0x2002)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the ARIA block cipher.
 *
 *          The size of the key is related to the ARIA algorithm variant. For algorithms except the
 *          XTS block cipher mode, the following key sizes are used:
 *          - ARIA-128 uses a 16-byte key : @c key_bits = 128
 *          - ARIA-192 uses a 24-byte key : @c key_bits = 192
 *          - ARIA-256 uses a 32-byte key : @c key_bits = 256
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the following key sizes are used:
 *          - ARIA-128-XTS uses two 16-byte keys : @c key_bits = 256
 *          - ARIA-192-XTS uses two 24-byte keys : @c key_bits = 384
 *          - ARIA-256-XTS uses two 32-byte keys : @c key_bits = 512
 *
 *          The ARIA block cipher is defined in A Description of the ARIA Encryption Algorithm
 *          [RFC5794](https://datatracker.ietf.org/doc/html/rfc5794).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_ARIA ((psa_key_type_t)0x2406)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the Camellia block cipher.
 *
 * @details The size of the key is related to the Camellia algorithm variant. For algorithms except
 *          the XTS block cipher mode, the following key sizes are used:
 *          - Camellia-128 uses a 16-byte key : @c key_bits = 128
 *          - Camellia-192 uses a 24-byte key : @c key_bits = 192
 *          - Camellia-256 uses a 32-byte key : @c key_bits = 256
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the following key sizes are used:
 *          - Camellia-128-XTS uses two 16-byte keys : @c key_bits = 256
 *          - Camellia-192-XTS uses two 24-byte keys : @c key_bits = 384
 *          - Camellia-256-XTS uses two 32-byte keys : @c key_bits = 512
 *
 *          The Camellia block cipher is defined in Specification of Camellia — a 128-bit Block
 *          Cipher [NTT-CAM](https://info.isl.ntt.co.jp/crypt/eng/camellia/specifications) and
 *          also described in A Description of the Camellia Encryption
 *          Algorithm [RFC3713](https://tools.ietf.org/html/rfc3713).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_CAMELLIA ((psa_key_type_t)0x2403)

/**
 * @brief   Key for the ChaCha20 stream cipher or the ChaCha20-Poly1305 AEAD algorithm.
 *
 * @details The ChaCha20 key size is 256 bits (32 bytes).
 *          - Use algorithm @ref PSA_ALG_STREAM_CIPHER to use this key with the ChaCha20 cipher for
 *            unauthenticated encryption. See @ref PSA_ALG_STREAM_CIPHER for details of this
 *            algorithm.
 *          - Use algorithm @ref PSA_ALG_CHACHA20_POLY1305 to use this key with the ChaCha20 cipher
 *            and Poly1305 authenticator for AEAD. See @ref PSA_ALG_CHACHA20_POLY1305 for details
 *            of this algorithm.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_STREAM_CIPHER
 *          - @ref PSA_ALG_CHACHA20_POLY1305
 */
#define PSA_KEY_TYPE_CHACHA20 ((psa_key_type_t)0x2004)

/**
 * @brief   A secret for key derivation.
 *
 * @details This key type is for high-entropy secrets only. For low-entropy secrets,
 *          @ref PSA_KEY_TYPE_PASSWORD should be used instead.
 *
 *          These keys can be used in the @ref PSA_KEY_DERIVATION_INPUT_SECRET or
 *          @ref PSA_KEY_DERIVATION_INPUT_PASSWORD input step of key derivation algorithms.
 *
 *          The key policy determines which key derivation algorithm the key can be used for.
 *
 *          The bit size of a secret for key derivation must be a non-zero multiple of 8. The
 *          maximum size of a secret for key derivation is implementation defined.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_HKDF (secret input)
 *          - @ref PSA_ALG_TLS12_PRF (secret input)
 *          - @ref PSA_ALG_TLS12_PSK_TO_MS (secret input)
 */
#define PSA_KEY_TYPE_DERIVE ((psa_key_type_t)0x1200)

/**
 * @brief   Key for a cipher or MAC algorithm based on DES or 3DES (Triple-DES).
 *
 * @details The size of the key determines which DES algorithm is used:
 *          - Single DES uses an 8-byte key : @c key_bits = 64
 *          - 2-key 3DES uses a 16-byte key : @c key_bits = 128
 *          - 3-key 3DES uses a 24-byte key : @c key_bits = 192
 *
 * @warning Single DES and 2-key 3DES are weak and strongly deprecated and are only recommended for
 *          decrypting legacy data.
 *          3-key 3DES is weak and deprecated and is only recommended for use in legacy protocols.
 *
 *          The DES and 3DES block ciphers are defined in NIST Special Publication 800-67:
 *          Recommendation for the Triple Data Encryption Algorithm (TDEA) Block Cipher
 *          [SP800-67](https://doi.org/10.6028/NIST.SP.800-67r2).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 */
#define PSA_KEY_TYPE_DES ((psa_key_type_t)0x2301)

/**
 * @brief   Extract the group family from a Diffie-Hellman key type.
 *
 * @param   type A Diffie-Hellman key type: a value of type @ref psa_key_type_t such that
 *          @ref PSA_KEY_TYPE_IS_DH(@p type) is true.
 *
 * @return  The Diffie-Hellman group family id, if type is a supported Diffie-Hellman key.
 *          Unspecified if type is not a supported Diffie-Hellman key.
 */
#define PSA_KEY_TYPE_DH_GET_FAMILY(type) \
    ((psa_dh_family_t)((type) & 0x00ff))

/**
 * @brief   Finite-field Diffie-Hellman key pair: both the private key and public key.
 *
 * @details @b Compatible @b algorithms
 *          - @ref PSA_ALG_FFDH
 *
 * @param   group A value of type @ref psa_dh_family_t that identifies the Diffie-Hellman group
 *          family to be used.
 */
#define PSA_KEY_TYPE_DH_KEY_PAIR(group) \
    ((psa_key_type_t)(0x7200 | (group)))

/**
 * @brief   Finite-field Diffie-Hellman public key.
 *
 * @details @b Compatible @b algorithms
 *          None. Finite-field Diffie-Hellman public keys are exported to use in a key agreement
 *          algorithm, and the peer key is provided to the @ref PSA_ALG_FFDH key agreement
 *          algorithm as a buffer of key data.
 *
 * @param   group A value of type @ref psa_dh_family_t that identifies the Diffie-Hellman group
 *          family to be used.
 */
#define PSA_KEY_TYPE_DH_PUBLIC_KEY(group) \
    ((psa_key_type_t)(0x4200 | (group)))

/**
 * @brief   Extract the curve family from an elliptic curve key type.
 *
 * @param   type An elliptic curve key type: a value of type @ref psa_key_type_t such that
 *          @ref PSA_KEY_TYPE_IS_ECC(@p type) is true.
 *
 * @return  The elliptic curve family id, if type is a supported elliptic curve key.
 *          Unspecified if type is not a supported elliptic curve key.
 */
#define PSA_KEY_TYPE_ECC_GET_FAMILY(type) \
    ((psa_ecc_family_t)((type) & 0x00ff))

/**
 * @brief   Base value for ECC public keys
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE            ((psa_key_type_t)0x4100)

/**
 * @brief   Base value for ECC key pairs
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR_BASE              ((psa_key_type_t)0x7100)

/**
 * @brief   Mask for ECC curves
 */
#define PSA_KEY_TYPE_ECC_CURVE_MASK                 ((psa_key_type_t)0x00ff)

/**
 * @brief   Elliptic curve key pair: both the private and public key.
 *
 * @details The size of an elliptic curve key is the bit size associated with the curve, that is,
 *          the bit size of q for a curve over a field Fq. See the documentation of each Elliptic
 *          curve family for details.
 *
 *          @b Compatible @b algorithms
 *          -   Elliptic curve key pairs can be used in Asymmetric signature and Key agreement
 *              algorithms.
 *          -   The set of compatible algorithms depends on the Elliptic curve key family. See the
 *              Elliptic curve family for details.
 *
 * @param   curve A value of type @ref psa_ecc_family_t that identifies the ECC curve to be used.
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR(curve)         \
    (PSA_KEY_TYPE_ECC_KEY_PAIR_BASE | (curve))

/**
 * @brief   Elliptic curve public key.
 *
 * @details The size of an elliptic curve public key is the same as the corresponding private key.
 *          See @ref PSA_KEY_TYPE_ECC_KEY_PAIR() and the documentation of each Elliptic curve
 *          family for details.
 *
 *          @b Compatible @b algorithms
 *          Elliptic curve public keys can be used for verification in asymmetric signature
 *          algorithms.
 *          The set of compatible algorithms depends on the elliptic curve key family. See each
 *          elliptic curve family for details.
 *
 * @param   curve A value of type @ref psa_ecc_family_t that identifies the ECC curve to be used.
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve)              \
    (PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE | (curve))

/**
 * @brief   Elliptic curve public key.
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve)              \
    (PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE | (curve))

/**
 * @brief Whether a key type is an elliptic curve key, either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC(type) \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) & 0xff00) == 0x4100)

/**
 * @brief Whether a key type is an elliptic curve key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) \
    (((type) & 0xff00) == 0x7100)

/**
 * @brief Whether a key type is an elliptic curve public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type) \
    (((type) & 0xff00) == 0x4100)

/**
 * @brief   The public key type corresponding to a key pair type.
 *          You may also pass a key pair type as type, it will be left unchanged.
 *
 * @param   type A public key type or key pair type.
 *
 * @return  The corresponding public key type.
 *          If type is not a public key or a key pair, the return value is undefined.
 */
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) \
    ((psa_key_type_t)((type) & ~0x3000))

/**
 * @brief   HMAC key.
 *
 * @details The key policy determines which underlying hash algorithm the key can be used for.
 *
 *          The bit size of an HMAC key must be a non-zero multiple of 8. An HMAC key is typically
 *          the same size as the output of the underlying hash algorithm. An HMAC key that is
 *          longer than the block size of the underlying hash algorithm will be hashed before use.
 *
 *          When an HMAC key is created that is longer than the block size, it is implementation
 *          defined whether the implementation stores the original HMAC key, or the hash of the
 *          HMAC key. If the hash of the key is stored, the key size reported by
 *          @ref psa_get_key_attributes() will be the size of the hashed key.
 *
 * @note    @ref PSA_HASH_LENGTH(@p alg) provides the output size of hash algorithm @c alg,
 *          in bytes.
 *          @ref PSA_HASH_BLOCK_LENGTH(@p alg) provides the block size of hash algorithm
 *          @c alg, in bytes.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_HMAC
 */
#define PSA_KEY_TYPE_HMAC ((psa_key_type_t)0x1100)

/**
 * @brief Whether a key type is asymmetric: either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ASYMMETRIC(type) \
    (((type) & 0x4000) == 0x4000)

/**
 * @brief Whether a key type is a Diffie-Hellman key, either a key pair or a public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH(type) \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) & 0xff00) == 0x4200)

/**
 * @brief Whether a key type is a Diffie-Hellman key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH_KEY_PAIR(type) \
    (((type) & 0xff00) == 0x7200)

/**
 * @brief Whether a key type is a Diffie-Hellman public key.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type) \
    (((type) & 0xff00) == 0x4200)

/**
 * @brief Whether a key type is a key pair containing a private part and a public part.
 *
 * @param type  A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_KEY_PAIR(type) \
    (((type) & 0x7000) == 0x7000)

/**
 * @brief Whether a key type is the public part of a key pair.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */

#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type) \
    (((type) & 0x7000) == 0x4000)

/**
 * @brief Whether a key type is an RSA key. This includes both key pairs and public keys.
 *
 * @param type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_RSA(type) \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == 0x4001)

/**
 * @brief   Whether a key type is an unstructured array of bytes.
 *          This encompasses both symmetric keys and non-key data.
 *
 * @param   type A key type: a value of type @ref psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) \
    (((type) & 0x7000) == 0x1000 || ((type) & 0x7000) == 0x2000)

/**
 * @brief   The key pair type corresponding to a public key type.
 *
 * @details If type is a key pair type, it will be left unchanged.
 *
 * @param   type A public key type or key pair type.
 *
 * @return  The corresponding key pair type. If type is not a public key or a key pair, the return
 *          value is undefined.
 */
#define PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type) \
    ((psa_key_type_t)((type) | 0x3000))

/**
 * @brief   An invalid key type value.
 *
 * @details Zero is not the encoding of any key type.
 */
#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)

/**
 * @brief   A low-entropy secret for password hashing or key derivation.
 *
 * @details This key type is suitable for passwords and passphrases which are typically intended to
 *          be memorizable by humans, and have a low entropy relative to their size. It can be used
 *          for randomly generated or derived keys with maximum or near-maximum entropy, but
 *          @ref PSA_KEY_TYPE_DERIVE is more suitable for such keys. It is not suitable for
 *          passwords with extremely low entropy, such as numerical PINs.
 *
 *          These keys can be used in the @ref PSA_KEY_DERIVATION_INPUT_PASSWORD input step of key
 *          derivation algorithms. Algorithms that accept such an input were designed to accept
 *          low-entropy secret and are known as password hashing or key stretching algorithms.
 *
 *          These keys cannot be used in the @ref PSA_KEY_DERIVATION_INPUT_SECRET input step of key
 *          derivation algorithms, as the algorithms expect such an input to have high entropy.
 *
 *          The key policy determines which key derivation algorithm the key can be used for, among
 *          the permissible subset defined above.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PBKDF2_HMAC() (password input)
 *          - @ref PSA_ALG_PBKDF2_AES_CMAC_PRF_128 (password input)
 */
#define PSA_KEY_TYPE_PASSWORD ((psa_key_type_t)0x1203)

/**
 * @brief   A secret value that can be used to verify a password hash.
 *
 * @details The key policy determines which key derivation algorithm the key can be used for, among
 *          the same permissible subset as for @ref PSA_KEY_TYPE_PASSWORD.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PBKDF2_HMAC() (key output and verification)
 *          - @ref PSA_ALG_PBKDF2_AES_CMAC_PRF_128 (key output and verification)
 */
#define PSA_KEY_TYPE_PASSWORD_HASH ((psa_key_type_t)0x1205)

/**
 * @brief   A secret value that can be used when computing a password hash.
 *
 * @details The key policy determines which key derivation algorithm the key can be used for, among
 *          the subset of algorithms that can use pepper.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_PBKDF2_HMAC() (salt input)
 *          - @ref PSA_ALG_PBKDF2_AES_CMAC_PRF_128 (salt input)
 */
#define PSA_KEY_TYPE_PEPPER ((psa_key_type_t)0x1206)

/**
 * @brief   Raw data.
 *
 * @details A “key” of this type cannot be used for any cryptographic operation. Applications can
 *          use this type to store arbitrary data in the keystore.
 *
 *          The bit size of a raw key must be a non-zero multiple of 8. The maximum size of a raw
 *          key is implementation defined.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_HKDF (non-secret inputs)
 *          - @ref PSA_ALG_TLS12_PRF (non-secret inputs)
 *          - @ref PSA_ALG_TLS12_PSK_TO_MS (non-secret inputs)
 */
#define PSA_KEY_TYPE_RAW_DATA ((psa_key_type_t)0x1001)

/**
 * @brief   RSA key pair: both the private and public key.
 *
 * @details The size of an RSA key is the bit size of the modulus.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_RSA_OAEP
 *          - @ref PSA_ALG_RSA_PKCS1V15_CRYPT
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW
 *          - @ref PSA_ALG_RSA_PSS
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT
 */
#define PSA_KEY_TYPE_RSA_KEY_PAIR ((psa_key_type_t)0x7001)

/**
 * @brief   RSA public key.
 *
 * @details The size of an RSA key is the bit size of the modulus.
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_RSA_OAEP (encryption only)
 *          - @ref PSA_ALG_RSA_PKCS1V15_CRYPT (encryption only)
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN (signature verification only)
 *          - @ref PSA_ALG_RSA_PKCS1V15_SIGN_RAW (signature verification only)
 *          - @ref PSA_ALG_RSA_PSS (signature verification only)
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT (signature verification only)
 */
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY ((psa_key_type_t)0x4001)

/**
 * @brief   Key for a cipher, AEAD or MAC algorithm based on the SM4 block cipher.
 *
 * @details For algorithms except the XTS block cipher mode, the SM4 key size is 128 bits
 *          (16 bytes).
 *
 *          For the XTS block cipher mode (@ref PSA_ALG_XTS), the SM4 key size is 256 bits
 *          (two 16-byte keys).
 *
 *          The SM4 block cipher is defined in GM/T 0002-2012: SM4 block cipher algorithm
 *          [CSTC0002](http://www.gmbz.org.cn/main/viewfile/20180108015408199368.html)
 *          (English version [CSTC0002/E](http://www.gmbz.org.cn/main/postDetail.html?
 *          id=20180404044052)).
 *
 *          @b Compatible @b algorithms
 *          - @ref PSA_ALG_CBC_MAC
 *          - @ref PSA_ALG_CMAC
 *          - @ref PSA_ALG_CTR
 *          - @ref PSA_ALG_CFB
 *          - @ref PSA_ALG_OFB
 *          - @ref PSA_ALG_XTS
 *          - @ref PSA_ALG_CBC_NO_PADDING
 *          - @ref PSA_ALG_CBC_PKCS7
 *          - @ref PSA_ALG_ECB_NO_PADDING
 *          - @ref PSA_ALG_CCM
 *          - @ref PSA_ALG_GCM
 */
#define PSA_KEY_TYPE_SM4 ((psa_key_type_t)0x2405)

/**
 * @brief   Permission for the implementation to cache the key.
 *
 * @details This flag allows the implementation to make additional copies of the key material that
 *          are not in storage and not for the purpose of an ongoing operation. Applications can
 *          use it as a hint to keep the key around for repeated access.
 *
 *          An application can request that cached key material is removed from memory by calling
 *          @ref psa_purge_key().
 *
 *          The presence of this usage flag when creating a key is a hint:
 *          - An implementation is not required to cache keys that have this usage flag.
 *          - An implementation must not report an error if it does not cache keys.
 *
 *          If this usage flag is not present, the implementation must ensure key material is
 *          removed from memory as soon as it is not required for an operation or for maintenance
 *          of a volatile key.
 *
 *          This flag must be preserved when reading back the attributes for all keys, regardless
 *          of key type or implementation behavior.
 */
#define PSA_KEY_USAGE_CACHE ((psa_key_usage_t)0x00000004)

/**
 * @brief   Permission to copy the key.
 *
 * @details This flag allows the use of @ref psa_copy_key() to make a copy of the key with the same
 *          policy or a more restrictive policy.
 *
 *          For lifetimes for which the key is located in a secure element which enforce the
 *          non-exportability of keys, copying a key outside the secure element also requires the
 *          usage flag @ref PSA_KEY_USAGE_EXPORT. Copying the key inside the secure element is
 *          permitted with just @ref PSA_KEY_USAGE_COPY if the secure element supports it. For keys
 *          with the lifetime @ref PSA_KEY_LIFETIME_VOLATILE or @ref PSA_KEY_LIFETIME_PERSISTENT,
 *          the usage flag @ref PSA_KEY_USAGE_COPY is sufficient to permit the copy.
 */
#define PSA_KEY_USAGE_COPY ((psa_key_usage_t)0x00000002)

/**
 * @brief   Permission to decrypt a message with the key.
 *
 * @details This flag allows the key to be used for a symmetric decryption operation, for an AEAD
 *          decryption-and-verification operation, or for an asymmetric decryption operation, if
 *          otherwise permitted by the key’s type and policy. The flag must be present on keys used
 *          with the following APIs:
 *          - @ref psa_cipher_decrypt()
 *          - @ref psa_cipher_decrypt_setup()
 *          - @ref psa_aead_decrypt()
 *          - @ref psa_aead_decrypt_setup()
 *          - @ref psa_asymmetric_decrypt()
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_DECRYPT ((psa_key_usage_t)0x00000200)

/**
 * @brief   Permission to derive other keys or produce a password hash from this key.
 *
 * @details This flag allows the key to be used for a key derivation operation or for a key
 *          agreement operation, if otherwise permitted by the key’s type and policy.
 *
 *          This flag must be present on keys used with the following APIs:
 *          - @ref psa_key_derivation_key_agreement()
 *          - @ref psa_raw_key_agreement()
 *
 *          If this flag is present on all keys used in calls to
 *          @ref psa_key_derivation_input_key() for a key derivation operation, then it permits
 *          calling @ref psa_key_derivation_output_bytes() or @ref psa_key_derivation_output_key()
 *          at the end of the operation.
 */
#define PSA_KEY_USAGE_DERIVE ((psa_key_usage_t)0x00004000)

/**
 * @brief   Permission to encrypt a message with the key.
 *
 * @details This flag allows the key to be used for a symmetric encryption operation, for an AEAD
 *          encryption-and-authentication operation, or for an asymmetric encryption operation, if
 *          otherwise permitted by the key’s type and policy. The flag must be present on keys used
 *          with the following APIs:
 *          - @ref psa_cipher_encrypt()
 *          - @ref psa_cipher_encrypt_setup()
 *          - @ref psa_aead_encrypt()
 *          - @ref psa_aead_encrypt_setup()
 *          - @ref psa_asymmetric_encrypt()
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_ENCRYPT ((psa_key_usage_t)0x00000100)

/**
 * @brief   Permission to export the key.
 *
 * @details This flag allows the use of @ref psa_export_key() to export a key from the
 *          cryptoprocessor. A public key or the public part of a key pair can always be exported
 *          regardless of the value of this permission flag.
 *
 *          This flag can also be required to copy a key using @ref psa_copy_key() outside of a
 *          secure element. See also @ref PSA_KEY_USAGE_COPY.
 *
 *          If a key does not have export permission, implementations must not allow the key to be
 *          exported in plain form from the cryptoprocessor, whether through @ref psa_export_key()
 *          or through a proprietary interface. The key might still be exportable in a wrapped
 *          form, i.e. in a form where it is encrypted by another key.
 */
#define PSA_KEY_USAGE_EXPORT ((psa_key_usage_t)0x00000001)

/**
 * @brief   Permission to sign a message hash with the key.
 *
 * @details This flag allows the key to be used to sign a message hash as part of an asymmetric
 *          signature operation, if otherwise permitted by the key’s type and policy. The flag must
 *          be present on keys used when calling @ref psa_sign_hash().
 *
 *          This flag automatically sets @ref PSA_KEY_USAGE_SIGN_MESSAGE : if an application sets
 *          the flag @ref PSA_KEY_USAGE_SIGN_HASH when creating a key, then the key always has the
 *          permissions conveyed by @ref PSA_KEY_USAGE_SIGN_MESSAGE, and the flag
 *          @ref PSA_KEY_USAGE_SIGN_MESSAGE will also be present when the application queries the
 *          usage flags of the key.
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_SIGN_HASH ((psa_key_usage_t)0x00001000)

/**
 * @brief   Permission to sign a message with the key.
 *
 * @details This flag allows the key to be used for a MAC calculation operation or for an
 *          asymmetric message signature operation, if otherwise permitted by the key’s type and
 *          policy. The flag must be present on keys used with the following APIs:
 *          - @ref psa_mac_compute()
 *          - @ref psa_mac_sign_setup()
 *          - @ref psa_sign_message()
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_SIGN_MESSAGE ((psa_key_usage_t)0x00000400)

/**
 * @brief   Permission to verify the result of a key derivation, including password hashing.
 *
 * @details This flag allows the key to be used in a key derivation operation, if otherwise
 *          permitted by the key’s type and policy.
 *
 *          This flag must be present on keys used with @ref psa_key_derivation_verify_key().
 *
 *          If this flag is present on all keys used in calls to
 *          @ref psa_key_derivation_input_key() for a key derivation operation, then it permits
 *          calling @ref psa_key_derivation_verify_bytes() or @ref psa_key_derivation_verify_key()
 *          at the end of the operation.
 */
#define PSA_KEY_USAGE_VERIFY_DERIVATION ((psa_key_usage_t)0x00008000)

/**
 * @brief   Permission to verify a message hash with the key.
 *
 * @details This flag allows the key to be used to verify a message hash as part of an asymmetric
 *          signature verification operation, if otherwise permitted by the key’s type and policy.
 *          The flag must be present on keys used when calling @ref psa_verify_hash().
 *
 *          This flag automatically sets @ref PSA_KEY_USAGE_VERIFY_MESSAGE : if an application sets
 *          the flag @ref PSA_KEY_USAGE_VERIFY_HASH when creating a key, then the key always has
 *          the permissions conveyed by @ref PSA_KEY_USAGE_VERIFY_MESSAGE, and the flag
 *          @ref PSA_KEY_USAGE_VERIFY_MESSAGE will also be present when the application queries the
 *          usage flags of the key.
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_VERIFY_HASH ((psa_key_usage_t)0x00002000)

/**
 * @brief   Permission to verify a message signature with the key.
 *
 * @details This flag allows the key to be used for a MAC verification operation or for an
 *          asymmetric message signature verification operation, if otherwise permitted by the
 *          key’s type and policy. The flag must be present on keys used with the following APIs:
 *          - @ref psa_mac_verify()
 *          - @ref psa_mac_verify_setup()
 *          - @ref psa_verify_message()
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_VERIFY_MESSAGE ((psa_key_usage_t)0x00000800)

/**
 * @brief   Sufficient output buffer size for @ref psa_raw_key_agreement(), for any of the
 *          supported key types and key agreement algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_raw_key_agreement() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE().
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE \
/* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_raw_key_agreement().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_raw_key_agreement() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type A supported key type.
 * @param   key_bits The size of the key in bits.
 *
 * @return  A sufficient output buffer size for the specified key type and size.
 *          0 if key type is not supported.
 *          If the parameters are not valid, the return value is unspecified.
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, key_bits) \
/* implementation-defined value */

/**
 * @brief   A sufficient signature buffer size for @ref psa_sign_message() and
 *          @ref psa_sign_hash(), for any of the supported key types and asymmetric signature
 *          algorithms.
 *
 * @details If the size of the signature buffer is at least this large, it is guaranteed that
 *          @ref psa_sign_message() and @ref psa_sign_hash() will not fail due to an insufficient
 *          buffer size.
 *
 *          See also @ref PSA_SIGN_OUTPUT_SIZE().
 */
#define PSA_SIGNATURE_MAX_SIZE /* implementation-defined value */

/**
 * @brief   This macro returns the maximum supported length of the PSK for the TLS-1.2 PSK-to-MS
 *          key derivation.
 *
 * @details This implementation-defined value specifies the maximum length for the PSK input used
 *          with a @ref PSA_ALG_TLS12_PSK_TO_MS() key agreement algorithm.
 *
 *          Quoting Pre-Shared Key Ciphersuites for Transport Layer Security (TLS)
 *          [RFC4279](https://tools.ietf.org/html/rfc4279.html) §5.3:
 *          TLS implementations supporting these cipher suites MUST support arbitrary PSK
 *          identities up to 128 octets in length, and arbitrary PSKs up to 64 octets in length.
 *          Supporting longer identities and keys is RECOMMENDED.
 *
 *          Therefore, it is recommended that implementations define
 *          @ref PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE with a value greater than or equal to 64.
 */
#define PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE /* implementation-defined value */

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_CRYPTO_VALUES_H */
/** @} */
