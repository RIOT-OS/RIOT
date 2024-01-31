/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Contexts for the RIOT hashes module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef HASHES_PSA_RIOT_HASHES_H
#define HASHES_PSA_RIOT_HASHES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"

#if (IS_USED(MODULE_PSA_RIOT_HASHES_MD5))
#include "hashes/md5.h"

typedef md5_ctx_t psa_hashes_md5_ctx_t;
#endif

#if (IS_USED(MODULE_PSA_RIOT_HASHES_SHA_1))
#include "hashes/sha1.h"

typedef sha1_context psa_hashes_sha1_ctx_t;
#endif

#if (IS_USED(MODULE_PSA_RIOT_HASHES_SHA_224))
#include "hashes/sha224.h"

typedef sha224_context_t psa_hashes_sha224_ctx_t;
#endif

#if (IS_USED(MODULE_PSA_RIOT_HASHES_SHA_256))
#include "hashes/sha256.h"

typedef sha256_context_t psa_hashes_sha256_ctx_t;
#endif

#if (IS_USED(MODULE_PSA_RIOT_HASHES_SHA_512))
#include "hashes/sha512.h"

typedef sha512_context_t psa_hashes_sha512_ctx_t;
#endif

#if (IS_USED(MODULE_PSA_RIOT_HASHES_HMAC_SHA256))
#include "hashes/sha256.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* HASHES_PSA_RIOT_HASHES_H */
/** @} */
