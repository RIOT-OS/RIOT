/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Device-specific test header file ETHOS driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "ethos.h"
#include "ethos_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETHOS_NUM   ARRAY_SIZE(ethos_params)
#define NETDEV_ETH_MINIMAL_NUMOF      ETHOS_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
