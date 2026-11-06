/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_sam_common
 * @brief           Implementation specific CPU configuration options
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Sebastian Meiling <s@mlng.net>
 */

#include "cpu_conf_common.h"
#include "vendor/sam.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   PERIPH_COUNT_IRQn
#define CPU_FLASH_BASE                  IFLASH0_ADDR
#define CPU_HAS_BITBAND                 (1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
