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
 * @brief   Device-specific test header file DOSE driver
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "dose.h"
#include "dose_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DOSE_NUM   ARRAY_SIZE(dose_params)
#define NETDEV_ETH_MINIMAL_NUMOF      DOSE_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
