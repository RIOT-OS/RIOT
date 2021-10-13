/*
 * Copyright (C) 2016 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_hashes_sha1 SHA-1
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of the SHA-1 hashing function

 * @{
 *
 * @file
 * @brief       SHA-1 interface definition
 *
 * @author      Wei Dai and others
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

/* This code is public-domain - it is based on libcrypt
 * placed in the public domain by Wei Dai and other contributors. */

#ifndef HASHES_SHA1_H
#define HASHES_SHA1_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of SHA-1 digests in byte
 */
#define SHA1_DIGEST_LENGTH  (20)

/**
 * @brief   Length of SHA-1 block in byte
 */
#define SHA1_BLOCK_LENGTH   (64)

/**
 * @brief SHA-1 algorithm context
 * @internal
 **/
typedef struct {
    /** internal buffer */
    uint32_t buffer[SHA1_BLOCK_LENGTH / sizeof(uint32_t)];
    /** buffering current state of hashing */
    uint32_t state[SHA1_DIGEST_LENGTH / sizeof(uint32_t)];
    /** already processed bytes */
    uint32_t byte_count;
    /** internal state variable to keep track if the buffer is filled before
     * proceeding to hash this block */
    uint8_t buffer_offset;
    /** internal state of the key buffer */
    uint8_t key_buffer[SHA1_BLOCK_LENGTH];
    /** temporary buffer for the inner hashing */
    uint8_t inner_hash[SHA1_DIGEST_LENGTH];
} sha1_context;

/**
 * @brief Initialize SHA-1 message digest context
 *
 * @param[in] ctx     Pointer to the SHA-1 context to initialize
 */
void sha1_init(sha1_context *ctx);

/**
 * @brief Update the SHA-1 context with a portion of the message being hashed
 *
 * @param[in] ctx  Pointer to the SHA-1 context to update
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
void sha1_update(sha1_context *ctx, const void *data, size_t len);

/**
 * @brief Finalizes the SHA-1 message digest
 *
 * @param[in] ctx     Pointer to the SHA-1 context
 * @param[out] digest Result location, must be 20 byte
 *
 */
void sha1_final(sha1_context *ctx, void *digest);

/**
 * @brief   Calculate a SHA1 hash from the given data
 *
 * @param[out] digest Result location, must be 20 byte
 * @param[in] data    Input data
 * @param[in] len     Length of @p buf
 */
void sha1(void *digest, const void *data, size_t len);

/**
 * @brief Initialize SHA-1 message digest context with MAC
 *
 * @param[in] ctx        Pointer to the SHA-1 context to initialize
 * @param[in] key        Key used in the HMAC-SHA1 computation
 * @param[in] key_length The size in bytes of @p key
 */
void sha1_init_hmac(sha1_context *ctx, const void *key, size_t key_length);

/**
 * @brief Finalizes the SHA-1 message digest with MAC
 *
 * @param[in] ctx     Pointer to the SHA-1 context
 * @param[out] digest Result location, must be 20 byte
 */
void sha1_final_hmac(sha1_context *ctx, void *digest);

#ifdef __cplusplus
}
#endif

#endif /* HASHES_SHA1_H */
/** @} */
