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
 * @brief   Device-specific test header file ENCX24J600 ethernet device driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "encx24j600.h"
#include "encx24j600_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENCX24J600_NUM   ARRAY_SIZE(encx24j600_params)
#define NETDEV_ETH_MINIMAL_NUMOF      ENCX24J600_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
