/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32l4 STM32L4
 * @brief           STM32L4 specific code
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

#if defined(CPU_MODEL_STM32L496ZG) || defined(CPU_MODEL_STM32L496AG)
#include "vendor/stm32l496xx.h"
#elif defined(CPU_MODEL_STM32L476RG) || defined(CPU_MODEL_STM32L476VG)
#include "vendor/stm32l476xx.h"
#elif defined(CPU_MODEL_STM32L475VG)
#include "vendor/stm32l475xx.h"
#elif defined(CPU_MODEL_STM32L412KB)
#include "vendor/stm32l412xx.h"
#elif defined(CPU_MODEL_STM32L432KC)
#include "vendor/stm32l432xx.h"
#elif defined(CPU_MODEL_STM32L433RC)
#include "vendor/stm32l433xx.h"
#elif defined(CPU_MODEL_STM32L452RE)
#include "vendor/stm32l452xx.h"
#elif defined(CPU_MODEL_STM32L4R5ZI)
#include "vendor/stm32l4r5xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32L432KC) || defined(CPU_MODEL_STM32L433RC)
#define CPU_IRQ_NUMOF                   (83U)
#elif defined(CPU_MODEL_STM32L496ZG) || defined(CPU_MODEL_STM32L496AG)
#define CPU_IRQ_NUMOF                   (91U)
#elif defined(CPU_MODEL_STM32L4R5ZI)
#define CPU_IRQ_NUMOF                   (95U)
#else
#define CPU_IRQ_NUMOF                   (82U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @name   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE      (2048U)

#define FLASHPAGE_NUMOF     (STM32_FLASHSIZE / FLASHPAGE_SIZE)

/* The minimum block size which can be written is 8B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (8U)
/* Writing should be always 8 bytes aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (8U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
