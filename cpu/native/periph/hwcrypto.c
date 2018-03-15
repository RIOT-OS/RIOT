/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     native_cpu
 * @ingroup     drivers_periph_hwcrypto
 *
 * @{
 *
 * @file
 * @brief       Hardware crypto driver implementation for native
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <string.h>

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>

#include "mutex.h"
#include "assert.h"

#include "periph/hwcrypto.h"

/**
 * @brief   Type definition of the hardware crypto device state.
 */
typedef union {
    struct {
        hwcrypto_cipher_t cipher;
        hwcrypto_mode_t mode;
        uint8_t key[32] __attribute__((aligned));
        union {
            uint8_t iv[16] __attribute__((aligned));
            uint8_t counter[16] __attribute__((aligned));
        } opts;
    } cipher;
    struct {
        hwcrypto_hash_t hash;
        union {
            SHA_CTX sha1;
            SHA256_CTX sha256;
            SHA512_CTX sha512;
        } sha;
    } hash;
} state_t;

/**
 * @brief   Global lock to ensure mutual exclusive access to crypto hardware.
 */
static mutex_t hwcrypto_lock[HWCRYPTO_NUMOF];

/**
 * @brief   Hardware crypto device state.
 */
static state_t state[HWCRYPTO_NUMOF];

void hwcrypto_init(hwcrypto_t dev)
{
    assert(dev < HWCRYPTO_NUMOF);

    /* initialize lock */
    mutex_init(&hwcrypto_lock[dev]);

    /* clear the state */
    memset(&state[dev], 0, sizeof(state_t));
}

int hwcrypto_cipher_init(hwcrypto_t dev, hwcrypto_cipher_t cipher, hwcrypto_mode_t mode)
{
    /* check if cipher is supported */
    if (!hwcrypto_cipher_supported(dev, cipher)) {
        return HWCRYPTO_NOTSUP;
    }

    /* initialize state */
    state[dev].cipher.cipher = cipher;
    state[dev].cipher.mode = mode;

    return HWCRYPTO_OK;
}

int hwcrypto_cipher_set(hwcrypto_t dev, hwcrypto_opt_t option, const void *value, uint32_t size)
{
    switch (option) {
        case HWCRYPTO_OPT_KEY:
            if (state[dev].cipher.cipher == HWCRYPTO_AES128 && size == 16) {
                memcpy(state[dev].cipher.key, value, 16);
            }
            else if (state[dev].cipher.cipher == HWCRYPTO_AES256 && size == 32) {
                memcpy(state[dev].cipher.key, value, 32);
            }
            else {
                return HWCRYPTO_INVALID;
            }

            break;
        case HWCRYPTO_OPT_IV:
            if (state[dev].cipher.mode != HWCRYPTO_MODE_CBC &&
                state[dev].cipher.mode != HWCRYPTO_MODE_OFB &&
                state[dev].cipher.mode != HWCRYPTO_MODE_CFB
                ) {
                return HWCRYPTO_NOTSUP;
            }

            if (size != 16) {
                return HWCRYPTO_INVALID;
            }

            memcpy(state[dev].cipher.opts.iv, value, 16);
            break;
        case HWCRYPTO_OPT_COUNTER:
            if (state[dev].cipher.mode != HWCRYPTO_MODE_CTR) {
                return HWCRYPTO_NOTSUP;
            }

            if (size != 16) {
                return HWCRYPTO_INVALID;
            }

            memcpy(state[dev].cipher.opts.counter, value, 16);
            break;
        default:
            return HWCRYPTO_NOTSUP;
    }

    return HWCRYPTO_OK;
}

static int hwcrypto_cipher_encrypt_decrypt(hwcrypto_t dev, const uint8_t *plain_block, uint8_t *cipher_block, uint32_t block_size, bool encrypt)
{
    (void) dev;
    (void) plain_block;
    (void) cipher_block;
    (void) block_size;
    (void) encrypt;

    return HWCRYPTO_NOTSUP;
}

int hwcrypto_cipher_encrypt(hwcrypto_t dev, const uint8_t *plain_block, uint8_t *cipher_block, uint32_t block_size)
{
    return hwcrypto_cipher_encrypt_decrypt(dev, plain_block, cipher_block, block_size, true);
}

int hwcrypto_cipher_decrypt(hwcrypto_t dev, const uint8_t *cipher_block, uint8_t *plain_block, uint32_t block_size)
{
    return hwcrypto_cipher_encrypt_decrypt(dev, cipher_block, plain_block, block_size, false);
}

int hwcrypto_hash_init(hwcrypto_t dev, hwcrypto_hash_t hash)
{
    /* check if hash algorithm is supported */
    if (!hwcrypto_hash_supported(dev, hash)) {
        return HWCRYPTO_NOTSUP;
    }

    /* initialize state */
    state[dev].hash.hash = hash;

    switch (hash) {
        case HWCRYPTO_SHA1:
            SHA1_Init(&state[dev].hash.sha.sha1);
            break;
        case HWCRYPTO_SHA224:
            SHA224_Init(&state[dev].hash.sha.sha256);
            break;
        case HWCRYPTO_SHA256:
            SHA256_Init(&state[dev].hash.sha.sha256);
            break;
        case HWCRYPTO_SHA384:
            SHA384_Init(&state[dev].hash.sha.sha512);
            break;
        case HWCRYPTO_SHA512:
            SHA512_Init(&state[dev].hash.sha.sha512);
            break;
        default:
            return HWCRYPTO_NOTSUP;
    }

    return HWCRYPTO_OK;
}

int hwcrypto_hash_update(hwcrypto_t dev, const uint8_t *block, uint32_t block_size)
{
    switch (state[dev].hash.hash) {
        case HWCRYPTO_SHA1:
            SHA1_Update(&state[dev].hash.sha.sha1, block, block_size);
            break;
        case HWCRYPTO_SHA224:
            SHA224_Update(&state[dev].hash.sha.sha256, block, block_size);
            break;
        case HWCRYPTO_SHA256:
            SHA256_Update(&state[dev].hash.sha.sha256, block, block_size);
            break;
        case HWCRYPTO_SHA384:
            SHA384_Update(&state[dev].hash.sha.sha512, block, block_size);
            break;
        case HWCRYPTO_SHA512:
            SHA512_Update(&state[dev].hash.sha.sha512, block, block_size);
            break;
        default:
            return HWCRYPTO_NOTSUP;
    }

    return block_size;
}

int hwcrypto_hash_final(hwcrypto_t dev, uint8_t *result, uint32_t result_size)
{
    uint8_t tmp[64];

    switch (state[dev].hash.hash) {
        case HWCRYPTO_SHA1:
            if (result_size > 20) {
                return HWCRYPTO_INVALID;
            }

            SHA1_Final(tmp, &state[dev].hash.sha.sha1);
            break;
        case HWCRYPTO_SHA224:
            if (result_size > 28) {
                return HWCRYPTO_INVALID;
            }

            SHA224_Final(tmp, &state[dev].hash.sha.sha256);
            break;
        case HWCRYPTO_SHA256:
            if (result_size > 32) {
                return HWCRYPTO_INVALID;
            }

            SHA256_Final(tmp, &state[dev].hash.sha.sha256);
            break;
        case HWCRYPTO_SHA384:
            if (result_size > 48) {
                return HWCRYPTO_INVALID;
            }

            SHA384_Final(tmp, &state[dev].hash.sha.sha512);
            break;
        case HWCRYPTO_SHA512:
            if (result_size > 64) {
                return HWCRYPTO_INVALID;
            }

            SHA512_Final(tmp, &state[dev].hash.sha.sha512);
            break;
        default:
            return HWCRYPTO_NOTSUP;
    }

    /* copy the number of bytes requested */
    memcpy(result, tmp, result_size);

    return result_size;
}

int hwcrypto_acquire(hwcrypto_t dev)
{
    mutex_lock(&hwcrypto_lock[dev]);

    return HWCRYPTO_OK;
}

int hwcrypto_release(hwcrypto_t dev)
{
    mutex_unlock(&hwcrypto_lock[dev]);

    return HWCRYPTO_OK;
}
