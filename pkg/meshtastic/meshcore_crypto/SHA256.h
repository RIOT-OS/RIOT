/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       SHA-256 and HMAC-SHA-256 for MeshCore, with the API of the
 *              Arduino Crypto library, based on the RIOT hashes module
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hashes/sha256.h"

#ifdef __cplusplus
}
#endif

/**
 * @brief   SHA-256 hash, or HMAC-SHA-256 after resetHMAC()
 */
class SHA256 {
public:
    SHA256()
    {
        reset();
    }

    /**
     * @brief   Starts a new hash
     */
    void reset()
    {
        _hmac = false;
        sha256_init(&_ctx.sha);
    }

    /**
     * @brief   Adds data to the hash
     *
     * @param[in] data  data to add
     * @param[in] len   length of @p data
     */
    void update(const void *data, size_t len)
    {
        if (_hmac) {
            hmac_sha256_update(&_ctx.hmac, data, len);
        }
        else {
            sha256_update(&_ctx.sha, data, len);
        }
    }

    /**
     * @brief   Finishes the hash
     *
     * @param[out] hash     the hash, truncated to @p len bytes
     * @param[in] len       length of @p hash
     */
    void finalize(void *hash, size_t len)
    {
        uint8_t digest[SHA256_DIGEST_LENGTH];
        sha256_final(&_ctx.sha, digest);
        _copy(hash, len, digest);
    }

    /**
     * @brief   Starts a new HMAC
     *
     * @param[in] key       key of the HMAC
     * @param[in] key_len   length of @p key
     */
    void resetHMAC(const void *key, size_t key_len)
    {
        _hmac = true;
        hmac_sha256_init(&_ctx.hmac, key, key_len);
    }

    /**
     * @brief   Finishes the HMAC
     *
     * @param[in] key       key of the HMAC, already given to resetHMAC()
     * @param[in] key_len   length of @p key
     * @param[out] hash     the HMAC, truncated to @p len bytes
     * @param[in] len       length of @p hash
     */
    void finalizeHMAC(const void *key, size_t key_len, void *hash, size_t len)
    {
        (void)key;
        (void)key_len;
        uint8_t digest[SHA256_DIGEST_LENGTH];
        hmac_sha256_final(&_ctx.hmac, digest);
        _copy(hash, len, digest);
    }

private:
    static void _copy(void *dest, size_t len, const uint8_t *digest)
    {
        memcpy(dest, digest, (len < SHA256_DIGEST_LENGTH) ? len : SHA256_DIGEST_LENGTH);
    }

    bool _hmac;
    union {
        sha256_context_t sha;
        hmac_context_t hmac;
    } _ctx;
};

/** @} */
