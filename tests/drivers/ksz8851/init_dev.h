/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Device-specific test header file KSZ8851SNL ethernet device driver
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "ksz8851.h"
#include "ksz8851_params.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KSZ8851_NUM   ARRAY_SIZE(ksz8851_params)
#define NETDEV_ETH_MINIMAL_NUMOF      KSZ8851_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
