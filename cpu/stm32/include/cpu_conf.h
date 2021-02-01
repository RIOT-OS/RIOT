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

#include <stdint.h>
#include "cpu_conf_common.h"

#if defined(CPU_LINE_STM32F030x4)
#include "vendor/stm32f030x4.h"
#elif defined(CPU_LINE_STM32MP157Cxx)
#include "vendor/stm32mp157cxx_cm4.h"
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
#elif CPU_FAM_STM32WL
#include "stm32wlxx.h"
#include "irqs/wl/irqs.h"
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
/* STM32MP1 family has no flah */
#if !defined(CPU_FAM_STM32MP1)
#define CPU_FLASH_BASE                  FLASH_BASE
#endif

/* CPU_IRQ_NUMOF cannot be determined automatically from cmsis header */
#if defined(CPU_LINE_STM32F030x4)
#define CPU_IRQ_NUMOF                   (28U)
#elif defined(CPU_MODEL_STM32MP157CAC)
#define CPU_IRQ_NUMOF                   (150U)
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
   || defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32WL)
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

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define FLASHPAGE_ERASE_STATE           (0x00U)
#endif

#ifdef FLASHPAGE_SIZE
#define FLASHPAGE_NUMOF                 (STM32_FLASHSIZE / FLASHPAGE_SIZE)
#endif

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
#define PERIPH_FLASHPAGE_CUSTOM_PAGESIZES
#define PERIPH_FLASHPAGE_NEEDS_FLASHPAGE_ADDR

/**
 * @brief stm32 dual bank configuration
 *
 * By default, the stm32f4 series with 1MB flash enable the DB1M flag to split
 * the 1MB flash into two banks, 2MB devices are always split in two banks.
 * On both the stm32f4 and the stm32f7 this can be modified with user
 * programmable flags. Detecting the settings at runtime is not supported
 *
 * @note This must match the setting on the MCU. by default it is assumed that
 * the user has not changed this setting manually.
 */
#if (defined(FLASH_OPTCR_DB1M) && (STM32_FLASHSIZE >= (1024 * 1024)))
#define FLASHPAGE_DUAL_BANK             1
#else
#define FLASHPAGE_DUAL_BANK             0
#endif

/* stm32f7 uses single bank with 32KB to 256KB sectors on a number of devices */
#if defined(CPU_FAM_STM32F7)
#if defined(CPU_LINE_STM32F745xx) || \
    defined(CPU_LINE_STM32F746xx) || \
    defined(CPU_LINE_STM32F750xx) || \
    defined(CPU_LINE_STM32F756xx) || \
    defined(CPU_LINE_STM32F765xx) || \
    defined(CPU_LINE_STM32F767xx) || \
    defined(CPU_LINE_STM32F769xx) || \
    defined(CPU_LINE_STM32F777xx) || \
    defined(CPU_LINE_STM32F779xx)
#define FLASHPAGE_MIN_SECTOR_SIZE       (32 * 1024)
#elif defined(CPU_LINE_STM32F722xx) || \
      defined(CPU_LINE_STM32F723xx) || \
      defined(CPU_LINE_STM32F730xx) || \
      defined(CPU_LINE_STM32F732xx) || \
      defined(CPU_LINE_STM32F733xx)
#define FLASHPAGE_MIN_SECTOR_SIZE       (16 * 1024)
#else
/* Intentionally error on an unknown line to prevent flashpage errors */
#error Unknown STM32F7 Line, unable to determine FLASHPAGE_MIN_SECTOR_SIZE
#endif

#else /* CPU_FAM_STM32F7 */
#define FLASHPAGE_MIN_SECTOR_SIZE       (16 * 1024)
#endif

#if FLASHPAGE_DUAL_BANK
/* Number of "large" sectors + 4 for the small sectors that together equal a
 * single large sector. Times two to account for the two banks */
#define FLASHPAGE_NUMOF                 ((STM32_FLASHSIZE / \
                                         (8 * FLASHPAGE_MIN_SECTOR_SIZE)) + 8)
#else
/* Number of "large" sectors + 4 for the small sectors that together equal a
 * single large sector, eg: 1 MB = 7 * 128 KB sectors + 1 64 KB and 4 16 KB
 * sectors */
#define FLASHPAGE_NUMOF                 ((STM32_FLASHSIZE / \
                                         (8 * FLASHPAGE_MIN_SECTOR_SIZE)) + 4)
#endif

#endif

/* The minimum block size which can be written depends on the family.
 * However, the erase block is always FLASHPAGE_SIZE.
 */
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32WL)
#define FLASHPAGE_WRITE_BLOCK_SIZE            (8U)
typedef uint64_t stm32_flashpage_block_t;
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
      defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#define FLASHPAGE_WRITE_BLOCK_SIZE            (4U)
typedef uint32_t stm32_flashpage_block_t;
#else
#define FLASHPAGE_WRITE_BLOCK_SIZE            (2U)
typedef uint16_t stm32_flashpage_block_t;
#endif

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32WL)
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT       (8U)
#else
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT       (4U)
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
