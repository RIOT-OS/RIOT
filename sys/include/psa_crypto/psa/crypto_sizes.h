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
 * @file        crypto_sizes.h
 * @brief       Size definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @note        Some of the macros in this file have already been copied here from
 *              the PSA API specification, but are not implemented, yet.
 *              They are marked by comments that either say "specification-defined"
 *              or "implementation-defined".
 *              These macros will be implemented successively in the future.
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_SIZES_H
#define PSA_CRYPTO_PSA_CRYPTO_SIZES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "crypto_values.h"

/**
 * @brief   Functions to convert bits to bytes
 *
 * @param   bits
 *
 * @return  Number of bytes contained in bits
 */
#define PSA_BITS_TO_BYTES(bits) (size_t)(((bits) + 7) / 8)

/**
 * @brief   Functions to convert bytes to bits
 *
 * @param   bytes
 *
 * @return  Number of bits contained in bytes
 */
#define PSA_BYTES_TO_BITS(bytes) ((bytes) * 8)

/**
 * @brief   Maximum key size in bytes, determined by the build system.
 *
 * @details The maximum key size is set automatically, depending on
 *          the features chosen at compile-time. They should not be
 *          changed manually.
 */
#if (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1) || \
     IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519) || \
     IS_USED(MODULE_PSA_CIPHER_AES_256_CBC) || \
     IS_USED(MODULE_PSA_MAC_HMAC_SHA_256) || \
     IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256))
#define CONFIG_PSA_MAX_KEY_SIZE 32
#elif (IS_USED(MODULE_PSA_CIPHER_AES_192_CBC) || \
       IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1))
#define CONFIG_PSA_MAX_KEY_SIZE 24
#elif (IS_USED(MODULE_PSA_CIPHER_AES_128_CBC)) || \
      (IS_USED(MODULE_PSA_CIPHER_AES_128_ECB))
#define CONFIG_PSA_MAX_KEY_SIZE 16
#else
#define CONFIG_PSA_MAX_KEY_SIZE 0
#endif

/**
 * @brief   Number of required allocated asymmetric key pair slots.
 *
 * @details These should be defined by the developer to
 *          fit their requirements. The default number is 0.
 */
#ifndef CONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT
#define CONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT  0
#endif

/**
 * @brief   Number of required allocated single key slots.
 *
 * @details These should be defined by the developer to
 *          fit their requirements. The default number is 0.
 */
#ifndef CONFIG_PSA_SINGLE_KEY_COUNT
#define CONFIG_PSA_SINGLE_KEY_COUNT  0
#endif

/**
 * @brief   Number of required allocated protected key slots.
 *
 * @details These should be defined by the developer to
 *          fit their requirements. The default number is 5.
 */
#ifndef CONFIG_PSA_PROTECTED_KEY_COUNT
#if (IS_USED(MODULE_PSA_SECURE_ELEMENT))
#define CONFIG_PSA_PROTECTED_KEY_COUNT 5
#else
#define CONFIG_PSA_PROTECTED_KEY_COUNT  0
#endif
#endif

/**
 * @brief   A sufficient plaintext buffer size for @ref psa_aead_decrypt(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the plaintext buffer is at least this large,
 *          it is guaranteed that @ref psa_aead_decrypt() will not fail due
 *          to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_DECRYPT_OUTPUT_SIZE().
 *
 * @param   ciphertext_length Size of the ciphertext in bytes.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient plaintext buffer size for @ref psa_aead_decrypt(), in bytes.
 *
 * @details If the size of the plaintext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_decrypt() will not fail due to an insufficient buffer size. Depending on
 *          the algorithm, the actual size of the plaintext might be smaller.
 *
 *          See also @ref PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type            A symmetric key type that is compatible with algorithm alg.
 * @param   alg                 An AEAD algorithm: a value of type @ref psa_algorithm_t
 *                              such that @ref PSA_ALG_IS_AEAD(@p alg) is true.
 * @param   ciphertext_length   Size of the ciphertext in bytes.
 *
 * @return  The AEAD plaintext size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_encrypt(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the ciphertext buffer is at least this large,
 *          it is guaranteed that @ref psa_aead_encrypt() will not fail due to an insufficient
 *          buffer size.
 *
 *          See also @ref PSA_AEAD_ENCRYPT_OUTPUT_SIZE().
 *
 * @param   plaintext_length Size of the plaintext in bytes.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_encrypt(), in bytes.
 *
 * @details If the size of the ciphertext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_encrypt() will not fail due to an insufficient buffer size. Depending on
 *          the algorithm, the actual size of the ciphertext might be smaller.
 *
 *          See also @ref PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type            A symmetric key type that is compatible with algorithm alg.
 * @param   alg                 An AEAD algorithm: a value of type @ref psa_algorithm_t such
 *                              that @ref PSA_ALG_IS_AEAD(alg) is true.
 * @param   plaintext_length    Size of the plaintext in bytes.
 *
 * @return  The AEAD ciphertext size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_finish(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the ciphertext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient ciphertext buffer size.
 *
 *          See also @ref PSA_AEAD_FINISH_OUTPUT_SIZE().
 */
