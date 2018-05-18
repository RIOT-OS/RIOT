/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_hwcrypto
 *
 * @{
 *
 * @file
 * @brief       Low-level hardware crypto driver implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <string.h>

#include "mutex.h"
#include "assert.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "periph/hwcrypto.h"

#ifdef STM32_PM_STOP
#include "pm_layered.h"
#endif

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
 * @brief   Global lock to ensure mutual exclusive access to hardware crypto
 *          device.
 */
static mutex_t hwcrypto_lock = MUTEX_INIT;

/**
 * @brief   Hardware crypto device state.
 */
static state_t state;

static inline AES_TypeDef *dev(hwcrypto_t hwcrypto)
{
    return hwcrypto_config[hwcrypto].aes_dev;
}

void hwcrypto_init(hwcrypto_t dev)
{
    assert(dev < HWCRYPTO_NUMOF);
}

int hwcrypto_cipher_init(hwcrypto_t dev, hwcrypto_cipher_t cipher, hwcrypto_mode_t mode)
{
    assert(dev < HWCRYPTO_NUMOF);

    if (!hwcrypto_cipher_supported(dev, cipher)) {
        return HWCRYPTO_NOTSUP;
    }

    if (mode != HWCRYPTO_MODE_ECB &&
            mode != HWCRYPTO_MODE_CBC &&
            mode != HWCRYPTO_MODE_CTR) {
        return HWCRYPTO_NOTSUP;
    }

    state.cipher = cipher;
    state.mode = mode;

    return HWCRYPTO_OK;
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
                return HWCRYPTO_INVALID;
            }

            break;
        case HWCRYPTO_OPT_IV:
            if (state.mode != HWCRYPTO_MODE_CBC) {
                return HWCRYPTO_NOTSUP;
            }

            if (size != 16) {
                return HWCRYPTO_INVALID;
            }

            memcpy(state.opts.iv, value, 16);
            break;
        case HWCRYPTO_OPT_COUNTER:
            if (state.mode != HWCRYPTO_MODE_CTR) {
                return HWCRYPTO_NOTSUP;
            }

            if (size != 16) {
                return HWCRYPTO_INVALID;
            }

            memcpy(state.opts.counter, value, 16);
            break;
        default:
            return HWCRYPTO_NOTSUP;
    }

    return HWCRYPTO_OK;
}

static uint32_t chaining_mode(hwcrypto_mode_t mode)
{
    switch (mode) {
    case HWCRYPTO_MODE_ECB:
        return 0;
    case HWCRYPTO_MODE_CBC:
        return AES_CR_CHMOD_0;
    case HWCRYPTO_MODE_CTR:
        return AES_CR_CHMOD_1;
    default:
        return 0;
    }
}

