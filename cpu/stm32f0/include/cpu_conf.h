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
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef STM32F0_CPU_CONF_H
#define STM32F0_CPU_CONF_H

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_STM32F051R8
#include "stm32f051x8.h"
#endif
#ifdef CPU_MODEL_STM32F091RC
#include "stm32f091xc.h"
#endif
#ifdef CPU_MODEL_STM32F072RB
#include "stm32f072xb.h"
#endif
#ifdef CPU_MODEL_STM32F070RB
#include "stm32f070xb.h"
#endif
#ifdef CPU_MODEL_STM32F030R8
#include "stm32f030x8.h"
#endif
#ifdef CPU_MODEL_STM32F042K6
#include "stm32f042x6.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (31U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STM32F0_CPU_CONF_H */
/** @} */