#define PSA_AEAD_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_finish().
 *
 * @details If the size of the ciphertext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient ciphertext buffer size. The
 *          actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_AEAD_FINISH_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  A sufficient ciphertext buffer size for the specified key type and algorithm.
 *          If the key type or AEAD algorithm is not recognized, or the parameters are incompatible,
 *          return 0. An implementation can return either 0 or a correct size for a key type and
 *          AEAD algorithm that it recognizes, but does not support.
 */
#define PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg) \
/* implementation-defined value */

/**
 * @brief   The default nonce size for an AEAD algorithm, in bytes.
 *
 * @details If the size of the nonce buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_generate_nonce() will not fail due to an insufficient buffer size.
 *
 *          For most AEAD algorithms, @ref PSA_AEAD_NONCE_LENGTH() evaluates to the exact size of
 *          the nonce generated by @ref psa_aead_generate_nonce().
 *
 *          See also @ref PSA_AEAD_NONCE_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  The default nonce size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_NONCE_LENGTH(key_type, alg) /* implementation-defined value */

/**
 * @brief   A sufficient buffer size for storing the nonce generated by
 *          @ref psa_aead_generate_nonce(), for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the nonce buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_generate_nonce() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_NONCE_LENGTH().
 */
#define PSA_AEAD_NONCE_MAX_SIZE /* implementation-defined value */

/**
 * @brief   The length of a tag for an AEAD algorithm, in bytes.
 *
 * @details This is the size of the tag output from @ref psa_aead_finish().
 *          If the size of the tag buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient tag buffer size.
 *
 *          See also @ref PSA_AEAD_TAG_MAX_SIZE.
 *
 * @param   key_type    The type of the AEAD key.
 * @param   key_bits    The size of the AEAD key in bits.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  The tag length for the specified algorithm and key.
 *          0 if the AEAD algorithm does not have an identified tag that can be distinguished from
 *          the rest of the ciphertext.
 *          0 if the AEAD algorithm is not recognized or not supported.
 */
#define PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg) \
/* implementation-defined value */

/**
 * @brief   A sufficient buffer size for storing the tag output by @ref psa_aead_finish(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the tag buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_TAG_LENGTH().
 */
#define PSA_AEAD_TAG_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_aead_update(), for any of the supported key
 *          types and AEAD algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_update() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_UPDATE_OUTPUT_SIZE().
 *
 * @param   input_length Size of the input in bytes.
 */
#define PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_aead_update().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_update() will not fail due to an insufficient buffer size. The actual
 *          size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE.
 *
 * @param   key_type        A symmetric key type that is compatible with algorithm alg.
 * @param   alg             An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                          @ref PSA_ALG_IS_AEAD(@p alg) is true.
 * @param   input_length    Size of the input in bytes.
 *
 * @return  A sufficient output buffer size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_aead_update(), for any of the supported key
 *          types and AEAD algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_update() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_UPDATE_OUTPUT_SIZE().
 *
 * @param   input_length Size of the input in bytes.
 */
#define PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient plaintext buffer size for @ref psa_aead_verify(), in bytes.
 *
 * @details If the size of the plaintext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_verify() will not fail due to an insufficient plaintext buffer size. The
 *          actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  A sufficient plaintext buffer size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg) \
/* implementation-defined value */

/**
 * @brief   Maximum size of a hash supported by this implementation, in bytes.
 *
 *          See also @ref PSA_HASH_LENGTH().
 */
