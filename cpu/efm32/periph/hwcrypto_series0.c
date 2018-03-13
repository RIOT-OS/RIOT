/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_hwcrypto
 *
 * @{
 *
 * @file
 * @brief       Low-level hardware crypto driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <string.h>

#include "mutex.h"
#include "assert.h"

#include "periph/hwcrypto.h"

#include "em_cmu.h"
#include "em_aes.h"

/**
 * @brief   Type definition of the hardware crypto device state.
 */
typedef struct {
    hwcrypto_cipher_t cipher;
    hwcrypto_mode_t mode;
    uint8_t key[32] __attribute__((aligned));
    union {
        uint8_t iv[16] __attribute__((aligned));
        uint8_t counter[16] __attribute__((aligned));
    } opts;
} state_t;

/**
 * @brief   Type of a 128-bit counter.
 */
typedef struct {
    uint64_t hi;
    uint64_t lo;
} counter128_t;

/**
 * @brief   Global lock to ensure mutual exclusive access to hardware crypto
 *          device.
 */
static mutex_t hwcrypto_lock = MUTEX_INIT;

/**
 * @brief   Hardware crypto device state.
 */
static state_t state;

/**
 * @brief   Helper for incrementing a 128-bit counter.
 */
static void hwcrypto_cipher_increment(uint8_t *ctr)
{
    counter128_t *counter = (counter128_t *)ctr;

    /* on overflow of low, increment high */
    if (counter->lo == 0) {
        counter->hi++;
    }
}

int hwcrypto_init(hwcrypto_t dev)
{
    assert(dev < HWCRYPTO_NUMOF);

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_AES, true);

    return 0;
}

int hwcrypto_cipher_init(hwcrypto_t dev, hwcrypto_cipher_t cipher, hwcrypto_mode_t mode)
{
    (void) dev;

    /* check if cipher is supported */
    if (!hwcrypto_cipher_supported(dev, cipher)) {
        return -1;
    }

    /* initialize state */
    state.cipher = cipher;
    state.mode = mode;

    return 0;
}

int hwcrypto_cipher_set(hwcrypto_t dev, hwcrypto_opt_t option, const void *value, uint32_t size)
{
    (void) dev;

    switch (option) {
        case HWCRYPTO_OPT_KEY:
            if (state.cipher == HWCRYPTO_AES128 && size == 16) {
                memcpy(state.key, value, 16);
            }
            else if (state.cipher == HWCRYPTO_AES256 && size == 32) {
                memcpy(state.key, value, 32);
            }
            else {
                /* incorrect size */
                return -2;
            }

            break;
        case HWCRYPTO_OPT_IV:
            if (state.mode != HWCRYPTO_MODE_CBC &&
                state.mode != HWCRYPTO_MODE_OFB &&
                state.mode != HWCRYPTO_MODE_CFB
                ) {
                /* other modes don't use iv */
                return -1;
            }

            if (size != 16) {
                /* incorrect iv size */
                return -2;
            }

            memcpy(state.opts.iv, value, 16);
            break;
        case HWCRYPTO_OPT_COUNTER:
            if (state.mode != HWCRYPTO_MODE_CTR) {
                /* other modes don't use counter */
                return -1;
            }

            if (size != 16) {
                /* incorrect counter size */
                return -2;
            }

            memcpy(state.opts.counter, value, 16);
            break;
        default:
            /* option not supported */
            return -1;
    }

    return 0;
}

static int hwcrypto_cipher_encrypt_decrypt(const uint8_t *plain_block, uint8_t *cipher_block, uint32_t block_size, bool encrypt)
{
    /* blocks must be aligned */
    assert(!((intptr_t) plain_block & 0x03));
    assert(!((intptr_t) cipher_block & 0x03));

    /* block size must be multiple of 16 */
    if ((block_size % 16) != 0) {
        return -2;
    }

    switch (state.cipher) {
        case HWCRYPTO_AES128:
            switch (state.mode) {
                case HWCRYPTO_MODE_ECB:
                    AES_ECB128(cipher_block, plain_block, block_size, state.key, encrypt);
                    break;
                case HWCRYPTO_MODE_CBC:
                    AES_CBC128(cipher_block, plain_block, block_size, state.key, state.opts.iv, encrypt);
                    break;
                case HWCRYPTO_MODE_CFB:
                    AES_CFB128(cipher_block, plain_block, block_size, state.key, state.opts.iv, encrypt);
                    break;
                case HWCRYPTO_MODE_OFB:
                    AES_OFB128(cipher_block, plain_block, block_size, state.key, state.opts.iv);
                    break;
                case HWCRYPTO_MODE_CTR:
                    AES_CTR128(cipher_block, plain_block, block_size, state.key, state.opts.counter, hwcrypto_cipher_increment);
                    break;
                default:
                    return -1;
            }

            break;
        case HWCRYPTO_AES256:
            switch (state.mode) {
                case HWCRYPTO_MODE_ECB:
                    AES_ECB256(cipher_block, plain_block, block_size, state.key, encrypt);
                    break;
                case HWCRYPTO_MODE_CBC:
                    AES_CBC256(cipher_block, plain_block, block_size, state.key, state.opts.iv, encrypt);
                    break;
                case HWCRYPTO_MODE_CFB:
                    AES_CFB256(cipher_block, plain_block, block_size, state.key, state.opts.iv, encrypt);
                    break;
                case HWCRYPTO_MODE_OFB:
                    AES_OFB256(cipher_block, plain_block, block_size, state.key, state.opts.iv);
                    break;
                case HWCRYPTO_MODE_CTR:
                    AES_CTR256(cipher_block, plain_block, block_size, state.key, state.opts.counter, hwcrypto_cipher_increment);
                    break;
                default:
                    return -1;
            }

            break;
        default:
            return -1;
    }

    return block_size;
}

int hwcrypto_cipher_encrypt(hwcrypto_t dev, const uint8_t *plain_block, uint8_t *cipher_block, uint32_t block_size)
{
    (void) dev;

    return hwcrypto_cipher_encrypt_decrypt(plain_block, cipher_block, block_size, true);
}

int hwcrypto_cipher_decrypt(hwcrypto_t dev, const uint8_t *cipher_block, uint8_t *plain_block, uint32_t block_size)
{
    (void) dev;

    return hwcrypto_cipher_encrypt_decrypt(cipher_block, plain_block, block_size, false);
}

int hwcrypto_hash_init(hwcrypto_t dev, hwcrypto_hash_t hash)
{
    (void) dev;
    (void) hash;

    /* not supported */
    return -1;
}

int hwcrypto_hash_update(hwcrypto_t dev, const uint8_t *block, uint32_t block_size)
{
    (void) dev;
    (void) block;
    (void) block_size;

    /* not supported */
    return -1;
}

int hwcrypto_hash_final(hwcrypto_t dev, uint8_t *result, uint32_t result_size)
{
    (void) dev;
    (void) result;
    (void) result_size;

    /* not supported */
    return -1;
}

int hwcrypto_acquire(hwcrypto_t dev)
{
    (void) dev;

    mutex_lock(&hwcrypto_lock);

    return 0;
}

int hwcrypto_release(hwcrypto_t dev)
{
    (void) dev;

    mutex_unlock(&hwcrypto_lock);

    return 0;
}

void hwcrypto_poweron(hwcrypto_t dev)
{
    (void) dev;

    CMU_ClockEnable(cmuClock_AES, true);
}

void hwcrypto_poweroff(hwcrypto_t dev)
{
    (void) dev;

    CMU_ClockEnable(cmuClock_AES, false);
}
