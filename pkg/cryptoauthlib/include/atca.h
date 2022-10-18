/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_cryptoauthlib
 * @{
 *
 * @file
 * @brief       Default addresses and device descriptor for CryptoAuth devices
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef ATCA_H
#define ATCA_H

#include "periph/i2c.h"
#include "cryptoauthlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Default device and word addresses for ATECC508A
 * @{
 */
#define ATCA_I2C_ADDR (0xC0)    /**< Default device address is 0xC0 */

#define ATCA_WAKE_ADDR   (0x00) /**< Word address to write 0 to device */
#define ATCA_SLEEP_ADDR  (0x01) /**< Word address to write to for sleep mode */
#define ATCA_IDLE_ADDR   (0x02) /**< Word address to write to for idle mode */
#define ATCA_DATA_ADDR   (0x03) /**< Word address to read and write to data area */
/** @} */

#if (IS_ACTIVE(CONFIG_PSA_SE_ATECCX08A))
#include "psa/crypto_types.h"

/**
 * @brief   Structure containing the Cryptoauthlib specific AES context
 */
typedef struct {
    uint8_t iv[16];
    psa_encrypt_or_decrypt_t direction;
    union atca_context {
        atca_aes_cbc_ctx_t aes_cbc;
    } aes_ctx;
} psa_atca_cipher_context_t;
#endif /* CONFIG_PSA_SE_ATECCX08A */

#ifdef __cplusplus
}
#endif

#endif /* ATCA_H */
/** @} */
