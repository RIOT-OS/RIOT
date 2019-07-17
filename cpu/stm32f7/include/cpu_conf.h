/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f7 STM32F7
 * @brief           STM32F7 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.pertersen@fu-berlin.de>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#include "vendor/stm32f7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_LINE_STM32F746xx)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_LINE_STM32F767xx) || defined(CPU_LINE_STM32F769xx)
#define CPU_IRQ_NUMOF                   (110U)
#elif defined(CPU_LINE_STM32F722xx)
#define CPU_IRQ_NUMOF                   (104U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief   Flash sector configuration
 *
 * @{
 */
#define FLASH_DUAL_BANK           (0)
#if defined(CPU_LINE_STM32F746xx) || defined(CPU_LINE_STM32F722xx)
#define FLASHSECTOR_SIZE_MIN      (16*1024U)
#elif defined(CPU_LINE_STM32F767xx) || defined(CPU_LINE_STM32F769xx)
#define FLASHSECTOR_SIZE_MIN      (32*1024UL)
#endif

/**
 * @brief   Flash page configuration
 *
 *          NOTE: STM32F7 flash is organized in sectors, FLASHPAGE_* is
 *                defined as a wrapper over sectors.
 *
 * @{
 */
/* To keep the same flashpage functionality an arbitrary 1K < FLASHSECTOR_SIZE_MIN
 * (size of smaller sector) is defined
 */
#define FLASHPAGE_SIZE            (1024U)
#define FLASHPAGE_NUMOF           (STM32_FLASHSIZE / FLASHPAGE_SIZE)
/* The minimum block size which can be written is 4B. However, the erase
 * depends on the specific sector.
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (4U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (4U)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
