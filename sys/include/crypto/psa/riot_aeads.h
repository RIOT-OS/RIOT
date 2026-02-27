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
#  if IS_USED(MODULE_PSA_RIOT_CIPHER_CHACHA20)
#    include "riot_ciphers.h"
#  endif
#  if IS_USED(MODULE_PERIPH_CIPHER_CHACHA20)
#    include "psa_periph_chacha20_ctx.h"
#    include "crypto/poly1305.h"
#  endif
typedef struct {
    /* This uses a modified Chacha20 context to handle
     * multipart operations */
    psa_cipher_chacha20_ctx_t chacha; /**< ChaCha20 context */
    poly1305_ctx_t poly;              /**< Poly1305 context*/
} psa_aead_chacha20_poly1305_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
