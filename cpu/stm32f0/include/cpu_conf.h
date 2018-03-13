/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f0 STM32F0
 * @brief           STM32F0 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_STM32F051R8
#include "vendor/stm32f051x8.h"
#endif
#ifdef CPU_MODEL_STM32F091RC
#include "vendor/stm32f091xc.h"
#endif
#ifdef CPU_MODEL_STM32F072RB
#include "vendor/stm32f072xb.h"
#endif
#ifdef CPU_MODEL_STM32F070RB
#include "vendor/stm32f070xb.h"
#endif
#ifdef CPU_MODEL_STM32F030R8
#include "vendor/stm32f030x8.h"
#endif
#ifdef CPU_MODEL_STM32F042K6
#include "vendor/stm32f042x6.h"
#endif
#ifdef CPU_MODEL_STM32F031K6
#include "vendor/stm32f031x6.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32F030R8)
#define CPU_IRQ_NUMOF                   (29U)
#elif defined(CPU_MODEL_STM32F031K6)
#define CPU_IRQ_NUMOF                   (28U)
#elif defined(CPU_MODEL_STM32F051R8) || defined(CPU_MODEL_STM32F091RC)
#define CPU_IRQ_NUMOF                   (31U)
#else /* CPU_MODEL_STM32F042K6, CPU_MODEL_STM32F070RB, CPU_MODEL_STM32F072RB */
#define CPU_IRQ_NUMOF                   (32U)
#endif
/** @} */

/**
 * @brief   Flash page configuration
 *
 * STM32F03x, STM32F04x, STM32F05x: up to 64 pages of 1K
 * STM32F07x, STM32F09x: up to 128 pages of 2K
 *
 * @{
 */
#if defined(CPU_MODEL_STM32F091RC) || defined(CPU_MODEL_STM32F072RB)
#define FLASHPAGE_SIZE      (2048U)
#elif defined(CPU_MODEL_STM32F051R8) || defined(CPU_MODEL_STM32F042K6) \
   || defined(CPU_MODEL_STM32F070RB) || defined(CPU_MODEL_STM32F030R8)
#define FLASHPAGE_SIZE      (1024U)
#endif

#if defined(CPU_MODEL_STM32F091RC)
#define FLASHPAGE_NUMOF     (128U)
#elif defined(CPU_MODEL_STM32F051R8) || defined(CPU_MODEL_STM32F072RB) \
   || defined(CPU_MODEL_STM32F030R8) || defined(CPU_MODEL_STM32F070RB)
#define FLASHPAGE_NUMOF     (64U)
#elif defined(CPU_MODEL_STM32F042K6)
#define FLASHPAGE_NUMOF     (32U)
#endif

/* The minimum block size which can be written is 2B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (2U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (4U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
