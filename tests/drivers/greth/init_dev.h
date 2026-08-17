/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Device-specific test header file for the GRETH driver test
 *
 * @author      Matvii Ivashchenko
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "greth.h"
#include "greth_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of GRETH devices to test
 */
#define GRETH_NUM                   ARRAY_SIZE(greth_params)

/**
 * @brief   Number of network devices the test harness shall handle
 */
#define NETDEV_ETH_MINIMAL_NUMOF    GRETH_NUM

#ifdef __cplusplus
}
#endif

/** @} */
