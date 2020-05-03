/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_stm32
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

#if CPU_FAM_STM32F0
#include "vendor/stm32f0xx.h"
#elif CPU_FAM_STM32F1
#include "vendor/stm32f1xx.h"
#elif CPU_FAM_STM32F2
#include "vendor/stm32f2xx.h"
#elif CPU_FAM_STM32F3
#include "vendor/stm32f3xx.h"
#elif CPU_FAM_STM32F4
#include "vendor/stm32f4xx.h"
#elif CPU_FAM_STM32F7
#include "vendor/stm32f7xx.h"
#elif CPU_FAM_STM32L0
#include "vendor/stm32l0xx.h"
#elif CPU_FAM_STM32L1
#include "vendor/stm32l1xx.h"
#elif CPU_FAM_STM32L4
#include "vendor/stm32l4xx.h"
#elif CPU_FAM_STM32WB
#include "vendor/stm32wbxx.h"
#else
#error Not supported CPU family
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_LINE_STM32F030x8)
#define CPU_IRQ_NUMOF                   (29U)
#elif defined(CPU_LINE_STM32F031x6) || defined(CPU_LINE_STM32F030x4)
#define CPU_IRQ_NUMOF                   (28U)
#elif defined(CPU_LINE_STM32F051x8) || defined(CPU_LINE_STM32F091xC)
#define CPU_IRQ_NUMOF                   (31U)
#elif defined (CPU_FAM_STM32F0)
#define CPU_IRQ_NUMOF                   (32U)
#elif defined(CPU_LINE_STM32F103xE)
#define CPU_IRQ_NUMOF                   (60U)
#elif defined (CPU_FAM_STM32F1)
#define CPU_IRQ_NUMOF                   (43U)
#elif defined (CPU_FAM_STM32F2)
#define CPU_IRQ_NUMOF                   (81U)
#elif defined(CPU_LINE_STM32F303xE)
#define CPU_IRQ_NUMOF                   (85U)
#elif defined(CPU_FAM_STM32F3)
#define CPU_IRQ_NUMOF                   (82U)
#elif defined(CPU_LINE_STM32F401xE)
#define CPU_IRQ_NUMOF                   (85U)
#elif defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx) \
    || defined(CPU_LINE_STM32F415xx)
#define CPU_IRQ_NUMOF                   (82U)
#elif defined(CPU_LINE_STM32F410Rx)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_LINE_STM32F411xE)
#define CPU_IRQ_NUMOF                   (86U)
#elif defined(CPU_LINE_STM32F412Zx) || defined(CPU_LINE_STM32F446xx)
#define CPU_IRQ_NUMOF                   (97U)
#elif defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)
#define CPU_IRQ_NUMOF                   (102U)
#elif defined(CPU_LINE_STM32F429xx) || defined(CPU_LINE_STM32F437xx)
#define CPU_IRQ_NUMOF                   (91U)
#elif defined(CPU_LINE_STM32F746xx)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_LINE_STM32F767xx) || defined(CPU_LINE_STM32F769xx)
#define CPU_IRQ_NUMOF                   (110U)
#elif defined(CPU_LINE_STM32F722xx) || defined(CPU_LINE_STM32F723xx)
#define CPU_IRQ_NUMOF                   (104U)
#elif defined(CPU_LINE_STM32L031xx)
#define CPU_IRQ_NUMOF                   (30U)
#elif defined(CPU_FAM_STM32L0)
#define CPU_IRQ_NUMOF                   (32U)
#elif defined(CPU_MODEL_STM32L151RB_A) || defined(CPU_MODEL_STM32L151CB) || \
    defined(CPU_MODEL_STM32L151CB_A)
#define CPU_IRQ_NUMOF                   (45U)
#elif defined(CPU_FAM_STM32L1)
#define CPU_IRQ_NUMOF                   (57U)
#elif defined(CPU_MODEL_STM32L432KC) || defined(CPU_MODEL_STM32L433RC)
#define CPU_IRQ_NUMOF                   (83U)
#elif defined(CPU_MODEL_STM32L496ZG) || defined(CPU_MODEL_STM32L496AG)
#define CPU_IRQ_NUMOF                   (91U)
#elif defined(CPU_MODEL_STM32L4R5ZI)
#define CPU_IRQ_NUMOF                   (95U)
#elif defined(CPU_FAM_STM32L4)
#define CPU_IRQ_NUMOF                   (82U)
#elif defined(CPU_MODEL_STM32WB55RG)
#define CPU_IRQ_NUMOF                   (63U)
#else
#error Number of IRQs not configured for this CPU
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#if defined(CPU_FAM_STM32WB)
#define FLASHPAGE_SIZE                  (4096U)
#elif defined(CPU_LINE_STM32F091xC) || defined(CPU_LINE_STM32F072xB) \
   || defined(CPU_LINE_STM32F030xC) || defined(CPU_LINE_STM32F103xE) \
   || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4)
#define FLASHPAGE_SIZE                  (2048U)
#elif defined(CPU_LINE_STM32F051x8) || defined(CPU_LINE_STM32F042x6) \
   || defined(CPU_LINE_STM32F070xB) || defined(CPU_LINE_STM32F030x8) \
   || defined(CPU_LINE_STM32F030x4) || defined(CPU_LINE_STM32F103xB)
#define FLASHPAGE_SIZE                  (1024U)
#elif defined(CPU_FAM_STM32L1)
#define FLASHPAGE_SIZE                  (256U)
#elif defined(CPU_FAM_STM32L0)
#define FLASHPAGE_SIZE                  (128U)
#endif

#define FLASHPAGE_NUMOF                 (STM32_FLASHSIZE / FLASHPAGE_SIZE)

/* The minimum block size which can be written depends on the family.
 * However, the erase block is always FLASHPAGE_SIZE.
 */
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB)
#define FLASHPAGE_RAW_BLOCKSIZE         (8U)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define FLASHPAGE_RAW_BLOCKSIZE         (4U)
#else
#define FLASHPAGE_RAW_BLOCKSIZE         (2U)
#endif

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB)
#define FLASHPAGE_RAW_ALIGNMENT         (8U)
#else
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_RAW_ALIGNMENT         (4U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
