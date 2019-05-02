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

#if defined(CPU_MODEL_STM32L151RBA)
#define STM32L1XX_MD (1U)
#include "vendor/stm32l151xba.h"
#elif defined(CPU_MODEL_STM32L151CB)
#define STM32L1XX_MD (1U)
#include "vendor/stm32l151xb.h"
#elif defined(CPU_MODEL_STM32L151RC)
#include "vendor/stm32l151xc.h"
#elif defined(CPU_MODEL_STM32L152RE)
#include "vendor/stm32l152xe.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32L151RBA) || defined(CPU_MODEL_STM32L151CB)
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
#if defined(CPU_MODEL_STM32L152RE) || defined(CPU_MODEL_STM32L151RC) || defined(CPU_MODEL_STM32L151CB)
#define FLASHPAGE_SIZE             (256U)
#if defined(CPU_MODEL_STM32L152RE)
#define FLASHPAGE_NUMOF            (2048U)    /* 512KB */
#endif
#if defined(CPU_MODEL_STM32L151RC)
#define FLASHPAGE_NUMOF            (1024U)    /* 256KB */
#endif
#if defined(CPU_MODEL_STM32L151CB)
#define FLASHPAGE_NUMOF            (512U)     /* 128KB */
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
