/*
 * Copyright (C) 2024 TU Dresden
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
 * @file        asymmetric_signature/algorithm.h
 * @brief       Asymmetric signature algorithm definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/algorithm.h"
#include "psa/hash/algorithm.h"

/**
 * @brief   Category for asymmetric signature algorithms
 */
#define PSA_ALG_CATEGORY_SIGN ((psa_algorithm_t)0x06000000)

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
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_ASYMMETRIC_SIGNATURE)

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
    (PSA_ALG_GET_HASH(alg) == PSA_ALG_ANY_HASH)

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
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW ((psa_algorithm_t)0x06000200)

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
 * @brief   Base of ECDSA algorithms
 */
#define PSA_ALG_ECDSA_BASE ((psa_algorithm_t)0x06000600)

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
#define PSA_ALG_ECDSA_ANY PSA_ALG_ECDSA_BASE

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

#ifdef __cplusplus
}
#endif

/** @} */
