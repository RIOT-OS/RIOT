/*
 * SPDX-FileCopyrightText: 2023 Stefan Schmidt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Device-specific test header file W5500 ethernet device driver
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "w5500.h"
#include "w5500_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define W5500_NUM   ARRAY_SIZE(w5500_params)
#define NETDEV_ETH_MINIMAL_NUMOF      W5500_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
