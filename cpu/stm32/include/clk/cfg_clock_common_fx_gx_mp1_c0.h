/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2018-2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Base STM32Fx/Gx/MP1/C0 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock common configuration (F0/F1/F2/F3/F4/F7/G0/G4/MP1/C0)
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CONFIG_USE_CLOCK_PLL            0
#else
#if defined(CPU_FAM_STM32C0)  /* PLL not supported in STM32C0 */
#define CONFIG_USE_CLOCK_PLL            0
#define CONFIG_USE_CLOCK_HSI            1
#else
#define CONFIG_USE_CLOCK_PLL            1     /* Use PLL by default */
#endif
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSE))
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CONFIG_CLOCK_HSE
#if defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32MP1) || defined(CPU_FAM_STM32C0)
#define CONFIG_CLOCK_HSE                       MHZ(24)
#else
#define CONFIG_CLOCK_HSE                       MHZ(8)
#endif
#endif

#ifndef CONFIG_CLOCK_HSI
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
#define CONFIG_CLOCK_HSI                       MHZ(8)
#elif defined(CPU_FAM_STM32MP1)
#define CONFIG_CLOCK_HSI                       MHZ(64)
#elif defined(CPU_FAM_STM32C0)
#define CONFIG_CLOCK_HSI                       MHZ(48)
#else
#define CONFIG_CLOCK_HSI                       MHZ(16)
#endif
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
