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
 * @brief       Low-level hardware crypto peripheral driver
 *
 * Modern CPUs may be equipped with a hardware crypto peripheral to speed up
 * cryptographical operations such as (as)symmetric ciphers and hashing
 * algorithms. With no or little intervention, the CPU can typically execute
 * more operations than the software alternative.
 *
 * This driver provides an abstract for the different ciphers and hashes that
 * may be supported. Using HAVE_HWCRYPTO_xxx defines, software may choose to
 * use the hardware crypto alternative, or fall back to a software variant.
 *
 * Current interface is optimized for (as)symmetric block ciphers and block
 * hashes.
 *
 * All operations block until they are finished. Operations may change the
 * internal peripheral state (e.g. load the key, keep the digest interally),
 * therefore must be used to control mutual exclusive access.
 *
 * @{
 * @file
 * @brief       Low-level hardware crypto driver interface definitions
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef HWCRYPTO_H
#define HWCRYPTO_H

#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard file in case no HWCRYPTO device is defined */
#if HWCRYPTO_NUMOF

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
    HWCRYPTO_OPT_MODE
} hwcrypto_opt_t;

/**
 * @brief   Supported cipher modes of operation, to be used with
 *          @p HWCRYPTO_OPT_MODE.
 *
 * Extend at will.
 */
typedef enum {
    HWCRYPTO_MODE_ECB,                /**< electronic codebook mode */
    HWCRYPTO_MODE_CBC,                /**< cipher block chaining mode */
    HWCRYPTO_MODE_CFB,                /**< cipher feedback mode */
    HWCRYPTO_MODE_OFB,                /**< output feedback mode */
    HWCRYPTO_MODE_CTR                 /**< counter mode */
} hwcrypto_mode_t;

/**
 * @brief   Default type for one single byte for blocks of cipher or plain
 *          data.
 *
 * Some crypto devices need aligned blocks, therefore override if needed.
 *
 * @{
 */
#ifndef HAVE_HWCRYPTO_BLOCK_T
typedef uint8_t hwcrypto_block_t; /* default to no alignment */
#endif
/** @} */

/**
 * @brief   Default cipher context type for ciphers with keys up to 256 bits.
 *
 * The context should store the selected cipher. If needed, the context should
 * provide a buffer to store a key.
 *
 * For performance reasons, the default context type aligns the key.
 */
#ifndef HAVE_HWCRYPTO_CIPHER_CONTEXT_T
typedef struct {
    hwcrypto_cipher_t cipher;         /**< selected cipher method */
} hwcrypto_cipher_context_t;
#endif

/**
 * @brief   Default hash context type for hashes with digests up to 256 bits.
 *
 * The context should store the selected hashing algorithm. If needed the
 * context should provide a buffer for the intermediate digest.
 *
 * For performance reasons, the default context type aligns the digest.
 */
#ifndef HAVE_HWCRYPTO_HASH_CONTEXT_T
typedef struct {
    hwcrypto_hash_t hash;             /**< selected hash method */
} hwcrypto_hash_context_t;
#endif

/**
 * @brief   Initialize the hardware crypto peripheral.
 *
 * The peripheral should be turned on in this method.
 *
 * @return                  0 on success
 * @return                  -1 if an error occurs
 */
int hwcrypto_init(void);

/**
 * @brief   Initialize hardware crypto for cipher.
 *
 * The implementation must use a key (size) as defined by the cipher. It must
 * not apply a different key scheduling algorithm.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively. A context may become invalid after the peripheral is powered
 * off and on again.
 *
 * @param[inout] context    cipher context to initialize
 * @param[in] cipher        cipher to initialize
 *
 * @return                  0 on success
 * @return                  -1 if cipher is not supported
 */
int hwcrypto_cipher_init(hwcrypto_cipher_context_t* context,
                         hwcrypto_cipher_t cipher);

/**
 * @brief   Set an option for the given cipher context.
 *
 * If the given option is not supported by the 
 *
 * @param[inout] context    initialized cipher context
 * @param[in] option        option to set from @p hwcrypto_opt_t
 * @param[in] value         pointer to option value
 * @param[in] size          option size
 *
 * @return                  0 on success
 * @return                  -1 if option is not supported or applicable
 * @return                  -2 if value is not supported or applicable
 */
