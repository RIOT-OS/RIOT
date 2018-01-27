/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_efm32 Silicon Labs EFM32/EFR32/EZR32
 * @ingroup     cpu
 * @brief       Support for Silicon Labs EFM32/EFR32/EZR32 CPUs
 * @{
 *
 * @file
 * @brief       Implementation specific CPU configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (EXT_IRQ_COUNT + 1)
#define CPU_FLASH_BASE                  (FLASH_BASE)
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE                  (FLASH_PAGE_SIZE)
#define FLASHPAGE_NUMOF                 (FLASH_SIZE / FLASH_PAGE_SIZE)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
