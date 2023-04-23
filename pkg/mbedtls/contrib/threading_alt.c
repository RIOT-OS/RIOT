/*
 * Copyright (C) 2021 HAW Hamburg
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
 * @}
 */

#include <assert.h>
#include <string.h>

#include "threading_alt.h"

#if IS_ACTIVE(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"

void mbedtls_platform_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    mutex_init(&mutex->riot_mutex);
}

void mbedtls_platform_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    (void)mutex;
}

int mbedtls_platform_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    mutex_lock(&mutex->riot_mutex);
    return 0;
}

int mbedtls_platform_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    mutex_unlock(&mutex->riot_mutex);
    return 0;
}

void threading_mbedtls_riot_init(void)
{
    /* Configure mbedTLS to use RIOT specific threading functions. */
    mbedtls_threading_set_alt( mbedtls_platform_mutex_init,
                               mbedtls_platform_mutex_free,
                               mbedtls_platform_mutex_lock,
                               mbedtls_platform_mutex_unlock );
}
#endif /* MBEDTLS_THREADING_ALT */