#define PSA_HASH_MAX_SIZE   (64)

/**
 * @brief   The input block size of a hash algorithm, in bytes.
 *
 * @details Hash algorithms process their input data in blocks. Hash operations will retain any
 *          partial blocks until they have enough input to fill the block or until the operation
 *          is finished.
 *
 *          This affects the output from @ref psa_hash_suspend().
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  The block size in bytes for the specified hash algorithm. If the hash algorithm is not
 *          recognized, return 0. An implementation can return either 0 or the correct size for a
 *          hash algorithm that it recognizes, but does not support.
 */
#define PSA_HASH_BLOCK_LENGTH(alg)                                  \
    (                                                               \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD2 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD4 ? 64 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD5 ? 64 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_RIPEMD160 ? 64 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_1 ? 64 :          \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_224 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_256 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_384 ? 128 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512 ? 128 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_224 ? 128 :   \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_256 ? 128 :   \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_224 ? 144 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_256 ? 136 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_384 ? 104 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_512 ? 72 :       \
        0)

/**
 * @brief   The size of the output of @ref psa_hash_compute() and @ref psa_hash_finish(), in bytes.
 *
 * @details This is also the hash length that @ref psa_hash_compare() and @ref psa_hash_verify()
 *          expect.
 *
 *          See also @ref PSA_HASH_MAX_SIZE.
 *
 * @param   alg A hash algorithm or an HMAC algorithm: a value of type @ref psa_algorithm_t such
 *              that (@ref PSA_ALG_IS_HASH(@p alg) || @ref PSA_ALG_IS_HMAC(@p alg)) is true.
 *
 * @return  The hash length for the specified hash algorithm.
 *          0 if the hash algorithm is not recognized or not supported.
 */
#define PSA_HASH_LENGTH(alg)                                      \
    (                                                           \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD2 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD4 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD5 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_RIPEMD160 ? 20 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_1 ? 20 :          \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_224 ? 28 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_256 ? 32 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_384 ? 48 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_224 ? 28 :    \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_256 ? 32 :    \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_224 ? 28 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_256 ? 32 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_384 ? 48 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_512 ? 64 :       \
        0)

/**
 * @brief   The size of the output of @ref psa_mac_compute() and @ref psa_mac_sign_finish(),
 *          in bytes.
 *
 * @details If the size of the MAC buffer is at least this large, it is guaranteed that
 *          @ref psa_mac_compute() and @ref psa_mac_sign_finish() will not fail due to an
 *          insufficient buffer size.
 *
 *          This is also the MAC length that @ref psa_mac_verify() and @ref psa_mac_verify_finish()
 *          expect.
 *
 *          See also @ref PSA_MAC_MAX_SIZE.
 *
 * @param   key_type    The type of the MAC key.
 * @param   key_bits    The size of the MAC key in bits.
 * @param   alg         A MAC algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_MAC(@p alg) is true.
 *
 * @return  The MAC length for the specified algorithm with the specified key parameters.
 *          0 if the MAC algorithm is not recognized or not supported.
 *          Unspecified if the key parameters are not consistent with the algorithm.
 */
#define PSA_MAC_LENGTH(key_type, key_bits, alg)  \
    ((PSA_ALG_IS_HMAC(alg)) ? PSA_HASH_LENGTH(PSA_ALG_HMAC_GET_HASH(alg)) :         \
     PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) ? PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) : \
     ((void)(key_type), (void)(key_bits), 0))

/**
 * @brief   A sufficient buffer size for storing the MAC output by @ref psa_mac_verify() and
 *          @ref psa_mac_verify_finish(), for any of the supported key types and MAC algorithms.
 *
 * @details If the size of the MAC buffer is at least this large, it is guaranteed that
 *          @ref psa_mac_verify() and @ref psa_mac_verify_finish() will not fail due to an
 *          insufficient buffer size.
 *
 *          See also @ref PSA_MAC_LENGTH().
 */
#if (IS_USED(MODULE_PSA_MAC_HMAC_SHA_512) || \
     IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_512))    /* 64 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_SHA_384) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_384))    /* 48 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_SHA_256) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA_512_256) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_256))    /* 32 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_SHA_224) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA_512_224) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA3_224))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA3_224))    /* 28 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_RIPEMD160) || \
       IS_USED(MODULE_PSA_MAC_HMAC_SHA_1))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA_1))       /* 20 */
