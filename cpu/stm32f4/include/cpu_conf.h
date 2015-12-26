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
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#if defined(CPU_MODEL_STM32F401RE)
#include "stm32f401xe.h"
#elif defined(CPU_MODEL_STM32F407VG)
#include "stm32f407xx.h"
#elif defined(CPU_MODEL_STM32F415RG)
#include "stm32f415xx.h"
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

/**
 * @brief Length for reading CPU_ID
 */
#define CPUID_ID_LEN                    (12)

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
