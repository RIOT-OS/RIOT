/*
 * Copyright (C) 2021 HAW Hamburg
 *               2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       Initializes RIOT mbedtls modue
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 * @}
 */

#include "modules.h"
#include "log.h"

#if IS_USED(MODULE_MBEDTLS_THREADING)
#include "mbedtls/threading.h"
#include "threading_alt.h"
#endif
#if IS_USED(MODULE_MBEDTLS_ENTROPY)
#include "entropy_mbedtls_riot.h"
#endif
#if IS_USED(MODULE_MBEDTLS_RANDOM)
#include "random_mbedtls_riot.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

void auto_init_mbedtls(void)
{
#if IS_USED(MODULE_MBEDTLS_THREADING_ALT)
    threading_mbedtls_riot_init();
#endif
#if IS_USED(MODULE_MBEDTLS_ENTROPY)
    entropy_mbedtls_riot_init();
#endif
#if IS_USED(MODULE_MBEDTLS_RANDOM)
    random_mbedtls_riot_init();
#endif
}