#elif (IS_USED(MODULE_PSA_MAC_HMAC_MD2) || \
       IS_USED(MODULE_PSA_MAC_HMAC_MD4) || \
       IS_USED(MODULE_PSA_MAC_HMAC_MD5))
#define PSA_MAC_MAX_SIZE (PSA_HASH_LENGTH(PSA_ALG_MD5))         /* 16 */
#else
#define PSA_MAC_MAX_SIZE 0
#endif

/**
 * @brief   The block size of a block cipher.
 *
 * @note    It is possible to build stream cipher algorithms on top of a block cipher,
 *          for example CTR mode (@ref PSA_ALG_CTR). This macro only takes the key type
 *          into account, so it cannot be used to determine the size of the data that
 *          @ref psa_cipher_update() might buffer for future processing in general.
 *
 * @param   type  A cipher key type (value of type @ref psa_key_type_t).
 *
 * @return  The block size for a block cipher, or 1 for a stream cipher.
 */
#define PSA_BLOCK_CIPHER_BLOCK_LENGTH(type)                                     \
    (1u << (((type) >> 8) & 7))

/**
 * @brief   The maximum block size of a block cipher supported by the implementation.
 *
 * @details See also @ref PSA_BLOCK_CIPHER_BLOCK_LENGTH().
 */
#define PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_cipher_decrypt(), for any of the supported
 *          key types and cipher algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_cipher_decrypt() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_CIPHER_DECRYPT_OUTPUT_SIZE().
 *
 * @param   input_length Size of the input in bytes.
 */
#define PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length) \
    (input_length)

/**
 * @brief   The maximum size of the output of @ref psa_cipher_decrypt(), in bytes.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed
 *          that @ref psa_cipher_decrypt() will not fail due to an insufficient buffer size.
 *          Depending on the algorithm, the actual size of the output might be smaller.
 *
 *          See also @ref PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type        A symmetric key type that is compatible with algorithm alg.
 * @param   alg             A cipher algorithm (PSA_ALG_XXX value such that
 *                          @ref PSA_ALG_IS_CIPHER(@p alg) is true).
 * @param   input_length    Size of the input in bytes.
 *
 * @return  A sufficient output size for the specified key type and algorithm.
 *          0 if the key type or cipher algorithm is not recognized, or the parameters
 *          are incompatible.
 */
#define PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
    (input_length - PSA_CIPHER_IV_LENGTH(key_type, alg))

/**
 * @brief   A sufficient output buffer size for @ref psa_cipher_encrypt(), for any of the supported
 *          key types and cipher algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_cipher_encrypt() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_CIPHER_ENCRYPT_OUTPUT_SIZE().
 *
 * @param   input_length Size of the input in bytes.
 */
#define PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(input_length) \
    (PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CBC_NO_PADDING, input_length))

/**
 * @brief   The maximum size of the output of @ref psa_cipher_encrypt(), in bytes.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed
 *          that @ref psa_cipher_encrypt() will not fail due to an insufficient buffer size.
 *          Depending on the algorithm, the actual size of the output might be smaller.
 *
 *          See also @ref PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type        A symmetric key type that is compatible with algorithm alg.
 * @param   alg             A cipher algorithm (PSA_ALG_XXX value such that
 *                          @ref PSA_ALG_IS_CIPHER(@p alg) is true).
 * @param   input_length    Size of the input in bytes.
 *
 * @return  A sufficient output size for the specified key type and algorithm.
 *          0 if the key type or cipher algorithm is not recognized, not supported or the
 *          parameters are incompatible.
 */
#define PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_length) \
    (input_length + PSA_CIPHER_IV_LENGTH(key_type, alg))

/**
 * @brief   A sufficient output buffer size for @ref psa_cipher_finish(), for any of the supported
 *          key types and cipher algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_cipher_finish() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_CIPHER_FINISH_OUTPUT_SIZE().
 */
#define PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_cipher_finish().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_cipher_finish() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         A cipher algorithm: a value of type psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_CIPHER(@p alg) is true.
 *
 * @return  A sufficient output size for the specified key type and algorithm.
 *          0 if the key type or cipher algorithm is not recognized, not supported or the
 *          parameters are incompatible.
 */
