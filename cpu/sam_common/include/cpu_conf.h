/*
 * Copyright (C)    2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
