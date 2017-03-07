/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32l0 STM32L0
 * @brief           STM32L0 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.pertersen@fu-berlin.de>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef STM32L0_CPU_CONF_H
#define STM32L0_CPU_CONF_H

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_STM32L073RZ
#include "vendor/stm32l073xx.h"
#endif
#ifdef CPU_MODEL_STM32L053R8
#include "vendor/stm32l053xx.h"
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
