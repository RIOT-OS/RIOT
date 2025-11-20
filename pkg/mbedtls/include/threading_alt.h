/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_mbedtls
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "riot_mbedtls_config.h"

#if IS_ACTIVE(MBEDTLS_THREADING_ALT)
#include "mutex.h"

/**
 * @brief Context structure for alternative mbedtls threading implementation.
 */
typedef struct {
    mutex_t riot_mutex; /**< RIOT mutex structure */
} mbedtls_threading_mutex_t;

#endif /* MBEDTLS_THREADING_ALT */

#ifdef __cplusplus
}
#endif
/** @} */
