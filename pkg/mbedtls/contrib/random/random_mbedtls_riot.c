/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mbedtls_random
 *
 * @{
 * @file
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stddef.h>

#include "modules.h"
#include "entropy_mbedtls_riot.h"
#include "random_mbedtls_riot.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/hmac_drbg.h"

static mbedtls_entropy_context _entropy;

#if IS_USED(MODULE_MBEDTLS_CTR_DRBG)
static mbedtls_ctr_drbg_context _ctr_drbg;
#endif

#if IS_USED(MODULE_MBEDTLS_HMAC_DRBG)
static mbedtls_hmac_drbg_context _hmac_drbg;
#endif

void random_mbedtls_riot_init(void)
{
    mbedtls_entropy_init(&_entropy);

#if IS_USED(MODULE_MBEDTLS_CTR_DRBG)
    mbedtls_ctr_drbg_init(&_ctr_drbg);
    /* entropy sources are added by module mbedtls_entropy */
    mbedtls_ctr_drbg_seed(&_ctr_drbg,
                          mbedtls_entropy_func,
                          &_entropy,
                          (const unsigned char *)"r10T-0$",
                          7);
#endif

#if IS_USED(MODULE_MBEDTLS_HMAC_DRBG)
    mbedtls_hmac_drbg_init(&_hmac_drbg);
    const mbedtls_md_info_t *md_info = NULL;
#if IS_USED(MODULE_MBEDTLS_SHA256)
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
#elif IS_USED(MODULE_MBEDTLS_SHA1)
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
#endif
    /* entropy sources are added by module mbedtls_entropy */
    mbedtls_hmac_drbg_seed(&_hmac_drbg,
                           md_info,
                           mbedtls_entropy_func,
                           &_entropy,
                           (const unsigned char *)"r10T-0$",
                           7);
#endif
}

void random_ctr_drbg_mbedtls_get(void *out, size_t size)
{
    (void)out; (void)size;
#if IS_USED(MODULE_MBEDTLS_CTR_DRBG)
    mbedtls_ctr_drbg_random(&_ctr_drbg, out, size);
#endif
}

void random_hmac_drbg_mbedtls_get(void *out, size_t size)
{
    (void)out; (void)size;
#if IS_USED(MODULE_MBEDTLS_HMAC_DRBG)
    mbedtls_hmac_drbg_random(&_hmac_drbg, out, size);
#endif
}
