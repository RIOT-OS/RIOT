/*
 * Copyright (C) 2016 Fundación Inria Chile
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_hashes_cmac AES_CMAC
 * @ingroup     sys_hashes_keyed
 * @brief       Implementation of the AES CMAC hashing function
 * @deprecated  Will be removed after the 2023.01 release. Please use
 *              @ref sys_hashes_aes128_cmac instead.

 * @{
 *
 * @file
 * @brief       AES_CMAC interface definition
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#ifndef HASHES_CMAC_H
#define HASHES_CMAC_H

#include "hashes/aes128_cmac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of AES_CMAC block in bytes
 */
#define CMAC_BLOCK_SIZE AES128_CMAC_BLOCK_SIZE

/**
 * @brief   CMAC calculation context
 */
typedef aes128_cmac_context_t cmac_context_t;

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
static inline int cmac_init(cmac_context_t *ctx,
                            const uint8_t *key, uint8_t key_size)
{
    return aes128_cmac_init(ctx, key, key_size);
}

/**
 * @brief Update the CMAC context with a portion of the message being hashed
 *
 * @param[in] ctx  Pointer to the CMAC context to update
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
static inline void cmac_update(cmac_context_t *ctx,
                               const void *data, size_t len)
{
    aes128_cmac_update(ctx, data, len);
}

/**
 * @brief Finalizes the CMAC message digest
 *
 * @param[in] ctx     Pointer to the CMAC context
 * @param[out] digest Result location
 */
static inline void cmac_final(cmac_context_t *ctx, void *digest)
{
    aes128_cmac_final(ctx, digest);
}

#ifdef __cplusplus
}
#endif

#endif /* HASHES_CMAC_H */
/** @} */
