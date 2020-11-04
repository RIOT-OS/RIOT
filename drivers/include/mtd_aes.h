/*
 * Copyright (C) 2020 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_aes  MTD AES encryption
 * @ingroup     drivers_storage
 * @brief       Driver for flash memory encryption
 *
 * @{
 *
 * @brief       Interface definitions for MTD AES encryption
 *
 * @author      Juergen Fitschen <jfi@ssv-embedded.de>
 */

#ifndef MTD_AES_H
#define MTD_AES_H

#include <stdint.h>
#include <stdbool.h>
#include "mtd.h"
#include "mutex.h"
#include "crypto/aes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MTD AES instance
 */
typedef struct {
    mtd_dev_t mtd;                /**< MTD context                         */
    mtd_dev_t *parent;            /**< MTD context of the parent device    */
    uint64_t iv[2];  /**< initialization vector for AES-CTR   */
    cipher_t cipher;              /**< context for AES encryption          */
} mtd_aes_t;

/**
 * @brief   Intializes the AES context for mtd_aes
 *
 * Please note: The underlying MTD must be initialized separately.
 *
 * @param      aes    the MTD AED context
 * @param[in]  parent the parent MTD device
 * @param[in]  key    the AES key
 * @param[in]  iv     the initialization vector for CTR mode
 *
 * @return CIPHER_INIT_SUCCESS if initialization was successful
 * @return <= 0 if an error occurred
 */
extern const mtd_desc_t mtd_aes_driver;


int mtd_aes_init(mtd_aes_t * aes, mtd_dev_t * parent,
                 const uint8_t key[AES_KEY_SIZE], const uint8_t iv[AES_BLOCK_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /* MTD_AES_H */
/** @} */
