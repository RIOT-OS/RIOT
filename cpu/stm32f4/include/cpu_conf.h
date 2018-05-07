/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2048 OTA keys S.A.
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
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_LINE_STM32F401)
#include "vendor/stm32f401xe.h"
#elif defined(CPU_LINE_STM32F407)
#include "vendor/stm32f407xx.h"
#elif defined(CPU_LINE_STM32F410)
#include "vendor/stm32f410rx.h"
#elif defined(CPU_LINE_STM32F411)
#include "vendor/stm32f411xe.h"
#elif defined(CPU_LINE_STM32F412)
#include "vendor/stm32f412zx.h"
#elif defined(CPU_LINE_STM32F413)
#include "vendor/stm32f413xx.h"
#elif defined(CPU_LINE_STM32F415)
#include "vendor/stm32f415xx.h"
#elif defined(CPU_LINE_STM32F423)
#include "vendor/stm32f423xx.h"
#elif defined(CPU_LINE_STM32F446)
#include "vendor/stm32f446xx.h"
#elif defined(CPU_LINE_STM32F429)
#include "vendor/stm32f429xx.h"
#elif defined(CPU_LINE_STM32F437)
#include "vendor/stm32f437xx.h"
#elif defined(CPU_LINE_STM32F446)
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
#if defined(CPU_LINE_STM32F401)
#define CPU_IRQ_NUMOF                   (85U)
#elif defined(CPU_LINE_STM32F407) || defined(CPU_LINE_STM32F415)
#define CPU_IRQ_NUMOF                   (82U)
#elif defined(CPU_LINE_STM32F410)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_LINE_STM32F411)
#define CPU_IRQ_NUMOF                   (86U)
#elif defined(CPU_LINE_STM32F412) || defined(CPU_LINE_STM32F446)
#define CPU_IRQ_NUMOF                   (97U)
#elif defined(CPU_LINE_STM32F413) || defined(CPU_LINE_STM32F423)
#define CPU_IRQ_NUMOF                   (102U)
#elif defined(CPU_LINE_STM32F429) || defined(CPU_LINE_STM32F437)
#define CPU_IRQ_NUMOF                   (91U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
