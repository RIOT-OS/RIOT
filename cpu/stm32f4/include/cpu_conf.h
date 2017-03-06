/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f4 STM32F4
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F4
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef STM32F4_CPU_CONF_H
#define STM32F4_CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_MODEL_STM32F401RE)
#include "vendor/stm32f401xe.h"
#elif defined(CPU_MODEL_STM32F407VG)
#include "vendor/stm32f407xx.h"
#elif defined(CPU_MODEL_STM32F411RE)
#include "vendor/stm32f411xe.h"
#elif defined(CPU_MODEL_STM32F413ZH)
#include "vendor/stm32f413xx.h"
#elif defined(CPU_MODEL_STM32F415RG)
#include "vendor/stm32f415xx.h"
#elif defined(CPU_MODEL_STM32F446RE)
#include "vendor/stm32f446xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (82U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STM32F4_CPU_CONF_H */
/** @} */
