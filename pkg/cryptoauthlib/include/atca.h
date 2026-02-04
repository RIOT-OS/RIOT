/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

#include "modules.h"
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

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
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

/**
 * @brief   Structure containing information about which type of key
 *          can be held by a specific key slot on an ATECCX08 device.
 *
 *          If a slot should be ignored by the implementation,
 *          just mark it as persistent and occupied.
 */
typedef struct {
    /* Type of key permitted for slot */
    psa_key_type_t key_type_allowed;
    /* Specify if key should be stored persistently or can be overwritten */
    uint8_t key_persistent;
    /* Specify whether slot is already occupied */
    uint8_t slot_occupied;
} psa_atca_slot_config_t;
#endif /* MODULE_PSA_SECURE_ELEMENT_ATECCX08A */

#ifdef __cplusplus
}
#endif

/** @} */
