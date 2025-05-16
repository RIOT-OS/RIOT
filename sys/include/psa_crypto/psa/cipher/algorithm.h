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
 * @file        cipher/algorithm.h
 * @brief       Cipher algorithm definitions for the PSA Crypto API
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
 * @brief   Category for cipher algorithms.
 */
#define PSA_ALG_CATEGORY_CIPHER ((psa_algorithm_t)0x04000000)

/**
 * @brief   Whether the specified algorithm is a symmetric cipher algorithm.
 *
 * @param   alg An algorithm identifier (value of type @ref psa_algorithm_t).
 *
 * @return  1 if alg is a symmetric cipher algorithm,
 *          0 if algorithm is not recognized or not supported
 */
#define PSA_ALG_IS_CIPHER(alg) (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_CIPHER)

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
#define PSA_ALG_IS_STREAM_CIPHER(alg) (((alg) & 0x7f800000) == 0x04800000)

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
#define PSA_ALG_STREAM_CIPHER ((psa_algorithm_t)0x04800100)

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
#define PSA_ALG_CTR ((psa_algorithm_t)0x04c01000)

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
#define PSA_ALG_CFB ((psa_algorithm_t)0x04c01100)

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
#define PSA_ALG_OFB ((psa_algorithm_t)0x04c01200)

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
#define PSA_ALG_XTS ((psa_algorithm_t)0x0440ff00)

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
#define PSA_ALG_CBC_NO_PADDING ((psa_algorithm_t)0x04404000)

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
#define PSA_ALG_CBC_PKCS7 ((psa_algorithm_t)0x04404100)

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
#define PSA_ALG_ECB_NO_PADDING ((psa_algorithm_t)0x04404400)

#ifdef __cplusplus
}
#endif

/** @} */
