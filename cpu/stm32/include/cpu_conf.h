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

#if defined(CPU_LINE_STM32F030x4)
#include "vendor/stm32f030x4.h"
#elif CPU_FAM_STM32F0
#include "stm32f0xx.h"
#include "irqs/f0/irqs.h"
#elif CPU_FAM_STM32F1
#include "stm32f1xx.h"
#include "irqs/f1/irqs.h"
#elif CPU_FAM_STM32F2
#include "stm32f2xx.h"
#include "irqs/f2/irqs.h"
#elif CPU_FAM_STM32F3
#include "stm32f3xx.h"
#include "irqs/f3/irqs.h"
#elif CPU_FAM_STM32F4
#include "stm32f4xx.h"
#include "irqs/f4/irqs.h"
#elif CPU_FAM_STM32F7
#include "stm32f7xx.h"
#include "irqs/f7/irqs.h"
#elif CPU_FAM_STM32G0
#include "stm32g0xx.h"
#include "irqs/g0/irqs.h"
#elif CPU_FAM_STM32G4
#include "stm32g4xx.h"
#include "irqs/g4/irqs.h"
#elif CPU_FAM_STM32L0
#include "stm32l0xx.h"
#include "irqs/l0/irqs.h"
#elif CPU_FAM_STM32L1
#include "stm32l1xx.h"
#include "irqs/l1/irqs.h"
#elif CPU_FAM_STM32L4
#include "stm32l4xx.h"
#include "irqs/l4/irqs.h"
#elif CPU_FAM_STM32L5
#include "stm32l5xx.h"
#include "irqs/l5/irqs.h"
#elif CPU_FAM_STM32WB
#include "stm32wbxx.h"
#include "irqs/wb/irqs.h"
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
#define CPU_FLASH_BASE                  FLASH_BASE

/* CPU_IRQ_NUMOF cannot be determined automatically from cmsis header */
#if defined(CPU_LINE_STM32F030x4)
#define CPU_IRQ_NUMOF                   (28U)
#endif
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#if defined(CPU_FAM_STM32WB)
#define FLASHPAGE_SIZE                  (4096U)
#elif defined(CPU_LINE_STM32F091xC) || defined(CPU_LINE_STM32F072xB) \
   || defined(CPU_LINE_STM32F030xC) || defined(CPU_LINE_STM32F103xE) \
   || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) \
   || defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) \
   || defined(CPU_FAM_STM32L5)
#define FLASHPAGE_SIZE                  (2048U)
#elif defined(CPU_LINE_STM32F051x8) || defined(CPU_LINE_STM32F042x6) \
   || defined(CPU_LINE_STM32F070xB) || defined(CPU_LINE_STM32F030x8) \
   || defined(CPU_LINE_STM32F030x4) || defined(CPU_LINE_STM32F103xB) \
   || defined(CPU_LINE_STM32F031x6)
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
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
#define FLASHPAGE_RAW_BLOCKSIZE         (8U)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define FLASHPAGE_RAW_BLOCKSIZE         (4U)
#else
#define FLASHPAGE_RAW_BLOCKSIZE         (2U)
#endif

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
#define FLASHPAGE_RAW_ALIGNMENT         (8U)
#else
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_RAW_ALIGNMENT         (4U)
#endif
/** @} */

/**
 * @brief   Bit-Band configuration
 * @{
 */
#ifdef SRAM_BB_BASE
#define CPU_HAS_BITBAND 1
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
