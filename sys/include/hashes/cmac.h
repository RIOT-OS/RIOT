/*
 * Copyright (C) 2016 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_hashes_cmac AES_CMAC
 * @ingroup     sys_hashes
 * @brief       Implementation of the AES_CMAC hashing function

 * @{
 *
 * @file
 * @brief       AES_CMAC interface definition
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 */

//TODO: Add source

#include <stdio.h>
#include "crypto/ciphers.h"

#define CMAC_BLOCK_SIZE 16

typedef struct {
    /** AES128 context */
    cipher_t aes_ctx;
    /** auxiliar array for CMAC calculations **/
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
 * @param[in] key     Size of the key
 */
int cmac_init(cmac_context_t *ctx, const uint8_t *key, uint8_t keySize);

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
 *
 */
void cmac_final(cmac_context_t *ctx, void *digest);

/**
 * @brief   Calculate a CMAC hash from the given data
 *
 * @param[out] digest Result location
 * @param[in] data    Input data
 * @param[in] len     Length of @p buf
 */
