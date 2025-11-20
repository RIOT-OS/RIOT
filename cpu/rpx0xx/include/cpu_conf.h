/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rpx0xx
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
*/

#include <stdint.h>

#include "macros/units.h"
#include "cpu_conf_common.h"
#include "vendor/RP2040.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_FLASH_BASE                  ROM_START_ADDR
#define CPU_IRQ_NUMOF                   (32U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
