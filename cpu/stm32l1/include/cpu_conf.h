/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32l1 STM32L1
 * @brief           CPU specific implementations for the STM32F1
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

/**
 * @brief  STM32 L1 cpu type
 *
 * From CMSIS header file, allowed values for L1 cpu types are:
 * * STM32L1XX_MD:
 *   * Ultra Low Power Medium-density devices: STM32L151x6xx, STM32L151x8xx,
 *     STM32L151xBxx, STM32L152x6xx, STM32L152x8xx, STM32L152xBxx,
 *     STM32L151x6xxA, STM32L151x8xxA, STM32L151xBxxA, STM32L152x6xxA,
 *     SM32L152x8xxA and STM32L152xBxxA.
 *   * Ultra Low Power Medium-density Value Line devices: STM32L100x6xx,
 *     STM32L100x8xx and STM32L100xBxx.
 *
 * * STM32L1XX_MDP:
 *   * Ultra Low Power Medium-density Plus devices: STM32L151xCxx,
 *     STM32L152xCxx and STM32L162xCxx
 *   * Ultra Low Power Medium-density Plus Value Line devices: STM32L100xCxx
 *
 * * STM32L1XX_HD: Ultra Low Power High-density devices: STM32L151xDxx,
 *    STM32L152xDxx and STM32L162xDxx
 *
 * * STM32L1XX_XL: Ultra Low Power XL-density devices: STM32L151xExx,
 *   STM32L152xExx and STM32L162xExx
 */
#if defined(CPU_MODEL_STM32L151RBA)
#define STM32L1XX_MD (1U)
#elif defined(CPU_MODEL_STM32L151RC)
#define STM32L1XX_MDP (1U)
#else
#define STM32L1XX_XL (1U)
#endif
#include "vendor/stm32l1xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32L151RBA)
#define CPU_IRQ_NUMOF                   (45U)
#else
#define CPU_IRQ_NUMOF                   (57U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @name   Flash page configuration
 * @{
 */
#if defined(CPU_MODEL_STM32L152RE) || defined(CPU_MODEL_STM32L151RC)
#define FLASHPAGE_SIZE             (256U)
#if defined(CPU_MODEL_STM32L152RE)
#define FLASHPAGE_NUMOF            (2048U)    /* 512KB */
#endif
#if defined(CPU_MODEL_STM32L151RC)
#define FLASHPAGE_NUMOF            (1024U)    /* 256KB */
#endif
#endif
/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
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