#define PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg) \
/* implementation-defined value */

/**
 * @brief   The default IV size for a cipher algorithm, in bytes.
 *
 * @details The IV that is generated as part of a call to @ref psa_cipher_encrypt() is always
 *          the default IV length for the algorithm.
 *
 *          This macro can be used to allocate a buffer of sufficient size to
 *          store the IV output from @ref psa_cipher_generate_iv() when using
 *          a multi-part cipher operation.
 *
 *          See also @ref PSA_CIPHER_IV_MAX_SIZE.
 *
 * @warning This macro may evaluate its arguments multiple times or
 *          zero times, so you should not pass arguments that contain
 *          side effects.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 *
 * @param   alg         A cipher algorithm (PSA_ALG_XXX value such that
 *                      @ref PSA_ALG_IS_CIPHER(@p alg) is true)
 *
 * @return  The default IV size for the specified key type and algorithm.
 *          0, if the algorithm does not use an IV, if key type or cipher
 *          algorithm are not recognized or if the parameters are not compatible.
 *
 */
#define PSA_CIPHER_IV_LENGTH(key_type, alg) \
    (PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) > 1 && \
     ((alg) == PSA_ALG_CBC_NO_PADDING) ? 16 : 0)

/**
 * @brief   A sufficient buffer size for storing the IV generated by @ref psa_cipher_generate_iv(),
 *          for any of the supported key types and cipher algorithms.
 *
 * @details If the size of the IV buffer is at least this large, it is guaranteed that
 *          @ref psa_cipher_generate_iv() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_CIPHER_IV_LENGTH().
 */
#define PSA_CIPHER_IV_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_cipher_update(),
 *          for any of the supported key types and cipher algorithms.
 *
 * @details If the size of the output buffer is at least this large,
 *          it is guaranteed that @ref psa_cipher_update() will not fail
 *          due to an insufficient buffer size.
 *
 *          See also @ref PSA_CIPHER_UPDATE_OUTPUT_SIZE().
 *
 * @param input_length  Size of the input in bytes.
 */
#define PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(input_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_cipher_update().
 *
 * @details If the size of the output buffer is at least this large,
 *          it is guaranteed that @ref psa_cipher_update() will not fail
 *          due to an insufficient buffer size. The actual size of the
 *          output might be smaller in any given call.
 *
 *          See also @ref PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE.
 *
 * @param   key_type        A symmetric key type that is compatible with algorithm alg.
 * @param   alg             A cipher algorithm (PSA_ALG_XXX value such that
 *                          @ref PSA_ALG_IS_CIPHER(alg) is true).
 * @param   input_length    Size of the input in bytes.
 *
 * @return  A sufficient output size for the specified key type and algorithm.
 *          0 if the key type or cipher algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
/* implementation-defined value */

/**
 * @brief   The size of the algorithm field that is part of the output of @ref psa_hash_suspend(),
 *          in bytes.
 *
 * @details Applications can use this value to unpack the hash suspend state that is output by
 *          @ref psa_hash_suspend().
 */
#define PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH ((size_t)4)

/**
 * @brief   The size of the hash-state field that is part of the output of @ref psa_hash_suspend(),
 *          in bytes.
 *
 * @details Applications can use this value to unpack the hash suspend state that is output by
 *          @ref psa_hash_suspend().
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  The size, in bytes, of the hash-state field of the hash suspend state for the specified
 *          hash algorithm.
 *          0 if the hash algorithm is not recognized or not supported.
 */
#define PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) \
/* specification-defined value */

/**
 * @brief   The size of the input-length field that is part of the output of
 *          @ref psa_hash_suspend(), in bytes.
 *
 * @details Applications can use this value to unpack the hash suspend state that is output
 *          by @ref psa_hash_suspend().
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  The size, in bytes, of the input-length field of the hash suspend state for the
 *          specified hash algorithm.
 *          0 i f the hash algorithm is not recognized or not supported.
 */
#define PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) \
/* specification-defined value */

/**
 * @brief   A sufficient hash suspend state buffer size for @ref psa_hash_suspend(),
 *          for any supported hash algorithms.
 *
 * @details If the size of the hash state buffer is at least this large, it is guaranteed that
 *          @ref psa_hash_suspend() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_HASH_SUSPEND_OUTPUT_SIZE().
 */
