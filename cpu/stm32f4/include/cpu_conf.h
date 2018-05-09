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

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_MODEL_STM32F401RE)
#include "vendor/stm32f401xe.h"
#elif defined(CPU_MODEL_STM32F407VG)
#include "vendor/stm32f407xx.h"
#elif defined(CPU_MODEL_STM32F410RB)
#include "vendor/stm32f410rx.h"
#elif defined(CPU_MODEL_STM32F411RE)
#include "vendor/stm32f411xe.h"
#elif defined(CPU_MODEL_STM32F412ZG)
#include "vendor/stm32f412zx.h"
#elif defined(CPU_MODEL_STM32F413CG) || defined(CPU_MODEL_STM32F413RG) \
    || defined(CPU_MODEL_STM32F413MG) || defined(CPU_MODEL_STM32F413VG) \
    || defined(CPU_MODEL_STM32F413ZG) || defined(CPU_MODEL_STM32F413CH) \
    || defined(CPU_MODEL_STM32F413RH) || defined(CPU_MODEL_STM32F413MH) \
    || defined(CPU_MODEL_STM32F413VH) || defined(CPU_MODEL_STM32F413ZH)
#include "vendor/stm32f413xx.h"
#elif defined(CPU_MODEL_STM32F415RG)
#include "vendor/stm32f415xx.h"
#elif defined(CPU_MODEL_STM32F423CH) || defined(CPU_MODEL_STM32F423RH) \
    || defined(CPU_MODEL_STM32F423MH) || defined(CPU_MODEL_STM32F423VH) \
    || defined(CPU_MODEL_STM32F423ZH)
#include "vendor/stm32f423xx.h"
#elif defined(CPU_MODEL_STM32F446RE) || defined(CPU_MODEL_STM32F446ZE)
#include "vendor/stm32f446xx.h"
#elif defined(CPU_MODEL_STM32F429ZI)
#include "vendor/stm32f429xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32F401RE)
#define CPU_IRQ_NUMOF                   (85U)
#elif defined(CPU_MODEL_STM32F407VG) || defined(CPU_MODEL_STM32F415RG)
#define CPU_IRQ_NUMOF                   (82U)
#elif defined(CPU_MODEL_STM32F410RB)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_MODEL_STM32F411RE)
#define CPU_IRQ_NUMOF                   (86U)
#elif defined(CPU_MODEL_STM32F412ZG) || defined(CPU_MODEL_STM32F446RE) \
    || defined(CPU_MODEL_STM32F446ZE)
#define CPU_IRQ_NUMOF                   (97U)
#elif defined(CPU_MODEL_STM32F413CG) || defined(CPU_MODEL_STM32F413RG) \
    || defined(CPU_MODEL_STM32F413MG) || defined(CPU_MODEL_STM32F413VG) \
    || defined(CPU_MODEL_STM32F413ZG) || defined(CPU_MODEL_STM32F413CH) \
    || defined(CPU_MODEL_STM32F413RH) || defined(CPU_MODEL_STM32F413MH) \
    || defined(CPU_MODEL_STM32F413VH) || defined(CPU_MODEL_STM32F413ZH) \
    || defined(CPU_MODEL_STM32F423CH) || defined(CPU_MODEL_STM32F423RH) \
    || defined(CPU_MODEL_STM32F423MH) || defined(CPU_MODEL_STM32F423VH) \
    || defined(CPU_MODEL_STM32F423ZH)
#define CPU_IRQ_NUMOF                   (102U)
#elif defined(CPU_MODEL_STM32F429ZI)
#define CPU_IRQ_NUMOF                   (91U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
