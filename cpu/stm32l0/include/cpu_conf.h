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

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_STM32L073RZ
#include "vendor/stm32l073xx.h"
#endif
#ifdef CPU_MODEL_STM32L072CZ
#include "vendor/stm32l072xx.h"
#endif
#ifdef CPU_MODEL_STM32L053R8
#include "vendor/stm32l053xx.h"
#endif
#ifdef CPU_MODEL_STM32L031K6
#include "vendor/stm32l031xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32L031K6)
#define CPU_IRQ_NUMOF                   (30U)
#else
#define CPU_IRQ_NUMOF                   (32U)
#endif
/** @} */

/**
 * @name    Flash page configuration
 * @{
 */
#if defined(CPU_MODEL_STM32L073RZ) || defined(CPU_MODEL_STM32L072CZ) || \
    defined(CPU_MODEL_STM32L053R8) || defined(CPU_MODEL_STM32L031K6)
#define FLASHPAGE_SIZE      (128U)
#endif

#if defined(CPU_MODEL_STM32L073RZ) || defined(CPU_MODEL_STM32L072CZ)
#define FLASHPAGE_NUMOF     (1536U)
#elif defined(CPU_MODEL_STM32L053R8)
#define FLASHPAGE_NUMOF     (512U)
#elif defined(CPU_MODEL_STM32L031K6)
#define FLASHPAGE_NUMOF     (256U)
#endif

/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (4U)
/* Writing should be always 4 byte aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (4U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