#define PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient hash suspend state buffer size for @ref psa_hash_suspend(), in bytes.
 *
 * @details If the size of the hash state buffer is at least this large, it is guaranteed that
 *          @ref psa_hash_suspend() will not fail due to an insufficient buffer size. The actual
 *          size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE.
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(alg) is true.
 *
 * @return  A sufficient output size for the algorithm.
 *          0 if the hash algorithm is not recognized, or is not supported by
 *          @ref psa_hash_suspend().
 *
 *          For a supported hash algorithm alg, the following expression is true:
 *          @code
 *          PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) == PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH +
 *                                               PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) +
 *                                               PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) +
 *                                               PSA_HASH_BLOCK_LENGTH(alg) - 1
 *          @endcode
 */
#define PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) /* specification-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_asymmetric_decrypt(),
 *          for any of the supported key types and asymmetric encryption algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_decrypt() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE().
 */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE \
/* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_asymmetric_decrypt().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_decrypt() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    An asymmetric key type, either a key pair or a public key.
 * @param   key_bits    The size of the key in bits.
 * @param   alg         An asymmetric encryption algorithm: a value of type psa_algorithm_t such
 *                      that @ref PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(@p alg) is true.
 *
 * @return  A sufficient output buffer size for the specified asymmetric encryption algorithm
 *          and key parameters.
 *          0 if the asymmetric encryption algorithm and key parameters are not supported.
 *          Unspecified if the parameters are not valid.
 */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_asymmetric_encrypt(),
 *          for any of the supported key types and asymmetric encryption algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_encrypt() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE().
 */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_asymmetric_encrypt().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_encrypt() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    An asymmetric key type, either a key pair or a public key.
 * @param   key_bits    The size of the key in bits.
 * @param   alg         An asymmetric encryption algorithm: a value of type psa_algorithm_t
 *                      such that @ref PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(@p alg) is true.
 *
 * @return  A sufficient output buffer size for the specified asymmetric encryption algorithm
 *          and key parameters.
 *          0 if the asymmetric encryption algorithm and key parameters are not supported.
 *          Unspecified if the parameters are not valid.
 */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
/* implementation-defined value */

/**
 * @brief   Maximum size of the export encoding of an ECC keypair.
 *
 * @details The representation of an ECC keypair follows
 *          https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html#key-formats
 *          and is dependent on the family:
 *          - for twisted Edwards curves: 32B
 *          - for Weierstrass curves: `ceiling(m/8)`-byte string, big-endian
 *            where m is the bit size associated with the curve.
 */
#define PSA_KEY_EXPORT_ECC_KEY_MAX_SIZE(key_type, key_bits)                  \
    (size_t)\
    (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_TWISTED_EDWARDS ? 32 : \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_SECP_R1 ? PSA_BITS_TO_BYTES(key_bits) : \
     0))

/**
 * @brief   Sufficient output buffer size for @ref psa_export_key().
 *
 * @details The following code illustrates how to allocate enough memory to export a key by
 *          querying the key type and size at runtime.
 *
 *          @code
 *          @ref psa_key_attributes_t attributes = @ref PSA_KEY_ATTRIBUTES_INIT;
 *          @ref psa_status_t status;
 *          status = @ref psa_get_key_attributes(key, &attributes);
 *          if (status != @ref PSA_SUCCESS)
 *              handle_error(...);
 *          @ref psa_key_type_t key_type = @ref psa_get_key_type(&attributes);
 *          size_t key_bits = @ref psa_get_key_bits(&attributes);
 *          size_t buffer_size = @ref PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits);
 *          @ref psa_reset_key_attributes(&attributes);
 *          uint8_t *buffer = malloc(buffer_size);
 *          if (buffer == NULL)
 *              handle_error(...);
 *          size_t buffer_length;
 *          status = @ref psa_export_key(key, buffer, buffer_size, &buffer_length);
 *          if (status != @ref PSA_SUCCESS)
 *              handle_error(...);
 *          @endcode
 *
 *          See also @ref PSA_EXPORT_KEY_PAIR_MAX_SIZE and @ref PSA_EXPORT_PUBLIC_KEY_MAX_SIZE.
 *
 * @param   key_type A supported key type.
 * @param   key_bits The size of the key in bits.
 *
 * @return  If the parameters are valid and supported, return a buffer size in bytes that
 *          guarantees that @ref psa_export_key() or @ref psa_export_public_key() will not fail
 *          with @ref PSA_ERROR_BUFFER_TOO_SMALL.
 *          0 if the parameters are a valid combination that is not supported by the implementation.
 *          Unspecified if the parameters are not valid.
 */
