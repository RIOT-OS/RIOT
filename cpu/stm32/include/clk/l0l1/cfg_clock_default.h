/*
 * Copyright (C) 2018-2020 Inria
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
 * @brief       Default STM32L0/STM32L1 clock configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_L0L1_CFG_CLOCK_DEFAULT_H
#define CLK_L0L1_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration
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

#ifndef CONFIG_USE_CLOCK_MSI
#define CONFIG_USE_CLOCK_MSI            0
#endif /* CONFIG_USE_CLOCK_MSI */

#ifndef CONFIG_USE_CLOCK_HSE
#define CONFIG_USE_CLOCK_HSE            0
#endif /* CONFIG_USE_CLOCK_HSE */

#ifndef CONFIG_USE_CLOCK_HSI
#define CONFIG_USE_CLOCK_HSI            0
#endif /* CONFIG_USE_CLOCK_HSI */

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

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            0
#endif

#ifndef CLOCK_HSE
#define CLOCK_HSE                       MHZ(24)
#endif
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE < MHZ(1) || CLOCK_HSE > MHZ(24))
#error "HSE clock frequency must be between 1MHz and 24MHz"
#endif

#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            0
#endif

#define CLOCK_HSI                       MHZ(16)

#ifndef CONFIG_CLOCK_MSI
#define CONFIG_CLOCK_MSI                KHZ(4194)
#endif

/* The following parameters configure a 32MHz system clock with HSI as input clock */
#ifndef CONFIG_CLOCK_PLL_DIV
#define CONFIG_CLOCK_PLL_DIV            (2)
#endif
#ifndef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL            (4)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_MSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#if CLOCK_HSE < MHZ(2)
#error "HSE must be greater than 2MHz when used as PLL input clock"
#endif
#define CLOCK_PLL_SRC                   (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CLOCK_HSI)
#endif /* CONFIG_BOARD_HAS_HSE */
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = ((PLL_IN / PLL_PREDIV) * PLL_MUL)
 * with:
 * PLL_IN:          input clock is HSE if available or HSI otherwise
 * PLL_DIV :        divider, allowed values: 2, 3, 4. Default is 2.
 * PLL_MUL:         multiplier, allowed values: 3, 4, 6, 8, 12, 16, 24, 32, 48. Default is 4.
 * CORECLOCK        -> 32MHz MAX!
 */
#define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_DIV) * CONFIG_CLOCK_PLL_MUL)
#if CLOCK_CORECLOCK > MHZ(32)
#error "SYSCLK cannot exceed 32MHz"
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* max: 32MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)   /* max: 32MHz */
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)
#endif
#define CLOCK_APB2                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB2_DIV)   /* max: 32MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CLK_L0L1_CFG_CLOCK_DEFAULT_H */
/** @} */
