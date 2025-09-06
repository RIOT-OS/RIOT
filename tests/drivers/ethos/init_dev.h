/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
