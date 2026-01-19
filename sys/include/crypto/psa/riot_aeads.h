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
#include "crypto/chacha20poly1305.h"
typedef chacha20poly1305_ctx_t psa_aead_chacha20_poly1305_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
