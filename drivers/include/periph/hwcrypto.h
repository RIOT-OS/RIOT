/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
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
 * more operations than the software alternative, using less instructions.
 *
 * This driver provides an abstract for the different ciphers and hashes that
 * may be supported. Using HAVE_HWCRYPTO_xxx defines, software may choose to
 * use the hardware crypto alternative, or fall back to a software variant if
 * not defined.
 *
 * Current interface is optimized for (as)symmetric block ciphers and block
 * hashes. It supports multiple hardware crypto peripherals, including limited
 * support.
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

#include <stdbool.h>
#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default hardware crypto device access macro
 */
#ifndef HWCRYPTO_DEV
#define HWCRYPTO_DEV(x)     (x)
#endif

/**
 * @brief   Hardware crypto type identifier
 */
#ifndef HAVE_HWCRYPTO_T
typedef unsigned int hwcrypto_t;
#endif

/**
 * @brief   Status codes used by the hardware crypto driver interface
 */
enum {
    HWCRYPTO_OK         =  0,   /**< everything went as planned */
    HWCRYPTO_NOTSUP     = -1,   /**< cipher, hash or argument not supported */
    HWCRYPTO_INVALID    = -2,   /**< argument value invalid */
    HWCRYPTO_ERROR      = -3,   /**< other error */
};

/**
 * @brief   Supported ciphers.
 *
 * Define HAVE_HWCRYPTO_xxx in periph_cpu.h for each supported cipher.
 *
 * Extend at will, but also update drivers/periph_common/hwcrypto.c.
 */
typedef enum {
    HWCRYPTO_AES128,                  /**< AES128 cipher */
    HWCRYPTO_AES192,                  /**< AES192 cipher */
    HWCRYPTO_AES256,                  /**< AES256 cipher */
    HWCRYPTO_DES,                     /**< DES cipher */
    HWCRYPTO_3DES,                    /**< 3DES cipher */
    HWCRYPTO_TWOFISH,                 /**< Two-fish cipher */
    HWCRYPTO_RSA512,                  /**< RSA512 cipher */
    HWCRYPTO_RSA1024,                 /**< RSA1024 cipher */
    HWCRYPTO_RSA2048,                 /**< RSA2048 cipher */
    HWCRYPTO_RSA4096,                 /**< RSA4096 cipher */
    HWCRYPTO_ECC128,                  /**< ECC128 cipher */
    HWCRYPTO_ECC192,                  /**< ECC192 cipher */
    HWCRYPTO_ECC256,                  /**< ECC256 cipher */
    HWCRYPT_CIPHER_NUMOF              /**< number of ciphers available */
} hwcrypto_cipher_t;

/**
 * @brief   Supported hashing algorithms.
 *
 * Define HAVE_HWCRYPTO_xxx in periph_cpu.h for each supported hashing
 * algorithm.
 *
 * Extend at will, but also update drivers/periph_common/hwcrypto.c.
 */
