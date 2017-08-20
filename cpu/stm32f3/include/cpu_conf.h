/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f3 STM32F3
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F3
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_MODEL_STM32F303VC)
#include "vendor/stm32f303xc.h"
#elif defined(CPU_MODEL_STM32F334R8)
#include "vendor/stm32f334x8.h"
#elif defined(CPU_MODEL_STM32F303RE) || defined(CPU_MODEL_STM32F303ZE)
#include "vendor/stm32f303xe.h"
#elif defined(CPU_MODEL_STM32F303K8)
#include "vendor/stm32f303x8.h"
#elif defined(CPU_MODEL_STM32F302R8)
#include "vendor/stm32f302x8.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32F303RE) || defined (CPU_MODEL_STM32F303ZE)
#define CPU_IRQ_NUMOF                   (85U)
#else
#define CPU_IRQ_NUMOF                   (82U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
