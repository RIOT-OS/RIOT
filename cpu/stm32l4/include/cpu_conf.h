/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32l4 STM32L4
 * @brief           STM32L4 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.pertersen@fu-berlin.de>
*/

#ifndef STM32L4_CPU_CONF_H
#define STM32L4_CPU_CONF_H

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_STM32L476RG
#include "vendor/stm32l476xx.h"
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
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STM32L4_CPU_CONF_H */
/** @} */