#define PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits) \
    (PSA_KEY_TYPE_IS_PUBLIC_KEY(key_type) ? PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits) : \
    (PSA_KEY_TYPE_IS_ECC(key_type) ? PSA_KEY_EXPORT_ECC_KEY_MAX_SIZE(key_type, key_bits) : \
     0))

/**
 * @brief   Check whether the key size is a valid ECC size for key type.
 *
 * @param   type key type of of type @ref psa_key_type_t
 * @param   bits Key size of type @ref psa_key_bits_t
 */
#define PSA_ECC_KEY_SIZE_IS_VALID(type, bits) \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(type) == PSA_ECC_FAMILY_TWISTED_EDWARDS ?  \
        (bits == 255) :                                                     \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(type) == PSA_ECC_FAMILY_SECP_R1 ?          \
        (bits == 128 || \
         bits == 192 || \
         bits == 224 || \
         bits == 256 || \
         bits == 384) : \
    0))

/**
 * @brief   The maximum size of an asymmetric private key.
 */
#define PSA_MAX_PRIV_KEY_SIZE   (PSA_BYTES_TO_BITS(CONFIG_PSA_MAX_KEY_SIZE))

/**
 * @brief   Sufficient buffer size for exporting any asymmetric key pair.
 *
 * @details This value must be a sufficient buffer size when calling @ref psa_export_key() to
 *          export any asymmetric key pair that is supported by the implementation, regardless of
 *          the exact key type and key size.
 *
 *          See also @ref PSA_EXPORT_KEY_OUTPUT_SIZE().
 */
#if (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1) || \
     IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256))
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE \
    (PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_ECC_FAMILY_SECT_R1, 256))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519))
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE \
    (PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_ECC_FAMILY_TWISTED_EDWARDS, 255))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1))
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE \
    (PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_ECC_FAMILY_SECT_R1, 192))
#else
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE 0
#endif

/**
 * @brief   Maximum size of the export encoding of an ECC public key.
 *
 * @details The representation of an ECC public key is dependent on the family:
 *          - for twisted Edwards curves: 32B
 *          - for Weierstrass curves:
 *            - The byte 0x04;
 *            - `x_P` as a `ceiling(m/8)`-byte string, big-endian;
 *            - `y_P` as a `ceiling(m/8)`-byte string, big-endian;
 *            - where m is the bit size associated with the curve.
 *            - 1 byte + 2 * point size.
 */
#define PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(key_type, key_bits)                  \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_TWISTED_EDWARDS ? 32 : \
     ((size_t)(2 * PSA_BITS_TO_BYTES(key_bits) + 1)))

/**
 * @brief   Sufficient output buffer size for @ref psa_export_public_key().
 *
 * @details This macro returns a compile-time constant if its arguments are
 *          compile-time constants.
 *
 * @warning This macro may evaluate its arguments multiple times or
 *          zero times, so you should not pass arguments that contain
 *          side effects.
 *
 *          The following code illustrates how to allocate enough memory to export
 *          a public key by querying the key type and size at runtime.
 *
 *          @code
 *          @ref psa_key_attributes_t attributes = @ref PSA_KEY_ATTRIBUTES_INIT;
 *          @ref psa_status_t status;
 *          status = @ref psa_get_key_attributes(key, &attributes);
 *          if (status != @ref PSA_SUCCESS) handle_error(...);
 *          @ref psa_key_type_t key_type = @ref psa_get_key_type(&attributes);
 *          size_t key_bits = @ref psa_get_key_bits(&attributes);
 *          size_t buffer_size = @ref PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits);
 *          @ref psa_reset_key_attributes(&attributes);
 *          uint8_t *buffer = malloc(buffer_size);
 *          if (buffer == NULL) handle_error(...);
 *          size_t buffer_length;
 *          status = @ref psa_export_public_key(key, buffer, buffer_size, &buffer_length);
 *          if (status != @ref PSA_SUCCESS) handle_error(...);
 *          @endcode
 *
 * @param   key_type    A public key or key pair key type.
 * @param   key_bits    The size of the key in bits.
 *
 * @return  A buffer size in bytes that guarantees that @ref psa_export_public_key() will not fail
 *          with @ref PSA_ERROR_BUFFER_TOO_SMALL.
 *          0 if the parameters are a valid combination that is not supported.
 *          Unspecified if the parameters are not valid, the return value is unspecified.
 *          If the parameters are valid and supported, return the same result as
 *          @ref PSA_EXPORT_KEY_OUTPUT_SIZE(
 *          @ref PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(@p key_type), @p key_bits).
 */
