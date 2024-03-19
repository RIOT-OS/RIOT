/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_libcose
 * @{
 *
 * @file
 * @brief       RIOT as a crypto backend for libcose implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>

#include "crypto/chacha20poly1305.h"

#include "cose.h"
#include "cose/crypto.h"
#include "cose/crypto/selectors.h"

int cose_crypto_aead_encrypt_chachapoly(uint8_t *c, size_t *clen,
                                        const uint8_t *msg, size_t msglen,
                                        const uint8_t *aad, size_t aadlen,
                                        const uint8_t *npub, const uint8_t *k)
{
    if (*clen < msglen + CHACHA20POLY1305_TAG_BYTES) {
        return COSE_ERR_INVALID_PARAM;
    }
    chacha20poly1305_encrypt(c, msg, msglen, aad, aadlen, k, npub);
    *clen = msglen + CHACHA20POLY1305_TAG_BYTES;
    return COSE_OK;
}

int cose_crypto_aead_decrypt_chachapoly(uint8_t *msg, size_t *msglen,
                                        const uint8_t *c, size_t clen,
                                        const uint8_t *aad, size_t aadlen,
                                        const uint8_t *npub, const uint8_t *k)
{
    if (chacha20poly1305_decrypt(c, clen, msg, msglen, aad, aadlen, k, npub) == 1) {
        return COSE_OK;
    }
    else {
        return COSE_ERR_CRYPTO;
    }
}
