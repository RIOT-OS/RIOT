/*
 * Copyright (C) 2016 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_hashes_cmac AES_CMAC
 * @ingroup     sys_hashes_keyed
 * @brief       Implementation of the AES CMAC hashing function

 * @{
 *
 * @file
 * @brief       AES_CMAC interface definition
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifndef HASHES_CMAC_H
#define HASHES_CMAC_H

#include <stdio.h>
#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of AES_CMAC block in bytes
 */
#define CMAC_BLOCK_SIZE 16

/**
 * @brief   AES_CMAC calculation context
 */
typedef struct {
    /** AES128 context */
    cipher_t aes_ctx;
    /** auxiliary array for CMAC calculations **/
    uint8_t X[CMAC_BLOCK_SIZE];
    /** current last block **/
    uint8_t M_last[CMAC_BLOCK_SIZE];
    /** last byte in last block */
    uint32_t M_n;
} cmac_context_t;

/**
 * @brief Initialize CMAC message digest context
 *
 * @param[in] ctx     Pointer to the CMAC context to initialize
 * @param[in] key     Key to be set
 * @param[in] key_size     Size of the key
 *
 * @return CIPHER_INIT_SUCCESS if the initialization was successful.
 *         CIPHER_ERR_INVALID_KEY_SIZE if the key size is not valid.
 */
int cmac_init(cmac_context_t *ctx, const uint8_t *key, uint8_t key_size);

/**
 * @brief Update the CMAC context with a portion of the message being hashed
 *
 * @param[in] ctx  Pointer to the CMAC context to update
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
void cmac_update(cmac_context_t *ctx, const void *data, size_t len);

/**
 * @brief Finalizes the CMAC message digest
 *
 * @param[in] ctx     Pointer to the CMAC context
 * @param[out] digest Result location
 */
void cmac_final(cmac_context_t *ctx, void *digest);

#ifdef __cplusplus
}
#endif

#endif /* HASHES_CMAC_H */
/** @} */