static int hwcrypto_cipher_encrypt_decrypt(hwcrypto_t hwcrypto, const uint8_t *in,
                                           uint8_t *out, uint32_t block_size,
                                           bool encrypt)
{
    /* blocks must be aligned */
    assert(!((intptr_t) in & 0x03));
    assert(!((intptr_t) out & 0x03));

    /* block size must be multiple of 16 */
    if ((block_size % 16) != 0) {
        return HWCRYPTO_INVALID;
    }

    /* Swap bytes */
    dev(hwcrypto)->CR = AES_CR_DATATYPE_1;
    /* Set chaining mode */
    dev(hwcrypto)->CR |= chaining_mode(state.mode);
    /* Copy key */
    uint32_t *p;
    int i = 0;
    if (state.cipher == HWCRYPTO_AES256) {
        dev(hwcrypto)->CR |= AES_CR_KEYSIZE;
        p = (uint32_t *)&dev(hwcrypto)->KEYR7;
        for (; i < 4; i++, p--) {
            *p = state.key[4 * i + 3] | ((uint32_t) state.key[4 * i + 2] << 8) |
                    ((uint32_t) state.key[4 * i + 1] << 16) |
                    ((uint32_t) state.key[4 * i] << 24);
        }
    }
    p = (uint32_t *)&dev(hwcrypto)->KEYR3;
    for (int j = i + 4; i < j; i++, p--) {
        *p = state.key[4 * i + 3] | ((uint32_t) state.key[4 * i + 2] << 8) |
                ((uint32_t) state.key[4 * i + 1] << 16) |
                ((uint32_t) state.key[4 * i] << 24);
    }
    if (!encrypt) {
        /* Key derivation */
        if (state.mode == HWCRYPTO_MODE_CBC || state.mode == HWCRYPTO_MODE_ECB) {
            dev(hwcrypto)->CR |= AES_CR_CCFC;
            dev(hwcrypto)->CR |= AES_CR_MODE_0;
            dev(hwcrypto)->CR |= AES_CR_EN;
            while ((dev(hwcrypto)->SR & AES_SR_CCF) == 0) { }
        }
        /* Decryption mode */
        dev(hwcrypto)->CR &= ~AES_CR_EN;
        dev(hwcrypto)->CR &= ~AES_CR_MODE;
        dev(hwcrypto)->CR |= AES_CR_MODE_1;
        DEBUG("CR=0x%08" PRIX32 "\n", dev(hwcrypto)->CR);
    }
    /* Copy IV */
    if (state.mode == HWCRYPTO_MODE_CBC) {
        p = (uint32_t *)&dev(hwcrypto)->IVR3;
        for (int i = 0; i < 4; i++, p--) {
            *p = state.opts.iv[4 * i + 3] | ((uint32_t) state.opts.iv[4 * i + 2] << 8) |
                    ((uint32_t) state.opts.iv[4 * i + 1] << 16) |
                    ((uint32_t) state.opts.iv[4 * i] << 24);
        }
    }
    /* Copy counter */
    if (state.mode == HWCRYPTO_MODE_CTR) {
        p = (uint32_t *)&dev(hwcrypto)->IVR3;
        for (int i = 0; i < 4; i++, p--) {
            *p = state.opts.counter[4 * i + 3] | ((uint32_t) state.opts.counter[4 * i + 2] << 8) |
                    ((uint32_t) state.opts.counter[4 * i + 1] << 16) |
                    ((uint32_t) state.opts.counter[4 * i] << 24);
        }
    }

    /* Set-up DMA transfer */
    dma_acquire(hwcrypto_config[hwcrypto].in_dma);
    dma_acquire(hwcrypto_config[hwcrypto].out_dma);

    dma_configure(hwcrypto_config[hwcrypto].in_dma, hwcrypto_config[hwcrypto].in_dma_chan,
                  in, (void *)&dev(hwcrypto)->DINR, block_size,
                  DMA_MEM_TO_PERIPH, DMA_INC_SRC_ADDR | DMA_DATA_WIDTH_WORD);

    dma_configure(hwcrypto_config[hwcrypto].out_dma, hwcrypto_config[hwcrypto].out_dma_chan,
                  (const void *)&dev(hwcrypto)->DOUTR, out, block_size,
                  DMA_PERIPH_TO_MEM, DMA_INC_DST_ADDR | DMA_DATA_WIDTH_WORD);

    dma_start(hwcrypto_config[hwcrypto].in_dma);
    dma_start(hwcrypto_config[hwcrypto].out_dma);

    dev(hwcrypto)->CR |= AES_CR_EN;
    dev(hwcrypto)->CR |= AES_CR_DMAINEN | AES_CR_DMAOUTEN;

    dma_wait(hwcrypto_config[hwcrypto].in_dma);
    dma_wait(hwcrypto_config[hwcrypto].out_dma);

    dma_stop(hwcrypto_config[hwcrypto].in_dma);
    dma_stop(hwcrypto_config[hwcrypto].out_dma);

    dma_release(hwcrypto_config[hwcrypto].in_dma);
    dma_release(hwcrypto_config[hwcrypto].out_dma);

    /* Turn off AES */
    dev(hwcrypto)->CR = 0;

    return block_size;
}

int hwcrypto_cipher_encrypt(hwcrypto_t dev, const uint8_t *plain_block,
                            uint8_t *cipher_block, uint32_t block_size)
{
    return hwcrypto_cipher_encrypt_decrypt(dev, plain_block, cipher_block, block_size, true);
}

int hwcrypto_cipher_decrypt(hwcrypto_t dev, const uint8_t *cipher_block,
                            uint8_t *plain_block, uint32_t block_size)
{
    return hwcrypto_cipher_encrypt_decrypt(dev, cipher_block, plain_block, block_size, false);
}

int hwcrypto_hash_init(hwcrypto_t dev, hwcrypto_hash_t hash)
{
    (void) dev;
    (void) hash;

    return HWCRYPTO_NOTSUP;
}

int hwcrypto_hash_update(hwcrypto_t dev, const uint8_t *block, uint32_t block_size)
{
    (void) dev;
    (void) block;
    (void) block_size;

    return HWCRYPTO_NOTSUP;
}

int hwcrypto_hash_final(hwcrypto_t dev, uint8_t *result, uint32_t result_size)
{
    (void) dev;
    (void) result;
    (void) result_size;

    return HWCRYPTO_NOTSUP;
}

int hwcrypto_acquire(hwcrypto_t dev)
{
    assert(dev < HWCRYPTO_NUMOF);

    mutex_lock(&hwcrypto_lock);

#ifdef STM32_PM_STOP
    pm_block(STM32_PM_STOP);
#endif
    periph_clk_en(hwcrypto_config[dev].ahbbus, hwcrypto_config[dev].rccmask);

    return HWCRYPTO_OK;
}

int hwcrypto_release(hwcrypto_t dev)
{
    assert(dev < HWCRYPTO_NUMOF);

    periph_clk_dis(hwcrypto_config[dev].ahbbus, hwcrypto_config[dev].rccmask);
#ifdef STM32_PM_STOP
    pm_unblock(STM32_PM_STOP);
#endif

    mutex_unlock(&hwcrypto_lock);

    return HWCRYPTO_OK;
}
