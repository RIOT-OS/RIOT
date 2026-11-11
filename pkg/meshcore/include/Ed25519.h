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
 * @brief       Ed25519 signature verification for MeshCore, with the API of
 *              the Arduino Crypto library, based on the monocypher package
 *
 * The other Ed25519 operations of MeshCore use the ed25519 library provided by
 * MeshCore.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Verifies an Ed25519 signature, provided by monocypher
 *
 * The ed25519 library of MeshCore and monocypher-ed25519.h use the same
 * include guard (ED25519_H), the function of monocypher is declared here.
 *
 * @param[in] signature     the signature
 * @param[in] public_key    the public key
 * @param[in] message       the signed message
 * @param[in] message_size  length of @p message
 *
 * @return  0 if the signature is valid
 * @return  -1 otherwise
 */
int crypto_ed25519_check(const uint8_t signature[64], const uint8_t public_key[32],
                         const uint8_t *message, size_t message_size);

#ifdef __cplusplus
}
#endif

/**
 * @brief   Ed25519 signatures
 */
class Ed25519 {
public:
    /**
     * @brief   Verifies a signature
     *
     * @param[in] signature     the signature, 64 bytes
     * @param[in] public_key    the public key, 32 bytes
     * @param[in] message       the signed message
     * @param[in] len           length of @p message
     *
     * @return  true if the signature is valid
     */
    static bool verify(const uint8_t *signature, const uint8_t *public_key,
                       const void *message, size_t len)
    {
        return crypto_ed25519_check(signature, public_key, (const uint8_t *)message, len) == 0;
    }
};

/** @} */
