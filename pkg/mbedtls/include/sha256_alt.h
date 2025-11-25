/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mbedtls
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#ifndef SHA256_ALT_H
#define SHA256_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "riot_mbedtls_config.h"

#if IS_ACTIVE(MBEDTLS_SHA256_ALT)
#include "hashes/sha256.h"
#include "hashes/sha224.h"

/**
 * @brief Context structure for alternative mbedtls SHA-256 implementation.
 */
typedef struct {
    sha256_context_t riot_sha256_ctx; /**< RIOT SHA-256 context structure */
    uint8_t is224;                    /**< SHA-224/256 switch */
} mbedtls_sha256_context;

#endif /* MBEDTLS_SHA256_ALT */

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SHA256_ALT_H */
