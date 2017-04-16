/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f2 STM32F2
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F2
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_MODEL_STM32F205RG)
#include "vendor/stm32f205xx.h"
#elif defined(CPU_MODEL_STM32F207ZG)
#include "vendor/stm32f207xx.h"
#elif defined(CPU_MODEL_STM32F215RG) || defined(CPU_MODEL_STM32F215VG) || defined(CPU_MODEL_STM32F215VE)
#include "vendor/stm32f215xx.h"
#elif defined(CPU_MODEL_STM32F217ZG)
#include "vendor/stm32f217xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (81U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