int hwcrypto_cipher_set(hwcrypto_cipher_context_t* context,
                        hwcrypto_opt_t option,
                        const void* value,
                        uint32_t size);

/**
 * @brief   Perform a cipher encrypt operation for an initialized context.
 *
 * The plain block may overlap with the cipher block, but not partially.
 *
 * The method must accept block sizes that are a multiple of the smallest block
 * size. This allows for minimal CPU intervention for large blocks of data.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively. A context may become invalid after the peripheral is powered
 * off and on again.
 *
 * @param[inout] context    initialized cipher context
 * @param[in] plain_block   the plain input buffer
 * @param[out] cipher_block the cipher output buffer (may overlap plain_block)
 * @param[in] block_size    size of the plain_block and cipher_block in bytes
 *
 * @return                  0 on success
 * @return                  -1 if cipher is not supported
 * @return                  -2 if block_size is invalid
 */
int hwcrypto_cipher_encrypt(hwcrypto_cipher_context_t* context,
                            const hwcrypto_block_t *plain_block,
                            hwcrypto_block_t *cipher_block,
                            uint32_t block_size);

/**
 * @brief   Perform a cipher decrypt operation for an initialized context.
 *
 * The cipher block may overlap with the plain block, but not partially.
 *
 * The method must accept block sizes that are a multiple of the smallest block
 * size. This allows for minimal CPU intervention for large blocks of data.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * @param[inout] context    initialized cipher context
 * @param[in] cipher_block  the cipher input buffer
 * @param[out] plain_block  the plain output buffer (may overlap cipher_block)
 * @param[in] block_size    size of the cipher_block and plain_block in bytes
 *
 * @return                  0 on success
 * @return                  -1 if cipher is not supported
 * @return                  -2 if block_size is invalid
 */
int hwcrypto_cipher_decrypt(hwcrypto_cipher_context_t* context,
                            const hwcrypto_block_t *cipher_block,
                            hwcrypto_block_t *plain_block,
                            uint32_t block_size);

/**
 * @brief   Initialize hardware crypto for given hash algorithm.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively. A context may become invalid after the peripheral is powered
 * off and on inbetween.
 *
 * @param[inout] context    hash context
 * @param[in] hash          desired hash algorithm
 *
 * @return                  0 on success
 * @return                  -1 if hash is not supported
 * @return                  -2 if context could not be prepared
 */
int hwcrypto_hash_init(hwcrypto_hash_context_t* context, hwcrypto_hash_t hash);

/**
 * @brief   Update the digest with a block of data.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively. A context may become invalid after the peripheral is powered
 * off and on inbetween.
 *
 * @param[inout] context    initialized hash context
 * @param[in] block         block of data to update digest with
 * @param[in] block_size    number of bytes in data block
 *
 * @return                  number of bytes added to digest
 * @return                  -1 if hash is not supported
 * @return                  -2 if block_size is incorrect
 * @return                  -3 if digest could not be updated
 */
int hwcrypto_hash_update(hwcrypto_hash_context_t* context,
                         const hwcrypto_block_t* block,
                         uint32_t block_size);

/**
 * @brief   Finalize the hash and copy the result to result buffer.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * @param[inout] context    initialized hash context
 * @param[out] cipher       result output buffer
 * @param[in] result_size   number of bytes to copy from digest to result
 *
 * @return                  number of bytes copied
 * @return                  -1 if hash is not supported
 */
int hwcrypto_hash_final(hwcrypto_hash_context_t* context,
                        hwcrypto_block_t* result,
                        uint32_t result_size);

/**
 * @brief   Get mutually exclusive access to the hardware crypto peripheral.
 *
 * In case the peripheral is busy, this function will block until the
 * peripheral is available again.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int hwcrypto_acquire(void);

/**
 * @brief   Release the hardware crypto peripheral to be used by others.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int hwcrypto_release(void);

/**
 * @brief   Power on the hardware crypto peripheral.
 */
void hwcrypto_poweron(void);

/**
 * @brief   Power off the hardware crypto peripheral.
 */
void hwcrypto_poweroff(void);

#endif /* HWCRYPTO_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* HWCRYPTO_H */
/** @} */
