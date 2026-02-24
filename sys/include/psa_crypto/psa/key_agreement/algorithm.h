/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        key_agreement/algorithm.h
 * @brief       Key agreement algorithm definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/algorithm.h"

/**
 * @brief   Category for key agreement algorithms
 */
#define PSA_ALG_CATEGORY_KEY_AGREEMENT ((psa_algorithm_t)0x09000000)

/**
 * @brief   Whether the specified algorithm is a key agreement algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a key agreement algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_KEY_AGREEMENT(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_KEY_AGREEMENT)

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

#ifdef __cplusplus
}
#endif

/** @} */
