/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
