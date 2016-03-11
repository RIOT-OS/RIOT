/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_ezr32wg Silicon Labs EZR32WG
 * @ingroup     cpu
 * @brief       Support for the Silicon Labs EZR32 Wonder Gecko CPU
 * @{
 *
 * @file
 * @brief       Implementation specific CPU configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if (CPU_MODEL == ezr32wg330f256r60)
#include "ezr32wg330f256r60.h"
#else
#error "No CPU headers for the defined CPU_MODEL found"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (FPUEH_IRQn + 1)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
