/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2018-2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Base STM32Lx/Wx clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_CFG_CLOCK_COMMON_LX_WX_H
#define CLK_CFG_CLOCK_COMMON_LX_WX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration (L0/L1/L4/L5/WB/WL)
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#define CONFIG_USE_CLOCK_PLL            0
#else
#define CONFIG_USE_CLOCK_PLL            1     /* Use PLL by default */
#endif
#endif /* CONFIG_USE_CLOCK_PLL */
/** @} */

#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_MSI) || IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
     IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_MSI) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
     IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use MSI as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_MSI) || \
     IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_MSI) || \
     IS_ACTIVE(CONFIG_USE_CLOCK_HSE))
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CLOCK_HSE
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define CLOCK_HSE                       MHZ(24)
#else
#define CLOCK_HSE                       MHZ(8)
#endif
#endif

#define CLOCK_HSI                       MHZ(16)

#ifndef CONFIG_CLOCK_MSI
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define CONFIG_CLOCK_MSI                KHZ(4194)
#else
#define CONFIG_CLOCK_MSI                MHZ(48)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLK_CFG_CLOCK_COMMON_LX_WX_H */
/** @} */
