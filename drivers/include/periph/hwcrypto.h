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

#include "hwcrypto_types.h"

/* must be included after hwcrypto_types.h */
#include "periph_hwcrypto.h"

#ifdef __cplusplus
extern "C" {
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
 * The mode parameter can be used to change the cipher's mode of operation. If
 * a mode does not apply, use @p HWCRYPTO_MODE_NONE.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively. A context may become invalid after the peripheral is powered
 * off and on again.
 *
 * @param[inout] context    cipher context to initialize
 * @param[in] cipher        cipher to initialize
 * @param[in] mode          cipher mode of operation.
 *
 * @return                  0 on success
 * @return                  -1 if cipher is not supported
 */
int hwcrypto_cipher_init(hwcrypto_cipher_context_t* context,
                         hwcrypto_cipher_t cipher,
                         hwcrypto_mode_t mode);

/**
 * @brief   Set an option for the given cipher context.
 *
 * If the given option or value is not supported by the cipher, an error code
 * is returned.
 *
 * This operation may alter the peripheral's state, therefore it should be used
 * exclusively. A context may become invalid after the peripheral is powered
 * off and on again.
 *
 * The peripheral's state is undefined if this method is used after one or more
 * encryption or decryption round(s).
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
                            const uint8_t *plain_block,
                            uint8_t *cipher_block,
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
                            const uint8_t *cipher_block,
                            uint8_t *plain_block,
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
                         const uint8_t* block,
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
                        uint8_t* result,
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

#ifdef __cplusplus
}
#endif

#endif /* HWCRYPTO_H */
/** @} */