#define PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits)                           \
    (PSA_KEY_TYPE_IS_ECC(key_type) ? PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(key_type, key_bits) : \
     0)

/**
 * @brief   Sufficient buffer size for exporting any asymmetric public key.
 *
 * @details This macro expands to a compile-time constant integer. This value is
 *          a sufficient buffer size when calling @ref psa_export_key() or
 *          @ref psa_export_public_key() to export any asymmetric public key,
 *          regardless of the exact key type and key size.
 *
 *          See also @ref PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(@p key_type, @p key_bits).
 */
#if (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1) || \
     IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
    (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_ECC_FAMILY_SECT_R1, 256))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
    (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_ECC_FAMILY_SECT_R1, 192))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
    (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_ECC_FAMILY_TWISTED_EDWARDS, 255))
#else
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE 0
#endif

/**
 * @brief   The maximum size of an asymmetric private key buffer. If only a secure element driver is
 *          present, the private key will always be stored in a key slot and PSA Crypto will only
 *          allocate memory for an 8 Byte key slot number.
 */
#define PSA_MAX_PRIV_KEY_BUFFER_SIZE (PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE))

/**
 * @brief   The maximum size of an asymmetric private key pair.
 */
#define PSA_MAX_ASYMMETRIC_KEYPAIR_SIZE     (PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE) + \
                                             PSA_EXPORT_PUBLIC_KEY_MAX_SIZE)

/**
 * @brief   The maximum size of the used key data.
 */
#if IS_USED(MODULE_PSA_ASYMMETRIC)
#define PSA_MAX_KEY_DATA_SIZE  (PSA_EXPORT_PUBLIC_KEY_MAX_SIZE)
#else
#define PSA_MAX_KEY_DATA_SIZE  (CONFIG_PSA_MAX_KEY_SIZE)
#endif

/**
 * @brief   The maximum size of an unstructured key.
 */
#define PSA_MAX_UNSTRUCTURED_KEY_SIZE (CONFIG_PSA_MAX_KEY_SIZE)

/**
 * @brief   ECDSA signature size for a given curve bit size
 *
 * @note    This macro returns a compile-time constant if its argument is one.
 *
 * @param   curve_bits    Curve size in bits.
 *
 * @return  Signature size in bytes.
 */
#define PSA_ECDSA_SIGNATURE_SIZE(curve_bits)    \
    ((size_t)(PSA_BITS_TO_BYTES(curve_bits) * 2))

/**
 * @brief   Sufficient signature buffer size for @ref psa_sign_message() and @ref psa_sign_hash().
 *
 * @details If the size of the signature buffer is at least this large, it is guaranteed that
 *          @ref psa_sign_message() and @ref psa_sign_hash() will not fail due to an insufficient
 *          buffer size. The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_SIGNATURE_MAX_SIZE.
 *
 * @param   key_type An asymmetric key type. This can be a key pair type or a public key type.
 * @param   key_bits The size of the key in bits.
 * @param   alg      The signature algorithm.
 *
 * @return  A sufficient signature buffer size for the specified asymmetric signature algorithm and
 *          key parameters.
 *          0 if algorithm and key parameters are not supported.
 *          If the parameters are not valid, the return value is unspecified.
 */
#define PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)        \
    (PSA_KEY_TYPE_IS_ECC(key_type) ? PSA_ECDSA_SIGNATURE_SIZE(key_bits) : \
     ((void)alg, 0))

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_CRYPTO_SIZES_H */
/** @} */
