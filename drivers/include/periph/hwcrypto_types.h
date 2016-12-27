/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_hwcrypt Hardware Crypto
 * @ingroup     drivers_periph
 * @brief       Low-level hardware crypto type definitions
 *
 * @{
 * @file
 * @brief       Low-level hardware crypto type definitions
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef HWCRYPTO_TYPES_H
#define HWCRYPTO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Supported ciphers.
 *
 * Define HAVE_HWCRYPTO_xxx in your target's periph_cpu.h if needed.
 *
 * Extend at will.
 */
typedef enum {
#ifdef HAVE_HWCRYPTO_AES128
    HWCRYPTO_AES128,                  /**< AES128 cipher */
#endif
#ifdef HAVE_HWCRYPTO_AES192
    HWCRYPTO_AES192,                  /**< AES192 cipher */
#endif
#ifdef HAVE_HWCRYPTO_AES256
    HWCRYPTO_AES256,                  /**< AES256 cipher */
#endif
#ifdef HAVE_HWCRYPTO_DES
    HWCRYPTO_DES,                     /**< DES cipher */
#endif
#ifdef HAVE_HWCRYPTO_3DES
    HWCRYPTO_3DES,                    /**< 3DES cipher */
#endif
#ifdef HAVE_HWCRYPTO_TWOFISH
    HWCRYPTO_TWOFISH,                 /**< Two-fish cipher */
#endif
#ifdef HAVE_HWCRYPTO_RSA512
    HWCRYPTO_RSA512,                  /**< RSA512 cipher */
#endif
#ifdef HAVE_HWCRYPTO_RSA1024
    HWCRYPTO_RSA1024,                 /**< RSA1024 cipher */
#endif
#ifdef HAVE_HWCRYPTO_RSA2048
    HWCRYPTO_RSA2048,                 /**< RSA2048 cipher */
#endif
#ifdef HAVE_HWCRYPTO_RSA4096
    HWCRYPTO_RSA4096,                 /**< RSA4096 cipher */
#endif
#ifdef HAVE_HWCRYPTO_ECC128
    HWCRYPTO_ECC128,                  /**< ECC128 cipher */
#endif
#ifdef HAVE_HWCRYPTO_ECC192
    HWCRYPTO_ECC192,                  /**< ECC192 cipher */
#endif
#ifdef HAVE_HWCRYPTO_ECC256
    HWCRYPTO_ECC256,                  /**< ECC256 cipher */
#endif
    HWCRYPT_CIPHER_NUMOF              /**< number of ciphers available */
} hwcrypto_cipher_t;

/**
 * @brief   Supported hashing algorithms.
 *
 * Define HAVE_HWCRYPTO_xxx in your CPUs periph_cpu.h if needed.
 *
 * Extend at will.
 */
typedef enum {
#ifdef HAVE_HWCRYPTO_MD4
    HWCRYPTO_MD4,                     /**< MD4 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_MD5
    HWCRYPTO_MD5,                     /**< MD5 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_SHA1
    HWCRYPTO_SHA1,                    /**< SHA1 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_SHA3
    HWCRYPTO_SHA3,                    /**< SHA3 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_SHA224
    HWCRYPTO_SHA224,                  /**< SHA224 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_SHA256
    HWCRYPTO_SHA256,                  /**< SHA256 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_SHA384
    HWCRYPTO_SHA384,                  /**< SHA384 hashing algorithm */
#endif
#ifdef HAVE_HWCRYPTO_SHA512
    HWCRYPTO_SHA512,                  /**< SHA512 hashing algorithm */
#endif
    HWCRYPT_HASH_NUMOF                /**< number of hashes available */
} hwcrypto_hash_t;

/**
 * @brief   Setup options for cipher and hash methods.
 *
 * Extend at will.
 */
typedef enum {
    HWCRYPTO_OPT_KEY,
    HWCRYPTO_OPT_IV,
    HWCRYPTO_OPT_COUNTER,
    HWCRYPTO_OPT_PADDING,
} hwcrypto_opt_t;

/**
 * @brief   Supported cipher modes of operation, to be used with
 *          @p HWCRYPTO_OPT_MODE.
 *
 * Extend at will.
 */
typedef enum {
    HWCRYPTO_MODE_NONE,               /**< no mode */
    HWCRYPTO_MODE_ECB,                /**< electronic codebook mode */
    HWCRYPTO_MODE_CBC,                /**< cipher block chaining mode */
    HWCRYPTO_MODE_CFB,                /**< cipher feedback mode */
    HWCRYPTO_MODE_OFB,                /**< output feedback mode */
    HWCRYPTO_MODE_CTR                 /**< counter mode */
} hwcrypto_mode_t;

/**
 * @brief   Cipher context type for ciphers.
 *
 * The context should store the selected cipher. If needed, the context should
 * provide a buffer to store a key.
 */
typedef struct {
    hwcrypto_cipher_t cipher;         /**< selected cipher method */
    hwcrypto_mode_t mode;             /**< selected cipher mode */
} hwcrypto_cipher_context_t;

/**
 * @brief   Default context type for hashes
 *
 * The context should store the selected hashing algorithm. If needed the
 * context should provide a buffer for the intermediate digest.
 */
typedef struct {
    hwcrypto_hash_t hash;             /**< selected hash method */
} hwcrypto_hash_context_t;

#ifdef __cplusplus
}
#endif

#endif /* HWCRYPTO_TYPES_H */
/** @} */
