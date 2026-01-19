/* IoT-TODO */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Contexts for the RIOT aead module
 *
 * @author      Oliver Fritz <oliver.fritz-default@protonmail.com>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_PSA_RIOT_AEAD_CHACHA20_POLY1305)
#include "riot_ciphers.h"
/* IoT-TODO: maybe union */
typedef struct {
    /* This uses a modified Chacha20 context to handle
     * multipart operations */
    psa_cipher_chacha20_ctx_t ctx; /**< ChaCha20 context */
    poly1305_ctx_t poly;           /**< Poly1305 context*/
} psa_aead_chacha20_poly1305_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
