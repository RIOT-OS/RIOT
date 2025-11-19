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
 * @brief   Device-specific test header file W5100 ethernet device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "w5100.h"
#include "w5100_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define W5100_NUM   ARRAY_SIZE(w5100_params)
#define NETDEV_ETH_MINIMAL_NUMOF      W5100_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