typedef enum {
    HWCRYPTO_MD4,                     /**< MD4 hashing algorithm */
    HWCRYPTO_MD5,                     /**< MD5 hashing algorithm */
    HWCRYPTO_SHA1,                    /**< SHA1 hashing algorithm */
    HWCRYPTO_SHA3,                    /**< SHA3 hashing algorithm */
    HWCRYPTO_SHA224,                  /**< SHA224 hashing algorithm */
    HWCRYPTO_SHA256,                  /**< SHA256 hashing algorithm */
    HWCRYPTO_SHA384,                  /**< SHA384 hashing algorithm */
    HWCRYPTO_SHA512,                  /**< SHA512 hashing algorithm */
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
 * @brief   Determine if the given hardware crypto peripheral supports the
 *          given cipher.
 *
 * This method should not require the peripheral to be initialized or acquired.
 * It should also not change its state.
 *
 * The method should return a deterministic result.
 *
 * @param[in] dev           the device
 * @param[in] cipher        cipher to check
 *
 * @return                  true if supported
 * @return                  false if unsupported
 */
bool hwcrypto_cipher_supported(hwcrypto_t dev, hwcrypto_cipher_t cipher);

/**
 * @brief   Determine if the given hardware crypto peripheral supports the
 *          given hashing algorithm.
 *
 * This method should not require the peripheral to be initialized or acquired.
 * It should also not change its state.
 *
 * The method should return a deterministic result.
 *
 * @param[in] dev           the device
 * @param[in] hash          hashing algorithm to check
 *
 * @return                  true if supported
 * @return                  false if unsupported
 */
bool hwcrypto_hash_supported(hwcrypto_t dev, hwcrypto_hash_t hash);

/**
 * @brief   Initialize the hardware crypto peripheral.
 *
 * The method should initialize the peripheral. It may power on the device if
 * needed, but it must ensure that the peripheral is powered off when exiting
 * this method.
 *
 * This function is intended to be called by the board initialization code
 * during system startup to prepare the (shared) hardware crypto device for
 * further usage. It uses the board specific initialization parameters as
 * defined in the board's `periph_conf.h`.
 *
 * Errors (e.g. invalid @p dev parameter) are not signaled through a return
 * value, but should be signaled using the assert() function internally.
 *
 * @param[in] dev           the device to initialize
 */
void hwcrypto_init(hwcrypto_t dev);

/**
 * @brief   Initialize hardware crypto device for a given encryption or
 *          decryption algorithm.
 *
 * The mode parameter can be used to change the cipher's mode of operation. If
 * a mode does not apply, use @p HWCRYPTO_MODE_NONE.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * @param[in] dev           the device to initialize
 * @param[in] cipher        cipher to initialize
 * @param[in] mode          cipher mode of operation
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_NOTSUP if cipher or mode is not supported
 * @return                  HWCRYPTO_ERROR if other error
 */
int hwcrypto_cipher_init(hwcrypto_t dev,
                         hwcrypto_cipher_t cipher,
                         hwcrypto_mode_t mode);

/**
 * @brief   Set an option for the given hardware crypto device in cipher mode.
 *
 * If the given option or value is not supported by the cipher, an error code
 * is returned.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * A hardware crypto device not initialized for cipher mode may produce
 * unexpected results.
 *
 * The peripheral's state may be undefined if this method is used after one or
 * more encryption or decryption round(s).
 *
 * @param[in] dev           the device to initialize
 * @param[in] option        option from @p hwcrypto_opt_t to initialize
 * @param[in] value         pointer to option value
 * @param[in] size          option size
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_NOTSUP if option not supported or
 *                          applicable
 * @return                  HWCRYPTO_INVALID if value is not supported or
 *                          applicable
 * @return                  HWCRYPTO_ERROR if other error
 */
int hwcrypto_cipher_set(hwcrypto_t dev,
                        hwcrypto_opt_t option,
                        const void* value,
                        uint32_t size);

/**
 * @brief   Perform a cipher encrypt operation for an initialized hardware
 *          crypto device in cipher mode.
 *
 * The plain block may overlap with the cipher block, but not partially.
 *
 * The method must accept block sizes that are a multiple of the smallest block
 * size. This allows for minimal CPU intervention for large blocks of data.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * A hardware crypto device not initialized for cipher mode may produce
 * unexpected results.
 *
 * @param[in] dev           the device to initialize
 * @param[in] plain_block   the plain input buffer
 * @param[out] cipher_block the cipher output buffer (may overlap plain_block)
 * @param[in] block_size    size of the plain_block and cipher_block in bytes
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_NOTSUP if cipher operation not supported
 * @return                  HWCRYPTO_INVALID if @p block_size is invalid
 * @return                  HWCRYPTO_ERROR if other error
 */
int hwcrypto_cipher_encrypt(hwcrypto_t dev,
                            const uint8_t *plain_block,
                            uint8_t *cipher_block,
                            uint32_t block_size);

/**
 * @brief   Perform a cipher decrypt operation for an initialized hardware
 *          crypto device in cipher mode
 *
 * The cipher block may overlap with the plain block, but not partially.
 *
 * The method must accept block sizes that are a multiple of the smallest block
 * size. This allows for minimal CPU intervention for large blocks of data.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * A hardware crypto device not initialized for cipher mode may produce
 * unexpected results.
 *
 * @param[in] dev           the device to initialize
 * @param[in] cipher_block  the cipher input buffer
 * @param[out] plain_block  the plain output buffer (may overlap cipher_block)
 * @param[in] block_size    size of the cipher_block and plain_block in bytes
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_NOTSUP if cipher operation not supported
 * @return                  HWCRYPTO_INVALID if @p block_size is invalid
 * @return                  HWCRYPTO_ERROR if other error
 */
int hwcrypto_cipher_decrypt(hwcrypto_t dev,
                            const uint8_t *cipher_block,
                            uint8_t *plain_block,
                            uint32_t block_size);

/**
 * @brief   Initialize hardware crypto device for a given hashing algorithm.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * @param[in] dev           the device to initialize
 * @param[in] hash          desired hash algorithm
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_NOTSUP if hash is not supported
 * @return                  HWCRYPTO_ERROR if other error
 */
int hwcrypto_hash_init(hwcrypto_t dev, hwcrypto_hash_t hash);

/**
 * @brief   Update the digest with a block of data.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * A hardware crypto device not initialized for hashing may produce unexpected
 * results.
 *
 * @param[in] dev           the device to initialize
 * @param[in] block         block of data to update digest with
 * @param[in] block_size    number of bytes in data block
 *
 * @return                  number of bytes added to digest
 * @return                  HWCRYPTO_NOTSUP if hashing operation not supported
 * @return                  HWCRYPTO_INVALID if block_size is incorrect
 * @return                  HWCRYPTO_ERROR if digest could not be updated
 */
int hwcrypto_hash_update(hwcrypto_t dev,
                         const uint8_t* block,
                         uint32_t block_size);

/**
 * @brief   Finalize the hash and copy the result to result buffer.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively.
 *
 * A hardware crypto device not initialized for hashing may produce unexpected
 * results.
 *
 * @param[in] dev           the device to initialize
 * @param[out] cipher       result output buffer
 * @param[in] result_size   number of bytes to copy from digest to result
 *
 * @return                  number of bytes copied
 * @return                  HWCRYPTO_NOTSUP if hash is not supported
 * @returned                HWCRYPTO_INVALID if @p result_size exceeds hash
 *                          size
 */
int hwcrypto_hash_final(hwcrypto_t dev,
                        uint8_t* result,
                        uint32_t result_size);

/**
 * @brief   Get mutually exclusive access to the hardware crypto peripheral.
 *
 * In case the peripheral is busy, this function will block until the
 * peripheral is available again.
 *
 * @param[in] dev           the device to initialize
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_ERROR on error
 */
int hwcrypto_acquire(hwcrypto_t dev);

/**
 * @brief   Release the hardware crypto peripheral to be used by others.
 *
 * @param[in] dev           the device to initialize
 *
 * @return                  HWCRYPTO_OK on success
 * @return                  HWCRYPTO_ERROR on error
 */
int hwcrypto_release(hwcrypto_t dev);

#ifdef __cplusplus
}
#endif

#endif /* HWCRYPTO_H */
/** @} */
