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
 * @brief       AES-128 block cipher for MeshCore, with the API of the Arduino
 *              Crypto library, based on the RIOT crypto module
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "crypto/ciphers.h"

#ifdef __cplusplus
}
#endif

/**
 * @brief   AES-128 block cipher
 */
class AES128 {
public:
    /**
     * @brief   Sets the key
     *
     * @param[in] key       the key
     * @param[in] len       length of @p key, 16 bytes
     *
     * @return  true on success
     */
    bool setKey(const uint8_t *key, size_t len)
    {
        return cipher_init(&_cipher, CIPHER_AES, key, len) == CIPHER_INIT_SUCCESS;
    }

    /**
     * @brief   Encrypts a block of 16 bytes
     *
     * @param[out] output   the encrypted block
     * @param[in] input     the block to encrypt
     */
    void encryptBlock(uint8_t *output, const uint8_t *input)
    {
        cipher_encrypt(&_cipher, input, output);
    }

    /**
     * @brief   Decrypts a block of 16 bytes
     *
     * @param[out] output   the decrypted block
     * @param[in] input     the block to decrypt
     */
    void decryptBlock(uint8_t *output, const uint8_t *input)
    {
        cipher_decrypt(&_cipher, input, output);
    }

private:
    cipher_t _cipher;
};

/** @} */
